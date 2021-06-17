#ifndef _COIN_INCLUDE
#define _COIN_INCLUDE

#include "Sprite.h"
#include "TileMap.h"
#include "Player.h"
#include <vector>

struct InfoState {
	int fact = 0;
};


class Coin {

public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Player *player);
	void update(int deltaTime);
	void render();
	void setPosition(const glm::vec2 & posWindow, const glm::vec2 & posMap);
	glm::ivec2 getPosCoin() const { return posCoin; }
	void setRoom(int room) {this->room = room;}
	bool getStartRound() { return player->getStartRound();}
	void updateStates();
	void setVelocity(glm::ivec2 v) {velocity = v;}


private:
	void changeAnimation(int animId);
	void checkBorderCollisions();
	void checkPaddelCollisions();

	glm::ivec2 tileMapDispl, posCoin, posCoinIni, lastposPlayer, velocity, direction;
	int room;
	Texture spritesheet;
	Sprite *sprite;
	Player *player;
	int current_coin, coinCont;
	int seg, tick;
	bool move, show, finished, first;
	vector<InfoState> states;

};

#endif
