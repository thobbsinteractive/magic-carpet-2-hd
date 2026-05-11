#define _CRT_SECURE_NO_WARNINGS
#include "port_net.h"
#include <thread>
#include <cassert>
#include <vector>
#include <mutex>
#include <deque>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET socket_t;
#define CLOSE_SOCKET closesocket
#define SOCK_INVALID INVALID_SOCKET
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
typedef int socket_t;
#define CLOSE_SOCKET close
#define SOCK_INVALID (-1)
#endif

uint8_t* Zero_pointer_net = nullptr;

int timest_index = 0;
clock_t timest_timer = 0;
const int timest_max_mess = 400;
std::string timest_buffer[timest_max_mess];

//#define TEST_NETWORK_MESSAGES
//#define TEST_BAD_NETWORK
//#define TEST_TIME_NETWORK_MESSAGES

using namespace std;

typedef struct {
	myNCB* connection;
	long startTime;
	int timeout;
	int32_t index;
} connectionTime;
std::vector<connectionTime> handleConnections;

enum Neti_type {
	NETI_NOT_SET,
	NETI_ADD_NAME,
	NETI_ADD_NAME_REJECT,
	NETI_ADD_NAME_OK,
	NETI_CALL,
	NETI_CALL_ACCEPT,
	NETI_CALL_REJECT,
	NETI_LISTEN,
	NETI_LISTEN_REJECT,
	NETI_RECEIVE
};

Neti_type netstate_shared = NETI_NOT_SET;

const int32_t MESS_UNKNOWN = -1;
const int32_t MESS_CLIENT_SERVER_NAME_ADDED = 0;
const int32_t MESS_SERVER_SERVER_NAME_ADDED = 1;
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
const int32_t MESS_CLIENT_SEND = 15;
const int32_t MESS_SERVER_SEND_OK = 16;
const int32_t MESS_SERVER_SEND = 17;
const int32_t MESS_CLIENT_GET_IP = 18;
const int32_t MESS_SERVER_GIVE_IP = 19;

char* MessageIndexToText(int32_t index)
{
	switch (index) {
	case MESS_CLIENT_SERVER_NAME_ADDED:  return (char*)"MESS_CLIENT_SERVER_NAME_ADDED";
	case MESS_SERVER_SERVER_NAME_ADDED:  return (char*)"MESS_SERVER_SERVER_NAME_ADDED";
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
	case MESS_CLIENT_SEND:               return (char*)"MESS_CLIENT_SEND";
	case MESS_SERVER_SEND_OK:            return (char*)"MESS_SERVER_SEND_OK";
	case MESS_SERVER_SEND:               return (char*)"MESS_SERVER_SEND";
	case MESS_CLIENT_GET_IP:             return (char*)"MESS_CLIENT_GET_IP";
	case MESS_SERVER_GIVE_IP:            return (char*)"MESS_SERVER_GIVE_IP";
	}
	return (char*)"";
}

#pragma pack(push, 1)
typedef struct message_info {
	uint32_t size;
	int32_t  message;
	int32_t  index;
	int32_t  port;
	shadow_myNCB messNCB;
	char data[2048 * 30];
};
#pragma pack()

// ---------------------------------------------------------------------------
// TypeIpPort - used both for network names and as sender identifier
// (replaces asio::ip::udp::endpoint throughout)
// ---------------------------------------------------------------------------
typedef struct {
	std::string adress;
	int port;
} TypeIpPort;

// ---------------------------------------------------------------------------
// saved message now uses TypeIpPort instead of asio endpoint
// ---------------------------------------------------------------------------
typedef struct {
	std::string messages;
	TypeIpPort  sender;
} type_savedMessages;
std::deque<type_savedMessages> savedMessages;

// ---------------------------------------------------------------------------
// shadow_myNCB conversion helpers
// ---------------------------------------------------------------------------
shadow_myNCB myNCBtoShadow(myNCB from) {
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

myNCB myNCBfromShadow(shadow_myNCB from) {
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
// Message packing / unpacking
// ---------------------------------------------------------------------------
std::string DataToString(message_info messInfo)
{
	size_t total_size = messInfo.size + sizeof(shadow_myNCB) + 24;
	return std::string((const char*)&messInfo, total_size);
}

std::string Pack_Message(uint32_t message, shadow_myNCB locNCB, int32_t locIndex,
	int32_t port = -1, const char* data = nullptr, int size_of_data = 0)
{
	message_info locmessage_info;
	locmessage_info.message = message;
	locmessage_info.size = size_of_data;
	locmessage_info.messNCB = locNCB;
	locmessage_info.index = locIndex;
	locmessage_info.port = port;
	if (data)
		memcpy(locmessage_info.data, data, size_of_data);
	return DataToString(locmessage_info);
}

message_info Unpack_Message(std::string data)
{
	message_info output;
	for (int i = 0; i < (int)(sizeof(shadow_myNCB) + 24); i++)
		((char*)(&output))[i] = data[i];
	for (int i = (int)(sizeof(shadow_myNCB) + 24);
		i < (int)(output.size + sizeof(shadow_myNCB) + 24); i++)
		((char*)(&output))[i] = data[i];
	return output;
}

void singleThreadSleep(int sleepMs)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
}

std::string IHaveNameStrP = "";

// ---------------------------------------------------------------------------
// Debug logging
// ---------------------------------------------------------------------------
FILE* debug_net_output;
const char* debug_net_filename1 = "net_messages_log.txt";
std::string debug_net_filename2 = {};
bool debug_net_first = true;
std::mutex print_mt;

void debug_net_printf(const char* format, ...) {
	print_mt.lock();
	char prbuffer[1024 * 40];
	va_list arg;
	va_start(arg, format);
	vsprintf(prbuffer, format, arg);
	va_end(arg);

	if (debug_net_first) {
		debug_net_output = fopen(debug_net_filename2.c_str(), "wt");
		debug_net_first = false;
	}
	else {
		debug_net_output = fopen(debug_net_filename2.c_str(), "at");
	}
	fprintf(debug_net_output, "%d|%s", clock(), prbuffer);
	fclose(debug_net_output);
#ifdef DEBUG_PRINT_DEBUG_TO_SCREEN
	printf(prbuffer);
#endif
	print_mt.unlock();
}

// ---------------------------------------------------------------------------
// Network name registry
// ---------------------------------------------------------------------------
std::vector<std::string> NetworkName;
std::vector<TypeIpPort>  clientIpPort;

std::vector<std::string> ListenName;
std::vector<TypeIpPort>  clientListenID;
std::vector<std::string> ListenName2;
std::vector<TypeIpPort>  clientListenID2;

std::vector<myNCB*> clientConnection;

bool receiveServerAddName = false;

bool ReceiveServerAddName() {
	return receiveServerAddName;
}

TypeIpPort GetIpPortFromName(std::string name) {
	for (int i = 0; i < (int)NetworkName.size(); i++)
		if (!NetworkName[i].compare(name))
			return clientIpPort[i];
	TypeIpPort tempIpPort;
	tempIpPort.adress = "x999";
	tempIpPort.port = -1;
	return tempIpPort;
}

std::string GetNameNetworkFromId(TypeIpPort ipPort) {
	for (int i = 0; i < (int)NetworkName.size(); i++)
		if ((ipPort.adress == clientIpPort[i].adress) && (ipPort.port == clientIpPort[i].port))
			return NetworkName[i];
	return "";
}

std::string GetNameNetwork(std::string name) {
	for (std::string locNetworkName : NetworkName)
		if (!name.compare(locNetworkName))
			return locNetworkName;
	return "";
}

int GetIndexNetworkIpPort(TypeIpPort ipPort) {
	for (int i = 0; i < (int)clientIpPort.size(); i++)
		if ((ipPort.adress == clientIpPort[i].adress) && (ipPort.port == clientIpPort[i].port))
			return i;
	return -1;
}

int GetNameNetworkIndex(std::string name) {
	for (int i = 0; i < (int)NetworkName.size(); i++)
		if (!name.compare(NetworkName[i]))
			return i;
	return -1;
}

void AddNetworkName(std::string name, TypeIpPort ipPort) {
	if (!GetNameNetwork(name).compare("")) {
		NetworkName.push_back(name);
		clientIpPort.push_back(ipPort);
	}
#ifdef TEST_NETWORK_MESSAGES
	debug_net_printf("AddNetworkName - net name added:%s %s %d\n",
		name.c_str(), ipPort.adress.c_str(), ipPort.port);
#endif
}

bool ExistNetworkName(std::string name, TypeIpPort ipPort) {
	for (int i = 0; i < (int)NetworkName.size(); i++)
		if ((!name.compare(NetworkName[i])) &&
			(clientIpPort[i].adress == ipPort.adress) &&
			(clientIpPort[i].port == ipPort.port))
			return true;
	return false;
}

TypeIpPort GetOtherSide(TypeIpPort ipPort) {
	for (int i = 0; i < (int)clientListenID.size(); i++)
		if ((clientListenID[i].adress == ipPort.adress) &&
			(clientListenID[i].port == ipPort.port))
			return clientListenID2[i];
	for (int i = 0; i < (int)clientListenID2.size(); i++)
		if ((clientListenID2[i].adress == ipPort.adress) &&
			(clientListenID2[i].port == ipPort.port))
			return clientListenID[i];
	TypeIpPort tempIpPort;
	tempIpPort.adress = "x1000";
	tempIpPort.port = -1;
	return tempIpPort;
}

std::string GetListenNetwork(std::string name) {
	for (std::string locListenName : ListenName)
		if (!name.compare(locListenName))
			return locListenName;
	return "";
}

int GetNameListenIndex(std::string name) {
	for (int i = 0; i < (int)ListenName.size(); i++)
		if (!name.compare(ListenName[i]))
			return i;
	return -1;
}

int GetName2ListenIndex(std::string name) {
	for (int i = 0; i < (int)ListenName2.size(); i++)
		if (!name.compare(ListenName2[i]))
			return i;
	return -1;
}

void AddListenName(myNCB* connection) {
	if (!GetListenNetwork(connection->ncb_callName_10).compare("")) {
		ListenName.push_back(connection->ncb_callName_10);
		clientListenID.push_back(GetIpPortFromName(connection->ncb_callName_10));
		ListenName2.push_back(connection->ncb_name_26);
		clientListenID2.push_back(GetIpPortFromName(connection->ncb_name_26));
#ifdef TEST_NETWORK_MESSAGES
		debug_net_printf("AddListenName - listen name added:%s %s\n",
			connection->ncb_callName_10, connection->ncb_name_26);
#endif
	}
}

bool AddListenName2(const shadow_myNCB* connection) {
	TypeIpPort id1 = GetIpPortFromName(connection->ncb_callName_10);
	if (id1.adress == "x999") return false;
	TypeIpPort id2 = GetIpPortFromName(connection->ncb_name_26);
	if (id2.adress == "x999") return false;

	int indexid = GetNameListenIndex(connection->ncb_name_26);
	if (indexid == -1) return false;

	clientListenID[indexid] = id1;
	clientListenID2[indexid] = id2;
#ifdef TEST_NETWORK_MESSAGES
	debug_net_printf("listen accepted:%s %d,%s %d\n",
		connection->ncb_callName_10, indexid,
		connection->ncb_name_26, id2.port);
#endif
	return true;
}

bool IsListenName2(const shadow_myNCB* connection) {
	TypeIpPort id1 = GetIpPortFromName(connection->ncb_callName_10);
	if (id1.adress == "x999") return false;
	TypeIpPort id2 = GetIpPortFromName(connection->ncb_name_26);
	if (id2.adress == "x999") return false;

	int indexid = GetNameListenIndex(connection->ncb_name_26);
	if (indexid == -1) return false;
	if (ListenName[indexid] != connection->ncb_callName_10) return false;
	if ((clientListenID[indexid].adress != id1.adress) ||
		(clientListenID[indexid].port != id1.port))   return false;
	if (ListenName2[indexid] != connection->ncb_name_26) return false;
	if ((clientListenID2[indexid].adress != id2.adress) ||
		(clientListenID2[indexid].port != id1.port))   return false;
	return true;
}

// FIX #2: RemoveListenName - guard against -1 index, re-search after first erase
void RemoveListenName(std::string name) {
	int index = GetNameListenIndex(name);
	if (index != -1) {
		ListenName.erase(ListenName.begin() + index);
		clientListenID.erase(clientListenID.begin() + index);
		ListenName2.erase(ListenName2.begin() + index);
		clientListenID2.erase(clientListenID2.begin() + index);
	}
	// Re-search after first erase - vectors have shifted
	int index2 = GetName2ListenIndex(name);
	if (index2 != -1) {
		ListenName.erase(ListenName.begin() + index2);
		clientListenID.erase(clientListenID.begin() + index2);
		ListenName2.erase(ListenName2.begin() + index2);
		clientListenID2.erase(clientListenID2.begin() + index2);
	}
}

void RemoveNetworkName(std::string name) {
	int index = GetNameNetworkIndex(name);
	if (index == -1) return;
	NetworkName.erase(NetworkName.begin() + index);
	clientIpPort.erase(clientIpPort.begin() + index);
}

// ---------------------------------------------------------------------------
// Mutex / state helpers
// ---------------------------------------------------------------------------
std::mutex handleConnections_mt;
std::mutex netstate_mt;
std::mutex networkTimeout_mt;

Neti_type netstate()
{
	Neti_type result;
	netstate_mt.lock();
	result = netstate_shared;
	netstate_mt.unlock();
	return result;
}

void netstate(Neti_type input)
{
	netstate_mt.lock();
	netstate_shared = input;
	netstate_mt.unlock();
}

std::mutex clientConnection_mt;

bool setListen(myNCB* locNCB) {
	clientConnection_mt.lock();
	bool result = false;
	for (int i = 0; i < (int)clientConnection.size(); i++)
		if (memcmp(locNCB->ncb_name_26, clientConnection[i]->ncb_name_26,
			sizeof(clientConnection[i]->ncb_name_26)) == 0)
		{
			clientConnection[i]->ncb_lsn_2 = 20;
			clientConnection[i]->ncb_cmd_cplt_49 = 0;
			result = true;
			break;
		}
	clientConnection_mt.unlock();
	return result;
}

connectionTime* GetConnection(int32_t index) {
	connectionTime* result = nullptr;
	for (auto& locConn : handleConnections)
		if (locConn.index == index)
			result = &locConn;
	return result;
}

void DeleteConnection(int32_t index)
{
	for (auto locConn = handleConnections.begin(); locConn != handleConnections.end();) {
		if (locConn->index == index)
			locConn = handleConnections.erase(locConn);
		else
			++locConn;
	}
}

void SetNetworkTime(int32_t index, int timeout) {
	handleConnections_mt.lock();
	for (auto& locConn : handleConnections)
		if (locConn.index == index)
			locConn.timeout = timeout;
	handleConnections_mt.unlock();
}

int32_t lastIndex = 0;
std::mutex index_mt;

int32_t GetNextIndex() {
	index_mt.lock();
	lastIndex++;
	if (lastIndex > 1000000) lastIndex = 0;
	int32_t locIndex = lastIndex;
	index_mt.unlock();
	return locIndex;
}

void setListenConnection(myNCB* connection) {
	clientConnection_mt.lock();
	bool result = false;
	for (int i = 0; i < (int)clientConnection.size(); i++)
		if (clientConnection[i] == connection) { result = true; break; }
	if (!result)
		clientConnection.push_back(connection);
	clientConnection_mt.unlock();
}

void deleteListenConnection(myNCB* connection) {
	clientConnection_mt.lock();
	for (int i = 0; i < (int)clientConnection.size(); i++)
		if (clientConnection[i] == connection) {
			clientConnection.erase(clientConnection.begin() + i);
			break;
		}
	clientConnection_mt.unlock();
}

// ---------------------------------------------------------------------------
// Receive message queue
// ---------------------------------------------------------------------------
std::vector<std::string> recMessages;
std::mutex RecMess_mt;

void AddRecMess(std::string message) {
	RecMess_mt.lock();
	recMessages.push_back(message);
	RecMess_mt.unlock();
}

unsigned int GetRecCount() {
	RecMess_mt.lock();
	unsigned int result = (unsigned int)recMessages.size();
	RecMess_mt.unlock();
	return result;
}

std::string GetRecMess() {
	std::string result;
	RecMess_mt.lock();
	result = recMessages.front();
	recMessages.erase(recMessages.begin());
	RecMess_mt.unlock();
	return result;
}

// FIX #1: memcpy -> memcmp, correct field comparison
void CleanMessages(myNCB locNCB) {
	RecMess_mt.lock();
	for (auto it = recMessages.begin(); it != recMessages.end();) {
		message_info unpackedMess = Unpack_Message(*it);
		if ((memcmp(unpackedMess.messNCB.ncb_name_26,
			locNCB.ncb_name_26, sizeof(locNCB.ncb_name_26)) == 0) ||
			(memcmp(unpackedMess.messNCB.ncb_callName_10,
				locNCB.ncb_name_26, sizeof(locNCB.ncb_name_26)) == 0))
		{
			it = recMessages.erase(it);
		}
		else {
			++it;
		}
	}
	RecMess_mt.unlock();
}

// ---------------------------------------------------------------------------
// NetworkClass
// ---------------------------------------------------------------------------
namespace MyNetworkLib {

	class NetworkClass
	{
	private:
		thread receiver_thread;
		thread receiver2_thread;
		thread handler_thread;
		bool HandleReceiver2On;
		bool HandleReceiverOn;
		bool HandleHandlerOn;
		int clPort;
		int clServerPort;
		std::string clHost;
		bool clIam_server = false;
		bool IpPortIsSet = false;

		std::vector<TypeIpPort> registered;
		bool serverAddname = false;

		void Receiver2();
		void Receiver();

		bool HandleMessagesServerPre(std::string bufferStr, TypeIpPort sender);
		bool HandleMessagesClientPre(std::string bufferStr, TypeIpPort sender);
		void HandleMessagesServer(std::string bufferStr, TypeIpPort sender);
		void HandleMessagesClient(std::string bufferStr, TypeIpPort sender);

		void Handler();
		void SendGiveIPMessage(TypeIpPort sender, int port);

		bool Registered(TypeIpPort ipPort);
		void Register(TypeIpPort ipPort);

		void StartReceiver2();
		void StopReceiver2();
		void StartReceiver();
		void StopReceiver();
		void StartHandler();
		void StopHandler();

		void SendToServer(const std::string& message);
		void SendToClient(const std::string& message, std::string adress, int port);

	public:
		NetworkClass(bool iam_server, std::string host, int port, int serverPort, bool IAmServer);
		~NetworkClass();

		bool SendMessage_UDP(const std::string& message, const std::string& destination_ip,
			unsigned short port);

		void AddName(myNCB* connection, int32_t index);
		void AddNameReceive(bool nameIsOk, uint32_t id);
		void CancelNetwork(myNCB* connection, int32_t index);
		void DeleteNetwork(myNCB* connection, int32_t index);
		void CallNetwork(myNCB* connection, int32_t index);
		void ListenNetwork(myNCB* connection, int32_t index);
		void SendNetwork(myNCB* connection, int32_t index);
		void ReceiveNetwork(myNCB* connection);
	};

	NetworkClass::NetworkClass(bool iam_server, std::string host, int port,
		int serverPort, bool IAmServer)
	{
		clPort = port;
		clServerPort = serverPort;
		clHost = host;
		clIam_server = iam_server;
		StartReceiver();
		StartReceiver2();
		StartHandler();
	}

	NetworkClass::~NetworkClass() {
		StopReceiver();
		StopReceiver2();
		StopHandler();
	}

	bool NetworkClass::SendMessage_UDP(const std::string& message,
		const std::string& destination_ip,
		unsigned short port)
	{
		if (!IpPortIsSet) {
			message_info unpacked_message = Unpack_Message(message);
			if ((unpacked_message.message != MESS_CLIENT_GET_IP) &&
				(unpacked_message.message != MESS_SERVER_GIVE_IP))
				return false;
		}
#ifdef TEST_NETWORK_MESSAGES
		debug_net_printf("Send message to %s:%d\n", destination_ip.c_str(), port);
#endif
		socket_t sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock == SOCK_INVALID) return false;

		sockaddr_in remote{};
		remote.sin_family = AF_INET;
		remote.sin_port = htons(port);
		if (inet_pton(AF_INET, destination_ip.c_str(), &remote.sin_addr) <= 0) {
			CLOSE_SOCKET(sock);
			return false;
		}
		int sent = sendto(sock, message.c_str(), (int)message.size(), 0,
			(sockaddr*)&remote, sizeof(remote));
		CLOSE_SOCKET(sock);
		return (sent != -1);
	}

	void NetworkClass::StartReceiver2() {
		HandleReceiver2On = true;
		receiver2_thread = std::thread(&NetworkClass::Receiver2, this);
	}

	void NetworkClass::StopReceiver2() {
		HandleReceiver2On = false;
		// Wake the blocking recvfrom so the thread can exit
		SendMessage_UDP("Xend", "127.0.0.1", clPort);
		singleThreadSleep(500);
		receiver2_thread.join();
	}

	void NetworkClass::StartReceiver() {
		HandleReceiverOn = true;
		receiver_thread = std::thread(&NetworkClass::Receiver, this);
	}

	void NetworkClass::StopReceiver() {
		HandleReceiverOn = false;
		singleThreadSleep(500);
		receiver_thread.join();
	}

	void NetworkClass::StartHandler() {
		HandleHandlerOn = true;
		handler_thread = std::thread(&NetworkClass::Handler, this);
	}

	void NetworkClass::StopHandler() {
		HandleHandlerOn = false;
		singleThreadSleep(500);
		handler_thread.join();
	}

	void NetworkClass::SendGiveIPMessage(TypeIpPort sender, int port) {
		shadow_myNCB nullNCB{};
		nullNCB.ncb_command_0 = 254;
		SendToClient(
			Pack_Message(MESS_SERVER_GIVE_IP, nullNCB, -1, port,
				sender.adress.c_str(), (int)(1 + strlen(sender.adress.c_str()))),
			sender.adress, port);
	}

	bool NetworkClass::Registered(TypeIpPort ipPort) {
		for (int i = 0; i < (int)registered.size(); i++)
			if ((registered[i].adress == ipPort.adress) && (registered[i].port == ipPort.port))
				return true;
		return false;
	}

	void NetworkClass::Register(TypeIpPort ipPort) {
		registered.push_back(ipPort);
	}

	// ---------------------------------------------------------------------------
	// Handler - processes timed connections
	// FIX #6: collect indices to delete, erase after loop to avoid iterator invalidation
	// ---------------------------------------------------------------------------
	void NetworkClass::Handler() {
		while (HandleHandlerOn)
		{
			if (!IpPortIsSet) {
				shadow_myNCB nullNCB{};
				nullNCB.ncb_command_0 = 254;
				SendToServer(Pack_Message(MESS_CLIENT_GET_IP, nullNCB, GetNextIndex(), clPort));
				singleThreadSleep(500);
				continue;
			}

			if (!receiveServerAddName) {
				shadow_myNCB nullNCB{};
				nullNCB.ncb_command_0 = 254;
				SendToServer(Pack_Message(MESS_CLIENT_SERVER_NAME_ADDED, nullNCB,
					GetNextIndex(), clPort));
				singleThreadSleep(1000);
			}

			handleConnections_mt.lock();

			// --- Pass 1: handle incoming RECEIVE messages ---
			std::vector<int32_t> toDelete;

			for (connectionTime& actConnection : handleConnections)
			{
				if (actConnection.connection->ncb_command_0 != 0x95) continue;

				if (GetRecCount() > 0)
				{
					message_info unpacked_message = Unpack_Message(GetRecMess());
					if ((unpacked_message.message != MESS_SERVER_SEND) ||
						(unpacked_message.messNCB.ncb_command_0 != 0x94))
					{
						// Not our message - put it back
						AddRecMess(Pack_Message(unpacked_message.message,
							unpacked_message.messNCB,
							unpacked_message.index,
							clServerPort,
							unpacked_message.data,
							unpacked_message.size));
						break;
					}
					if (actConnection.connection->ncb_bufferLength_8 != unpacked_message.size)
						break;

					actConnection.connection->ncb_bufferLength_8 = unpacked_message.size;
					memcpy(actConnection.connection->ncb_buffer_4.p,
						unpacked_message.data, unpacked_message.size);
					actConnection.connection->ncb_cmd_cplt_49 = 0x0;
#ifdef TEST_NETWORK_MESSAGES
					debug_net_printf("processEnd: RECEIVE complete %d bytes\n",
						actConnection.connection->ncb_bufferLength_8);
#endif
					toDelete.push_back(actConnection.index);
				}
			}

			// --- Pass 2: handle timeouts ---
			clock_t actClock = clock();
			for (connectionTime& actConnection : handleConnections)
			{
				if (actClock <= actConnection.startTime + actConnection.timeout) continue;

				bool deleteConnection = true;
#ifdef TEST_NETWORK_MESSAGES
				debug_net_printf("processEnd: TIMEOUT cmd:%x\n",
					actConnection.connection->ncb_command_0);
#endif
				switch (actConnection.connection->ncb_command_0)
				{
				case 0x35: { // CANCEL
					actConnection.connection->ncb_retcode_1 = 0x0;
					strcpy(actConnection.connection->ncb_name_26, "");
					strcpy(actConnection.connection->ncb_callName_10, "");
					actConnection.connection->ncb_cmd_cplt_49 = 0x0;
					break;
				}
				case 0x7F: { // INIT
					actConnection.connection->ncb_cmd_cplt_49 = 0;
					break;
				}
				case 0x90: { // CALL
					if (netstate() == NETI_CALL) {
						actConnection.startTime = actClock;
						deleteConnection = false;
						SendToServer(Pack_Message(MESS_CLIENT_MESSAGE_CALL,
							myNCBtoShadow(*actConnection.connection),
							actConnection.index, clPort,
							actConnection.connection->ncb_name_26,
							sizeof(actConnection.connection->ncb_name_26)));
					}
					break;
				}
				case 0x91: { // LISTEN
					break;
				}
				case 0x94: { // SEND
					actConnection.connection->ncb_cmd_cplt_49 = 0;
					break;
				}
				case 0x95: { // RECEIVE timeout
					if (GetRecCount() > 0) {
						message_info unpacked_message = Unpack_Message(GetRecMess());
						if ((unpacked_message.message != MESS_SERVER_SEND) ||
							(unpacked_message.messNCB.ncb_command_0 != 0x94))
						{
							AddRecMess(Pack_Message(unpacked_message.message,
								unpacked_message.messNCB,
								unpacked_message.index,
								clServerPort,
								unpacked_message.data,
								unpacked_message.size));
							break;
						}
					}
					actConnection.connection->ncb_bufferLength_8 = 0;
					actConnection.connection->ncb_cmd_cplt_49 = 0;
					break;
				}
				case 0xb0: { // ADD_NAME
					if (netstate() == NETI_ADD_NAME_REJECT) {
						actConnection.connection->ncb_cmd_cplt_49 = 22;
					}
					else if (netstate() == NETI_ADD_NAME_OK) {
						IHaveNameStrP = actConnection.connection->ncb_name_26;
						actConnection.connection->ncb_cmd_cplt_49 = 0;
					}
					else {
						actConnection.startTime = actClock;
						deleteConnection = false;
						SendToServer(Pack_Message(MESS_CLIENT_TESTADDNAME,
							myNCBtoShadow(*actConnection.connection),
							actConnection.index, clPort,
							actConnection.connection->ncb_name_26,
							sizeof(actConnection.connection->ncb_name_26)));
#ifdef TEST_NETWORK_MESSAGES
						debug_net_printf("Retrying MESS_CLIENT_TESTADDNAME\n");
#endif
					}
					break;
				}
				case 0xb1: { // DELETE_NAME
					actConnection.connection->ncb_cmd_cplt_49 = 0;
					break;
				}
				default: {
					actConnection.connection->ncb_cmd_cplt_49 = 0;
					break;
				}
				}

				if (deleteConnection)
					toDelete.push_back(actConnection.index);
			}

			// Erase after both loops - safe, no iterator invalidation during iteration
			for (int32_t idx : toDelete)
				DeleteConnection(idx);

			handleConnections_mt.unlock();

			singleThreadSleep(1);
		}
	}

	// ---------------------------------------------------------------------------
	// Message handlers - server side
	// ---------------------------------------------------------------------------
	bool NetworkClass::HandleMessagesServerPre(std::string bufferStr, TypeIpPort sender)
	{
		message_info unpacked_message = Unpack_Message(bufferStr);
		if (unpacked_message.message == MESS_CLIENT_GET_IP) {
			SendGiveIPMessage(sender, unpacked_message.port);
			return true;
		}
		return false;
	}

	bool NetworkClass::HandleMessagesClientPre(std::string bufferStr, TypeIpPort sender)
	{
		message_info unpacked_message = Unpack_Message(bufferStr);
		if (unpacked_message.message == MESS_SERVER_GIVE_IP) {
			IpPortIsSet = true;
			return true;
		}
		return false;
	}

	void NetworkClass::HandleMessagesServer(std::string bufferStr, TypeIpPort sender)
	{
		message_info unpacked_message = Unpack_Message(bufferStr);

#ifdef TEST_NETWORK_MESSAGES
		debug_net_printf("Server: message - %s\n", MessageIndexToText(unpacked_message.message));
#endif

		if (unpacked_message.message == MESS_CLIENT_SERVER_NAME_ADDED)
		{
			TypeIpPort locIpPort;
			locIpPort.adress = sender.adress;
			locIpPort.port = unpacked_message.port;
			if (!Registered(locIpPort))
				Register(locIpPort);
			if (serverAddname) {
				shadow_myNCB nullNCB{};
				nullNCB.ncb_command_0 = 254;
				SendToClient(Pack_Message(MESS_SERVER_SERVER_NAME_ADDED, nullNCB,
					unpacked_message.index, -10),
					sender.adress, unpacked_message.port);
			}
		}
		else if (unpacked_message.message == MESS_CLIENT_TESTADDNAME)
		{
			TypeIpPort locIpPort;
			locIpPort.adress = sender.adress;
			locIpPort.port = unpacked_message.port;
			if (!GetNameNetwork(unpacked_message.data).compare("")) {
				AddNetworkName(unpacked_message.data, locIpPort);
				shadow_myNCB nullNCB{};
				nullNCB.ncb_command_0 = 254;
				SendToClient(Pack_Message(MESS_SERVER_TESTADDNAME_OK, nullNCB,
					unpacked_message.index, -10),
					sender.adress, unpacked_message.port);
				char compText[16];
				sprintf(compText, "NETH2%c0        ", unpacked_message.data[5]);
				if (!strcmp(unpacked_message.data, compText))
					serverAddname = true;
			}
			else if (ExistNetworkName(unpacked_message.data, locIpPort)) {
				shadow_myNCB nullNCB{};
				nullNCB.ncb_command_0 = 254;
				SendToClient(Pack_Message(MESS_SERVER_TESTADDNAME_OK, nullNCB,
					unpacked_message.index, -10),
					sender.adress, unpacked_message.port);
			}
			else {
				shadow_myNCB nullNCB{};
				nullNCB.ncb_command_0 = 254;
				SendToClient(Pack_Message(MESS_SERVER_TESTADDNAME_REJECT, nullNCB,
					unpacked_message.index, -10),
					sender.adress, unpacked_message.port);
			}
		}
		else if (unpacked_message.message == MESS_CLIENT_MESSAGE_LISTEN)
		{
			AddListenName((myNCB*)unpacked_message.data);
#ifdef TEST_NETWORK_MESSAGES
			debug_net_printf("Server: LISTEN %s ; %s\n",
				((myNCB*)unpacked_message.data)->ncb_callName_10,
				((myNCB*)unpacked_message.data)->ncb_name_26);
#endif
		}
		else if (unpacked_message.message == MESS_CLIENT_MESSAGE_CALL)
		{
			TypeIpPort callLocIpPort = GetIpPortFromName(unpacked_message.messNCB.ncb_callName_10);
			if (AddListenName2(&unpacked_message.messNCB)) {
				SendToClient(Pack_Message(MESS_SERVER_CALL_ACCEPT,
					unpacked_message.messNCB, unpacked_message.index),
					sender.adress, unpacked_message.port);
				SendToClient(Pack_Message(MESS_SERVER_LISTEN_ACCEPT,
					unpacked_message.messNCB, unpacked_message.index,
					clServerPort, unpacked_message.data, unpacked_message.size),
					callLocIpPort.adress, callLocIpPort.port);
			}
			else {
				SendToClient(Pack_Message(MESS_SERVER_CALL_REJECT,
					unpacked_message.messNCB, unpacked_message.index, -10),
					sender.adress, unpacked_message.port);
				SendToClient(Pack_Message(MESS_SERVER_LISTEN_REJECT,
					unpacked_message.messNCB, unpacked_message.index, -10),
					callLocIpPort.adress, callLocIpPort.port);
			}
		}
		else if (unpacked_message.message == MESS_CLIENT_CANCEL)
		{
			if (unpacked_message.data[0] != 0)
				RemoveListenName(unpacked_message.data);
		}
		else if (unpacked_message.message == MESS_CLIENT_DELETE)
		{
			CleanMessages(myNCBfromShadow(unpacked_message.messNCB));
			RemoveNetworkName(unpacked_message.data);
		}
		else if (unpacked_message.message == MESS_CLIENT_SEND)
		{
			TypeIpPort locIpPort;
			locIpPort.adress = sender.adress;
			locIpPort.port = unpacked_message.port;
			TypeIpPort otherid = GetOtherSide(locIpPort);
			if (otherid.adress != "x1000") {
				SendToClient(Pack_Message(MESS_SERVER_SEND,
					unpacked_message.messNCB, unpacked_message.index,
					clServerPort, unpacked_message.data, unpacked_message.size),
					otherid.adress, otherid.port);
				SendToClient(Pack_Message(MESS_SERVER_SEND_OK,
					unpacked_message.messNCB, unpacked_message.index),
					sender.adress, unpacked_message.port);
			}
		}
	}

	// ---------------------------------------------------------------------------
	// Message handler - client side
	// ---------------------------------------------------------------------------
	void NetworkClass::HandleMessagesClient(std::string bufferStr, TypeIpPort sender)
	{
		message_info unpacked_message = Unpack_Message(bufferStr);

#ifdef TEST_NETWORK_MESSAGES
		debug_net_printf("Client: message - %s\n", MessageIndexToText(unpacked_message.message));
#endif

		if (unpacked_message.message == MESS_SERVER_SERVER_NAME_ADDED)
		{
			receiveServerAddName = true;
		}
		else if (unpacked_message.message == MESS_SERVER_TESTADDNAME_OK)
		{
			netstate(NETI_ADD_NAME_OK);
			SetNetworkTime(unpacked_message.index, 0);
		}
		else if (unpacked_message.message == MESS_SERVER_TESTADDNAME_REJECT)
		{
			netstate(NETI_ADD_NAME_REJECT);
			SetNetworkTime(unpacked_message.index, 0);
		}
		else if (unpacked_message.message == MESS_SERVER_CALL_ACCEPT)
		{
			handleConnections_mt.lock();
			connectionTime* ptrConn = GetConnection(unpacked_message.index);
			if (ptrConn) {
				ptrConn->connection->ncb_retcode_1 = 0x00;
				ptrConn->connection->ncb_cmd_cplt_49 = 0x00;
			}
			handleConnections_mt.unlock();

			netstate(NETI_CALL_ACCEPT);
			SetNetworkTime(unpacked_message.index, 0);
#ifdef TEST_NETWORK_MESSAGES
			debug_net_printf("CLIENT CALL_ACCEPT\n");
#endif
		}
		else if (unpacked_message.message == MESS_SERVER_CALL_REJECT)
		{
			netstate(NETI_CALL_REJECT);
			SetNetworkTime(unpacked_message.index, 0);
		}
		else if (unpacked_message.message == MESS_SERVER_LISTEN_ACCEPT)
		{
			setListen((myNCB*)unpacked_message.data);
		}
		else if (unpacked_message.message == MESS_SERVER_LISTEN_REJECT)
		{
			// nothing to do
		}
		else if (unpacked_message.message == MESS_SERVER_SEND)
		{
			AddRecMess(Pack_Message(MESS_SERVER_SEND,
				unpacked_message.messNCB, unpacked_message.index,
				clServerPort, unpacked_message.data, unpacked_message.size));
			SetNetworkTime(unpacked_message.index, 0);
		}
		else if (unpacked_message.message == MESS_SERVER_SEND_OK)
		{
			SetNetworkTime(unpacked_message.index, 0);
		}
	}

	// ---------------------------------------------------------------------------
	// Receiver2 - blocking UDP receive loop (replaces asio socket)
	// ---------------------------------------------------------------------------
	std::mutex savedMessages_mt;
	void NetworkClass::Receiver2() {
		socket_t sock = socket(AF_INET, SOCK_DGRAM, 0);

		sockaddr_in local{};
		local.sin_family = AF_INET;
		local.sin_port = htons(clPort);
		local.sin_addr.s_addr = INADDR_ANY;
		bind(sock, (sockaddr*)&local, sizeof(local));

		while (HandleReceiver2On) {
			char buffer[65536];
			sockaddr_in from{};
			socklen_t fromlen = sizeof(from);

			int bytes = recvfrom(sock, buffer, sizeof(buffer), 0,
				(sockaddr*)&from, &fromlen);
			if (bytes <= 0) continue;

			std::string bufferStr(buffer, bytes);
			if (bufferStr == "Xend") continue;

#ifdef TEST_BAD_NETWORK
			message_info unpacked_message = Unpack_Message(bufferStr);
			if ((rand() % 3 == 0) ||
				(unpacked_message.message == MESS_CLIENT_MESSAGE_LISTEN) ||
				(unpacked_message.message == MESS_SERVER_LISTEN_ACCEPT))
			{
#endif
				type_savedMessages locMessage;
				locMessage.messages = bufferStr;
				char ipStr[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, &(from.sin_addr), ipStr, INET_ADDRSTRLEN);
				locMessage.sender.adress = ipStr;
				locMessage.sender.port = ntohs(from.sin_port);

				savedMessages_mt.lock();
				savedMessages.push_back(std::move(locMessage));
				savedMessages_mt.unlock();
#ifdef TEST_BAD_NETWORK
			}
#endif
			singleThreadSleep(1);
		}
		CLOSE_SOCKET(sock);
	}

	// ---------------------------------------------------------------------------
	// Receiver - dispatches saved messages
	// ---------------------------------------------------------------------------
	void NetworkClass::Receiver() {
		while (HandleReceiverOn) {
			type_savedMessages locMessage;
			bool hasMessage = false;
			savedMessages_mt.lock();
			if (!savedMessages.empty()) {
				locMessage = std::move(savedMessages.front());
				savedMessages.pop_front();
				hasMessage = true;
			}
			savedMessages_mt.unlock();
			if (!hasMessage) {
				singleThreadSleep(1);
				continue;
			}

			TypeIpPort sender = locMessage.sender;
			std::string bufferStr = std::move(locMessage.messages);

#ifdef TEST_NETWORK_MESSAGES
			debug_net_printf("Receive message from %s:%d\n", sender.adress.c_str(), sender.port);
#endif

			if (bufferStr.empty()) continue;

			if (clIam_server)
				if (HandleMessagesServerPre(bufferStr, sender)) continue;
			if (HandleMessagesClientPre(bufferStr, sender)) continue;

			if (clIam_server)
				HandleMessagesServer(bufferStr, sender);
			HandleMessagesClient(bufferStr, sender);
		}
	}

	void NetworkClass::SendToServer(const std::string& message) {
		SendMessage_UDP(message, clHost, clServerPort);
	}

	void NetworkClass::SendToClient(const std::string& message, std::string adress, int port) {
		SendMessage_UDP(message, adress, (unsigned short)port);
	}

	void NetworkClass::AddName(myNCB* connection, int32_t index) {
		SendToServer(Pack_Message(MESS_CLIENT_TESTADDNAME, myNCBtoShadow(*connection),
			index, clPort,
			connection->ncb_name_26, sizeof(connection->ncb_name_26)));
	}

	void NetworkClass::AddNameReceive(bool nameIsOk, uint32_t id) {
		// reserved
	}

	void NetworkClass::CancelNetwork(myNCB* connection, int32_t index) {
		SendToServer(Pack_Message(MESS_CLIENT_CANCEL, myNCBtoShadow(*connection),
			index, clPort,
			connection->ncb_name_26, sizeof(connection->ncb_name_26)));
		deleteListenConnection(connection);
	}

	void NetworkClass::DeleteNetwork(myNCB* connection, int32_t index) {
		SendToServer(Pack_Message(MESS_CLIENT_DELETE, myNCBtoShadow(*connection),
			index, clPort,
			connection->ncb_name_26, sizeof(connection->ncb_name_26)));
		singleThreadSleep(400);
		CleanMessages(*connection);
		savedMessages.clear();
	}

	void NetworkClass::CallNetwork(myNCB* connection, int32_t index) {
		SendToServer(Pack_Message(MESS_CLIENT_MESSAGE_CALL, myNCBtoShadow(*connection),
			index, clPort,
			(char*)connection, sizeof(*connection)));
	}

	void NetworkClass::ListenNetwork(myNCB* connection, int32_t index) {
		SendToServer(Pack_Message(MESS_CLIENT_MESSAGE_LISTEN, myNCBtoShadow(*connection),
			index, clPort,
			(char*)connection, sizeof(*connection)));
		setListenConnection(connection);
	}

	// FIX #5: assert instead of intentional divide-by-zero
	void NetworkClass::SendNetwork(myNCB* connection, int32_t index) {
		assert(connection->ncb_command_0 == 0x94 &&
			"SendNetwork: connection must have SEND command (0x94)");
		SendToServer(Pack_Message(MESS_CLIENT_SEND, myNCBtoShadow(*connection),
			index, clPort,
			(char*)connection->ncb_buffer_4.p,
			connection->ncb_bufferLength_8));
#ifdef TEST_NETWORK_MESSAGES
		debug_net_printf("SEND %d bytes\n", connection->ncb_bufferLength_8);
#endif
	}

	void NetworkClass::ReceiveNetwork(myNCB* connection) {
		// intentionally empty - handled in Handler loop
	}

} // namespace MyNetworkLib

// ---------------------------------------------------------------------------
// Global instance
// ---------------------------------------------------------------------------
MyNetworkLib::NetworkClass* locNetworkClass;

// ---------------------------------------------------------------------------
// makeNetwork - translates NCB command into UDP network operation
// ---------------------------------------------------------------------------
int lastnetworkname = 0;

void makeNetwork(myNCB* connection) {
	int     locTimeout = 0;
	int32_t locIndex = GetNextIndex();

	switch (connection->ncb_command_0) {
	case 0x35: { // CANCEL
		locTimeout = 10000;
#ifdef TEST_NETWORK_MESSAGES
		debug_net_printf("makeNetwork - CANCEL %s %s\n",
			connection->ncb_name_26, connection->ncb_callName_10);
#endif
		myNCB* secondcon = (myNCB*)connection->ncb_buffer_4.p;
		if (secondcon) {
			secondcon->ncb_retcode_1 = 0x0b;
			secondcon->ncb_cmd_cplt_49 = 0x0b;
		}
		connection->ncb_retcode_1 = 0x00;
		connection->ncb_lsn_2 = 0x00;
		connection->ncb_cmd_cplt_49 = 0x00;
		locNetworkClass->CancelNetwork(connection, locIndex);
		break;
	}
	case 0x7f: { // INIT
		locTimeout = 200;
#ifdef TEST_NETWORK_MESSAGES
		debug_net_printf("makeNetwork - INIT\n");
#endif
		connection->ncb_retcode_1 = 0x03;
		connection->ncb_cmd_cplt_49 = 0x03;
		break;
	}
	case 0x90: { // CALL
		locTimeout = 1000;
#ifdef TEST_NETWORK_MESSAGES
		debug_net_printf("makeNetwork - CALL %p|%s|%s|\n",
			connection, connection->ncb_name_26, connection->ncb_callName_10);
#endif
		connection->ncb_retcode_1 = 0xff;
		connection->ncb_lsn_2 = 0xe8;
		connection->ncb_cmd_cplt_49 = 0xff;
		locNetworkClass->CallNetwork(connection, locIndex);
		netstate(NETI_CALL);
		break;
	}
	case 0x91: { // LISTEN
		locTimeout = 200;
#ifdef TEST_NETWORK_MESSAGES
		debug_net_printf("makeNetwork - LISTEN %s %s\n",
			connection->ncb_name_26, connection->ncb_callName_10);
#endif
		connection->ncb_retcode_1 = 0xff;
		connection->ncb_cmd_cplt_49 = 0xff;
		locNetworkClass->ListenNetwork(connection, locIndex);
		netstate(NETI_LISTEN);
		singleThreadSleep(200);
		break;
	}
	case 0x92: { // HANG UP
		break;
	}
	case 0x94: { // SEND
		locTimeout = 0;
#ifdef TEST_NETWORK_MESSAGES
		debug_net_printf("makeNetwork - SEND %s %s\n",
			connection->ncb_name_26, connection->ncb_callName_10);
#endif
		connection->ncb_retcode_1 = 0xff;
		connection->ncb_cmd_cplt_49 = 0xff;
		locNetworkClass->SendNetwork(connection, locIndex);
		break;
	}
	case 0x95: { // RECEIVE
		locTimeout = 1500;
#ifdef TEST_NETWORK_MESSAGES
		debug_net_printf("makeNetwork - RECEIVE %s %s\n",
			connection->ncb_name_26, connection->ncb_callName_10);
#endif
		connection->ncb_retcode_1 = 0xff;
		connection->ncb_cmd_cplt_49 = 0xff;
		locNetworkClass->ReceiveNetwork(connection);
		netstate(NETI_RECEIVE);
		break;
	}
	case 0xb0: { // ADD_NAME
		locTimeout = 1000;
#ifdef TEST_NETWORK_MESSAGES
		debug_net_printf("makeNetwork - ADD_NAME %s %s\n",
			connection->ncb_name_26, connection->ncb_callName_10);
#endif
		connection->ncb_retcode_1 = 0xff;
		connection->ncb_num_3 = lastnetworkname + 0x02;
		lastnetworkname++;
		connection->ncb_cmd_cplt_49 = 0xff;
		locNetworkClass->AddName(connection, locIndex);
		netstate(NETI_ADD_NAME);
		break;
	}
	case 0xb1: { // DELETE_NAME
		locTimeout = 10000;
#ifdef TEST_NETWORK_MESSAGES
		debug_net_printf("makeNetwork - DELETE_NAME %s %s\n",
			connection->ncb_name_26, connection->ncb_callName_10);
#endif
		connection->ncb_retcode_1 = 0xff;
		locNetworkClass->DeleteNetwork(connection, locIndex);
		break;
	}
	}

	connectionTime locConn;
	locConn.connection = connection;
	locConn.timeout = locTimeout;
	locConn.startTime = clock();
	locConn.index = locIndex;

	handleConnections_mt.lock();
	handleConnections.push_back(locConn);
	handleConnections_mt.unlock();

#ifdef TEST_NETWORK_MESSAGES
	debug_net_printf("makeNetwork - connection registered index=%d\n", locIndex);
#endif
}

// ---------------------------------------------------------------------------
// Debug helpers
// ---------------------------------------------------------------------------
void printState(myNCB** connections) {
#ifdef TEST_NETWORK_MESSAGES
	for (int i = 0; i < 3; i++)
		debug_net_printf("NetworkGetState: %d %p lsn=%d cplt=%s\n",
			i, connections[i], connections[i]->ncb_lsn_2,
			(!connections[i]->ncb_cmd_cplt_49) ? "ok" : "pending");
#endif
}

void printState2(char* text) {
#ifdef TEST_NETWORK_MESSAGES
	debug_net_printf("%s", text);
#endif
}

void timeState(bool start, const char* text) {
#ifdef TEST_TIME_NETWORK_MESSAGES
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
	}
#endif
}

// ---------------------------------------------------------------------------
// Init / shutdown
// ---------------------------------------------------------------------------
void InitMyNetLib(bool iam_server, char* ip, int networkPort, int serverPort)
{
#ifdef _WIN32
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
	if (iam_server) networkPort = serverPort;
	locNetworkClass = new MyNetworkLib::NetworkClass(iam_server, ip, networkPort,
		serverPort, true);
}

void EndMyNetLib() {
	singleThreadSleep(500);
	delete locNetworkClass;
#ifdef _WIN32
	WSACleanup();
#endif
}