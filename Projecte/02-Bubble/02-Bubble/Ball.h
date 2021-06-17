#ifndef _BALL_INCLUDE
#define _BALL_INCLUDE

#include "Sprite.h"
#include "TileMap.h"


class Ball {
public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram);
	void update(int deltaTime);
	void render();

	void setTileMap(TileMap *tileMap);
	void setPosition(const glm::vec2 &pos);
	void checkBorderCollisions();
	bool checkBricksCollisions();
	bool getStartRound() { return startRound; }
	void setStartRound(bool b) { startRound = b; }
	glm::ivec2 getPosBall() { return posBall; }
	glm::ivec2 getVelocityBall() { return velocity; }
	void setVelocityBall(glm::ivec2 vel) { velocity = vel; }
	void setDirection(glm::ivec2 pos) {
		direction.x = pos.x; direction.y = pos.y;
	}
	glm::ivec2 getDirection() { return direction; }
	void setRoom(int room);
	int getRoom() { return room; }
	glm::vec2 toPosMap(glm::vec2 pos);
	glm::vec2 toPosWindow(glm::vec2 pos);
	void openDoor();
	void setbOpenDoor(int level, int i) { bopenDoor[level] = i; }
	void setLevel(int l) { level = l; }
	int *getbopenDoor() { return bopenDoor; }

private:
	bool startRound;
	ShaderProgram texProgram;
	glm::ivec2 tileMapDispl, posBall, velocity, velocity_aux, direction;
	int jumpAngle, startY;
	Texture spritesheet;
	Sprite *sprite;
	TileMap *map;
	int *tileMap;
	int mapy, mapx;
	int room, level;
	int bopenDoor[3];
};
#endif