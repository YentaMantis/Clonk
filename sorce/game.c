
#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string.h>


#include <SDL_mixer.h>
#include <SDL_audio.h>

#include <assert.h>
#include <time.h>

#include <windows.h>


#undef main

#include "game.h"





struct Gdata {
	int xfallpos;
	int TileOffset;	
	bool saved_block;		
	bool saveblock;			
	bool drop_saved;		
	bool cvar;				
	int saved_block_offset;
	int gamemode;
	int tilemap[10][15];

};

int Initialise() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		SDL_GetError();
		return 1;
	}
	return 0;
}
bool COL_LR(int xp, float yp, struct Gdata* gdata) {
	if (gdata->tilemap[xp/TILEWH][(int)ceil(yp / TILEWH)] != -1) {
		return false;
	}
	return true;
}


int DetectMatches(int lcl, struct Gdata* gdata) {
	int countCube = 0;
	for (int y = 0; y < 15; y++) {
		for (int x = 0; x < 10; x++) {		
			if (gdata->tilemap[x][y] != -1) {
				switch (gdata->tilemap[x][y]) {
				case ORB:
					if (y >= 3 && y < 12) {
						if (gdata->tilemap[x][y + 1] == ORB && gdata->tilemap[x][y + 2] == ORB && gdata->tilemap[x][y + 3] == ORB) {
							for (int i = 0; i < 4; i++) {
								gdata->tilemap[x][y + i] = EMPTY;
								gdata->tilemap[x][y + i] = EMPTY;
							}
							lcl++;
						}
					}
					break;
				case CROSS:
					if (x >= 2) {
						if (gdata->tilemap[x - 1][y] == CROSS && gdata->tilemap[x - 2][y] == CROSS) {
							for (int i = 0; i < 3; i++) {
								gdata->tilemap[x - i][y] = EMPTY;
							}
							lcl++;
						}
					}
					break;
				case GEM:
					if (x >= 3) {
						if (gdata->tilemap[x - 1][y + 1] == GEM && gdata->tilemap[x - 2][y + 2] == GEM && gdata->tilemap[x - 3][y + 3] == GEM) {
							for (int i = 0; i < 4; i++) {
								gdata->tilemap[x - i][y + i] = EMPTY;
							}
							lcl++;
						}
					}
					if (gdata->tilemap[x + 1][y + 1] == GEM && gdata->tilemap[x + 2][y + 2] == GEM && gdata->tilemap[x + 3][y + 3] == GEM) {
						for (int i = 0; i < 4; i++) {
							gdata->tilemap[x + i][y + i] = EMPTY;
						}
						lcl++;
					}
					break;
					
				case VORTEX:
					if (x < 9) {
						if (gdata->tilemap[x + 1][y] == VORTEX && gdata->tilemap[x + 1][y + 1] == VORTEX && gdata->tilemap[x][y + 1] == VORTEX) {
							gdata->tilemap[x][y] = EMPTY; gdata->tilemap[x + 1][y] = EMPTY; gdata->tilemap[x + 1][y + 1] = EMPTY; gdata->tilemap[x][y + 1] = EMPTY;
							lcl++;
						}

					}
					break;
					
				
				case CUBE:
					if (x > 0 < 9) {
						if (gdata->tilemap[x + 1][y] == CUBE && gdata->tilemap[x][y + 1] == CUBE && gdata->tilemap[x - 1][y] == CUBE && gdata->tilemap[x][y - 1] == CUBE) {
							gdata->tilemap[x + 1][y] = EMPTY; gdata->tilemap[x][y + 1] = EMPTY; gdata->tilemap[x - 1][y] = EMPTY; gdata->tilemap[x][y - 1] = EMPTY;
							gdata->tilemap[x][y] = EMPTY;
							lcl++;
						}
					}
					break;
			}
			}else {
				continue;
			}
			
			if (y != 14 && gdata->tilemap[x][y + 1] == -1) {
				gdata->tilemap[x][y + 1] = gdata->tilemap[x][y];
				gdata->tilemap[x][y] = -1;
			}
			
		}
	}
	return lcl;
}



bool Collision(float yp, struct Gdata* gdata) {
	for (int x = 0; x < 10; x++) {
		for (int y = 0; y < 15; y++) {
			if (gdata->tilemap[gdata->xfallpos / TILEWH][(int)yp / TILEWH + 1] != -1) {
				return true;
			}
		}
	}
	return false;
}

void DeInit(SDL_Renderer* renderer, SDL_Window* window) {
	SDL_Quit();
	TTF_Quit();
	Mix_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}


void ThrowText(SDL_Renderer* renderer,char* fpath, SDL_Color color, char* txt, int x, int y, int pt, bool center) {
	
	TTF_Font* font = TTF_OpenFont(fpath, pt);
	SDL_Surface* surf = TTF_RenderUTF8_Solid(font, txt, color);
	SDL_Texture* textT = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_Rect txtRext = { x,y,surf->w,surf->h };
	if (center) { txtRext.x = ((WIN_WIDTH / 2) - (surf->w/2)); }
	SDL_RenderCopy(renderer, textT, NULL, &txtRext);
	SDL_DestroyTexture(textT);
	SDL_FreeSurface(surf);
	TTF_CloseFont(font);
}


int IntegerRandom(int maxrange) {
	return (rand() % maxrange);
}

bool DropTileFunction(SDL_Renderer* renderer, char* tileset, bool Drop, float y, Mix_Chunk* SFX, struct Gdata* gdata) {
	int x=1;
	int offset=0;
	int a; 
	int b;

	if (Drop) {
		gdata->xfallpos = 5 * TILEWH;
		gdata->TileOffset = IntegerRandom(5) * TILEWH;
		x = gdata->xfallpos; offset = gdata->TileOffset;
	}
	else if (gdata->cvar == false) {
		x = gdata->xfallpos; offset = gdata->TileOffset;

	}else {
		x = gdata->xfallpos; 
		offset = gdata->saved_block_offset;
	}
	SDL_Texture* texture = Texture(renderer, tileset, 255);

	SDL_Texture* texture_g = Texture(renderer, tileset, 100);
	SDL_FRect InGameTile = { (x + WIN_WIDTH / 2) - (float)(TILEWH * 5), y, TILEWH, TILEWH };

	SDL_Rect Tile = { offset, 0, TILEWH, TILEWH };
	if (InGameTile.y + InGameTile.w >= WIN_HEIGHT || Collision(InGameTile.y, gdata)) {
		b = round((InGameTile.y / TILEWH));
		a = x / TILEWH;
		Mix_PlayChannel(1, SFX, 0);

		gdata->tilemap[a][b] = offset / TILEWH;
		if (gdata->drop_saved == true && gdata->cvar == true) {
			gdata->cvar = false;
			gdata->saved_block_offset = (Uint8)(23 * TILEWH);
		}
	
		return true;
	}
	if (gdata->saveblock == true && (gdata->saved_block_offset / TILEWH) > 5) {
		gdata->saved_block_offset = Tile.x;
		gdata->saveblock = false;
		gdata->drop_saved = true;
		return true;
	}
	else if ((gdata->saved_block_offset / TILEWH) < 5 && gdata->saveblock == false) {
		SDL_Rect SavedTileRect = { 64, 150, TILEWH, TILEWH };
		SDL_Rect SavedTileRectS = { gdata->saved_block_offset, 0, TILEWH, TILEWH };
		SDL_RenderCopy(renderer, texture, &SavedTileRectS, &SavedTileRect);
	}
	else if (gdata->saveblock == true) {
		Tile.x = gdata->saved_block_offset;
		gdata->saveblock = false;
		gdata->drop_saved = true;
		gdata->cvar = true;
	}


	SDL_FRect GuideTile = { (x + WIN_WIDTH / 2) - (float)(TILEWH * 5), DropDistFunc(x, y, gdata),TILEWH, TILEWH };
	SDL_RenderCopyF(renderer, texture_g, &Tile, &GuideTile);

	SDL_RenderCopyF(renderer, texture, &Tile, &InGameTile);


	
	SDL_DestroyTexture(texture);
	SDL_DestroyTexture(texture_g);

	return false;
}

void DrawDropped(SDL_Renderer* renderer, char* tileset, struct Gdata* gdata) {
	
	SDL_Texture* texture = Texture(renderer, tileset, 255);
	for (int x = 0; x < 10; x++) {
		for (int y = 0; y < 15; y++) {
			if (gdata->tilemap[x][y] == -1) { continue; };
			SDL_Rect slice = { TILEWH * gdata->tilemap[x][y], 0, TILEWH, TILEWH};
			SDL_FRect GameRect = { (x * 32) + (WIN_WIDTH/2) - (float)(TILEWH*5), y * 32.0f, TILEWH, TILEWH};
			SDL_RenderCopyF(renderer, texture, &slice, &GameRect);
		}
	}
	SDL_DestroyTexture(texture);
}

void ClearTheTileMap(struct Gdata* gdata) {
	for (int x = 0; x < 10; x++) {
		for (int y = 0; y < 15; y++) {
			gdata->tilemap[x][y] = -1;
		}
	}
}


void UpdateLvl(SDL_Renderer* renderer, double* lvl, int lcl, double diffmult) {
	static level = 0;
	SDL_Color textcolor = { 0, 0, 0 };
	char buff[255];


	for (int i = 0; i < 6; i++) {
		if (lvl[i] < (lcl * diffmult) && level < i) {
			level++;
		}
	}
	sprintf_s(buff, 0xFF, "%d", level);
	ThrowText(renderer, SSFONT, textcolor, buff, 550, 140, 10, false);
	return;
}
float DropSpeed(SDL_Renderer* renderer, int lcl, double diffmult) {

	double lvl[6] = {
		(diffmult * 6),
		(diffmult * 10),
		(diffmult * 20),
		(diffmult * 50),
		(diffmult * 100),
		(diffmult * 200)
	};
	UpdateLvl(renderer, lvl, lcl, diffmult);


	return (float)(lcl * diffmult + 0.05);

}


float DropDistFunc(int x, float DropY, struct Gdata* gdata) {
	int rx = x / TILEWH;
	int ry = (int)DropY / TILEWH;
	do {
		ry++;
		if (gdata->tilemap[rx][ry] != -1 || ry==15) {
			return (float)((ry*TILEWH) - TILEWH);
		}
	} while (true);
}


//
SDL_Rect UIbutton(SDL_Renderer* renderer, int x, int y, int width, int height, Uint8 btn, Uint8 alpha){
	SDL_Rect srcR = { btn * 64, 0, 64, 32 };
	SDL_Rect DesR = { x, y, width, height };
	SDL_Texture* texture = Texture(renderer, "./Assets/MEnuItems.bmp", alpha);
	SDL_RenderCopy(renderer, texture, &srcR, &DesR);
	if (alpha == 200) {
		srcR.x = 0; srcR.y = 0; DesR.w += 4; DesR.h += 4; DesR.y -= 2; DesR.x -= 2;
		SDL_RenderCopy(renderer, Texture(renderer, "./Assets/overlay.bmp", 255),&srcR, &DesR);
	}
	SDL_DestroyTexture(texture);
	return DesR;
}

int MouseCollision(SDL_Rect* ptr, unsigned int mx, unsigned int my, int count) {
	int i = 0;
	for (int i = 0; i < count; i++) {
		if ((int)mx > ptr[i].x && (int)mx < (ptr[i].x + ptr[i].w) && (int)my > ptr[i].y && (int)my < (ptr[i].y + ptr[i].h)) {
				return (i + 1);
		}
	}
	return -1;
}

void RenderGeneral(SDL_Renderer* renderer, int sx, int sy, int sw, int sh, int x, int y, int w, int h, char* path, Uint8 opacity) {
	SDL_Rect srcrect = {sx, sy, sw, sh};
	SDL_Rect destrect = {x, y, w, h};
	SDL_Texture* texture = Texture(renderer, path, opacity);
	SDL_RenderCopy(renderer, texture, &srcrect, &destrect);
	SDL_DestroyTexture(texture);
}

int main() {
	// struct define 

	struct Gdata gdata;
	struct Gdata* gdata_ptr = &gdata;


	gdata_ptr->saveblock = false;
	gdata_ptr->saved_block = false;
	gdata_ptr->saved_block_offset = 23*TILEWH;
	gdata_ptr->drop_saved = false;
	gdata_ptr->cvar = false;
	srand((unsigned int)time(NULL));
	
	double diffmult = 0.06;
	ClearTheTileMap(gdata_ptr);
	
	// Framerate varibles
	const int framerate = FRAMERATE;
	const int frameDelay = 1000 / framerate;
	Uint32 framestart;			
	int frametime;
	int RenderedEnemyWorldIndex = 0;

	// Gameplay varibles
	bool Drop = true;

	float DropY=0;
	int lcl=0;
	unsigned int score=0;


	// MenuCollisions and mouse
	unsigned int mouse_x = 0;
	unsigned int mouse_y = 0;
	bool leftclick = false;
	Uint8 res;
	Uint8 opacity[5] = {255, 255, 255, 255, 255};

	// menu button locations
	int Mdimensions[16] = {
		 ((WIN_WIDTH / 2) - 90),160,180,60,
		 ((WIN_WIDTH / 2) - 90),222,180,60,
		 ((WIN_WIDTH / 2) - 90),284,180,60,
		 ((WIN_WIDTH / 2) - 90),346,180,60
	};


	gdata_ptr->gamemode = GAME_MAIN_MENU;
	// Lib Init
	TTF_Init();

	Mix_Init(0);

	Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 4096);


	// Audio Setup
	Mix_AllocateChannels(16);
	Mix_Volume(1, 64);
	Mix_Volume(2, 20);
	Mix_Chunk* Sound[10] = {
		Mix_LoadWAV("./sfx/bonk.wav"),
		Mix_LoadWAV("./sfx/click.wav"),
		Mix_LoadWAV("./sfx/metal.wav")
	};
	Mix_Music* music[2] = {
		Mix_LoadMUS("./Music/Theme.wav"),
		Mix_LoadMUS("./Music/MenuTheme.wav")
	};

	Mix_PlayMusic(music[1], -1);					


	SDL_Rect* RectPtr;
	SDL_Rect* TempPtr;
	RectPtr = (SDL_Rect*) calloc(4, sizeof(SDL_Rect));
	if (RectPtr == NULL)
		exit(1);

	// strings for int to str conversion using sprint_s
	char strc[255]="";
	char score_string[255]="";

	



	// Init SDL
	Initialise();
	SDL_Window* window = SDL_CreateWindow("Clonk!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_OPENGL);
	if (!window) {
		PostErrorToTerminal();
		return -1;
	}
	SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		PostErrorToTerminal();
		return -1;
	}
	SDL_DisplayMode screen;
	if (SDL_GetDesktopDisplayMode(0, &screen) != 0)
		PostErrorToTerminal();

	// windows fullscreen nonsense

	DEVMODEA devmode = { 0 };
	DWORD PrevWidth = GetSystemMetrics(SM_CXSCREEN);
	DWORD PrevHeight = GetSystemMetrics(SM_CYSCREEN);


	devmode.dmSize = sizeof(DEVMODE);
	devmode.dmPelsWidth = 640;
	devmode.dmPelsHeight = 480;
	devmode.dmFields = DM_PELSHEIGHT | DM_PELSWIDTH;


	



	// background objects and render colour
	SDL_SetRenderDrawColor(renderer, 255, 199, 202, 1);

	bool saveblock = false; 

	// string for displaying score
	char score_[] = "Score:\n";

	// background texture
	SDL_Rect rect = {32*5, 0, TILEWH*10, TILEWH*15};
	SDL_Texture* b_texture = Texture(renderer, "./Assets/bricks.bmp", 255);
	SDL_Event Event;
	SDL_Color textcolor = { 0, 0, 0 };


	// fullscreen flag
	bool notfullscreen = true;



	while (gdata_ptr->gamemode != GAME_QUIT) {
		framestart = SDL_GetTicks();   

		while (SDL_PollEvent(&Event) != 0) {
			if (Event.type == SDL_QUIT) {
				gdata_ptr->gamemode = GAME_QUIT;
			}
			
			if (Event.type == SDL_MOUSEBUTTONDOWN) {
				if (Event.button.button == RI_MOUSE_LEFT_BUTTON_DOWN) {
					leftclick = true;
				}
			}
			if (Event.type == SDL_KEYDOWN) {
				switch (Event.key.keysym.sym) {

				case SDLK_ESCAPE:
					gdata_ptr->gamemode = GAME_QUIT;
					break;
				case SDLK_f:
					if (notfullscreen) {
						ChangeDisplaySettingsA(&devmode, 0);
						screen.w = devmode.dmPelsWidth;
						screen.h = devmode.dmPelsHeight;
						SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

						notfullscreen = false;
					}
					else { 
					devmode.dmPelsHeight = PrevHeight;
					devmode.dmPelsWidth = PrevWidth;
					
					ChangeDisplaySettingsA(&devmode, 0);
					SDL_SetWindowFullscreen(window, SDL_WINDOW_RESIZABLE); 
					devmode.dmPelsHeight = WIN_WIDTH;
					devmode.dmPelsWidth = WIN_HEIGHT;
					notfullscreen = true; }
					break;
				case SDLK_LEFT:
					if (gdata_ptr->xfallpos > 0 && COL_LR(gdata_ptr->xfallpos - TILEWH, DropY, &gdata)) {
						gdata_ptr->xfallpos -= TILEWH;
						Mix_PlayChannel(2, Sound[1], 0);
					}
						break;

				case SDLK_RIGHT:
					if (gdata_ptr->xfallpos < TILEWH * 9 && COL_LR(gdata_ptr->xfallpos + TILEWH, DropY, &gdata) && gdata_ptr->gamemode == GAME_ENDLESS) {
						Mix_PlayChannel(2, Sound[1], 0);
						gdata_ptr->xfallpos += TILEWH;
					}

					break;



				case SDLK_SPACE:
					if (gdata_ptr->gamemode == GAME_ENDLESS)
						DropY = DropDistFunc(gdata_ptr->xfallpos, DropY, &gdata);
					break;
				case SDLK_c:
				
					gdata_ptr->saveblock = true;
					break;
				case SDLK_q:
					ClearTheTileMap(&gdata);
					Mix_HaltMusic();
					Mix_FadeInMusic(music[1], -1, 2000);
					// reallocating memory
					TempPtr = (SDL_Rect*)realloc(RectPtr, 2 * sizeof(SDL_Rect));
					if (TempPtr == NULL) { exit(1); }
					else if (RectPtr != TempPtr) { RectPtr = TempPtr; }
					TempPtr = NULL;
					gdata_ptr->cvar = false;
					gdata_ptr->saved_block_offset = (Uint8)(23 * TILEWH);
					gdata_ptr->gamemode = GAME_OVER;
					break;
				case SDLK_w:
					lcl++;
					break;
				}


			}
		}
		switch (gdata_ptr->gamemode) {



		case GAME_ENDLESS:

			if (Drop)DropY = 0;
			if (GetAsyncKeyState(VK_DOWN)) { DropY += 2.5; }


			SDL_RenderCopy(renderer, b_texture, NULL, &rect);
			Drop = DropTileFunction(renderer, "./assets/arrows.bmp", Drop, DropY, Sound[0], &gdata);
			DrawDropped(renderer, "./assets/arrows.bmp", &gdata);
			lcl = DetectMatches(lcl, &gdata);
			DropY += DropSpeed(renderer, lcl, diffmult);
			score = lcl * 200;

			sprintf_s(strc, 0xFF, "%d", lcl);
			sprintf_s(score_string, 0xFF, "%d", score);

			ThrowText(renderer, SSFONT, textcolor, "Lineclears:", 490, 10, 20, false);
			ThrowText(renderer, SSFONT, textcolor, strc, 550, 40, 10, false);
			ThrowText(renderer, SSFONT, textcolor, "Score:", 490, 60, 20, false);
			ThrowText(renderer, SSFONT, textcolor, score_string, 550, 90, 10, false);
			ThrowText(renderer, SSFONT, textcolor, "Stored:", 10, 10, 20, false);
			ThrowText(renderer, SSFONT, textcolor, "Level:", 490, 110, 20, false);
			if (((int)DropY / TILEWH) == 0 && gdata_ptr->tilemap[gdata_ptr->xfallpos / TILEWH][(int)DropY / TILEWH] != -1) {

				ClearTheTileMap(&gdata);
				Mix_HaltMusic();
				Mix_FadeInMusic(music[1], -1, 2000);
				// reallocating memory
				TempPtr = (SDL_Rect*)realloc(RectPtr, 2 * sizeof(SDL_Rect));
				if (TempPtr == NULL) { exit(1); }
				else if (RectPtr != TempPtr) { RectPtr = TempPtr; }
				TempPtr = NULL;
				gdata_ptr->cvar = false;
				gdata_ptr->saved_block_offset = (Uint8)(23 * TILEWH);
				gdata_ptr->gamemode = GAME_OVER;
			}
			break;
		case GAME_OVER:
			SDL_GetMouseState(&mouse_x, &mouse_y);
			sprintf_s(strc, 0xFF, "%d", lcl);
			sprintf_s(score_string, 0xFF, "%d", score);

			RenderGeneral(renderer, 0, 0, 640, 480, 0, 0, 640, 480, "./Assets/game_over.bmp", 255);
			ThrowText(renderer, SSFONT, textcolor, "Results!", 0, 0, 50, true);
			ThrowText(renderer, SSFONT, textcolor, "Clears:", 240, 100, 40, false);
			ThrowText(renderer, SSFONT, textcolor, strc, 360, 100, 40, false);
			ThrowText(renderer, SSFONT, textcolor, "Score:", 240, 200, 40, false);
			ThrowText(renderer, SSFONT, textcolor, score_string, 360, 200, 40, false);

			RectPtr[0] = UIbutton(renderer, Mdimensions[0], Mdimensions[1] + 150, Mdimensions[2], Mdimensions[3], 8, opacity[0]);
			RectPtr[1] = UIbutton(renderer, Mdimensions[4], Mdimensions[5] + 150, Mdimensions[6], Mdimensions[7], 3, opacity[1]);
			res = MouseCollision(RectPtr, mouse_x, mouse_y, 2);
			for (int q = 0; q < 4; q++) { opacity[q] = 255; }
			switch (res) {
			case 1:
				if (leftclick) {
					lcl = 0;
					lcl, score = 0;
					Mix_PlayChannel(2, Sound[1], 0);
					Mix_HaltMusic();
					Mix_FadeInMusic(music[1], -1, 2000);
					TempPtr = (SDL_Rect*)realloc(RectPtr, 4 * sizeof(SDL_Rect));
					if (TempPtr == NULL) { exit(1); }
					else if (RectPtr != TempPtr) { RectPtr = TempPtr; }
					TempPtr = NULL;
					gdata_ptr->gamemode = GAME_MAIN_MENU;
				}
				opacity[0] = 200;
				break;
			case 2:
				if (leftclick) {
					lcl = 0;
					lcl, score = 0;
					Mix_PlayChannel(1, Sound[1], 0);
					Mix_HaltMusic();
					Mix_FadeInMusic(music[0], -1, 2000);
					// reallocating memory
					TempPtr = (SDL_Rect*)realloc(RectPtr, 1 * sizeof(SDL_Rect));
					if (TempPtr == NULL) { gdata_ptr->gamemode = GAME_QUIT; }
					else if (RectPtr != TempPtr) { RectPtr = TempPtr; }
					TempPtr = NULL;
					gdata_ptr->gamemode = GAME_ENDLESS;
				}
				opacity[1] = 200;
				break;
			}
			break;
		case GAME_MAIN_MENU:

			SDL_GetMouseState(&mouse_x, &mouse_y);
			RenderGeneral(renderer, 0, 0, 640, 480, 0, 0, 640, 480, "./Assets/menu_back_one.bmp", 255);
			RenderGeneral(renderer, 0, 0, 600, 300, WIN_WIDTH / 2 - 150, 0, 300, 150, "./Assets/logo.bmp", 255);
			if (RectPtr != NULL) {
				RectPtr[0] = UIbutton(renderer, Mdimensions[4], Mdimensions[5], Mdimensions[6], Mdimensions[7], 0, opacity[0]);
				RectPtr[1] = UIbutton(renderer, Mdimensions[8], Mdimensions[9], Mdimensions[10], Mdimensions[11], 2, opacity[3]);
			}
			else {
				exit(1);
			}

			res = MouseCollision(RectPtr, mouse_x, mouse_y, 3);
			for (int q = 0; q < 4; q++) { opacity[q] = 255; }

			switch (res) {
			case 1:
				if (leftclick) {
					Mix_PlayChannel(2, Sound[1], 0);
					Mix_HaltMusic();
					Mix_FadeInMusic(music[0], -1, 2000);
					gdata_ptr->gamemode = GAME_ENDLESS;
				}
				opacity[0] = 200;
				break;


				break;
			case 2:
				if (leftclick) {
					Mix_HaltMusic();
					gdata_ptr->gamemode = GAME_QUIT;
				}
				opacity[3] = 200;
				break;

			}
			break;
		}
		Render(renderer);

		frametime = SDL_GetTicks() - framestart;
		if (frameDelay > frametime) {
			SDL_Delay(frameDelay - frametime);
		}
		leftclick = false;
	}

	Mix_FreeChunk(Sound[0]);
	Mix_FreeChunk(Sound[1]);
	Mix_FreeChunk(Sound[2]);
	free(RectPtr);
	Mix_FreeMusic(music[0]);
	Mix_FreeMusic(music[1]);
	Mix_CloseAudio();
	SDL_DestroyTexture(b_texture);

	DeInit(renderer, window);

	devmode.dmPelsHeight = PrevHeight;
	devmode.dmPelsWidth = PrevWidth;

	ChangeDisplaySettingsA(&devmode, 0);

  	return 0;
}

void PostErrorToTerminal() {
	fprintf(stderr, "Error: %s", SDL_GetError());
}

SDL_Texture* Texture(SDL_Renderer* renderer, char* link, Uint8 alpha) {
	SDL_Surface* surface;
	surface = SDL_LoadBMP(link);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (SDL_SetTextureAlphaMod(texture, alpha) != 0) {
		PostErrorToTerminal();
	}
	SDL_FreeSurface(surface);
	return texture;
}

int Render(SDL_Renderer* renderer) {
	SDL_RenderPresent(renderer);
	SDL_RenderClear(renderer);
	return 0;
}
