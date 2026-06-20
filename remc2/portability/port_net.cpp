// port_net.cpp
// NetBIOS emulation layer over TCP.
//
// ARCHITECTURE
// ============
// All communication runs over TCP — both the control channel (name registration,
// CALL/LISTEN handshake) and the data channel (SEND/RECEIVE game data).
//
// TCP gives us guaranteed, in-order delivery for free, so there is no need for
// sequence numbers, ACKs, NACKs, reorder buffers, or retransmit timers.
//
// TOPOLOGY
// ========
//
//   Control channel (every node → server):
//     Each node maintains one persistent TCP connection to the server.
//     Commands: ADD_NAME, CALL, LISTEN, CANCEL, DELETE, GET_IP.
//     Replies:  TESTADDNAME_OK/REJECT, CALL_ACCEPT/REJECT, LISTEN_ACCEPT/REJECT.
//
//   Data channel (peer ↔ peer, after CALL/LISTEN succeeds):
//     The caller opens a fresh TCP socket directly to the listener's data port
//     (which equals the listener's clPort).  The listener accepts it from a
//     per-node TCP server socket also bound to clPort.
//     After setup both sides exchange MESS_DIRECT_SEND packets over that socket.
//     The channel is torn down on HANG_UP or peer disconnect.
//
// HEARTBEAT
// =========
//   TCP itself signals a broken link (recv returns 0 / error).
//   If no data is received for HEARTBEAT_TIMEOUT_MS the session is declared
//   dead and ncb_cmd_cplt_49 is set to NRC_SCLOSED so Network.cpp can
//   perform CANCEL / HANG_UP as on a real NetBIOS adapter.
//   After HEARTBEAT_PROBE_MS of silence a lightweight MESS_HEARTBEAT is sent.
//
// NCB COMMAND DISPATCH  (simulateInterupt → handleConnections[])
// =============================================================
//   Pass 1 – deliver pending RECEIVE data from per-session queues.
//   Pass 2 – timeout / retry per NCB command.
//   Pass 4 – heartbeat / dead-peer detection on established sessions.
//   (Pass 3 — UDP retransmit — is gone; TCP is reliable.)
//
// API COMPATIBILITY
// =================
//   simulateInterupt(), InitMyNetLib(), EndMyNetLib(), CleanMessages(),
//   printState(), printState2(), timeState(), IsRemoteAlive(),
//   ReceiveServerAddName() — all signatures unchanged from the UDP version.

//#define TEST_NETWORK_MESSAGES_NETWORK
//#define TEST_NETWORK_MESSAGES_PORTNET

#define _CRT_SECURE_NO_WARNINGS
#include "port_net.h"
#include <map>
#include <set>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <memory>
#include <chrono>
#include <cassert>
#include <algorithm>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET socket_t;
#define CLOSE_SOCKET   closesocket
#define SOCK_INVALID   INVALID_SOCKET
static int sock_errno() { return WSAGetLastError(); }
static bool would_block(int e) { return e == WSAEWOULDBLOCK || e == WSAEINPROGRESS; }
static void set_nonblocking(socket_t s) {
	unsigned long m = 1; ioctlsocket(s, FIONBIO, &m);
}
static void set_nodelay(socket_t s) {
	int one = 1; setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*)&one, sizeof(one));
}
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
typedef int socket_t;
#define CLOSE_SOCKET   close
#define SOCK_INVALID   (-1)
static int sock_errno() { return errno; }
static bool would_block(int e) { return e == EAGAIN || e == EWOULDBLOCK || e == EINPROGRESS; }
static void set_nonblocking(socket_t s) {
	int f = fcntl(s, F_GETFL, 0); fcntl(s, F_SETFL, f | O_NONBLOCK);
}
static void set_nodelay(socket_t s) {
	int one = 1; setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));
}
#endif

using namespace std;

// ---------------------------------------------------------------------------
// Timing constants
// ---------------------------------------------------------------------------
static const long HEARTBEAT_PROBE_MS = 1000; // silence (ms) before sending a probe
static const long HEARTBEAT_TIMEOUT_MS = 5000; // silence (ms) → peer dead → CANCEL
static const int  MAX_RETRIES = 8;    // CALL / ADD_NAME retries

// NetBIOS status codes
#define NRC_GOODRET  0x00
#define NRC_CMDTMO   0x05
#define NRC_SCLOSED  0x0a
#define NRC_CMDCAN   0x0b
#define NRC_INUSE    0x16
#define NRC_PENDING  0xff

// ---------------------------------------------------------------------------
// Globals required by Network.cpp / port_net.h
// ---------------------------------------------------------------------------
uint8_t* Zero_pointer_net = nullptr;
int         timest_index = 0;
clock_t     timest_timer = 0;
const int   timest_max_mess = 400;
std::string timest_buffer[timest_max_mess];

// ---------------------------------------------------------------------------
// Protocol message type constants  (wire-compatible with the UDP version)
// ---------------------------------------------------------------------------
const int32_t MESS_UNKNOWN = -1;
const int32_t MESS_CLIENT_TESTADDNAME = 4;
const int32_t MESS_SERVER_TESTADDNAME_OK = 5;
const int32_t MESS_SERVER_TESTADDNAME_REJECT = 6;
const int32_t MESS_CLIENT_MESSAGE_LISTEN = 7;
const int32_t MESS_CLIENT_MESSAGE_CALL = 8;
const int32_t MESS_SERVER_CALL_ACCEPT = 9;
const int32_t MESS_SERVER_LISTEN_ACCEPT = 10;
const int32_t MESS_SERVER_CALL_REJECT = 11;
const int32_t MESS_SERVER_LISTEN_REJECT = 12;
const int32_t MESS_CLIENT_CANCEL = 13;
const int32_t MESS_CLIENT_DELETE = 14;
const int32_t MESS_DIRECT_SEND = 15; // peer → peer game data
const int32_t MESS_HEARTBEAT = 17; // keep-alive probe
const int32_t MESS_HEARTBEAT_ACK = 20; // keep-alive reply
const int32_t MESS_CLIENT_GET_IP = 18;
const int32_t MESS_SERVER_GIVE_IP = 19;

char* MessageIndexToText(int32_t index)
{
	switch (index) {
	case MESS_CLIENT_TESTADDNAME:        return (char*)"MESS_CLIENT_TESTADDNAME";
	case MESS_SERVER_TESTADDNAME_OK:     return (char*)"MESS_SERVER_TESTADDNAME_OK";
	case MESS_SERVER_TESTADDNAME_REJECT: return (char*)"MESS_SERVER_TESTADDNAME_REJECT";
	case MESS_CLIENT_MESSAGE_LISTEN:     return (char*)"MESS_CLIENT_MESSAGE_LISTEN";
	case MESS_CLIENT_MESSAGE_CALL:       return (char*)"MESS_CLIENT_MESSAGE_CALL";
	case MESS_SERVER_CALL_ACCEPT:        return (char*)"MESS_SERVER_CALL_ACCEPT";
	case MESS_SERVER_LISTEN_ACCEPT:      return (char*)"MESS_SERVER_LISTEN_ACCEPT";
	case MESS_SERVER_CALL_REJECT:        return (char*)"MESS_SERVER_CALL_REJECT";
	case MESS_SERVER_LISTEN_REJECT:      return (char*)"MESS_SERVER_LISTEN_REJECT";
	case MESS_CLIENT_CANCEL:             return (char*)"MESS_CLIENT_CANCEL";
	case MESS_CLIENT_DELETE:             return (char*)"MESS_CLIENT_DELETE";
	case MESS_DIRECT_SEND:               return (char*)"MESS_DIRECT_SEND";
	case MESS_HEARTBEAT:                 return (char*)"MESS_HEARTBEAT";
	case MESS_HEARTBEAT_ACK:             return (char*)"MESS_HEARTBEAT_ACK";
	case MESS_CLIENT_GET_IP:             return (char*)"MESS_CLIENT_GET_IP";
	case MESS_SERVER_GIVE_IP:            return (char*)"MESS_SERVER_GIVE_IP";
	}
	return (char*)"MESS_UNKNOWN";
}

// ---------------------------------------------------------------------------
// shadow_myNCB helpers  (unchanged)
// ---------------------------------------------------------------------------
shadow_myNCB myNCBtoShadow(myNCB from)
{
	shadow_myNCB to;
	to.ncb_command_0 = from.ncb_command_0;
	to.ncb_retcode_1 = from.ncb_retcode_1;
	to.ncb_lsn_2 = from.ncb_lsn_2;
	to.ncb_num_3 = from.ncb_num_3;
	to.ncb_buffer_4 = (from.ncb_buffer_4.p - Zero_pointer_net);
	to.ncb_bufferLength_8 = from.ncb_bufferLength_8;
	for (int i = 0; i < 16; i++) to.ncb_callName_10[i] = from.ncb_callName_10[i];
	for (int i = 0; i < 16; i++) to.ncb_name_26[i] = from.ncb_name_26[i];
	to.ncb_rto_42 = from.ncb_rto_42;
	to.ncb_sto_43 = from.ncb_sto_43;
	to.ncb_cmd_cplt_49 = from.ncb_cmd_cplt_49;
	return to;
}

myNCB myNCBfromShadow(shadow_myNCB from)
{
	myNCB to;
	to.ncb_command_0 = from.ncb_command_0;
	to.ncb_retcode_1 = from.ncb_retcode_1;
	to.ncb_lsn_2 = from.ncb_lsn_2;
	to.ncb_num_3 = from.ncb_num_3;
	to.ncb_buffer_4.p = (from.ncb_buffer_4 + Zero_pointer_net);
	to.ncb_bufferLength_8 = from.ncb_bufferLength_8;
	for (int i = 0; i < 16; i++) to.ncb_callName_10[i] = from.ncb_callName_10[i];
	for (int i = 0; i < 16; i++) to.ncb_name_26[i] = from.ncb_name_26[i];
	to.ncb_rto_42 = from.ncb_rto_42;
	to.ncb_sto_43 = from.ncb_sto_43;
	to.ncb_cmd_cplt_49 = from.ncb_cmd_cplt_49;
	return to;
}

// ---------------------------------------------------------------------------
// Wire format
// Every message is framed as:
//   [uint32_t big-endian body_length][body bytes]
// where body = fixed message_info header + variable payload.
// The 4-byte length prefix is NOT counted in body_length.
// ---------------------------------------------------------------------------
static constexpr size_t MSG_WIRE_HEADER =
sizeof(uint32_t)       // size  (payload bytes)
+ sizeof(int32_t)        // message type
+ sizeof(int32_t)        // index
+ sizeof(int32_t)        // port
+ sizeof(uint32_t)       // seq_num  (always 0 over TCP)
+ sizeof(uint32_t)       // ack_num  (always 0 over TCP)
+ sizeof(shadow_myNCB);  // NCB snapshot

std::string DataToString(const message_info& m)
{
	size_t total = MSG_WIRE_HEADER + m.size;
	return std::string(reinterpret_cast<const char*>(&m), total);
}

std::string Pack_Message(int32_t      message,
	shadow_myNCB locNCB,
	int32_t      locIndex,
	int32_t      port = -1,
	const char* data = nullptr,
	int          size_of_data = 0,
	uint32_t     seq_num = 0,
	uint32_t     ack_num = 0)
{
	message_info m;
	memset(&m, 0, sizeof(m));
	m.message = message;
	m.size = size_of_data;
	m.messNCB = locNCB;
	m.index = locIndex;
	m.port = port;
	m.seq_num = seq_num;
	m.ack_num = ack_num;
	if (data && size_of_data > 0)
		memcpy(m.data, data, size_of_data);
	return DataToString(m);
}

void debug_net_printf(const char* format, ...);   // forward

message_info Unpack_Message(const std::string& data)
{
	message_info out;
	memset(&out, 0, sizeof(out));
	if (data.size() < MSG_WIRE_HEADER) {
#ifdef TEST_NETWORK_MESSAGES_PORTNET
		debug_net_printf("Unpack: too short %zu/%zu\n", data.size(), MSG_WIRE_HEADER);
#endif
		out.message = MESS_UNKNOWN;
		return out;
	}
	memcpy(&out, data.data(), MSG_WIRE_HEADER);
	if (out.size > sizeof(out.data) || MSG_WIRE_HEADER + out.size > data.size()) {
#ifdef TEST_NETWORK_MESSAGES_PORTNET
		debug_net_printf("Unpack: bad payload size %u\n", out.size);
#endif
		out.message = MESS_UNKNOWN;

		return out;
	}
	if (out.size > 0)
		memcpy(out.data, data.data() + MSG_WIRE_HEADER, out.size);
	return out;
}

// ---------------------------------------------------------------------------
// Debug logging
// ---------------------------------------------------------------------------
FILE* debug_net_output = nullptr;
std::string debug_net_filename1 = "net_messages_log1.txt";
bool        debug_net_first = true;
bool        debug_net = true;
std::mutex  debug_net_mutex;

void debug_net_printf(const char* format, ...)
{
	char buf[1024 * 40];
	va_list arg;
	va_start(arg, format);
	vsprintf(buf, format, arg);
	va_end(arg);
	std::lock_guard<std::mutex> lk(debug_net_mutex);
	debug_net_output = fopen(debug_net_filename1.c_str(), debug_net_first ? "wt" : "at");
	debug_net_first = false;
	if (debug_net_output) {
		fprintf(debug_net_output, "%ld|%s", (long)clock(), buf);
		fclose(debug_net_output);
	}
}

static std::string PayloadPreview(const char* d, uint32_t sz)
{
	if (!sz) return "";
	std::string h, a;
	int show = sz < 16 ? (int)sz : 16;
	char tmp[8];
	for (int i = 0; i < show; i++) {
		snprintf(tmp, sizeof(tmp), "%02X ", (unsigned char)d[i]);
		h += tmp;
		a += (d[i] >= 32 && d[i] < 127) ? d[i] : '.';
	}
	if ((int)sz > 16) { h += "..."; a += "..."; }
	return "hex=[" + h + "] asc=[" + a + "]";
}

static void LogPkt(const char* dir, const std::string& addr, int port,
	const message_info& u)
{
	std::string p = PayloadPreview(u.data, u.size);
#ifdef TEST_NETWORK_MESSAGES_PORTNET
	debug_net_printf("%s %s:%d  msg=%-30s size=%u name=[%.16s] call=[%.16s] idx=%d%s%s\n",
		dir, addr.c_str(), port, MessageIndexToText(u.message),
		u.size, u.messNCB.ncb_name_26, u.messNCB.ncb_callName_10, u.index,
		p.empty() ? "" : " ", p.c_str());
#endif
}

// ---------------------------------------------------------------------------
// TCP helpers — length-prefixed framing
// ---------------------------------------------------------------------------

// Send all bytes of msg on socket s (blocking).
static bool TcpSendFull(socket_t s, const std::string& msg)
{
	if (s == SOCK_INVALID) return false;
	uint32_t len = (uint32_t)msg.size();
	uint8_t hdr[4] = {
		(uint8_t)(len >> 24), (uint8_t)(len >> 16),
		(uint8_t)(len >> 8), (uint8_t)(len) };
	auto send_all = [s](const void* b, int n) -> bool {
		const char* p = (const char*)b; int left = n;
		while (left > 0) {
			int r = send(s, p, left, 0);
			if (r <= 0) return false;
			p += r; left -= r;
		}
		return true;
		};
	return send_all(hdr, 4) && send_all(msg.data(), (int)msg.size());
}

// Incremental receive buffer — appends data, extracts complete frames.
struct TcpRecvBuf {
	std::string              raw;
	std::vector<std::string> ready; // complete body strings

	// Max sane frame size: header + max payload
	static const uint32_t MAX_FRAME = (uint32_t)(sizeof(message_info) + 64);

	// Returns false if the socket is closed / errored.
	bool Poll(socket_t s) {
		char tmp[65536];
		while (true) {
			int n = recv(s, tmp, sizeof(tmp), 0);
			if (n > 0) { raw.append(tmp, n); }
			else if (n == 0) { return false; } // peer closed
			else { int e = sock_errno(); if (would_block(e)) break; return false; }
		}
		while (raw.size() >= 4) {
			uint32_t len = ((uint8_t)raw[0] << 24) | ((uint8_t)raw[1] << 16)
				| ((uint8_t)raw[2] << 8) | (uint8_t)raw[3];
			// Sanity check: reject frames that are impossibly large.
			// This catches framing de-sync (e.g. if a send was interrupted
			// mid-frame and the socket was re-used — should not happen on
			// a single TCP connection but defends against bugs).
			if (len > MAX_FRAME) {
#ifdef TEST_NETWORK_MESSAGES_PORTNET
				debug_net_printf("TcpRecvBuf: FATAL bad frame len=%u (max=%u), discarding %zu bytes — connection likely corrupted\n", len, MAX_FRAME, raw.size());
#endif
				raw.clear();
				return false; // treat as disconnect — upper layer will reconnect
			}
			if (raw.size() < 4 + len) break; // incomplete frame, wait for more data
			ready.push_back(raw.substr(4, len));
			raw.erase(0, 4 + len);
		}
		return true;
	}
};

// ---------------------------------------------------------------------------
// Name / connection registry  (unchanged API surface)
// ---------------------------------------------------------------------------
std::vector<std::string> NetworkName;
std::vector<TypeIpPort>  clientIpPort;
std::vector<std::string> ListenName;
std::vector<TypeIpPort>  clientListenID;
std::vector<std::string> ListenName2;
std::vector<TypeIpPort>  clientListenID2;
std::vector<myNCB*>      clientConnection; // established NCBs, for Pass 4

bool receiveServerAddName = false;
bool ReceiveServerAddName() { return receiveServerAddName; }

static std::string TrimName(const char* s, int maxLen = 16)
{
	int e = maxLen;
	while (e > 0 && (s[e - 1] == ' ' || s[e - 1] == '\0')) --e;
	return std::string(s, e);
}

TypeIpPort GetIpPortFromName(std::string name)
{
	std::string t = TrimName(name.c_str(), (int)name.size());
	for (int i = 0; i < (int)NetworkName.size(); i++)
		if (TrimName(NetworkName[i].c_str(), (int)NetworkName[i].size()) == t)
			return clientIpPort[i];
	TypeIpPort r; r.adress = "x999"; r.port = -1; return r;
}

std::string GetNameNetworkFromId(TypeIpPort ip)
{
	for (int i = 0; i < (int)NetworkName.size(); i++)
		if (ip.adress == clientIpPort[i].adress && ip.port == clientIpPort[i].port)
			return NetworkName[i];
	return "";
}

std::string GetNameNetwork(std::string name)
{
	std::string t = TrimName(name.c_str(), (int)name.size());
	for (auto& n : NetworkName)
		if (TrimName(n.c_str(), (int)n.size()) == t) return n;
	return "";
}

int GetNameNetworkIndex(std::string name)
{
	std::string t = TrimName(name.c_str(), (int)name.size());
	for (int i = 0; i < (int)NetworkName.size(); i++)
		if (TrimName(NetworkName[i].c_str(), (int)NetworkName[i].size()) == t) return i;
	return -1;
}

void AddNetworkName(std::string name, TypeIpPort ip)
{
	if (GetNameNetwork(name).empty()) { NetworkName.push_back(name); clientIpPort.push_back(ip); }
}

bool ExistNetworkName(std::string name, TypeIpPort ip)
{
	std::string t = TrimName(name.c_str(), (int)name.size());
	for (int i = 0; i < (int)NetworkName.size(); i++)
		if (TrimName(NetworkName[i].c_str(), (int)NetworkName[i].size()) == t &&
			clientIpPort[i].adress == ip.adress && clientIpPort[i].port == ip.port)
			return true;
	return false;
}

void RemoveNetworkName(std::string name)
{
	int idx = GetNameNetworkIndex(name);
	if (idx < 0) return;
	NetworkName.erase(NetworkName.begin() + idx);
	clientIpPort.erase(clientIpPort.begin() + idx);
}

TypeIpPort GetOtherSide(TypeIpPort ip)
{
	for (int i = 0; i < (int)clientListenID.size(); i++)
		if (clientListenID[i].adress == ip.adress && clientListenID[i].port == ip.port)
			return clientListenID2[i];
	for (int i = 0; i < (int)clientListenID2.size(); i++)
		if (clientListenID2[i].adress == ip.adress && clientListenID2[i].port == ip.port)
			return clientListenID[i];
	TypeIpPort r; r.adress = "x1000"; r.port = -1; return r;
}

std::string GetListenNetwork(std::string name)
{
	for (auto& n : ListenName) if (!name.compare(n)) return n;
	return "";
}

int GetNameListenIndex(std::string name)
{
	for (int i = 0; i < (int)ListenName.size(); i++)
		if (!name.compare(ListenName[i])) return i;
	return -1;
}

int GetName2ListenIndex(std::string name)
{
	for (int i = 0; i < (int)ListenName2.size(); i++)
		if (!name.compare(ListenName2[i])) return i;
	return -1;
}

void AddListenName(myNCB* c)
{
	if (!GetListenNetwork(c->ncb_callName_10).empty()) return;
	ListenName.push_back(c->ncb_callName_10);
	clientListenID.push_back(GetIpPortFromName(c->ncb_callName_10));
	ListenName2.push_back(c->ncb_name_26);
	clientListenID2.push_back(GetIpPortFromName(c->ncb_name_26));
}

bool AddListenName2(const shadow_myNCB* c)
{
	TypeIpPort id1 = GetIpPortFromName(c->ncb_callName_10);
	if (id1.adress == "x999") return false;
	TypeIpPort id2 = GetIpPortFromName(c->ncb_name_26);
	if (id2.adress == "x999") return false;
	int idx = GetNameListenIndex(c->ncb_name_26);
	if (idx < 0) return false;
	clientListenID[idx] = id1;
	clientListenID2[idx] = id2;
	return true;
}

void RemoveListenName(std::string name)
{
	int idx = GetNameListenIndex(name);
	if (idx >= 0) {
		ListenName.erase(ListenName.begin() + idx);
		clientListenID.erase(clientListenID.begin() + idx);
		ListenName2.erase(ListenName2.begin() + idx);
		clientListenID2.erase(clientListenID2.begin() + idx);
	}
	int idx2 = GetName2ListenIndex(name);
	if (idx2 >= 0) {
		ListenName.erase(ListenName.begin() + idx2);
		clientListenID.erase(clientListenID.begin() + idx2);
		ListenName2.erase(ListenName2.begin() + idx2);
		clientListenID2.erase(clientListenID2.begin() + idx2);
	}
}

// ---------------------------------------------------------------------------
// NCB state machine entry
// ---------------------------------------------------------------------------
enum Neti_type {
	NETI_NOT_SET, NETI_ADD_NAME, NETI_ADD_NAME_REJECT, NETI_ADD_NAME_OK,
	NETI_CALL, NETI_CALL_ACCEPT, NETI_CALL_REJECT,
	NETI_LISTEN, NETI_LISTEN_REJECT, NETI_RECEIVE
};

struct connectionTime {
	myNCB* connection;
	long      startTime;
	int       timeout;
	int32_t   index;
	Neti_type state;
	int       retryCount;
};

std::vector<connectionTime> handleConnections;
std::mutex                  connections_mutex;  // guards handleConnections
std::mutex                  clientConnMutex;    // guards clientConnection (separate!)

static connectionTime* GetConnection(int32_t index)
{
	for (auto& c : handleConnections) if (c.index == index) return &c;
	return nullptr;
}

int32_t lastIndex = 0;
int32_t GetNextIndex()
{
	if (++lastIndex > 1000000) lastIndex = 0;
	return lastIndex;
}

static void setListenConnection(myNCB* c)
{
	std::lock_guard<std::mutex> lk(clientConnMutex);
	for (auto* p : clientConnection) if (p == c) return;
	clientConnection.push_back(c);
}

static void deleteListenConnection(myNCB* c)
{
	std::lock_guard<std::mutex> lk(clientConnMutex);
	for (int i = 0; i < (int)clientConnection.size(); i++)
		if (clientConnection[i] == c) { clientConnection.erase(clientConnection.begin() + i); return; }
}

// Mark the LISTEN NCB that is waiting for the caller identified by tmp->ncb_name_26.
//
// LISTEN NCB layout:
//   ncb_callName_10 = name we listen FOR  (= the caller's name, e.g. "NETH201")
//   ncb_name_26     = our own name        (= the listener's name, e.g. "NETH200")
//
// The server sends LISTEN_ACCEPT with:
//   messNCB.ncb_name_26     = caller's name    ("NETH201") ← what we match against
//   messNCB.ncb_callName_10 = listener's name  ("NETH200") ← NOT used for matching
//
// So: tmp->ncb_name_26 (caller's name) == c->ncb_callName_10 (who we waited for).
static bool setListen(myNCB* tmp)
{
#ifdef TEST_NETWORK_MESSAGES_PORTNET
	debug_net_printf("setListen: looking for NCB whose callName=[%.16s]\n", tmp->ncb_name_26);
#endif
	std::lock_guard<std::mutex> lk(clientConnMutex);
	for (auto* c : clientConnection) {
		if (memcmp(tmp->ncb_name_26, c->ncb_callName_10, 16) == 0) {
			c->ncb_lsn_2 = 20;
			//c->ncb_cmd_cplt_49 = NRC_GOODRET;
#ifdef TEST_NETWORK_MESSAGES_PORTNET
			debug_net_printf("setListen: MATCH name=[%.16s] call=[%.16s] lsn=20\n", c->ncb_name_26, c->ncb_callName_10);
#endif
			return true;
		}
#ifdef TEST_NETWORK_MESSAGES_PORTNET
		debug_net_printf("setListen: candidate name=[%.16s] call=[%.16s] NO MATCH\n", c->ncb_name_26, c->ncb_callName_10);
#endif
	}
#ifdef TEST_NETWORK_MESSAGES_PORTNET
	debug_net_printf("setListen: NO MATCH\n");
#endif
	return false;
}

// ---------------------------------------------------------------------------
// Per-session TCP data connection
// One TcpSession exists for each active CALL/LISTEN pair.
// The session socket carries only MESS_DIRECT_SEND and MESS_HEARTBEAT frames.
// ---------------------------------------------------------------------------
struct TcpSession {
	socket_t    sock = SOCK_INVALID;
	TcpRecvBuf  rbuf;
	std::string peerAddr;
	int         peerPort = 0;
	myNCB* ownerNCB = nullptr; // the LISTEN or CALL NCB for this session

	// Received game payloads ready for Pass 1
	std::mutex              queueMtx;
	std::queue<std::string> dataQueue; // packed message_info strings (MESS_DIRECT_SEND)

	// Send-side mutex: prevents interleaved writes from SendNetwork (main/bg thread)
	// and PollSessions HEARTBEAT sends (network thread).
	std::mutex              sendMtx;

	// Liveness tracking
	std::chrono::steady_clock::time_point lastRx;
	std::chrono::steady_clock::time_point lastProbeSent;
	bool probePending = false;
	std::atomic<bool> alive{ false };

	void Touch() { lastRx = std::chrono::steady_clock::now(); probePending = false; }

	long SilenceMs() const {
		return (long)std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now() - lastRx).count();
	}

	// Send a packed message over the session socket.
	// Thread-safe: sendMtx prevents concurrent writes from different threads.
	bool Send(const std::string& msg) {
		if (!alive.load() || sock == SOCK_INVALID) return false;
		std::lock_guard<std::mutex> lk(sendMtx);
		if (!alive.load() || sock == SOCK_INVALID) return false; // re-check after lock
		if (debug_net) { message_info u = Unpack_Message(msg); LogPkt("DATA TX →", peerAddr, peerPort, u); }
		bool ok = TcpSendFull(sock, msg);
		if (!ok) { alive.store(false); }
		return ok;
	}

	void Close() {
		alive.store(false);
		std::lock_guard<std::mutex> lk(sendMtx); // wait for any in-progress send
		if (sock != SOCK_INVALID) { CLOSE_SOCKET(sock); sock = SOCK_INVALID; }
	}
};

static std::map<myNCB*, std::shared_ptr<TcpSession>> tcpSessions;
static std::mutex                                     sessions_mutex;

static std::shared_ptr<TcpSession> GetSession(myNCB* ncb)
{
	std::lock_guard<std::mutex> lk(sessions_mutex);
	auto it = tcpSessions.find(ncb);
	return (it != tcpSessions.end()) ? it->second : nullptr;
}

static void AddSession(myNCB* ncb, std::shared_ptr<TcpSession> s)
{
	std::lock_guard<std::mutex> lk(sessions_mutex); tcpSessions[ncb] = s;
}

static void RemoveSession(myNCB* ncb)
{
	std::lock_guard<std::mutex> lk(sessions_mutex);
	auto it = tcpSessions.find(ncb);
	if (it != tcpSessions.end()) { it->second->Close(); tcpSessions.erase(it); }
}

// ---------------------------------------------------------------------------
// IsRemoteAlive  (unchanged API)
// ---------------------------------------------------------------------------
bool IsRemoteAlive(const std::string& addr, int port)
{
	std::lock_guard<std::mutex> lk(sessions_mutex);
	for (auto& kv : tcpSessions) {
		auto& s = kv.second;
		if (s->alive && s->peerAddr == addr && s->peerPort == port)
			return s->SilenceMs() < HEARTBEAT_TIMEOUT_MS;
	}
	return false;
}

// ---------------------------------------------------------------------------
// CleanMessages  (unchanged API)
// ---------------------------------------------------------------------------
void CleanMessages(myNCB locNCB)
{
	std::lock_guard<std::mutex> lk(sessions_mutex);
	for (auto& kv : tcpSessions) {
		auto& s = kv.second;
		std::lock_guard<std::mutex> qlk(s->queueMtx);
		while (!s->dataQueue.empty()) s->dataQueue.pop();
	}
}

// ---------------------------------------------------------------------------
// Peer data-accept socket  (one per node, bound to clPort)
// ---------------------------------------------------------------------------
static socket_t peerListenSock = SOCK_INVALID;

static bool StartPeerListen(int port)
{
	peerListenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (peerListenSock == SOCK_INVALID) return false;
	int opt = 1;
	setsockopt(peerListenSock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
	set_nonblocking(peerListenSock);
	sockaddr_in a{}; a.sin_family = AF_INET; a.sin_port = htons((unsigned short)port);
	a.sin_addr.s_addr = INADDR_ANY;
	if (bind(peerListenSock, (sockaddr*)&a, sizeof(a)) != 0) {
#ifdef TEST_NETWORK_MESSAGES_PORTNET
		debug_net_printf("PeerListen: bind(%d) failed err=%d\n", port, sock_errno());
#endif
		CLOSE_SOCKET(peerListenSock); peerListenSock = SOCK_INVALID; return false;
	}
	listen(peerListenSock, 8);
#ifdef TEST_NETWORK_MESSAGES_PORTNET
	debug_net_printf("PeerListen: data port %d\n", port);
#endif
	return true;
}

// ---------------------------------------------------------------------------
// Control channel
// Client: one TCP socket to server (ctrlSock).
// Server: accept socket (ctrlListenSock) + one socket per client.
// ---------------------------------------------------------------------------
static socket_t  ctrlSock = SOCK_INVALID; // client's connection to server
static TcpRecvBuf ctrlRecvBuf;
static std::mutex ctrlSendMtx;

static socket_t  ctrlListenSock = SOCK_INVALID; // server's accept socket

struct ClientCtrl {
	socket_t    sock = SOCK_INVALID;
	TcpRecvBuf  rbuf;
	std::string addr;
	int         port = 0; // the SOURCE port of the TCP connection from the client
	int         dataPort = 0; // the client's clPort (data port), filled from GET_IP
};
static std::vector<ClientCtrl> ctrlClients;
// ONE recursive mutex protects all access to ctrlClients.
// recursive_mutex is needed because PollCtrlClients (outer lock) calls
// HandleServerMsg which calls SendToCtrlClient (inner lock) — same thread,
// same mutex.  std::mutex would deadlock; std::recursive_mutex allows it.
static std::recursive_mutex ctrlClientsMtx;

static bool SendCtrl(const std::string& msg)
{
	std::lock_guard<std::mutex> lk(ctrlSendMtx);
	if (ctrlSock == SOCK_INVALID) return false;
	if (debug_net) { message_info u = Unpack_Message(msg); LogPkt("CTRL TX →", "server", 0, u); }
	return TcpSendFull(ctrlSock, msg);
}

// Send a control reply to a connected client identified by addr + dataPort.
static bool SendToCtrlClient(const std::string& msg, const std::string& addr, int dataPort)
{
	std::lock_guard<std::recursive_mutex> lk(ctrlClientsMtx);
	for (auto& cc : ctrlClients) {
		if (cc.addr == addr && cc.dataPort == dataPort) {
			if (debug_net) { message_info u = Unpack_Message(msg); LogPkt("CTRL TX →", addr, dataPort, u); }
			return TcpSendFull(cc.sock, msg);
		}
	}
	// Fallback: match by addr only (dataPort not yet set)
	for (auto& cc : ctrlClients) {
		if (cc.addr == addr) {
			if (debug_net) { message_info u = Unpack_Message(msg); LogPkt("CTRL TX →", addr, dataPort, u); }
			return TcpSendFull(cc.sock, msg);
		}
	}
#ifdef TEST_NETWORK_MESSAGES_PORTNET
	debug_net_printf("SendToCtrlClient: no client %s:%d\n", addr.c_str(), dataPort);
#endif
	return false;
}

// ===========================================================================
// NetworkClass
// ===========================================================================
namespace MyNetworkLib {
	/*
	class NetworkClass {
	public:
		std::string clHost;
		int  clPort = 0, clServerPort = 0;
		bool clIam_server = false, clIam_client = false, IpPortIsSet = false;
		bool serverAddName = false, serverAddname = false;
		std::vector<TypeIpPort> registered;

		NetworkClass(bool iam_server, bool iam_client,
			std::string host, int port, int serverPort);
		~NetworkClass();

		void StartNetworkThread();
		void StopNetworkThread();
		void locUpdateNetworkSingleThread();

		// NCB dispatchers
		void AddName(myNCB*, int32_t);
		void CancelNetwork(myNCB*, int32_t);
		void DeleteNetwork(myNCB*, int32_t);
		void CallNetwork(myNCB*, int32_t);
		void ListenNetwork(myNCB*, int32_t);
		void SendNetwork(myNCB*, int32_t);

	private:
		std::thread       netThread;
		std::atomic<bool> netRunning{ false };

		// directPeers: ncb_name/callName (16 chars) → {addr, dataPort}
		struct PeerInfo { std::string addr; int dataPort; };
		std::map<std::string, PeerInfo> directPeers;

		void ConnectToServer();
		void AcceptCtrlClients();
		void PollCtrlClients();
		void PollCtrlSocket();
		void AcceptPeerConnections();
		void PollSessions();
		bool ConnectDataToPeer(myNCB* ncb, const std::string& addr, int dataPort);
		void HandleServerMsg(const std::string& raw, const std::string& senderAddr, int senderDataPort);
		void HandleClientMsg(const std::string& raw);
		void UpdateClient();
	};*/

	// ---------------------------------------------------------------------------
	NetworkClass::NetworkClass(bool iam_server, bool iam_client,
		std::string host, int port, int serverPort)
	{
		clPort = port;
		clServerPort = serverPort;
		clHost = iam_server ? "127.0.0.1" : host;
		clIam_server = iam_server;
		clIam_client = iam_client;
		IpPortIsSet = iam_server;

		// Start peer data port
		if (!StartPeerListen(clPort))
		{
#ifdef TEST_NETWORK_MESSAGES_PORTNET
			debug_net_printf("NetworkClass: peer listen on %d failed\n", clPort);
#endif
		}

		// Server: start control accept port
		if (clIam_server) {
			ctrlListenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			int opt = 1;
			setsockopt(ctrlListenSock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
			set_nonblocking(ctrlListenSock);
			sockaddr_in a{}; a.sin_family = AF_INET;
			a.sin_port = htons((unsigned short)clServerPort);
			a.sin_addr.s_addr = INADDR_ANY;
			if (bind(ctrlListenSock, (sockaddr*)&a, sizeof(a)) == 0)
				listen(ctrlListenSock, 16);
#ifdef TEST_NETWORK_MESSAGES_PORTNET
			else
				debug_net_printf("NetworkClass: ctrl listen on %d failed err=%d\n", clServerPort, sock_errno());
			debug_net_printf("NetworkClass: ctrl listen on port %d\n", clServerPort);
#endif
		}
	}

	NetworkClass::~NetworkClass()
	{
		StopNetworkThread();
		if (peerListenSock != SOCK_INVALID) { CLOSE_SOCKET(peerListenSock);   peerListenSock = SOCK_INVALID; }
		if (ctrlListenSock != SOCK_INVALID) { CLOSE_SOCKET(ctrlListenSock);   ctrlListenSock = SOCK_INVALID; }
		if (ctrlSock != SOCK_INVALID) { CLOSE_SOCKET(ctrlSock);         ctrlSock = SOCK_INVALID; }
	}

	void NetworkClass::StartNetworkThread()
	{
		if (netRunning.load()) return;
		netRunning.store(true);
		netThread = std::thread([this]() {
#ifdef TEST_NETWORK_MESSAGES_PORTNET
			debug_net_printf("NetThread: started\n");
#endif
			while (netRunning.load()) { locUpdateNetworkSingleThread(); std::this_thread::sleep_for(std::chrono::milliseconds(1)); }
#ifdef TEST_NETWORK_MESSAGES_PORTNET
			debug_net_printf("NetThread: stopped\n");
#endif
			});
	}

	void NetworkClass::StopNetworkThread()
	{
		if (!netRunning.load()) return;
		netRunning.store(false);
		if (netThread.joinable()) netThread.join();
	}

	// ---------------------------------------------------------------------------
	void NetworkClass::ConnectToServer()
	{
		if (ctrlSock != SOCK_INVALID) return;
		socket_t s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (s == SOCK_INVALID) return;
		set_nodelay(s);
		sockaddr_in a{}; a.sin_family = AF_INET;
		a.sin_port = htons((unsigned short)clServerPort);
		inet_pton(AF_INET, clHost.c_str(), &a.sin_addr);
		if (connect(s, (sockaddr*)&a, sizeof(a)) != 0) {
			int e = sock_errno();
			if (!would_block(e)) { CLOSE_SOCKET(s); return; }
		}
		set_nonblocking(s);
		{ std::lock_guard<std::mutex> lk(ctrlSendMtx); ctrlSock = s; }
		IpPortIsSet = true;
#ifdef TEST_NETWORK_MESSAGES_PORTNET
		debug_net_printf("ConnectToServer: connected to %s:%d\n", clHost.c_str(), clServerPort);
#endif
	}

	// ---------------------------------------------------------------------------
	void NetworkClass::AcceptCtrlClients()
	{
		if (ctrlListenSock == SOCK_INVALID) return;
		while (true) {
			sockaddr_in from{}; socklen_t fl = sizeof(from);
			socket_t s = accept(ctrlListenSock, (sockaddr*)&from, &fl);
			if (s == SOCK_INVALID) break;
			set_nonblocking(s); set_nodelay(s);
			char ip[INET_ADDRSTRLEN]; inet_ntop(AF_INET, &from.sin_addr, ip, sizeof(ip));
			ClientCtrl cc; cc.sock = s; cc.addr = ip; cc.port = ntohs(from.sin_port);
			std::lock_guard<std::recursive_mutex> lk(ctrlClientsMtx);
			ctrlClients.push_back(std::move(cc));
#ifdef TEST_NETWORK_MESSAGES_PORTNET
			debug_net_printf("AcceptCtrl: client %s:%d\n", ip, cc.port);
#endif
		}
	}

	// ---------------------------------------------------------------------------
	void NetworkClass::PollCtrlClients()
	{
		std::lock_guard<std::recursive_mutex> lk(ctrlClientsMtx);
		for (auto it = ctrlClients.begin(); it != ctrlClients.end(); ) {
			ClientCtrl& cc = *it;
			bool ok = cc.rbuf.Poll(cc.sock);
			for (auto& raw : cc.rbuf.ready) {
				message_info u = Unpack_Message(raw);
				if (debug_net) LogPkt("CTRL RX ←", cc.addr, cc.dataPort, u);
				// Intercept GET_IP to learn client's data port
				if (u.message == MESS_CLIENT_GET_IP) cc.dataPort = u.port;
				if (u.message == MESS_CLIENT_TESTADDNAME) cc.dataPort = u.port;
				HandleServerMsg(raw, cc.addr, cc.dataPort ? cc.dataPort : u.port);
			}
			cc.rbuf.ready.clear();
			if (!ok) {
#ifdef TEST_NETWORK_MESSAGES_PORTNET
				debug_net_printf("PollCtrlClients: client %s:%d disconnected\n", cc.addr.c_str(), cc.port);
#endif
				for (int ni = (int)NetworkName.size() - 1; ni >= 0; ni--) {
					if (clientIpPort[ni].adress == cc.addr && clientIpPort[ni].port == cc.dataPort) {
						RemoveListenName(NetworkName[ni]);
						RemoveNetworkName(NetworkName[ni]);
					}
				}
				CLOSE_SOCKET(cc.sock);
				it = ctrlClients.erase(it);
			}
			else { ++it; }
		}
	}

	// ---------------------------------------------------------------------------
	void NetworkClass::PollCtrlSocket()
	{
		if (ctrlSock == SOCK_INVALID) return;
		bool ok = ctrlRecvBuf.Poll(ctrlSock);
		for (auto& raw : ctrlRecvBuf.ready) {
			message_info u = Unpack_Message(raw);
			if (debug_net) LogPkt("CTRL RX ←", clHost, clServerPort, u);
			HandleClientMsg(raw);
		}
		ctrlRecvBuf.ready.clear();
		if (!ok) {
#ifdef TEST_NETWORK_MESSAGES_PORTNET
			debug_net_printf("PollCtrlSocket: server disconnected\n");
#endif
			std::lock_guard<std::mutex> lk(ctrlSendMtx);
			CLOSE_SOCKET(ctrlSock); ctrlSock = SOCK_INVALID;
			IpPortIsSet = false;
		}
	}

	// ---------------------------------------------------------------------------
	// Accept incoming peer data connections (both roles)
	// ---------------------------------------------------------------------------
	void NetworkClass::AcceptPeerConnections()
	{
		if (peerListenSock == SOCK_INVALID) return;
		while (true) {
			sockaddr_in from{}; socklen_t fl = sizeof(from);
			socket_t s = accept(peerListenSock, (sockaddr*)&from, &fl);
			if (s == SOCK_INVALID) break;
			set_nonblocking(s); set_nodelay(s);
			char ip[INET_ADDRSTRLEN]; inet_ntop(AF_INET, &from.sin_addr, ip, sizeof(ip));
			int callerClPort = ntohs(from.sin_port); // fallback na ephemeral
			for (int i = 0; i < (int)NetworkName.size(); i++)
				if (clientIpPort[i].adress == std::string(ip)) { callerClPort = clientIpPort[i].port; break; }
#ifdef TEST_NETWORK_MESSAGES_PORTNET
			debug_net_printf("AcceptPeer: data connection from %s\n", ip);
#endif

			// Find the LISTEN NCB that is waiting for an incoming data connection.
			// setListen() (called from PollCtrlSocket → HandleClientMsg a few lines above
			// in locUpdateNetworkSingleThread) sets ncb_lsn_2 = 20 before we get here,
			// so this lookup should always succeed when the LISTEN_ACCEPT and the TCP
			// connect() from the caller arrive in the same tick (true on loopback).
			// If for some reason it hasn't run yet, close this socket and let the
			// caller reconnect on the next tick.
			myNCB* listenNCB = nullptr;
			{
				std::lock_guard<std::mutex> lk(clientConnMutex);
				for (auto* c : clientConnection) {
					if (c->ncb_lsn_2 == 0) continue;
					bool hasAlive = false;
					{
						std::lock_guard<std::mutex> slk(sessions_mutex);
						auto it = tcpSessions.find(c);
						hasAlive = (it != tcpSessions.end() && it->second->alive);
					}
					if (!hasAlive) { listenNCB = c; break; }
				}
			}

			if (!listenNCB) {
				// setListen hasn't run yet for this NCB — reject and let caller retry
#ifdef TEST_NETWORK_MESSAGES_PORTNET
				debug_net_printf("AcceptPeer: WARNING no listenNCB ready, closing incoming connection from %s (will retry)\n", ip);
#endif
				CLOSE_SOCKET(s);
				continue;
			}

			auto sess = std::make_shared<TcpSession>();
			sess->sock = s;
			sess->peerAddr = std::string(ip);
			sess->peerPort = callerClPort;
			sess->alive = true;
			sess->ownerNCB = listenNCB;
			sess->Touch();
			AddSession(listenNCB, sess);
			listenNCB->ncb_cmd_cplt_49 = NRC_GOODRET;
#ifdef TEST_NETWORK_MESSAGES_PORTNET
			debug_net_printf("AcceptPeer: session listenNCB=%p name=[%.16s] call=[%.16s]\n", (void*)listenNCB, listenNCB->ncb_name_26, listenNCB->ncb_callName_10);
#endif
		}
	}

	// ---------------------------------------------------------------------------
	// Poll all data sessions
	// ---------------------------------------------------------------------------
	void NetworkClass::PollSessions()
	{
		std::vector<std::shared_ptr<TcpSession>> snaps;
		{
			std::lock_guard<std::mutex> lk(sessions_mutex);
			for (auto& kv : tcpSessions) snaps.push_back(kv.second);
		}

		for (auto& sess : snaps) {
			if (!sess->alive) continue;

			bool ok = sess->rbuf.Poll(sess->sock);
			for (auto& raw : sess->rbuf.ready) {
				sess->Touch();
				message_info u = Unpack_Message(raw);
				if (debug_net) LogPkt("DATA RX ←", sess->peerAddr, sess->peerPort, u);

				if (u.message == MESS_DIRECT_SEND) {
					std::lock_guard<std::mutex> qlk(sess->queueMtx);
					sess->dataQueue.push(raw);
#ifdef TEST_NETWORK_MESSAGES_PORTNET
					debug_net_printf("PollSessions: queued DIRECT_SEND size=%u (queue=%zu)\n", u.size, sess->dataQueue.size());
#endif
				}
				else if (u.message == MESS_HEARTBEAT) {
					shadow_myNCB n{}; n.ncb_command_0 = 0xFE;
					sess->Send(Pack_Message(MESS_HEARTBEAT_ACK, n, 0, clPort));
#ifdef TEST_NETWORK_MESSAGES_PORTNET
					debug_net_printf("PollSessions: HEARTBEAT → ACK\n");
#endif
				}
				else if (u.message == MESS_HEARTBEAT_ACK) {
#ifdef TEST_NETWORK_MESSAGES_PORTNET
					debug_net_printf("PollSessions: HEARTBEAT_ACK\n");
#endif
				}
			}
			sess->rbuf.ready.clear();

			if (!ok) {
#ifdef TEST_NETWORK_MESSAGES_PORTNET
				debug_net_printf("PollSessions: peer %s:%d disconnected\n", sess->peerAddr.c_str(), sess->peerPort);
#endif
				sess->alive = false;
				if (sess->ownerNCB) {
					sess->ownerNCB->ncb_retcode_1 = NRC_SCLOSED;
					sess->ownerNCB->ncb_cmd_cplt_49 = NRC_SCLOSED;
				}
				continue;
			}

			// Heartbeat
			long silence = sess->SilenceMs();
			if (silence >= HEARTBEAT_TIMEOUT_MS) {
#ifdef TEST_NETWORK_MESSAGES_PORTNET
				debug_net_printf("PollSessions: timeout %ldms peer=%s:%d\n", silence, sess->peerAddr.c_str(), sess->peerPort);
#endif
				sess->alive = false;
				if (sess->ownerNCB) {
					sess->ownerNCB->ncb_retcode_1 = NRC_SCLOSED;
					sess->ownerNCB->ncb_cmd_cplt_49 = NRC_SCLOSED;
				}
				sess->Close();
			}
			else if (silence >= HEARTBEAT_PROBE_MS && !sess->probePending) {
				long pSil = (long)std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::steady_clock::now() - sess->lastProbeSent).count();
				if (pSil >= HEARTBEAT_PROBE_MS) {
					sess->probePending = true;
					sess->lastProbeSent = std::chrono::steady_clock::now();
					shadow_myNCB n{}; n.ncb_command_0 = 0xFE;
					sess->Send(Pack_Message(MESS_HEARTBEAT, n, 0, clPort));
#ifdef TEST_NETWORK_MESSAGES_PORTNET
					debug_net_printf("PollSessions: HEARTBEAT probe silence=%ldms\n", silence);
#endif
				}
			}
		}
	}

	// ---------------------------------------------------------------------------
	bool NetworkClass::ConnectDataToPeer(myNCB* ncb, const std::string& addr, int dataPort)
	{
		socket_t s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (s == SOCK_INVALID) return false;
		set_nodelay(s);
		sockaddr_in a{}; a.sin_family = AF_INET;
		a.sin_port = htons((unsigned short)dataPort);
		inet_pton(AF_INET, addr.c_str(), &a.sin_addr);
#ifdef TEST_NETWORK_MESSAGES_PORTNET
		debug_net_printf("ConnectDataToPeer: %s:%d for ncb=[%.16s]\n", addr.c_str(), dataPort, ncb->ncb_name_26);
#endif
		if (connect(s, (sockaddr*)&a, sizeof(a)) != 0) {
			int e = sock_errno();
			if (!would_block(e))
			{
				CLOSE_SOCKET(s);
#ifdef TEST_NETWORK_MESSAGES_PORTNET
				debug_net_printf("ConnectDataToPeer: failed err=%d\n", e);
#endif
				return false;
			}
		}
		set_nonblocking(s);
		auto sess = std::make_shared<TcpSession>();
		sess->sock = s;
		sess->peerAddr = addr;
		sess->peerPort = dataPort;
		sess->alive = true;
		sess->ownerNCB = ncb;
		sess->Touch();
		AddSession(ncb, sess);
		return true;
	}

	// ---------------------------------------------------------------------------
	// Server message handler
	// ---------------------------------------------------------------------------
	void NetworkClass::HandleServerMsg(const std::string& raw,
		const std::string& senderAddr,
		int                senderDataPort)
	{
		message_info u = Unpack_Message(raw);

		if (u.message == MESS_CLIENT_GET_IP) {
			shadow_myNCB n{}; n.ncb_command_0 = 0xFE;
			std::string reply = Pack_Message(MESS_SERVER_GIVE_IP, n, -1, u.port,
				senderAddr.c_str(), (int)senderAddr.size() + 1);
			SendToCtrlClient(reply, senderAddr, u.port);
			return;
		}

		if (u.message == MESS_CLIENT_TESTADDNAME) {
			TypeIpPort ip{ senderAddr, u.port };
			shadow_myNCB n{}; n.ncb_command_0 = 0xFE;
			if (GetNameNetwork(u.data).empty()) {
				AddNetworkName(u.data, ip);
				SendToCtrlClient(Pack_Message(MESS_SERVER_TESTADDNAME_OK, n, u.index, -10), senderAddr, senderDataPort);
				char cmp[16] = { 0 };
				sprintf(cmp, "NETH2%c0", u.data[5]);
				while (strlen(cmp) < 15) strcat(cmp, " ");
				if (memcmp(u.data, cmp, 15) == 0) serverAddname = true;
			}
			else if (ExistNetworkName(u.data, ip)) {
				SendToCtrlClient(Pack_Message(MESS_SERVER_TESTADDNAME_OK, n, u.index, -10), senderAddr, senderDataPort);
			}
			else {
				SendToCtrlClient(Pack_Message(MESS_SERVER_TESTADDNAME_REJECT, n, u.index, -10), senderAddr, senderDataPort);
			}
		}
		else if (u.message == MESS_CLIENT_MESSAGE_LISTEN) {
			AddListenName((myNCB*)u.data);
		}
		else if (u.message == MESS_CLIENT_MESSAGE_CALL) {
			TypeIpPort callIP = GetIpPortFromName(u.messNCB.ncb_callName_10);
			if (AddListenName2(&u.messNCB)) {
				// Tell caller: listener IP + data port
				// If listener registered via loopback, use the server's real LAN IP as seen by caller
				std::string listenerIp = callIP.adress;
				if (listenerIp == "127.0.0.1" || listenerIp == "::1") {
					for (auto& cc2 : ctrlClients) {
						if (cc2.addr == senderAddr && cc2.dataPort == senderDataPort) {
							sockaddr_in la{}; socklen_t ll = sizeof(la);
							if (getsockname(cc2.sock, (sockaddr*)&la, &ll) == 0) {
								char rip[INET_ADDRSTRLEN];
								inet_ntop(AF_INET, &la.sin_addr, rip, sizeof(rip));
								if (strcmp(rip, "0.0.0.0") != 0) listenerIp = rip;
							}
							break;
						}
					}
				}
				struct CallAcceptInfo { char ip[64]; int dataPort; };
				CallAcceptInfo ci{}; strncpy(ci.ip, listenerIp.c_str(), 63); ci.dataPort = callIP.port;
				SendToCtrlClient(Pack_Message(MESS_SERVER_CALL_ACCEPT, u.messNCB, u.index, -10,
					(char*)&ci, sizeof(ci)),
					senderAddr, senderDataPort);

				// Tell listener: caller IP + data port
				struct ListenAcceptInfo { char ncb_data[sizeof(myNCB)]; char ip[64]; int dataPort; };
				ListenAcceptInfo li{};
				memcpy(li.ncb_data, u.data, u.size < (int)sizeof(li.ncb_data) ? u.size : sizeof(li.ncb_data));
				strncpy(li.ip, senderAddr.c_str(), 63);
				li.dataPort = u.port; // caller's clPort

				// Route LISTEN_ACCEPT to the listener's control socket
				// (ctrlClientsMtx already held recursively by PollCtrlClients caller)
				std::lock_guard<std::recursive_mutex> lk(ctrlClientsMtx);
				for (auto& cc : ctrlClients) {
					if (cc.addr == callIP.adress && cc.dataPort == callIP.port) {
						TcpSendFull(cc.sock,
							Pack_Message(MESS_SERVER_LISTEN_ACCEPT, u.messNCB, u.index,
								clServerPort, (char*)&li, sizeof(li)));
						break;
					}
				}
			}
			else {
				shadow_myNCB n{}; n.ncb_command_0 = 0xFE;
				SendToCtrlClient(Pack_Message(MESS_SERVER_CALL_REJECT, u.messNCB, u.index, -10), senderAddr, senderDataPort);
			}
		}
		else if (u.message == MESS_CLIENT_CANCEL) {
			if (u.data[0] != 0) RemoveListenName(u.data);
		}
		else if (u.message == MESS_CLIENT_DELETE) {
			CleanMessages(myNCBfromShadow(u.messNCB));
			RemoveNetworkName(u.data);
		}
	}

	// ---------------------------------------------------------------------------
	// Client message handler (replies from the server)
	// ---------------------------------------------------------------------------
	void NetworkClass::HandleClientMsg(const std::string& raw)
	{
		message_info u = Unpack_Message(raw);

		if (u.message == MESS_SERVER_GIVE_IP) {
			IpPortIsSet = true;
#ifdef TEST_NETWORK_MESSAGES_PORTNET
			debug_net_printf("HandleClient: GIVE_IP — confirmed\n");
#endif
			return;
		}

		if (u.message == MESS_SERVER_TESTADDNAME_OK) {
			std::lock_guard<std::mutex> lk(connections_mutex);
			connectionTime* ct = GetConnection(u.index);
			if (ct) ct->state = NETI_ADD_NAME_OK;
		}
		else if (u.message == MESS_SERVER_TESTADDNAME_REJECT) {
			std::lock_guard<std::mutex> lk(connections_mutex);
			connectionTime* ct = GetConnection(u.index);
			if (ct) ct->state = NETI_ADD_NAME_REJECT;
		}
		else if (u.message == MESS_SERVER_CALL_ACCEPT) {
			std::lock_guard<std::mutex> lk(connections_mutex);
			connectionTime* ct = GetConnection(u.index);
			if (ct) {
				ct->state = NETI_CALL_ACCEPT;
				ct->connection->ncb_retcode_1 = NRC_GOODRET;
				ct->connection->ncb_cmd_cplt_49 = NRC_GOODRET;
#ifdef TEST_NETWORK_MESSAGES_PORTNET
				debug_net_printf("HandleClient: CALL_ACCEPT idx=%d name=[%.16s]\n", u.index, ct->connection->ncb_name_26);
#endif
				struct { char ip[64]; int dataPort; } ci{};
				if (u.size >= (int)sizeof(ci)) {
					memcpy(&ci, u.data, sizeof(ci));
					if (ci.ip[0] && ci.dataPort > 0) {
						std::string key(u.messNCB.ncb_callName_10, 16);
						directPeers[key] = { std::string(ci.ip), ci.dataPort };
#ifdef TEST_NETWORK_MESSAGES_PORTNET
						debug_net_printf("HandleClient: CALL_ACCEPT peer=[%.16s] %s:%d\n", u.messNCB.ncb_callName_10, ci.ip, ci.dataPort);
#endif
						ConnectDataToPeer(ct->connection, std::string(ci.ip), ci.dataPort);
						// Register CALL NCB in clientConnection so Pass4 tracks
						// liveness the same way as LISTEN NCBs.
						// ncb_lsn_2=20 is required for Pass4 to consider it established.
						ct->connection->ncb_lsn_2 = 20;
						setListenConnection(ct->connection);
					}
				}
			}
		}
		else if (u.message == MESS_SERVER_CALL_REJECT) {
			std::lock_guard<std::mutex> lk(connections_mutex);
			connectionTime* ct = GetConnection(u.index);
			if (ct) ct->state = NETI_CALL_REJECT;
		}
		else if (u.message == MESS_SERVER_LISTEN_ACCEPT) {
			// tmp.ncb_name_26 must hold the CALLER's name so setListen can match it
			// against the LISTEN NCB's ncb_callName_10 (= who we were waiting for).
			// u.messNCB.ncb_name_26 IS the caller's name ("NETH201").
			// u.messNCB.ncb_callName_10 is the LISTENER's name ("NETH200") — wrong field.
			myNCB tmp; memset(&tmp, 0, sizeof(tmp));
			memcpy(tmp.ncb_name_26, u.messNCB.ncb_name_26, 16);  // caller's name
#ifdef TEST_NETWORK_MESSAGES_PORTNET
			debug_net_printf("HandleClient: LISTEN_ACCEPT callerName=[%.16s] listenerName=[%.16s]\n", u.messNCB.ncb_name_26, u.messNCB.ncb_callName_10);
#endif
			setListen(&tmp);

			// Record caller's data port
			struct { char ncb_data[sizeof(myNCB)]; char ip[64]; int dataPort; } li{};
			if (u.size >= (int)sizeof(li)) {
				memcpy(&li, u.data, sizeof(li));
				if (li.ip[0] && li.dataPort > 0) {
					std::string key(u.messNCB.ncb_name_26, 16);
					directPeers[key] = { std::string(li.ip), li.dataPort };
#ifdef TEST_NETWORK_MESSAGES_PORTNET
					debug_net_printf("HandleClient: LISTEN_ACCEPT caller=%s:%d\n", li.ip, li.dataPort);
#endif
					// Incoming TCP connection accepted by AcceptPeerConnections()
				}
			}
		}
		else if (u.message == MESS_SERVER_LISTEN_REJECT) {
			// Nothing; LISTEN keeps waiting
		}
	}

	// ---------------------------------------------------------------------------
	void NetworkClass::locUpdateNetworkSingleThread()
	{
		if (clIam_client && ctrlSock == SOCK_INVALID) ConnectToServer();
		if (clIam_server) { AcceptCtrlClients(); PollCtrlClients(); }
		if (clIam_client) PollCtrlSocket();
		AcceptPeerConnections();
		PollSessions();
		if (clIam_client) UpdateClient();
	}

	// ---------------------------------------------------------------------------
	// UpdateClient — NetBIOS NCB state machine
	// ---------------------------------------------------------------------------
	void NetworkClass::UpdateClient()
	{
		if (!IpPortIsSet) {
			shadow_myNCB n{}; n.ncb_command_0 = 0xFE;
			SendCtrl(Pack_Message(MESS_CLIENT_GET_IP, n, GetNextIndex(), clPort));
			return;
		}

		std::lock_guard<std::mutex> conn_lk(connections_mutex);
		std::vector<int32_t> toDelete;
		clock_t now = clock();

		// ------------------------------------------------------------------
		// Pass 1 — deliver RECEIVE operations from session data queues.
		// A RECEIVE NCB is matched to a session by comparing ncb_name_26
		// (the local NetBIOS name) with the session ownerNCB->ncb_name_26.
		// This correctly routes incoming data to the right side (NETH200 or
		// NETH201) without any size-matching magic.
		// ------------------------------------------------------------------
		for (connectionTime& conn : handleConnections)
		{
			if (conn.connection->ncb_command_0 != 0x95) continue;
			if (conn.connection->ncb_cmd_cplt_49 != NRC_PENDING) continue;
			if (!conn.connection->ncb_buffer_4.p) continue;

			// Find a session whose owner has the same local name
			std::shared_ptr<TcpSession> sess;
			{
				std::lock_guard<std::mutex> lk(sessions_mutex);
				for (auto& kv : tcpSessions) {
					if (!kv.second->alive || !kv.second->ownerNCB) continue;
					if (memcmp(kv.second->ownerNCB->ncb_name_26,
						conn.connection->ncb_name_26, 16) == 0)
					{
						sess = kv.second; break;
					}
				}
			}
			if (!sess) {
#ifdef TEST_NETWORK_MESSAGES_PORTNET
				debug_net_printf("Pass1: RECEIVE idx=%d no session name=[%.16s]\n", conn.index, conn.connection->ncb_name_26);
#endif
				continue;
			}

			std::string raw;
			{
				std::lock_guard<std::mutex> qlk(sess->queueMtx);
				if (sess->dataQueue.empty()) continue;
				raw = sess->dataQueue.front();
				sess->dataQueue.pop();
			}

			message_info u = Unpack_Message(raw);
			if (u.size > conn.connection->ncb_bufferLength_8) {
#ifdef TEST_NETWORK_MESSAGES_PORTNET
				debug_net_printf("Pass1: RECEIVE idx=%d size=%u > buf=%u DROP\n", conn.index, u.size, conn.connection->ncb_bufferLength_8);
#endif
				continue;
			}
			memcpy(conn.connection->ncb_buffer_4.p, u.data, u.size);
			conn.connection->ncb_bufferLength_8 = (uint16_t)u.size;
			conn.connection->ncb_retcode_1 = NRC_GOODRET;
			conn.connection->ncb_cmd_cplt_49 = NRC_GOODRET;
			toDelete.push_back(conn.index);
#ifdef TEST_NETWORK_MESSAGES_PORTNET
			debug_net_printf("Pass1: RECEIVE idx=%d DELIVERED size=%u from=[%.16s] to=[%.16s]\n", conn.index, u.size, u.messNCB.ncb_name_26, u.messNCB.ncb_callName_10);
#endif
		}

		// ------------------------------------------------------------------
		// Pass 2 — timeout / retry per NCB command
		// ------------------------------------------------------------------
		for (connectionTime& conn : handleConnections)
		{
			if (conn.connection->ncb_cmd_cplt_49 != NRC_PENDING &&
				conn.connection->ncb_command_0 != 0x91 &&
				conn.connection->ncb_command_0 != 0x95) continue;

			long elapsed = (long)((now - conn.startTime) * 1000 / CLOCKS_PER_SEC);
			if (elapsed < conn.timeout) continue;

			bool del = true;
			switch (conn.connection->ncb_command_0)
			{
			case 0x35: // CANCEL
				conn.connection->ncb_retcode_1 = NRC_GOODRET;
				conn.connection->ncb_cmd_cplt_49 = NRC_GOODRET;
				break;
			case 0x7F: // INIT
				conn.connection->ncb_cmd_cplt_49 = NRC_GOODRET;
				break;
			case 0x90: // CALL
				if (conn.state == NETI_CALL_ACCEPT) {
					conn.connection->ncb_retcode_1 = NRC_GOODRET;
					conn.connection->ncb_cmd_cplt_49 = NRC_GOODRET;
				}
				else if (conn.state == NETI_CALL_REJECT || conn.retryCount >= MAX_RETRIES) {
					conn.connection->ncb_cmd_cplt_49 = NRC_SCLOSED;
				}
				else {
					conn.startTime = now; conn.retryCount++; del = false;
					CallNetwork(conn.connection, conn.index);
				}
				break;
			case 0x91: // LISTEN — stays pending
				del = false;
				break;
			case 0x92: // HANG_UP
				conn.connection->ncb_lsn_2 = 0;
				conn.connection->ncb_cmd_cplt_49 = NRC_GOODRET;
				RemoveSession(conn.connection);
				deleteListenConnection(conn.connection);
				break;
			case 0x94: // SEND — complete immediately (TCP send already done)
				conn.connection->ncb_retcode_1 = NRC_GOODRET;
				conn.connection->ncb_cmd_cplt_49 = NRC_GOODRET;
				break;
			case 0x95: // RECEIVE — never times out
				del = false;
				break;
			case 0xb0: // ADD_NAME
				if (conn.state == NETI_ADD_NAME_OK) {
					conn.connection->ncb_retcode_1 = NRC_GOODRET;
					conn.connection->ncb_cmd_cplt_49 = NRC_GOODRET;
				}
				else if (conn.state == NETI_ADD_NAME_REJECT) {
					conn.connection->ncb_retcode_1 = NRC_INUSE;
					conn.connection->ncb_cmd_cplt_49 = NRC_INUSE;
				}
				else {
					conn.startTime = now; del = false;
					AddName(conn.connection, conn.index);
				}
				break;
			case 0xb1: // DELETE_NAME
				conn.connection->ncb_cmd_cplt_49 = NRC_GOODRET;
				break;
			default:
				conn.connection->ncb_cmd_cplt_49 = NRC_GOODRET;
				break;
			}
			if (del) toDelete.push_back(conn.index);
		}

		for (int32_t idx : toDelete)
			for (auto it = handleConnections.begin(); it != handleConnections.end(); )
				it = (it->index == idx) ? handleConnections.erase(it) : ++it;

		// ------------------------------------------------------------------
		// Pass 4 — session liveness → ncb_cmd_cplt_49
		// ------------------------------------------------------------------
		std::vector<myNCB*> connSnap;
		{ std::lock_guard<std::mutex> clk(clientConnMutex); connSnap = clientConnection; }
		for (auto* c : clientConnection)
		{
			if (c->ncb_lsn_2 == 0 || c->ncb_cmd_cplt_49 == NRC_PENDING) continue;
			auto sess = GetSession(c);
			bool alive = sess && sess->alive && sess->SilenceMs() < HEARTBEAT_TIMEOUT_MS;
			uint8_t old = c->ncb_cmd_cplt_49;
			if (alive) {
				c->ncb_cmd_cplt_49 = NRC_GOODRET;
			}
			else if (c->ncb_cmd_cplt_49 != NRC_SCLOSED) {
				c->ncb_cmd_cplt_49 = NRC_SCLOSED;
				c->ncb_retcode_1 = NRC_SCLOSED;
#ifdef TEST_NETWORK_MESSAGES_PORTNET
				debug_net_printf("Pass4: CLOSED name=[%.16s] lsn=%d\n", c->ncb_name_26, c->ncb_lsn_2);
#endif
			}
#ifdef TEST_NETWORK_MESSAGES_PORTNET
			debug_net_printf("Pass4: lsn=%d name=[%.16s] alive=%d cplt %02X→%02X\n", c->ncb_lsn_2, c->ncb_name_26, (int)alive, old, c->ncb_cmd_cplt_49);
#endif
		}
	}

	// ---------------------------------------------------------------------------
	// Public NCB dispatchers
	// ---------------------------------------------------------------------------
	void NetworkClass::AddName(myNCB* c, int32_t index)
	{
		SendCtrl(Pack_Message(MESS_CLIENT_TESTADDNAME, myNCBtoShadow(*c), index, clPort,
			c->ncb_name_26, sizeof(c->ncb_name_26)));
	}

	void NetworkClass::CancelNetwork(myNCB* c, int32_t index)
	{
		SendCtrl(Pack_Message(MESS_CLIENT_CANCEL, myNCBtoShadow(*c), index, clPort,
			c->ncb_name_26, sizeof(c->ncb_name_26)));
		deleteListenConnection(c); // removes from clientConnection (LISTEN or CALL NCB)
		RemoveSession(c);          // closes TCP data session
	}

	void NetworkClass::DeleteNetwork(myNCB* c, int32_t index)
	{
		SendCtrl(Pack_Message(MESS_CLIENT_DELETE, myNCBtoShadow(*c), index, clPort,
			c->ncb_name_26, sizeof(c->ncb_name_26)));
		locUpdateNetworkSingleThread();
		CleanMessages(*c);
	}

	void NetworkClass::CallNetwork(myNCB* c, int32_t index)
	{
		myNCB safe = *c; safe.ncb_buffer_4.p = nullptr;
		SendCtrl(Pack_Message(MESS_CLIENT_MESSAGE_CALL, myNCBtoShadow(*c), index, clPort,
			(char*)&safe, sizeof(safe)));
	}

	void NetworkClass::ListenNetwork(myNCB* c, int32_t index)
	{
#ifdef TEST_NETWORK_MESSAGES_PORTNET
		debug_net_printf("ListenNetwork: name=[%.16s] call=[%.16s]\n", c->ncb_name_26, c->ncb_callName_10);
#endif
		myNCB safe = *c; safe.ncb_buffer_4.p = nullptr;
		SendCtrl(Pack_Message(MESS_CLIENT_MESSAGE_LISTEN, myNCBtoShadow(*c), index, clPort,
			(char*)&safe, sizeof(safe)));
		setListenConnection(c);
	}

	void NetworkClass::SendNetwork(myNCB* c, int32_t index)
	{
		assert(c->ncb_command_0 == 0x94);

		// Find session by matching sender's ncb_name_26 to ownerNCB->ncb_name_26
		std::shared_ptr<TcpSession> sess;
		{
			std::lock_guard<std::mutex> lk(sessions_mutex);
			for (auto& kv : tcpSessions) {
				if (!kv.second->alive || !kv.second->ownerNCB) continue;
				if (memcmp(kv.second->ownerNCB->ncb_name_26, c->ncb_name_26, 16) == 0)
				{
					sess = kv.second; break;
				}
			}
		}

		if (!sess) {
#ifdef TEST_NETWORK_MESSAGES_PORTNET
			debug_net_printf("SendNetwork: no session for name=[%.16s]\n", c->ncb_name_26);
#endif
			c->ncb_cmd_cplt_49 = NRC_SCLOSED;
			return;
		}

		std::string packed = Pack_Message(MESS_DIRECT_SEND, myNCBtoShadow(*c), index, clPort,
			(char*)c->ncb_buffer_4.p, c->ncb_bufferLength_8);
		if (!sess->Send(packed)) {
#ifdef TEST_NETWORK_MESSAGES_PORTNET
			debug_net_printf("SendNetwork: TCP send failed for name=[%.16s]\n", c->ncb_name_26);
#endif
			sess->alive = false;
			c->ncb_cmd_cplt_49 = NRC_SCLOSED;
			return;
		}
		// TCP is reliable — mark SEND complete immediately
		c->ncb_retcode_1 = NRC_GOODRET;
		c->ncb_cmd_cplt_49 = NRC_GOODRET;
#ifdef TEST_NETWORK_MESSAGES_PORTNET
		debug_net_printf("SendNetwork: sent size=%u name=[%.16s]\n", c->ncb_bufferLength_8, c->ncb_name_26);
#endif
	}

} // namespace MyNetworkLib

// ---------------------------------------------------------------------------
// Global instance
// ---------------------------------------------------------------------------
MyNetworkLib::NetworkClass* locNetworkClass = nullptr;
std::string                 IHaveNameStrP = "";

// ---------------------------------------------------------------------------
// simulateInterupt — called by setNetbios_75044() in Network.cpp
// ---------------------------------------------------------------------------
int lastnetworkname = 0;

void simulateInterupt(myNCB* connection)
{
	int       locTimeout = 0;
	int32_t   locIndex = GetNextIndex();
	Neti_type state = NETI_NOT_SET;

	switch (connection->ncb_command_0)
	{
	case 0x35:  // CANCEL
		locTimeout = 10000;
		{
			myNCB* second = (myNCB*)connection->ncb_buffer_4.p;
			if (second) { second->ncb_retcode_1 = NRC_CMDCAN; second->ncb_cmd_cplt_49 = NRC_CMDCAN; }
		}
		connection->ncb_retcode_1 = NRC_GOODRET;
		connection->ncb_lsn_2 = 0;
		connection->ncb_cmd_cplt_49 = NRC_GOODRET;
		locNetworkClass->CancelNetwork(connection, locIndex);
		break;
	case 0x7F:  // INIT
		locTimeout = 200;
		connection->ncb_retcode_1 = 0x03;
		connection->ncb_cmd_cplt_49 = 0x03;
		break;
	case 0x90:  // CALL
		locTimeout = 1000;
		connection->ncb_retcode_1 = NRC_PENDING;
		connection->ncb_lsn_2 = 0xE8;
		connection->ncb_cmd_cplt_49 = NRC_PENDING;
		locNetworkClass->CallNetwork(connection, locIndex);
		state = NETI_CALL;
		break;
	case 0x91:  // LISTEN
		locTimeout = 200;
		connection->ncb_retcode_1 = NRC_PENDING;
		connection->ncb_cmd_cplt_49 = NRC_PENDING;
		locNetworkClass->ListenNetwork(connection, locIndex);
		state = NETI_LISTEN;
		break;
	case 0x92:  // HANG_UP
		locTimeout = 500;
		connection->ncb_cmd_cplt_49 = NRC_PENDING;
		break;
	case 0x94:  // SEND
		locTimeout = 5000;
		connection->ncb_retcode_1 = NRC_PENDING;
		connection->ncb_cmd_cplt_49 = NRC_PENDING;
		locNetworkClass->SendNetwork(connection, locIndex);
		break;
	case 0x95:  // RECEIVE
		locTimeout = 10000;
		connection->ncb_retcode_1 = NRC_PENDING;
		connection->ncb_cmd_cplt_49 = NRC_PENDING;
		state = NETI_RECEIVE;
		break;
	case 0xb0:  // ADD_NAME
		locTimeout = 1000;
		connection->ncb_retcode_1 = NRC_PENDING;
		connection->ncb_num_3 = lastnetworkname + 0x02;
		lastnetworkname++;
		connection->ncb_cmd_cplt_49 = NRC_PENDING;
		locNetworkClass->AddName(connection, locIndex);
		state = NETI_ADD_NAME;
		break;
	case 0xb1:  // DELETE_NAME
		locTimeout = 10000;
		connection->ncb_retcode_1 = NRC_PENDING;
		locNetworkClass->DeleteNetwork(connection, locIndex);
		break;
	}

	connectionTime lc;
	lc.connection = connection;
	lc.timeout = locTimeout;
	lc.startTime = clock();
	lc.index = locIndex;
	lc.state = state;
	lc.retryCount = 0;
	{ std::lock_guard<std::mutex> lk(connections_mutex); handleConnections.push_back(lc); }
}

// ---------------------------------------------------------------------------
// Debug helpers  (unchanged API)
// ---------------------------------------------------------------------------
void printState(myNCB** connections) {
#ifdef TEST_NETWORK_MESSAGES_NETWORK
	for (int i = 0; i < 3; i++)
		debug_net_printf("NetworkGetState: %d %p lsn=%d cplt=%s\n",
			i, connections[i], connections[i]->ncb_lsn_2,
			(!connections[i]->ncb_cmd_cplt_49) ? "ok" : "pending");
#endif
}
void printState2(char* text) {
#ifdef TEST_NETWORK_MESSAGES_NETWORK
	debug_net_printf("%s", text);
#endif
}
void timeState(bool start, const char* text) {
#ifdef TEST_NETWORK_MESSAGES_NETWORK
	/*
	if (start || (timest_index == 0)) timest_timer = clock();
	char buff[100];
	snprintf(buff, sizeof(buff), "%s | %d", text, (int)(clock() - timest_timer));
	timest_buffer[timest_index].assign(buff, strlen(buff));
	timest_index++;
	if (timest_index > timest_max_mess) {
		ofstream ofs("net_time_messages_log.txt", std::ofstream::out);
		for (int i = 0; i < timest_max_mess; i++)
			ofs << timest_buffer[i] << endl;
		ofs.close();
		exit(0);
	}*/
#endif
}

// ---------------------------------------------------------------------------
// Init / shutdown  (unchanged API)
// ---------------------------------------------------------------------------
void InitMyNetLib(bool iam_server, bool iam_client,
	char* ip, int networkPort, int serverPort)
{
#ifdef _WIN32
	WSADATA wsa; WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
	locNetworkClass = new MyNetworkLib::NetworkClass(
		iam_server, iam_client, ip, networkPort, serverPort);
	locNetworkClass->StartNetworkThread();
}

void EndMyNetLib()
{
	if (locNetworkClass) {
		locNetworkClass->StopNetworkThread();
		delete locNetworkClass;
		locNetworkClass = nullptr;
	}
#ifdef _WIN32
	WSACleanup();
#endif
}