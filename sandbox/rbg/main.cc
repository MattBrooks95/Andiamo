#include<iostream>

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>

using namespace std;


int main(){

	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	SDL_Window* window = SDL_CreateWindow(0,0,0,1200,600,0);
	if(window == NULL) cout << SDL_GetError() << endl;
	SDL_Renderer* renderer = SDL_CreateRenderer(window,-1,0);
	if(renderer == NULL) cout << SDL_GetError() << endl;

	SDL_Surface* bg_surf = IMG_Load("background.png");
	if(bg_surf == NULL) cout << SDL_GetError() << endl;
	SDL_Texture* bg_tex = SDL_CreateTextureFromSurface(renderer,bg_surf);
	if(bg_tex == NULL) cout << SDL_GetError() << endl;
	SDL_RenderCopy(renderer,bg_tex,NULL,NULL);

	SDL_Rect test = {200,200,0,0};

	Uint32 red; Uint32 green; Uint32 blue; Uint32 alpha;
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		red   = 0xff000000;
		green = 0x00ff0000;
		blue  = 0x0000ff00;
		alpha = 0x000000ff;
	#else
		red   = 0x000000ff;
		green = 0x0000ff00;
		blue  =	0x00ff0000;
		alpha = 0xff000000;
	#endif


	//variables for surface that is blitted
	string blit_me = "I love Ghost in the Shell!";
	SDL_Color color = {0,255,0,255};
	TTF_Font* sdl_font = TTF_OpenFont("LiberationSerif-Bold.ttf",28);
	if(sdl_font == NULL) cout << SDL_GetError() << endl;

	TTF_SizeText(sdl_font,blit_me.c_str(),&test.w,&test.h);
	cout << test.w << " " << test.h << endl;
	SDL_Surface* help = SDL_CreateRGBSurface(0,test.w,test.h,32,red,green,blue,alpha);
	SDL_Surface* words = TTF_RenderText_Solid(sdl_font,blit_me.c_str(),color);
	SDL_Color color2 = {0,0,255,255};
	if(SDL_FillRect(help,NULL,0xffff0000) != 0) cout << SDL_GetError() << endl;
	cout << test.w << " " << test.h << endl;
	if(SDL_BlitSurface(words,NULL,help,NULL) != 0) cout << "Error with blit: " << SDL_GetError() << endl;
	SDL_Texture* help_tex = SDL_CreateTextureFromSurface(renderer,help);
	SDL_RenderCopy(renderer,help_tex,NULL,&test);

	SDL_RenderPresent(renderer);

	SDL_Delay(5000);
	SDL_FreeSurface(bg_surf);
	SDL_DestroyTexture(bg_tex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	return 0; //exit success
}













