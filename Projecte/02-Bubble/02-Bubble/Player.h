#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE


#include "Sprite.h"
#include "TileMap.h"
#include "Ball.h"


// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class Player
{

public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Ball *ball);
	void update(int deltaTime);
	void render();
	
	void setTileMap(TileMap *tileMap);
	void setPosition(const glm::vec2 & posWindow);
	glm::ivec2 getPosPlayer() const { return posPlayerWindow; }
	void setPaddlePos(float x, float y);
	void checkPaddleCollisions();
	bool getStartRound();
	int getNumPaddel() { return numPaddel;}
	void decreasePaddelSizeInX();
	void incresePaddelSizeInX();
	void setMagnet(bool m);
	bool getMagnet() { return magnet; }

	void setCollisionWithpolice(bool collisionWithPolice) { this->collisionWithPolice = collisionWithPolice; }
	int  getWidth() {
		return width;
	}
	int  getHeight() {
		return height;
	}
private:
	void loadSpritesheet(int paddel, ShaderProgram &shaderProgram);

private:
	glm::ivec2 tileMapDispl, posPlayerWindow, posBall, posPlayerWindowBefore;
	int mapRoom;
	Texture spritesheet_paddle, spritesheet_eyes;
	Sprite *sprite_paddle, *sprite_eyes;
	TileMap *map;
	Ball *ball;
	bool bright, bdown, bup, go, gameover, restart, magnet, colision, space;
	int eyesCont, numPaddel;
	ShaderProgram shaderProgram;
	bool collisionWithPolice, godModeBall;
	int height, width;
};


#endif // _PLAYER_INCLUDE


