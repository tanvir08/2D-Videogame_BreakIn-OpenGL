#ifndef _POLICE_INCLUDE
#define _POLICE_INCLUDE


#include "Sprite.h"
#include "TileMap.h"
#include "Player.h"
class Police
{
public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Player *player);
	void update(int deltaTime);
	void render();
	void setTileMap(TileMap *tileMap);
	void setPosition(const glm::vec2 & posWindow);
	void setRoom(int room) {
		mapRoom = room;
	}
	int calculateEyeSprite();
	int relativeHorizontalPosition();
	int relativeVerticalPosition();
	bool checkPlayerCollisions();
	void iniMovement();
	void move();
	void restart();
private:
	void changeAnimation(int animId);

	glm::ivec2 tileMapDispl, posPolice, lastposPlayer, posIni;
	Player *player;
	int mapRoom;
	Texture spritesheet_eyes;
	Sprite *sprite_eyes;
	TileMap *map;
	bool bright, bdown, bup, go, gameover;
	bool look_around;
	int eyesCont;
	int current_looking_sprite;
	int matrix_positions[3][3];
	int playerHeight,playerwidth;
	bool arrive_destine;
	int width, height;
	int timeBetweenMovement;
	float distancia_total;
	int xa, ya;
	int needTime;
	float row,col;
	int counter;
	int velocity;
	glm::vec2 direccion, velocidad, acumulado;
	int move_in_x, move_in_y;
	bool angulo_modificado;
	bool arrive_pos_in_x,arrive_pos_in_y;
	bool vx_zero, vy_zero;
};
#endif
