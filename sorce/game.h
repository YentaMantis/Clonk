#pragma once
void PostErrorToTerminal();
SDL_Texture* Texture(SDL_Renderer* renderer, char* link, Uint8 alpha);
void LoadFileLineByLine(const char* filepath, char* buff, int arrlenght);
int Initialise();
bool COL_LR(int xp, float yp, struct Gdata* gdata);
int DetectMatches(int lcl, struct Gdata* gdata);
bool Collision(float yp, struct Gdata* gdata);
void DeInit(SDL_Renderer* renderer, SDL_Window* window);
void ThrowText(SDL_Renderer* renderer, char* fpath, SDL_Color color, char* txt, int x, int y, int pt, bool center);
void ClearTheTileMap(struct Gdata* gdata);
int IntegerRandom(int maxrange);
bool DropTileFunction(SDL_Renderer* renderer, char* tileset, bool Drop, float y, Mix_Chunk* SFX, struct Gdata* gdata);
void DrawDropped(SDL_Renderer* renderer, char* tileset, struct Gdata* gdata);
void ClearTheTileMap(struct Gdata* gdata);
int Render(SDL_Renderer* renderer);
void UpdateLvl(SDL_Renderer* renderer, double* lvl, int lcl, double diffmult);
float DropSpeed(SDL_Renderer* renderer, int lcl, double diffmult);
void UpdateLvl(renderer, lvl, lcl, diffmult);
float DropDistFunc(int x, float DropY, struct Gdata* gdata);
SDL_Rect UIbutton(SDL_Renderer* renderer, int x, int y, int width, int height, Uint8 btn, Uint8 alpha);
int MouseCollision(SDL_Rect* ptr, unsigned int mx, unsigned int my, int count);
void RenderGeneral(SDL_Renderer* renderer, int sx, int sy, int sw, int sh, int x, int y, int w, int h, char* path, Uint8 opacity);
#define WIN_WIDTH			640
#define WIN_HEIGHT			480
#define FRAMERATE			60
#define TILEWH				32
#define O					10
#define EMPTY				-1
#define ORB					0
#define CROSS				1
#define GEM					2
#define VORTEX				3
#define CUBE				4
#define SSFONT				"./Fonts/TIMESS__.ttf"
#define GAME_ENDLESS		0
#define GAME_ENDLESS_MENU   1
#define GAME_MAIN_MENU		2
#define GAME_SETTINGS		3
#define GAME_CREDITS		4
#define GAME_QUIT			5
#define GAME_OVER			6
