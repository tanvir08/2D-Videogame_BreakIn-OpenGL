#include "Coin.h"
#include "Game.h"
#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>


enum CoinStates {
	GREY, GREEN, YELLOW, RED, BLUE, PINK, INVISIBLE, POINTx3, POINTx6, POINTx1, POINTx2, POINTx4, POINTx5
};


void Coin::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Player * player) {
	spritesheet.loadFromFile("images/coinsAndPoints.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(28, 28), glm::vec2(0.125, 0.125), &spritesheet, &shaderProgram);

	sprite->setNumberAnimations(13);

	sprite->setAnimationSpeed(GREY, 8);			sprite->addKeyframe(GREY, glm::vec2(0.0f, 0.0f));
	sprite->setAnimationSpeed(GREEN, 8);		sprite->addKeyframe(GREEN, glm::vec2(0.125f, 0.0f));
	sprite->setAnimationSpeed(YELLOW, 8);		sprite->addKeyframe(YELLOW, glm::vec2(0.25f, 0.0f));
	sprite->setAnimationSpeed(RED, 8);			sprite->addKeyframe(RED, glm::vec2(0.375f, 0.0f));
	sprite->setAnimationSpeed(BLUE, 8);			sprite->addKeyframe(BLUE, glm::vec2(0.5f, 0.0f));
	sprite->setAnimationSpeed(PINK, 8);			sprite->addKeyframe(PINK, glm::vec2(0.625f, 0.0f));
	sprite->setAnimationSpeed(INVISIBLE, 8);	sprite->addKeyframe(INVISIBLE, glm::vec2(0.750f, 0.0f));

	sprite->setAnimationSpeed(POINTx3, 8);							sprite->addKeyframe(POINTx3, glm::vec2(0.0f, 0.125f));
		sprite->addKeyframe(POINTx3, glm::vec2(0.125f, 0.125f));	sprite->addKeyframe(POINTx3, glm::vec2(0.25f, 0.125f));
		sprite->addKeyframe(POINTx3, glm::vec2(0.125f, 0.125f));	sprite->addKeyframe(POINTx3, glm::vec2(0.0f, 0.125f));

	sprite->setAnimationSpeed(POINTx6, 8);							sprite->addKeyframe(POINTx6, glm::vec2(0.375f, 0.125f));
		sprite->addKeyframe(POINTx6, glm::vec2(0.5f, 0.125f));		sprite->addKeyframe(POINTx6, glm::vec2(0.625f, 0.125f));
		sprite->addKeyframe(POINTx6, glm::vec2(0.5f, 0.125f));		sprite->addKeyframe(POINTx6, glm::vec2(0.375f, 0.125f));

	sprite->setAnimationSpeed(POINTx1, 8);							sprite->addKeyframe(POINTx1, glm::vec2(0.0f, 0.25f));
		sprite->addKeyframe(POINTx1, glm::vec2(0.125f, 0.25f));		sprite->addKeyframe(POINTx1, glm::vec2(0.25f, 0.25f));
		sprite->addKeyframe(POINTx1, glm::vec2(0.125f, 0.25f));		sprite->addKeyframe(POINTx1, glm::vec2(0.0f, 0.25f));

	sprite->setAnimationSpeed(POINTx2, 8);							sprite->addKeyframe(POINTx2, glm::vec2(0.375f, 0.25f));
		sprite->addKeyframe(POINTx2, glm::vec2(0.5f, 0.25f));		sprite->addKeyframe(POINTx2, glm::vec2(0.625f, 0.25f));
		sprite->addKeyframe(POINTx2, glm::vec2(0.5f, 0.25f));		sprite->addKeyframe(POINTx2, glm::vec2(0.375f, 0.25f));

	sprite->setAnimationSpeed(POINTx4, 8);							sprite->addKeyframe(POINTx4, glm::vec2(0.0f, 0.375f));
		sprite->addKeyframe(POINTx4, glm::vec2(0.125f, 0.375f));	sprite->addKeyframe(POINTx4, glm::vec2(0.25f, 0.375f));
		sprite->addKeyframe(POINTx4, glm::vec2(0.125f, 0.375f));	sprite->addKeyframe(POINTx4, glm::vec2(0.0f, 0.375f));

	sprite->setAnimationSpeed(POINTx5, 8);							sprite->addKeyframe(POINTx5, glm::vec2(0.375f, 0.375f));
		sprite->addKeyframe(POINTx5, glm::vec2(0.5f, 0.375f));		sprite->addKeyframe(POINTx5, glm::vec2(0.625f, 0.375f));
		sprite->addKeyframe(POINTx5, glm::vec2(0.5f, 0.375f));		sprite->addKeyframe(POINTx5, glm::vec2(0.375f, 0.375f));

	current_coin = INVISIBLE;
	sprite->changeAnimation(current_coin);

	tileMapDispl = tileMapPos;

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posCoin.x), float(tileMapDispl.y + posCoin.y)));
	this->player = player;
	lastposPlayer = player->getPosPlayer();
	seg = tick = 0;
	move = finished = show = false;
	velocity = glm::vec2(1.0f, 1.0f);
	direction = glm::vec2(1.f, -1.f);
	states = vector<InfoState>(3);
	first = true;
}


void Coin::changeAnimation(int animId) {
	current_coin = animId;
	sprite->changeAnimation(current_coin);
}


void Coin::checkBorderCollisions() {
	//eje x
	if (posCoin.x <= 16) {posCoin.x = 16;direction.x *= -1;}
	else if (posCoin.x + 28 >= 372) { posCoin.x = 372 - 28; direction.x *= -1;}

	//eje y
	if (posCoin.y <= 16) { posCoin.y = 16; direction.y *= -1;}
	else if (posCoin.y + 28 >= 384) { posCoin.y = 384 - 28; direction.y *= -1;}
	
}

void Coin::checkPaddelCollisions() {
	int bx = player->getPosPlayer().x; int by = player->getPosPlayer().y;
	float bcx = bx + 32 / 2;
	if (bx+32 > posCoin.x && bx < posCoin.x+28 && by+12 > posCoin.y && by < posCoin.y+28) { //hay colision
		//calculo la diferencia de xsize y ysize para determinar en que parte de paddle ha tocado la pelota
		int ymin;
		if (posCoin.y > by) ymin = posCoin.y;
		else ymin = by;
		int ymax;
		if (posCoin.y+28 < by+12) ymax = by+12;
		else ymax = by+12;
		int ysize = ymax - ymin;

		int xmin;
		if (posCoin.x > bx) xmin = posCoin.x;
		else xmin = bx;
		int xmax;
		if (posCoin.x+28 < bx+32) xmax = posCoin.x+28;
		else xmax = bx+32;
		int xsize = xmax - xmin;

		float bcy = by + 12 / 2;
		float pPcy = posCoin.y + 28 / 2;
		if (xsize > ysize) {
			if (bcy > posCoin.y) show = true;
			else show = false;
		}
		else show = true;

	}
}

void Coin::updateStates() {
	changeAnimation(INVISIBLE);
	seg = 0;
	move = show = false;
}



void Coin::update(int deltaTime) {
	++tick;
	sprite->update(deltaTime);
	//changeAnimation(calculateEyeSprite())

	if (tick == 64) {
		++seg;

		if (current_coin == INVISIBLE && getStartRound() && (seg%(5+states[room-1].fact)==0)) {
			changeAnimation(GREY);
			move = true;
			posCoin = posCoinIni;
		}

	}


	if (move) {
		if (tick == 64 && (seg%2 == 0)) {
			current_coin = (current_coin + 1) % 6;
			//current_coin = BLUE;
			changeAnimation(current_coin);
		}
		checkBorderCollisions();

		checkPaddelCollisions();
		posCoin += velocity * direction;

		
	}

	if (show) {
		states[room-1].fact += 2;
		if (current_coin == YELLOW) {		//saca 2 Ball mas (descartado)
			changeAnimation(POINTx3);
			Game::instance().addValue(300, 1);
			if (player->getMagnet()) {
				player->setMagnet(false);
				player->decreasePaddelSizeInX();
			}
		}
		else if (current_coin == PINK) {	//solo suma puntos
			changeAnimation(POINTx6);
			Game::instance().addValue(600, 1);
		}
		else if (current_coin == GREY) {	//si tenias doble paddle, te quita y vuelve al normal
			changeAnimation(POINTx1);
			Game::instance().addValue(100, 1);
			if (player->getNumPaddel() == 2) player->decreasePaddelSizeInX();
			else if (player->getMagnet()) {
				player->setMagnet(false);
				player->decreasePaddelSizeInX();
			}
		}
		else if (current_coin == GREEN) {	//doble paddle
			changeAnimation(POINTx2);
			Game::instance().addValue(200, 1);
			if (player->getNumPaddel() == 1) {
				if (player->getMagnet()) {
					player->setMagnet(false);
					player->decreasePaddelSizeInX();
				}
				player->incresePaddelSizeInX();
			}

		}
		else if (current_coin == RED) {		//dispara
			changeAnimation(POINTx4);
			Game::instance().addValue(400, 1);
			if (player->getMagnet()) {
				player->setMagnet(false);
				player->decreasePaddelSizeInX();
			}
		}
		else if (current_coin == BLUE) {	//iman
			changeAnimation(POINTx5);
			Game::instance().addValue(500, 1);
			player->setMagnet(true);
			player->decreasePaddelSizeInX();
		}

		//changeAnimation(POINTx3);
		move = false;
		show = false;
		seg = 0;
	}
	else if (current_coin > 6 && tick == 64 && (seg%2 == 0)) {
		changeAnimation(INVISIBLE);
	}

	


	if(tick == 64) tick = 0;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posCoin.x), float(tileMapDispl.y + posCoin.y)));
}

void Coin::render() {
	sprite->render();
}


void Coin::setPosition(const glm::vec2 &posWindow, const glm::vec2 &posMap) {
	if (first) { posCoinIni = posWindow; first = false;}
	posCoin = posWindow;// window posicion
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posCoin.x), float(tileMapDispl.y + posCoin.y)));
}
