#ifdef _WIN64

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>

#define fImageW 16.0f
#define fImageH 8.0f

typedef signed int s32;
typedef unsigned int u32;
typedef signed long long s64;
typedef unsigned long long u64;

typedef struct sVec2d {
	float x;
	float y;
} sVec2d;

typedef struct sRect {
	sVec2d p;
	sVec2d d;
	sVec2d v;
	wchar_t sym; // 0x2588 0x2593 0x2592 0x2591 ' '
} sRect;

typedef struct sImage {
	sRect r;
	BOOL reset;
	wchar_t Image[(int)fImageH][(int)fImageW];
} sImage;

typedef struct Window{
    BOOL bRunning;

    float ScreenWidth;
    float ScreenHeight;

    sRect screen_rect;
    wchar_t* screen;
    HANDLE hConsole;
    DWORD dwBytesWritten;
    float fElapsedTime;
    float Fps;

    void (*SetUp)();
    void (*Update)();
} Window; 



BOOL bounce(float x,float y,float lx,float ly){
	if(x>=0&&x<lx&&y>=0&&y<ly) return TRUE;
	return FALSE;
}
wchar_t GetChar(sImage img,float x,float y) {
	float nx = (int)x / img.r.d.x * fImageW;
	float ny = (int)y / img.r.d.y * fImageH;
	if(bounce(nx,ny,fImageW,fImageH)){
		return img.Image[(int)ny][(int)nx];
	}
	return L' ';
}
void RenderImage(Window *w,sImage img) {
	for (float x = (int)img.r.p.x; x < (int)img.r.p.x + img.r.d.x; x+=1.0f) {
		for (float y = (int)img.r.p.y; y < (int)img.r.p.y + img.r.d.y; y+=1.0f) {
			if(bounce(x,y,w->ScreenWidth,w->ScreenHeight)){
				float nx = x - (int)img.r.p.x;
				float ny = y - (int)img.r.p.y;
				wchar_t c = GetChar(img,nx,ny);
				if(c==L' ') continue;
				w->screen[(int)((int)y * w->ScreenWidth + x)] = c;
			}
		}
	}
}
void RenderRect(Window *w,sRect rect) {
	for (int x = rect.p.x; x < rect.p.x + rect.d.x; x++) {
		for (int y = rect.p.y; y < rect.p.y + rect.d.y*0.5f; y++) {
			if(bounce(x,y,w->ScreenWidth,w->ScreenHeight)){
				w->screen[(int)(y * w->ScreenWidth + x)] = rect.sym;
			}
		}
	}
}
void RenderString(Window *w,wchar_t buffer[],size_t buffer_size,float px,float py) {
	for (int x = px; x < px+buffer_size; x++) {
		if(bounce((float)x,py,w->ScreenWidth,w->ScreenHeight)){
			w->screen[(int)(py * w->ScreenWidth + x)] = buffer[x];
		}
	}
}

BOOL KeyDown(unsigned short Key){
    return GetAsyncKeyState(Key) & 0x8000;
}
BOOL KeyPressed(unsigned short Key){
    return GetAsyncKeyState(Key) & 0x0001;
}
BOOL KeyReleased(unsigned short Key){
    return GetAsyncKeyState(Key) & 0x0000;
}
void ClearScreen(Window *w){
	RenderRect(w,w->screen_rect);
}
void Render(Window* w){
	swprintf_s(w->screen, 10, L"FPS=%3.2f ", w->Fps);
    w->screen[(int)(w->ScreenWidth * w->ScreenHeight - 1)] = '\0';
	WriteConsoleOutputCharacterW(w->hConsole, w->screen, w->ScreenWidth * w->ScreenHeight, (COORD){ 0,0 }, &(w->dwBytesWritten));
}
int Exit(Window *w){
	w->bRunning = FALSE;
	free(w->screen);
	free(w);
	return 0;
}
Window* Init(float width,float height,void (*SetUp)(),void (*Update)()){
    Window *w = (Window*)calloc(1,sizeof(Window));
    w->ScreenWidth = width;
    w->ScreenHeight = height;
    w->bRunning = TRUE;
    w->screen_rect = (sRect){ { 0.0f,0.0f },{ w->ScreenWidth,w->ScreenHeight },{ 0.0f,0.0f },L' ' };
    w->SetUp = SetUp;
    w->Update = Update;

    w->screen = (wchar_t*)calloc((int)(w->ScreenWidth * w->ScreenHeight),sizeof(wchar_t));
	w->hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	if (!w->hConsole) {
		printf("Couldn't create ScreenBuffer!\n");
		w->bRunning = FALSE;
		free(w->screen);
		free(w);
		return NULL;
	}
	if (!SetConsoleActiveScreenBuffer(w->hConsole)) {
		printf("Couldn't set ScreenBuffer!\n");
		w->bRunning = FALSE;
		free(w->screen);
		free(w);
		return NULL;
	}
    return w;
}
void Start(Window *w){
    w->SetUp();
	
	clock_t tp1 = clock();

	while (w->bRunning)
	{
		clock_t tp2 = clock();
		w->fElapsedTime = (float)(tp2-tp1)/CLOCKS_PER_SEC; 
        w->Fps = 1.0f / w->fElapsedTime;
		tp1 = tp2;

		if(KeyDown('E')){
			Exit(w);
			exit(0);
		}
        w->Update();
        Render(w);
        Sleep(10);
	}

	free(w->screen);
	w->screen = NULL;
	free(w);
	w = NULL;
}

#else
#error "TerminalWindow.h is only defined for Windows!"
#endif