#ifdef __linux__

#include "/home/codeleaded/System/Static/Library/TerminalEngine.h"

void Update(TerminalEngine* te){
    //TerminalCtl_Clear(&te->tc);

    if(InputMouse_Get(&te->im,INPUTMOUSE_BUTTON_L).DOWN){
        if(te->im.posX>=0 && te->im.posX<te->tc.width && te->im.posY>=0 && te->im.posY<te->tc.height)
            TerminalCtl_SetPixel(
                &te->tc,
                (Term_Coord){ te->im.posX,te->im.posY },
                L'A',
                Term_Color_New(0,0,0,0,0,0)
            );
    }
    if(InputKeyboard_Get(&te->ikb,INPUTKEYBOARD_KEY_Q).PRESSED){
        te->Running = 0;
    }
}

int main() {
    
    TerminalEngine tc = TerminalEngine_New(120,90);
    TerminalEngine_Start(&tc,Update);
    TerminalEngine_Free(&tc);
    
    return 0;
}

#elif defined _WIN64

#include "TerminalWindow.h"

Window *w;

void Setup(AlxWindow* w){
	
}
void Update(){
	ClearScreen(w);
	wchar_t s[] = L"Hello String\0";
	RenderString(w,s,13,10,20);
}

int main(int argc,char **argv){
	w = Init(120.0f,120.0f,SetUp,Update);
	if(w!=NULL) Start(w);
	return 0;
}

#endif
