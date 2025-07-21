#include "../engine/engine_support.h"
#include "port_sdl_joystick.h"
#include "port_sdl_vga_mouse.h"
#include "port_time.h"

#include <cstdint>

#include "../engine/sub_main_mouse.h"
#include "../engine/read_config.h"
#include "../engine/CommandLineParser.h"

#ifdef USE_DOSBOX
extern DOS_Device* DOS_CON;
#endif //USE_DOSBOX
#include "../engine/EventDispatcher.h"
#include "GameKey.h"
#include "KeyboardInputMapping.h"

SDL_Window* m_window = nullptr;
SDL_Renderer* m_renderer = nullptr;
SDL_Texture* m_texture = nullptr;
SDL_Surface* m_gamePalletisedSurface = nullptr;
SDL_Surface* m_gameRGBASurface = nullptr;
SDL_Color m_currentPalletColours[256];
uint8_t m_fontBuffer[256 * 256];
SDL_Surface* m_surfaceFont = nullptr;
uint8_t m_smallFontBuffer[128 * 128];
SDL_Surface* m_smallSurfaceFont = nullptr;

uint8_t LastPressedKey_1806E4; //3516e4
int8_t pressedKeys_180664[128]; // idb

uint16_t m_iOrigw = 640;
uint16_t m_iOrigh = 480;

uint16_t m_iWindowWidth = 640;
uint16_t m_iWindowHeight = 480;

bool m_bMaintainAspectRatio = true;
bool m_settingWindowGrabbed = true;

bool m_pressed = false;
uint16_t m_lastchar = 0;
Scene m_Scene = Scene::PREAMBLE_MENU;

const char* default_caption = "Magic Carpet 2 HD - (Community Update)";

bool m_initiated = false;
Uint8 tempPalettebuffer[768];

int oldWidth;

bool subBlitLock = false;

// Initalize Color Masks.
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
Uint32 redMask = 0xff000000;
Uint32 greenMask = 0x00ff0000;
Uint32 blueMask = 0x0000ff00;
Uint32 alphaMask = 0x000000ff;
#else
Uint32 redMask = 0x000000ff;
Uint32 greenMask = 0x0000ff00;
Uint32 blueMask = 0x00ff0000;
Uint32 alphaMask = 0xff000000;
#endif

std::vector<SDL_Rect> GetDisplays()
{
	int displays = SDL_GetNumVideoDisplays();

	// get display bounds for all displays
	std::vector<SDL_Rect> displayBounds;
	for (int i = 0; i < displays; i++) {
		displayBounds.push_back(SDL_Rect());
		SDL_GetDisplayBounds(i, &displayBounds.back());
	}

	return displayBounds;
}

SDL_Rect GetDisplayByIndex(uint8_t index)
{
	SDL_Rect display;
	display.x = 0;
	display.y = 0;
	display.w = 0;
	display.h = 0;

	std::vector<SDL_Rect> displayBounds = GetDisplays();

	if (index < displayBounds.size())
	{
		return displayBounds[index];
	}

	return display;
}

SDL_Rect FindDisplayByResolution(uint32_t width, uint32_t height)
{
	SDL_Rect display;
	display.x = 0;
	display.y = 0;
	display.w = width;
	display.h = height;

	std::vector<SDL_Rect> displayBounds = GetDisplays();

	for (int i = 0; i < displayBounds.size(); i++) {
		if (width <= displayBounds[i].w && height <= displayBounds[i].h)
			return displayBounds[i];
	}
	return display;
}

void VGA_Init(Uint32  /*flags*/, int windowWidth, int windowHeight, int gameResWidth, int gameResHeight, bool maintainAspectRatio, int displayIndex)
{
	if (!m_initiated)
	{
		m_bMaintainAspectRatio = maintainAspectRatio;
		m_iWindowWidth = windowWidth;
		m_iWindowHeight = windowHeight;

		//Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			Logger->error("SDL could not initialize! SDL_Error: {}", SDL_GetError());
			exit(0);
			//success = false;
		}
		else
		{

			init_sound();
			gamepad_sdl_init();

			SDL_ShowCursor(0);
			// Set hint before you create the Renderer!
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
			SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, "1");
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

			SDL_Rect display = GetDisplayByIndex(displayIndex);
			if (windowWidth > display.w || windowHeight > display.h)
			{
				display = FindDisplayByResolution(windowWidth, windowHeight);
			}
			m_window = SDL_CreateWindow(default_caption, display.x, display.y, display.w, display.h, SDL_WINDOW_FULLSCREEN_DESKTOP);
			ToggleFullscreen(!startWindowed);

			m_renderer =
				SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED |
					SDL_RENDERER_TARGETTEXTURE);

			SDL_SetRenderDrawColor(m_renderer, 0x00, 0x00, 0x00, 0xFF);

			// Now create your surface and convert the pixel format right away!
			// Converting the pixel format to match the texture makes for quicker updates, otherwise
			// It has to do the conversion each time you update the texture manually. This slows everything down.
			// Do it once, and don't have to worry about it again.

			CreateRenderSurfaces(gameResWidth, gameResHeight);

			SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);

			// Sure clear the screen first.. always nice.
			SDL_RenderClear(m_renderer);
			SDL_RenderPresent(m_renderer);

			SDL_SetWindowMouseRect(m_window, new SDL_Rect{ 0,0,640,480 });

			std::function<void(Scene)> callBack = SetMouseKeyboardScene;
			EventDispatcher::I->RegisterEvent(new Event<Scene>(EventType::E_SCENE_CHANGE, callBack));
		}
		if (!VGA_LoadFont())
		{
			Logger->error("Failed to load font!");
			exit(-1);
		}

		Set_basic_Palette1();
		//Draw_debug_matrix1();
		Draw_black();
		m_initiated = true;
	}
}

void SetMouseKeyboardScene(const Scene sceneId)
{
	m_Scene = sceneId;
}

void CreateRenderSurfaces(int width, int height)
{
	m_gamePalletisedSurface =
		SDL_CreateRGBSurface(
			SDL_SWSURFACE, width, height, 24,
			redMask, greenMask, blueMask, alphaMask);

	m_gamePalletisedSurface =
		SDL_ConvertSurfaceFormat(
			m_gamePalletisedSurface, SDL_PIXELFORMAT_INDEX8, 0);

	m_gameRGBASurface =
		SDL_CreateRGBSurface(
			SDL_SWSURFACE, width, height, 24,
			redMask, greenMask, blueMask, alphaMask);

	m_gameRGBASurface =
		SDL_ConvertSurfaceFormat(
			m_gameRGBASurface, SDL_PIXELFORMAT_RGB888, 0);

	m_texture = SDL_CreateTexture(m_renderer,
		SDL_PIXELFORMAT_RGB888,
		SDL_TEXTUREACCESS_STREAMING,
		m_gameRGBASurface->w, m_gameRGBASurface->h);
}

Uint8* VGA_Get_Palette() {
	return tempPalettebuffer;
}

uint16_t lastResHeight=0;

void SetPalette(SDL_Color* colours) {
	memcpy(m_currentPalletColours, colours, sizeof(SDL_Color) * 256);
	if (m_gamePalletisedSurface)
	{
		SDL_SetPaletteColors(m_gamePalletisedSurface->format->palette, m_currentPalletColours, 0, 256);
		//SubBlit(m_iOrigw, m_iOrigh);
	}
}

void Set_basic_Palette0() {
	SDL_Color colors[256];
	for (int i = 0; i < 256; i++) {
		tempPalettebuffer[i * 3] = i / 4;
		tempPalettebuffer[i * 3 + 1] = i / 4;
		tempPalettebuffer[i * 3 + 2] = i / 4;
		colors[i].r = tempPalettebuffer[i * 3];
		colors[i].g = tempPalettebuffer[i * 3 + 1];
		colors[i].b = tempPalettebuffer[i * 3 + 2];
	}
	SetPalette(colors);
}
void Set_basic_Palette1() {
	SDL_Color colors[256];
	for (int i = 0; i < 256; i++) {
		if (i != 0)
		{
			colors[i].r = 0;
			colors[i].g = 0;
			colors[i].b = 0;
		}
		else
		{
			colors[i].r = 255;
			colors[i].g = 255;
			colors[i].b = 255;
		}
		tempPalettebuffer[i * 3] = colors[i].r / 4;
		tempPalettebuffer[i * 3 + 1] = colors[i].g / 4;
		tempPalettebuffer[i * 3 + 2] = colors[i].b / 4;
	}
	SetPalette(colors);
}

void Set_basic_Palette3() {
	SDL_Color colors[256];
	for (int i = 0; i < 256; i++) {
		tempPalettebuffer[i * 3] = i;
		tempPalettebuffer[i * 3 + 1] = ((int)(i / 16)) * 16;
		tempPalettebuffer[i * 3 + 2] = (i % 16) * 16;
		colors[i].r = tempPalettebuffer[i * 3];
		colors[i].g = tempPalettebuffer[i * 3 + 1];
		colors[i].b = tempPalettebuffer[i * 3 + 2];
	}
	SetPalette(colors);
}

void putpixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {
	case 1:
		*p = pixel;
		break;

	case 2:
		*(Uint16*)p = pixel;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
		}
		else {
			p[0] = pixel & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = (pixel >> 16) & 0xff;
		}
		break;

	case 4:
		*(Uint32*)p = pixel;
		break;
	}
}

void Draw_debug_matrix0() {
	SDL_Rect dstrect;
	if (SDL_MUSTLOCK(m_gamePalletisedSurface)) {
		if (SDL_LockSurface(m_gamePalletisedSurface) < 0) {
			fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
			return;
		}
	}

	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 16; j++)
		{
			dstrect.x = i * m_gamePalletisedSurface->w / 16;
			dstrect.y = j * m_gamePalletisedSurface->h / 16;
			dstrect.w = m_gamePalletisedSurface->w / 16;
			dstrect.h = m_gamePalletisedSurface->h / 16;
			SDL_FillRect(m_gamePalletisedSurface, &dstrect, i * 16 + j/*SDL_MapRGB(screen->format, i*16+j, 0, 0)*/);
		}

	if (SDL_MUSTLOCK(m_gamePalletisedSurface)) {
		SDL_UnlockSurface(m_gamePalletisedSurface);
	}
	SubBlit(m_iOrigw, m_iOrigh);
};

void Draw_black() {
	SDL_Rect dstrect;
	if (SDL_MUSTLOCK(m_gamePalletisedSurface)) {
		if (SDL_LockSurface(m_gamePalletisedSurface) < 0) {
			fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
			return;
		}
	}

	dstrect.x = 0;
	dstrect.y = 0;
	dstrect.w = m_gamePalletisedSurface->w;
	dstrect.h = m_gamePalletisedSurface->h;
	SDL_FillRect(m_gamePalletisedSurface, &dstrect, 1);

	if (SDL_MUSTLOCK(m_gamePalletisedSurface)) {
		SDL_UnlockSurface(m_gamePalletisedSurface);
	}
	SubBlit(m_iOrigw, m_iOrigh);
};

void Draw_debug_matrix1() {
	SDL_Rect dstrect;
	if (SDL_MUSTLOCK(m_gamePalletisedSurface)) {
		if (SDL_LockSurface(m_gamePalletisedSurface) < 0) {
			fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
			return;
		}
	}

	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 16; j++)
		{
			dstrect.x = i * m_gamePalletisedSurface->w / 16;
			dstrect.y = j * m_gamePalletisedSurface->h / 16;
			dstrect.w = m_gamePalletisedSurface->w / 16;
			dstrect.h = m_gamePalletisedSurface->h / 16;
			SDL_FillRect(m_gamePalletisedSurface, &dstrect, 1);
		}

	if (SDL_MUSTLOCK(m_gamePalletisedSurface)) {
		SDL_UnlockSurface(m_gamePalletisedSurface);
	}
	SubBlit(m_iOrigw, m_iOrigh);
};

bool VGA_LoadFont()
{
	//Loading success flag
	bool success = true;

	//Load splash image
	std::string fontPath = GetSubDirectoryPath("font/16x16-font.bmp");
	m_surfaceFont = SDL_LoadBMP(fontPath.c_str());
	if (m_surfaceFont == nullptr)
	{
		Logger->error("Unable to load image {}! SDL Error: {}", "16x16-font.bmp", SDL_GetError());
		success = false;
	}

	uint8_t* pixels = (uint8_t*)m_surfaceFont->pixels;
	for (int yy = 0; yy < 256; yy++)
		for (int xx = 0; xx < 256; xx++)
			m_fontBuffer[yy * 256 + xx] = pixels[(yy * 256 + xx) * 3];

	fontPath = GetSubDirectoryPath("font/8x8-font.bmp");
	m_smallSurfaceFont = SDL_LoadBMP(fontPath.c_str());
	if (m_surfaceFont == nullptr)
	{
		Logger->error("Unable to load image {}! SDL Error: {]", "8x8-font.bmp", SDL_GetError());
		success = false;
	}

	pixels = (uint8_t*)m_smallSurfaceFont->pixels;
	for (int yy = 0; yy < 128; yy++)
		for (int xx = 0; xx < 128; xx++)
			m_smallFontBuffer[yy * 128 + xx] = pixels[(yy * 128 + xx) * 3];

	return success;
}

int lastpoz = 0;
int textwidth = 40;
int textheight = 30;

void Draw_letter(int letter_number, int pozx, int pozy) {
	SDL_Rect srcrect;
	SDL_Rect dstrect;
	srcrect.x = 16 * (letter_number % 16);
	srcrect.y = 16 * (int)(letter_number / 16);
	srcrect.w = 16;
	srcrect.h = 16;
	dstrect.x = 16 * pozx;
	dstrect.y = 16 * pozy;
	dstrect.w = 16;
	dstrect.h = 16;
	SDL_BlitSurface(m_surfaceFont, &srcrect, m_gamePalletisedSurface, &dstrect);
};

void Draw_letterToBuffer(int letter_number, int pozx, int pozy, uint8_t* buffer, char fontSize) {
	SDL_Rect srcrect;
	SDL_Rect dstrect;

	int fontSizePixels = 16;
	switch (fontSize)
	{
	case 'S':
		fontSizePixels = 8;
		break;
	default:
		fontSizePixels = 16;
		break;
	}

	srcrect.x = fontSizePixels * (letter_number % 16);
	srcrect.y = fontSizePixels * (letter_number / 16);
	srcrect.w = fontSizePixels;
	srcrect.h = fontSizePixels;
	dstrect.x = pozx;
	dstrect.y = pozy;
	dstrect.w = fontSizePixels;
	dstrect.h = fontSizePixels;

	for (int yy = 0; yy < dstrect.h; yy++)
		for (int xx = 0; xx < dstrect.w; xx++)
		{
			int srcx = xx + srcrect.x;
			int srcy = yy + srcrect.y;

			switch (fontSize)
			{
				case 'S':
				buffer[(dstrect.y + yy) * m_gamePalletisedSurface->w + (dstrect.x + xx)] = m_smallFontBuffer[srcx + (srcy * 128)];
				break;
			default:
				buffer[(dstrect.y + yy) * m_gamePalletisedSurface->w + (dstrect.x + xx)] = m_fontBuffer[srcx + (srcy * 256)];
				break;
			}
		}
};

void VGA_GotoXY(int x, int y) {
	lastpoz = y * textwidth + x;
};
int VGA_WhereX() {
	return(lastpoz % textwidth);
};
int VGA_WhereY() {
	return((int)(lastpoz / textwidth));
};

POSITION VGA_WhereXY() {
	POSITION result;
	result.x = (lastpoz % textwidth);
	result.y = ((int)(lastpoz / textwidth));
	return result;
};

void VGA_Draw_string(char* wrstring) {
	if(!m_gamePalletisedSurface)return;
	//SDL_Rect srcrect = { 0,0,0,0 };
	//SDL_Rect dstrect = { 0,0,0,0 };
	if (SDL_MUSTLOCK(m_gamePalletisedSurface)) {
		if (SDL_LockSurface(m_gamePalletisedSurface) < 0) {
			fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
			return;
		}
	}

	for (uint32_t i = 0; i < strlen(wrstring); i++)
	{
		if (wrstring[i] == '\n')
		{
			lastpoz += textwidth - lastpoz % textwidth;
		}
		else
		{
			if (lastpoz <= textwidth * textheight)
				Draw_letter(wrstring[i], lastpoz % textwidth, (int)(lastpoz / textwidth));
			lastpoz++;
		}
	}

	if (SDL_MUSTLOCK(m_gamePalletisedSurface)) {
		SDL_UnlockSurface(m_gamePalletisedSurface);
	}
	SubBlit(m_iOrigw, m_iOrigh);
	mydelay(10);
}

int drawCounter = 0;
void VGA_Draw_stringXYtoBuffer(const char* wrstring, int x, int y, uint8_t* buffer, char fontSize) {
	if (unitTests)return;

	if (oldWidth != m_gamePalletisedSurface->w)
		drawCounter = 0;
	if (drawCounter < 20)
	{
		drawCounter++;
		return;
	}
	int loclastpoz = 0;
	int fontSizePixels = 16;
	switch (fontSize)
	{
	case 'S':
		fontSizePixels = 8;
		break;
	default:
		fontSizePixels = 16;
		break;
	}
	for (uint32_t i = 0; i < strlen(wrstring); i++)
	{
		if (wrstring[i] == '\n')
		{
			loclastpoz += textwidth - loclastpoz % textwidth;
		}
		else
		{
			Draw_letterToBuffer(wrstring[i], x + (loclastpoz % textwidth) * fontSizePixels, y + (loclastpoz / textwidth) * fontSizePixels, buffer, fontSize);
			loclastpoz++;
		}
	}
}

void VGA_Init(int windowWidth, int windowHeight, int gameResWidth, int gameResHeight, bool maintainAspectRatio, int displayIndex) {
	if (unitTests)return;
	m_bMaintainAspectRatio = maintainAspectRatio;

#define SDL_HWPALETTE 0
	VGA_Init(SDL_HWPALETTE | SDL_INIT_AUDIO, windowWidth, windowHeight, gameResWidth, gameResHeight, maintainAspectRatio, displayIndex);
}

SDL_Rect dst;

void VGA_Resize(int width, int height) {
	m_iOrigw = width;
	m_iOrigh = height;
}

FILE* fptpal;
void SavePal(Uint8* Palettebuffer, char* filename)
{
	fptpal = fopen(filename, "wb");
	fwrite(Palettebuffer, 768, 1, fptpal);
	fclose(fptpal);
}

void VGA_Set_file_Palette(char* filename) {
	uint8_t Palettebuffer[768];
	fptpal = fopen(filename, "rb");
	fread(Palettebuffer, 768, 1, fptpal);
	fclose(fptpal);

	SDL_Color colors[256];
	for (int i = 0; i < 256; i++) {
		colors[i].r = 4 * Palettebuffer[i * 3];
		colors[i].g = 4 * Palettebuffer[i * 3 + 1];
		colors[i].b = 4 * Palettebuffer[i * 3 + 2];
	}
	SetPalette(colors);
}

void VGA_Set_Palette(Uint8* Palettebuffer) {
	memcpy(tempPalettebuffer, Palettebuffer, 768);
	SDL_Color colors[256];
	/* Fill colors with color information */
	for (int i = 0; i < 256; i++) {
		colors[i].r = 4 * Palettebuffer[i * 3];
		colors[i].g = 4 * Palettebuffer[i * 3 + 1];
		colors[i].b = 4 * Palettebuffer[i * 3 + 2];
	}

	SetPalette(colors);
}

void VGA_Set_Palette2(Uint8* Palettebuffer) {
	memcpy(tempPalettebuffer, Palettebuffer, 768);
	SDL_Color colors[256];
	for (int i = 0; i < 256; i++) {
		colors[i].r = Palettebuffer[i * 3];
		colors[i].g = Palettebuffer[i * 3 + 1];
		colors[i].b = Palettebuffer[i * 3 + 2];
	}
	SetPalette(colors);
}

void VGA_Write_basic_Palette(Uint8* Palettebuffer) {
	memcpy(tempPalettebuffer, Palettebuffer, 768);
}

void VGA_test() {
	//int x = m_gamePalletisedSurface->w / 2;
	//int y = m_gamePalletisedSurface->h / 2;

	/* Lock the screen for direct access to the pixels */
	if (SDL_MUSTLOCK(m_gamePalletisedSurface)) {
		if (SDL_LockSurface(m_gamePalletisedSurface) < 0) {
			fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
			return;
		}
	}

	for (int i = 0; i < 600; i++)
		for (int j = 0; j < 400; j++)
			putpixel(m_gamePalletisedSurface, i, j, 127);

	if (SDL_MUSTLOCK(m_gamePalletisedSurface)) {
		SDL_UnlockSurface(m_gamePalletisedSurface);
	}
	/* Update just the part of the display that we've changed */

	SubBlit(m_iOrigw, m_iOrigh);
}

/*

Keyboard scan code/character code combinations returned in AH/AL by
INT 16h functions 00h/01h (standard) and functions 01h/11h (extended)

Key                        Normal           Shift           Ctrl             Alt
						 Std     Ext     Std     Ext     Std     Ext     Std     Ext
Esc                     01/1B   01/1B   01/1B   01/1B   01/1B   01/1B     -     01/00
1 !                     02/31   02/31   02/21   02/21     -       -     78/00   78/00
2 @                     03/32   03/32   03/40   03/40   03/00   03/00   79/00   79/00
3 #                     04/33   04/33   04/23   04/23     -       -     7A/00   7A/00
4 $                     05/34   05/34   05/24   05/24     -       -     7B/00   7B/00
5 %                     06/35   06/35   06/25   06/25     -       -     7C/00   7C/00
6 ^                     07/36   07/36   07/5E   07/5E   07/1E   07/1E   7D/00   7D/00
7 &                     08/37   08/37   08/26   08/26     -       -     7E/00   7E/00
8 *                     09/38   09/38   09/2A   09/2A     -       -     7F/00   7F/00
9 (                     0A/39   0A/39   0A/28   0A/28     -       -     80/00   80/00
0 )                     0B/30   0B/30   0B/29   0B/29     -       -     81/00   81/00
- _                     0C/2D   0C/2D   0C/5F   0C/5F   0C/1F   0C/1F   82/00   82/00
= +                     0D/3D   0D/3D   0D/2B   0D/2B     -       -     83/00   83/00
Backspace               0E/08   0E/08   0E/08   0E/08   0E/7F   0E/7F     -     0E/00
Tab                     0F/09   0F/09   0F/00   0F/00     -     94/00     -     A5/00
q Q                     10/71   10/71   10/51   10/51   10/11   10/11   10/00   10/00
w W                     11/77   11/77   11/57   11/57   11/17   11/17   11/00   11/00
e E                     12/65   12/65   12/45   12/45   12/05   12/05   12/00   12/00
r R                     13/72   13/72   13/52   13/52   13/12   13/12   13/00   13/00
t T                     14/74   14/74   14/54   14/54   14/14   14/14   14/00   14/00
y Y                     15/79   15/79   15/59   15/59   15/19   15/19   15/00   15/00
u U                     16/75   16/75   16/55   16/55   16/15   16/15   16/00   16/00
i I                     17/69   17/69   17/49   17/49   17/09   17/09   17/00   17/00
o O                     18/6F   18/6F   18/4F   18/4F   18/0F   18/0F   18/00   18/00
p P                     19/70   19/70   19/50   19/50   19/10   19/10   19/00   19/00
[ {                     1A/5B   1A/5B   1A/7B   1A/7B   1A/1B   1A/1B     -     1A/00
] }                     1B/5D   1B/5D   1B/7D   1B/7D   1B/1D   1B/1D     -     1B/00
Enter                   lC/0D   1C/0D   1C/0D   1C/0D   1C/0A   1C/0A     -     1C/00
Ctrl                      *       *       *       *       *       *       *       *
a A                     1E/61   1E/61   1E/41   1E/41   1E/01   1E/01   1E/00   1E/00
s S                     1F/73   1F/73   1F/53   1F/53   1F/13   1F/13   1F/00   1F/00
d D                     20/64   20/64   20/44   20/44   20/04   20/04   20/00   20/00
f F                     21/66   21/66   21/46   21/46   21/06   21/06   21/00   21/00
g G                     22/67   22/67   22/47   22/47   22/07   22/07   22/00   22/00
h H                     23/68   23/68   23/48   23/48   23/08   23/08   23/00   23/00
j J                     24/6A   24/6A   24/4A   24/4A   24/0A   24/0A   24/00   24/00
k K                     25/6B   25/6B   25/4B   25/4B   25/0B   25/0B   25/00   25/00
l L                     26/6C   26/6C   26/4C   26/4C   26/0C   26/0C   26/00   26/00
; :                     27/3B   27/3B   27/3A   27/3A     -       -       -     27/00
' "                     28/27   28/27   28/22   28/22     -       -       -     28/00
` ~                     29/60   29/60   29/7E   29/7E     -       -       -     29/00
Left Shift                *       *       *       *       *       *       *       *
\ |                     2B/5C   2B/5C   2B/7C   2B/7C   2B/1C   2B/1C     -     2B/00
z Z                     2C/7A   2C/7A   2C/5A   2C/5A   2C/1A   2C/1A   2C/00   2C/00
x X                     2D/78   2D/78   2D/58   2D/58   2D/18   2D/18   2D/00   2D/00
c C                     2E/63   2E/63   2E/43   2E/43   2E/03   2E/03   2E/00   2E/00
v V                     2F/76   2F/76   2F/56   2F/56   2F/16   2F/16   2F/00   2F/00
b B                     30/62   30/62   30/42   30/42   30/02   30/02   30/00   30/00
n N                     31/6E   31/6E   31/4E   31/4E   31/0E   31/0E   31/00   31/00
m M                     32/6D   32/6D   32/4D   32/4D   32/0D   32/0D   32/00   32/00
, <                     33/2C   33/2C   33/3C   33/3C     -       -       -     33/00
. >                     34/2E   34/2E   34/3E   34/3E     -       -       -     34/00
/ ?                     35/2F   35/2F   35/3F   35/3F     -       -       -     35/00
Right Shift               *       *       *       *       *       *       *       *
*                       37/2A   37/2A   37/2A   37/2A     -     96/00     -     37/00
Alt                       *       *       *       *       *       *       *       *
Space                   39/20   39/20   39/20   39/20   39/20   39/20   39/20   39/20
Caps Lock                 *       *       *       *       *       *       *       *
F1                      3B/00   3B/00   54/00   54/00   5E/00   5E/00   68/00   68/00
F2                      3C/00   3C/00   55/00   55/00   5F/00   5F/00   69/00   69/00
F3                      3D/00   3D/00   56/00   56/00   60/00   60/00   6A/00   6A/00
F4                      3E/00   3E/00   57/00   57/00   61/00   61/00   6B/00   6B/00
F5                      3F/00   3F/00   58/00   58/00   62/00   62/00   6C/00   6C/00
F6                      40/00   40/00   59/00   59/00   63/00   63/00   6D/00   6D/00
F7                      41/00   41/00   5A/00   5A/00   64/00   64/00   6E/00   6E/00
F8                      42/00   42/00   5B/00   5B/00   65/00   65/00   6F/00   6F/00
F9                      43/00   43/00   5C/00   5C/00   66/00   66/00   70/00   70/00
F10                     44/00   44/00   5D/00   5D/00   67/00   67/00   71/00   71/00
Num Lock                  *       *       *       *       *       *       *       *
Scroll Lock               *       *       *       *       *       *       *       *
Home 7                  47/00   47/00   47/37   47/37   77/00   77/00   00/#    00/#
Up Arrow 8              48/00   48/00   48/38   48/38     -     8D/00   00/#    00/#
PgUp 9                  49/00   49/00   49/39   49/39   84/00   84/00   00/#    00/#
-                       4A/2D   4A/2D   4A/2D   4A/2D     -     8E/00     -     4A/00
Left Arrow 4            4B/00   4B/00   4B/34   4B/34   73/00   73/00   00/#    00/#
Center Key 5              -     4C/00   4C/35   4C/35     -     8F/00   00/#    00/#
Right Arrow 6           4D/00   4D/00   4D/36   4D/36   74/00   74/00   00/#    00/#
+                       4E/2B   4E/2B   4E/2B   4E/2B     -     90/00     -     4E/00
End 1                   4F/00   4F/00   4F/31   4F/31   75/00   75/00   00/#    00/#
Down Arrow 2            50/00   50/00   50/32   50/32     -     91/00   00/#    00/#
PgDn 3                  51/00   51/00   51/33   51/33   76/00   76/00   00/#    00/#
Ins 0                   52/00   52/00   52/30   52/30     -     92/00   00/#    00/#
Del .                   53/00   53/00   53/2E   53/2E     -     93/00     -       -
SysReq (84-key only)      *       *       *       *       *       *       *       *
PrtSc/SysReq (101-key)    *       *       *       *     72/00   72/00     *       *
Pause/Break (101-key)     *       *       *       *     00/00   00/00     *       *
F11 (101-key)             -     85/00     -     87/00     -     89/00     -     8B/00
F12 (101-key)             -     86/00     -     88/00     -     8A/00     -     8C/00
Home (101-key)          47/00   47/E0   47/00   47/E0   77/00   77/E0     -     97/00
Up Arrow (101-key)      48/00   48/E0   48/00   48/E0     -     8D/E0     -     98/00
Page Up (101-key)       49/00   49/E0   49/00   49/E0   84/00   84/E0     -     99/00
Left Arrow (101-key)    4B/00   4B/E0   4B/00   4B/E0   73/00   73/E0     -     9B/00
Right Arrow (101-key)   4D/00   4D/E0   4D/00   4D/E0   74/00   74/E0     -     9D/00
End (101-key)           4F/00   4F/E0   4F/00   4F/E0   75/00   75/E0     -     9F/00
Down Arrow (101-key)    50/00   50/E0   50/00   50/E0     -     91/E0     -     A0/00
Page Down (101-key)     51/00   51/E0   51/00   51/E0   76/00   76/E0     -     A1/00
Insert (101-key)        52/00   52/E0   52/00   52/E0     -     92/E0     -     A2/00
Delete (101-key)        53/00   53/E0   53/00   53/E0     -     93/E0     -     A3/00
/ (101-key)             35/2F   E0/2F   35/2F   E0/2F     -     95/00     -     A4/00
Enter (101-key)         1C/0D   E0/0D   1C/0D   E0/0D   1C/0A   E0/0A     -     A6/00
Right Alt (101-key)       *       *       *       *       *       *       *       *
Right Ctrl (101-key)      *       *       *       *       *       *       *       *
\ | (102-key)           56/5C   56/5C   56/7C   56/7C     -       -       -       -

- Keystroke ignored
* Processed by the BIOS
# Alt-keypad number returns the specified character code and scan code 0
*/

void ToggleFullscreen() {
	bool IsFullscreen = SDL_GetWindowFlags(m_window) & SDL_WINDOW_FULLSCREEN_DESKTOP;
	ToggleFullscreen(!IsFullscreen);
}

void ToggleFullscreen(bool fullScreen) {
	
	SDL_DisplayMode dm;
	if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
		SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
		return;
	}

	SDL_SetWindowMinimumSize(m_window, 640, 480);

	if (!fullScreen)
	{
		int top = 0;
		int left = 0;
		int bottom = 0;
		int right = 0;
		SDL_SetWindowFullscreen(m_window, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		SDL_SetWindowBordered(m_window, SDL_TRUE);
		SDL_SetWindowResizable(m_window, SDL_TRUE);
		SDL_GetWindowBordersSize(m_window, &top, &left, &bottom, &right);

		SDL_SetWindowPosition(m_window, left, top);
		if (windowResWidth >= dm.w || windowResHeight >= dm.h)
			SDL_SetWindowSize(m_window, windowResWidth - left - right, windowResHeight - top - bottom);
		else
			SDL_SetWindowSize(m_window, windowResWidth, windowResHeight);

		SDL_RestoreWindow(m_window);
		m_settingWindowGrabbed = false;
	}
	else
	{
		SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		SDL_SetWindowSize(m_window, dm.w, dm.h);
		m_settingWindowGrabbed = true;
	}
	SDL_SetWindowGrab(m_window, m_settingWindowGrabbed ? SDL_TRUE : SDL_FALSE);
}

void ToggleMouseGrabbed() {
	m_settingWindowGrabbed = !m_settingWindowGrabbed;
	SDL_SetWindowGrab(m_window, m_settingWindowGrabbed ? SDL_TRUE : SDL_FALSE);
}

bool HandleSpecialKeys(const SDL_Event &event) {
	bool specialKey = false;
	if ((event.key.keysym.sym == SDLK_RETURN) && (event.key.keysym.mod & KMOD_ALT)) {
		ToggleFullscreen();
		specialKey = true;
	}
	else if ((event.key.keysym.sym == SDLK_F10) && (event.key.keysym.mod & KMOD_CTRL)) {
		ToggleMouseGrabbed();
		specialKey = true;
	}
	return specialKey;
}

int PollSdlEvents()
{
	SDL_Event event;
	Uint8 buttonindex;
	Uint8 buttonstate;
	uint32_t buttonresult;
	gamepad_event_t gpe = {};

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_WINDOWEVENT:
			{
				if (event.window.event == SDL_WINDOWEVENT_EXPOSED || event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					int newWidth = 0;
					int newHeight = 0;
					SDL_GetWindowSize(m_window, &newWidth, &newHeight);
					m_iWindowWidth = newWidth;
					m_iWindowHeight = newHeight;
					if (EventDispatcher::I != nullptr)
						EventDispatcher::I->DispatchEvent<int, int>(EventType::E_WINDOW_SIZE_CHANGE, newWidth, newHeight);
				}
				break;
			}
			case SDL_KEYDOWN:
			{
				m_pressed = true;
				m_lastchar = (event.key.keysym.scancode << 8) + event.key.keysym.sym;

				if (!HandleSpecialKeys(event)) {
					SetPress(true, m_lastchar);
				}
				Logger->trace("Key {} press detected", m_lastchar);
				break;
			}
			case SDL_KEYUP:
			{
				m_lastchar = (event.key.keysym.scancode << 8) + event.key.keysym.sym;
				SetPress(false, m_lastchar);
				Logger->trace("Key {} release detected", m_lastchar);
				break;
			}
			case SDL_MOUSEMOTION:
			{
				SetMouseEvents(1, event.motion.x, event.motion.y);
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			{
				buttonresult = 0;

				buttonindex = event.button.button;
				buttonstate = event.button.state;
				switch (buttonindex)
				{
					case SDL_BUTTON_LEFT:
					{
						switch (buttonstate)
						{
							case SDL_PRESSED:
							{
								buttonresult |= 2;
								break;
							}
							case SDL_RELEASED:
							{
								buttonresult |= 4;
								break;
							}
						}
						break;
					}
					case SDL_BUTTON_RIGHT:
					{
						switch (buttonstate)
						{
							case SDL_PRESSED:
							{
								buttonresult |= 0x8;
								break;
							}
							case SDL_RELEASED:
							{
								buttonresult |= 0x10;
								break;
							}
						}
						break;
					}
				}
				SetMouseEvents(buttonresult, event.motion.x, event.motion.y);
				break;
			}
			case SDL_JOYAXISMOTION:
			{
				if (event.jaxis.which == gpc.controller_id) {
					// motion on controller 0
					//gps.initialized = 1;
					// actual axis data is being read via gamepad_poll_data()
					// to counteract jerkiness due to missing event triggers
					Logger->trace("axis {} event detected", event.jaxis.axis + 1);
				}
				break;
			}
			case SDL_JOYHATMOTION:
			{
				if (event.jhat.which == gpc.controller_id) {
					//gps.initialized = 1;
					// actual axis data is being read via gamepad_poll_data()
					Logger->trace("hat {} event detected", event.jhat.hat + 1);
				}
				break;
			}
			case SDL_JOYBUTTONDOWN:
			{
				if (event.jbutton.which == gpc.controller_id) {
					//gps.initialized = 1;
					gpe.btn_pressed = 1 << (event.jbutton.button + 1);
					Logger->trace("key {} press detected", event.jbutton.button + 1);
					gpe.flag |= GP_BTN_PRESSED;
				}
				break;
			}
			case SDL_JOYBUTTONUP:
			{
				if (event.jbutton.which == gpc.controller_id) {
					//gps.initialized = 1;
					gpe.btn_released = 1 << (event.jbutton.button + 1);
					Logger->trace("key {} release detected", event.jbutton.button + 1);
					gpe.flag |= GP_BTN_RELEASED;
				}
				break;
			}
			case SDL_QUIT: return 0;
		}
	}

	gamepad_poll_data(&gpe);

	return 1;
}

void SetMouseEvents(uint32_t buttons, int16_t x, int16_t y) 
{
	ScaleUpMouseCoordsToVga(x, y);
	MouseEvents(buttons, x, y);
}

void VGA_Set_mouse(int16_t x, int16_t y) 
{
	ScaleDownMouseCoordsToVga(x, y);
	SDL_WarpMouseInWindow(m_window, x, y);
	joystick_set_env(x, y);
}

void ScaleUpMouseCoordsToVga(int16_t& x, int16_t& y)
{
	if (m_iOrigw > 640)
	{
		float fx = (float)m_iOrigw / 640.0f;
		x = fx * x;
	}

	if (m_iOrigh > 480)
	{
		float fy = (float)m_iOrigh / 480.0f;
		y = fy * y;
	}
}

void ScaleDownMouseCoordsToVga(int16_t& x, int16_t& y)
{
	if (m_iOrigw > 640)
	{
		float fx = 640.0f / (float)m_iOrigw;
		x = fx * x;
	}

	if (m_iOrigh > 480)
	{
		float fy = 480.0f / (float)m_iOrigh;
		y = fy * y;
	}
}

void VGA_Blit(Uint8* srcBuffer) {
	if (unitTests)return;

	oldWidth = m_gamePalletisedSurface->w;
	if (CommandLineParams.DoHideGraphics()) return;
	PollSdlEvents();

	if (m_iOrigh != m_gamePalletisedSurface->h || m_iOrigw != m_gamePalletisedSurface->w)
	{
		SDL_RenderClear(m_renderer);
		FreeRenderSurfaces();
		CreateRenderSurfaces(m_iOrigw, m_iOrigh);
		SDL_SetPaletteColors(m_gamePalletisedSurface->format->palette, m_currentPalletColours, 0, 256);

		lastResHeight = m_iOrigh;
	}

	if (SDL_MUSTLOCK(m_gamePalletisedSurface)) {
		if (SDL_LockSurface(m_gamePalletisedSurface) < 0) {
			fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
			return;
		}
	}
	if (srcBuffer)
		memcpy(m_gamePalletisedSurface->pixels, srcBuffer, m_gamePalletisedSurface->h * m_gamePalletisedSurface->w);

	if (SDL_MUSTLOCK(m_gamePalletisedSurface)) {
		SDL_UnlockSurface(m_gamePalletisedSurface);
	}
	SubBlit(m_iOrigw, m_iOrigh);
	SOUND_UPDATE();
}

void SubBlit(uint16_t originalResWidth, uint16_t originalResHeight) {
	while (subBlitLock);//fix problem with quick blitting
	subBlitLock = true;

	SDL_Rect rectSrc;
	rectSrc.x = 0;
	rectSrc.y = 0;
	rectSrc.w = originalResWidth;
	rectSrc.h = originalResHeight;

	SDL_Rect dscrect;
	if (m_bMaintainAspectRatio)
	{
		float widthRatio = (float)m_iWindowWidth / (float)originalResWidth;
		float heightRatio = (float)m_iWindowHeight / (float)originalResHeight;

		if (widthRatio >= heightRatio)
		{
			dscrect.w = (int)((float)originalResWidth * heightRatio);
			dscrect.h = (int)((float)originalResHeight * heightRatio);
			dscrect.x = (m_iWindowWidth - dscrect.w) / 2;
			dscrect.y = 0;
		}
		else
		{
			dscrect.w = (int)((float)originalResWidth * widthRatio);
			dscrect.h = (int)((float)originalResHeight * widthRatio);
			dscrect.x = 0;
			dscrect.y = (m_iWindowHeight - dscrect.h) / 2;
		}
	}
	else
	{
		dscrect.w = m_iWindowWidth;
		dscrect.h = m_iWindowHeight;
		dscrect.x = 0;
		dscrect.y = 0;
	}

	SDL_BlitSurface(m_gamePalletisedSurface, NULL, m_gameRGBASurface, NULL);

	//if (SDL_SaveBMP(m_gameRGBASurface, "m_gameRGBASurface.bmp") != 0)
	//{
	//	// Error saving bitmap
	//	printf("SDL_SaveBMP failed: %s\n", SDL_GetError());
	//}

	SDL_UpdateTexture(m_texture, NULL, m_gameRGBASurface->pixels, m_gameRGBASurface->pitch);
	SDL_RenderCopy(m_renderer, m_texture, &rectSrc, &dscrect);

	SDL_RenderPresent(m_renderer);
	SDL_RenderClear(m_renderer);
	subBlitLock = false;
}

void VGA_Init_test() {//only for debug
	/* Create a display surface with a grayscale palette */
	SDL_Color colors[256];
	//neco
			/* Fill colors with color information */
	for (int i = 0; i < 256; i++) {
		colors[i].r = i;
		colors[i].g = i;
		colors[i].b = i;
	}

	/* Create display */

	if (!m_gamePalletisedSurface) {
		Logger->error("Couldn't set video mode: {}", SDL_GetError());
		exit(-1);
	}

	/* Set palette */

	SDL_SetPaletteColors(m_gamePalletisedSurface->format->palette, colors, 0, 256);

	//int x = m_gamePalletisedSurface->w / 2;
	//int y = m_gamePalletisedSurface->h / 2;

	/* Lock the screen for direct access to the pixels */
	if (SDL_MUSTLOCK(m_gamePalletisedSurface)) {
		if (SDL_LockSurface(m_gamePalletisedSurface) < 0) {
			fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
			return;
		}
	}

	for (int i = 0; i < 600; i++)
		for (int j = 0; j < 400; j++)
			putpixel(m_gamePalletisedSurface, i, j, 127);

	if (SDL_MUSTLOCK(m_gamePalletisedSurface)) {
		SDL_UnlockSurface(m_gamePalletisedSurface);
	}
	/* Update just the part of the display that we've changed */

	SDL_UpdateWindowSurface(m_window);
}

void VGA_Debug_Blit(int width, int height, Uint8* buffer) {
	VGA_Blit(buffer);
}

void VGA_close()
{
	clean_up_sound();
	gamepad_sdl_close();
	SDL_FreeSurface(m_surfaceFont);
	m_surfaceFont = nullptr;
	FreeRenderSurfaces();
	SDL_DestroyRenderer(m_renderer);
	m_renderer = nullptr;
	SDL_DestroyWindow(m_window);
	m_window = nullptr;
	SDL_Quit();
	//free(m_currentPalletColours);
}

void FreeRenderSurfaces()
{
	SDL_FreeSurface(m_gamePalletisedSurface);
	m_gamePalletisedSurface = nullptr;
	SDL_FreeSurface(m_gameRGBASurface);
	m_gameRGBASurface = nullptr;
	SDL_DestroyTexture(m_texture);
	m_texture = nullptr;
}

int16_t VGA_get_shift_status() {
	return 0;
}
bool VGA_check_standart_input_status() {
	bool locpressed = m_pressed;
	//uint16_t loclastchar = lastchar;
	m_pressed = false;
	return locpressed;
}

uint16_t TranslateSdlKeysToGameKeys(uint16_t loclastchar)
{
	auto sdl_char = (loclastchar & 0xff00) >> 8;

	if (m_Scene == Scene::SPELL_MENU ||
		m_Scene == Scene::FLIGHT)
	{
		if (sdl_char == inputMapping.Forward)
			return GameKey::UP;
		if (sdl_char == inputMapping.Backwards)
			return GameKey::DOWN;
		if (sdl_char == inputMapping.Left)
			return GameKey::LEFT;
		if (sdl_char == inputMapping.Right)
			return GameKey::RIGHT;
		if (sdl_char == inputMapping.SpellMenu)
			return GameKey::CTRL;
		if (sdl_char == inputMapping.Map)
			return GameKey::RETURN;
		if (sdl_char == inputMapping.SpellMenuMark)
			return GameKey::LSHIFT;
	}

	switch (sdl_char)
	{
	case SDL_SCANCODE_ESCAPE://esc
		loclastchar = GameKey::ESC;
		break;
	case SDL_SCANCODE_1://1
		loclastchar = GameKey::K1;
		break;
	case SDL_SCANCODE_2://2
		loclastchar = GameKey::K2;
		break;
	case SDL_SCANCODE_3://3
		loclastchar = GameKey::K3;
		break;
	case SDL_SCANCODE_4://4
		loclastchar = GameKey::K4;
		break;
	case SDL_SCANCODE_5://5
		loclastchar = GameKey::K5;
		break;
	case SDL_SCANCODE_6://6
		loclastchar = GameKey::K6;
		break;
	case SDL_SCANCODE_7://7
		loclastchar = GameKey::K7;
		break;
	case SDL_SCANCODE_8://8
		loclastchar = GameKey::K8;
		break;
	case SDL_SCANCODE_9://9
		loclastchar = GameKey::K9;
		break;
	case SDL_SCANCODE_0://0
		loclastchar = GameKey::K0;
		break;
	case SDL_SCANCODE_MINUS://-
	case SDL_SCANCODE_KP_MINUS:
		loclastchar = GameKey::MINUS;
		break;
	case SDL_SCANCODE_EQUALS://=
	case SDL_SCANCODE_KP_PLUS:
		loclastchar = GameKey::EQUALS;
		break;
	case SDL_SCANCODE_BACKSPACE://backspace
		loclastchar = GameKey::BACKSPACE;
		break;
	case SDL_SCANCODE_TAB://tab
	case SDL_SCANCODE_KP_TAB:
		loclastchar = GameKey::TAB;
		break;
	case SDL_SCANCODE_Q://q
		loclastchar = GameKey::Q;
		break;
	case SDL_SCANCODE_W://w
		loclastchar = GameKey::W;
		break;
	case SDL_SCANCODE_E://e
		loclastchar = GameKey::E;
		break;
	case SDL_SCANCODE_R://r
		loclastchar = GameKey::R;
		break;
	case SDL_SCANCODE_T://t
		loclastchar = GameKey::T;
		break;
	case SDL_SCANCODE_Y://y
		loclastchar = GameKey::Y;
		break;
	case SDL_SCANCODE_U://u
		loclastchar = GameKey::U;
		break;
	case SDL_SCANCODE_I://i
		loclastchar = GameKey::I;
		break;
	case SDL_SCANCODE_O://o
		loclastchar = GameKey::O;
		break;
	case SDL_SCANCODE_P://p
		loclastchar = GameKey::P;
		break;
	case SDL_SCANCODE_LEFTBRACKET://[
		loclastchar = GameKey::LEFTBRACKET;
		break;
	case SDL_SCANCODE_RIGHTBRACKET://]
		loclastchar = GameKey::RIGHTBRACKET;
		break;
	case SDL_SCANCODE_RETURN://enter
	case SDL_SCANCODE_RETURN2://enter
		loclastchar = GameKey::RETURN;
		break;
	case SDL_SCANCODE_LCTRL://ctrl
	case SDL_SCANCODE_RCTRL:
		loclastchar = GameKey::CTRL;
		break;
	case SDL_SCANCODE_A://a
		loclastchar = GameKey::A;
		break;
	case SDL_SCANCODE_S://s
		loclastchar = GameKey::S;
		break;
	case SDL_SCANCODE_D://d
		loclastchar = GameKey::D;
		break;
	case SDL_SCANCODE_F://f
		loclastchar = GameKey::F;
		break;
	case SDL_SCANCODE_G://g
		loclastchar = GameKey::G;
		break;
	case SDL_SCANCODE_H://h
		loclastchar = GameKey::H;
		break;
	case SDL_SCANCODE_J://j
		loclastchar = GameKey::J;
		break;
	case SDL_SCANCODE_K://k
		loclastchar = GameKey::K;
		break;
	case SDL_SCANCODE_L://l
		loclastchar = GameKey::L;
		break;
	case SDL_SCANCODE_SEMICOLON://;
		loclastchar = GameKey::SEMICOLON;
		break;
	case SDL_SCANCODE_APOSTROPHE://'
		loclastchar = GameKey::APOSTROPHE;
		break;
	case SDL_SCANCODE_GRAVE://`
		loclastchar = GameKey::GRAVE;
		break;
	case SDL_SCANCODE_LSHIFT://left shift
		loclastchar = GameKey::LSHIFT;
		break;
	case SDL_SCANCODE_BACKSLASH:// "\"
		loclastchar = GameKey::BACKSLASH;
		break;
	case SDL_SCANCODE_Z://z
		loclastchar = GameKey::Z;
		break;
	case SDL_SCANCODE_X://x
		loclastchar = GameKey::X;
		break;
	case SDL_SCANCODE_C://c
		loclastchar = GameKey::C;
		break;
	case SDL_SCANCODE_V://v
		loclastchar = GameKey::V;
		break;
	case SDL_SCANCODE_B://b
		loclastchar = GameKey::B;
		break;
	case SDL_SCANCODE_N://n
		loclastchar = GameKey::N;
		break;
	case SDL_SCANCODE_M://m
		loclastchar = GameKey::M;
		break;
	case SDL_SCANCODE_COMMA://,
		loclastchar = GameKey::COMMA;
		break;
	case SDL_SCANCODE_PERIOD://.
		loclastchar = GameKey::PERIOD;
		break;
	case SDL_SCANCODE_SLASH:// "/"
		loclastchar = GameKey::SLASH;
		break;
	case SDL_SCANCODE_RSHIFT://right shift
		loclastchar = GameKey::RSHIFT;
		break;
	case SDL_SCANCODE_LALT://alt
	case SDL_SCANCODE_RALT:
		loclastchar = GameKey::ALT;
		break;
	case SDL_SCANCODE_SPACE://space
		loclastchar = GameKey::SPACE;
		break;
	case SDL_SCANCODE_F1://f1
		loclastchar = GameKey::F1;
		break;
	case SDL_SCANCODE_F2://f2
		loclastchar = GameKey::F2;
		break;
	case SDL_SCANCODE_F3://f3
		loclastchar = GameKey::F3;
		break;
	case SDL_SCANCODE_F4://f4
		loclastchar = GameKey::F4;
		break;
	case SDL_SCANCODE_F5://f5
		loclastchar = GameKey::F5;
		break;
	case SDL_SCANCODE_F6://f6
		loclastchar = GameKey::F6;
		break;
	case SDL_SCANCODE_F7://f7
		loclastchar = GameKey::F7;
		break;
	case SDL_SCANCODE_F8://f8
		loclastchar = GameKey::F8;
		break;
	case SDL_SCANCODE_F9://f9
		loclastchar = GameKey::F9;
		break;
	case SDL_SCANCODE_F10://f10
		loclastchar = GameKey::F10;
		break;
	case SDL_SCANCODE_HOME://home
		loclastchar = GameKey::HOME;
		break;
	case SDL_SCANCODE_UP://up
		loclastchar = GameKey::UP;
		break;
	case SDL_SCANCODE_PAGEUP://pageup
		loclastchar = GameKey::PAGEUP;
		break;
	case SDL_SCANCODE_RIGHT://right
		loclastchar = GameKey::RIGHT;
		break;
	case SDL_SCANCODE_DOWN://down
		loclastchar = GameKey::DOWN;
		break;
	case SDL_SCANCODE_LEFT://left
		loclastchar = GameKey::LEFT;
		break;
	case SDL_SCANCODE_END://end
		loclastchar = GameKey::END;
		break;
	case SDL_SCANCODE_PAGEDOWN://pagedown
		loclastchar = GameKey::PAGEDOWN;
		break;
	case SDL_SCANCODE_INSERT://ins
		loclastchar = GameKey::INSERT;
		break;
	case SDL_SCANCODE_DELETE://del
		loclastchar = GameKey::DEL;
		break;
	}
	return loclastchar;
}

void VGA_cleanKeyBuffer() {
	uint16_t loclastchar = m_lastchar;
	m_lastchar = 0;
	loclastchar = TranslateSdlKeysToGameKeys(loclastchar);
	while (loclastchar != 0)
	{
		loclastchar = m_lastchar;
		m_lastchar = 0;
		loclastchar = TranslateSdlKeysToGameKeys(loclastchar);
	}
}

uint16_t VGA_read_char_from_buffer() {
	//bool locpressed = pressed;
	uint16_t loclastchar = m_lastchar;
	m_lastchar = 0;
	loclastchar = TranslateSdlKeysToGameKeys(loclastchar);
	return loclastchar;
}

void SetPress(bool locpressed, uint16_t loclastchar) {
	loclastchar = TranslateSdlKeysToGameKeys(loclastchar);

	if (locpressed)
	{
		LastPressedKey_1806E4 = (loclastchar & 0xff00) >> 8;// VGA_read_char_from_buffer();
		pressedKeys_180664[LastPressedKey_1806E4 & 0x7F] = LastPressedKey_1806E4;
	}
	else
	{
		pressedKeys_180664[((loclastchar & 0xff00) >> 8) & 0x7F] = 0;
	}
}

void VGA_mouse_clear_keys() {
	for (int i = 0; i < 128; i++)
		pressedKeys_180664[i] = 0;
}
