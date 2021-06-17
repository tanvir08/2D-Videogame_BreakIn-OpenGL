#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include "TexturedQuad.h"
#include "Text.h"
#include "Ball.h"
#include "ScoreBar.h"
#include "Police.h"
#include "Coin.h"


// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene
{

public:
	Scene();
	~Scene();

	void init();
	void update(int deltaTime);
	void render();
	void setGameState(int s);
	void decreaseLife();
	int getLife() { return lives; }
	void restartGame();
	void changeScroll(int i);
	void setOpenDoor(int i) { openDoor = i; }
	void changeScrollValue(int value, int type);
	void alertPolice();
	void stopCoin();
	void hitThePhone();

private:
	void initShaders();
	void startGame();

private:
	TileMap *currentMap, *maps[3];
	Player *player;
	Ball *ball;
	Police *police;
	ScoreBar *scorebar;
	ShaderProgram texProgram;
	Texture windowsTexs[6], texs[4]; // One for each of the folloing states: MENU,GAME_OVER,HELP,CREDITS  
	//Texture windowsPlayTexs[4], ; //PLAY(It's special because it can change depending on the level)
	float currentTime;
	glm::mat4 projection;
	int state;
	// Every state owns a set of variables
	// Important! : The varibles' names must follow the pattern: " nomstate_nomvarible "
	//MENU
	int menu_selected_pos;
	//PLAY
	bool is_game_over,is_game_win,is_level_win;
	//Scorebar variables
	int money, points, lives, bank, room;
	int batmode;
	int scroll,scroll_ini;
	bool scrolling_up, scrolling_down, is_first, current_scroll_finish;
	int scroll_down, scroll_up; // times to scroll
	int scrolling_positions; 
	int time_next_bank,time_back_bank;
	TexturedQuad *background, *texQuad[1];
	Text text; 
	bool goUp, goDown;
	int openDoor;
	Coin *coin;
	bool policeAwake;
	bool policeAwakeBank[3];
	bool policePausedBank[3];
	int  policeInRoomOfBank[3];
	bool win_level[3];
	bool phone_was_hit, first_game_over;
};


#endif // _SCENE_INCLUDE

