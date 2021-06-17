#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"
#include <chrono>
#include <thread>
#include "AudioEngine.h"

enum EyeStates {
	INITIAL, LEFT_TOP, LEFT_CENTER, LEFT_BOTTOM,
	RIGHT_TOP, RIGHT_CENTER, RIGHT_BOTTOM, TOP, CENTER, BOTTOM,
	CLOSE, END
};



void Player::loadSpritesheet(int paddel, ShaderProgram &shaderProgram) {
	if (paddel == 1) {
		spritesheet_paddle.loadFromFile("images/paddle.png", TEXTURE_PIXEL_FORMAT_RGBA);
		sprite_paddle = Sprite::createSprite(glm::ivec2(32, 12), glm::vec2(0.5, 1), &spritesheet_paddle, &shaderProgram);

	}
	else if (paddel == 2) {
		spritesheet_paddle.loadFromFile("images/paddlex2.png", TEXTURE_PIXEL_FORMAT_RGBA);
		sprite_paddle = Sprite::createSprite(glm::ivec2(64, 12), glm::vec2(1, 1), &spritesheet_paddle, &shaderProgram);
	}
	sprite_paddle->setNumberAnimations(1);
	sprite_paddle->setAnimationSpeed(0, 8);
	float x = 0.0f;
	if (magnet) x = 0.5f;
	sprite_paddle->addKeyframe(0, glm::vec2(x, 0.f));
	sprite_paddle->changeAnimation(0);


}

void Player::decreasePaddelSizeInX() { 
	numPaddel = 1;
	loadSpritesheet(numPaddel, shaderProgram);
}

void Player::incresePaddelSizeInX() {
	numPaddel = 2;
	loadSpritesheet(numPaddel, shaderProgram);
}


void Player::setMagnet(bool m) { magnet = m; }


/*procedimiento: inicializar - como actualizo al jugador - como lo muestro */
void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Ball *b) {
	width = 32;
	int height_paddle = 12;
	
	numPaddel = 1; magnet = false;
	this->shaderProgram = shaderProgram;
	loadSpritesheet(numPaddel, shaderProgram);


	int height_eyes = 40;

	height = height_eyes + height_paddle;

	spritesheet_eyes.loadFromFile("images/spritesheet_eyes.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite_eyes = Sprite::createSprite(glm::ivec2(32, 40), glm::vec2(0.2, 0.25), &spritesheet_eyes, &shaderProgram);

	sprite_eyes->setNumberAnimations(12);

	sprite_eyes->setAnimationSpeed(INITIAL, 8);			sprite_eyes->addKeyframe(INITIAL, glm::vec2(0.0f, 0.f));

	sprite_eyes->setAnimationSpeed(LEFT_TOP, 8);		sprite_eyes->addKeyframe(LEFT_TOP, glm::vec2(0.0f, 0.25f));
	sprite_eyes->setAnimationSpeed(LEFT_CENTER, 8);		sprite_eyes->addKeyframe(LEFT_CENTER, glm::vec2(0.0f, 0.5f));
	sprite_eyes->setAnimationSpeed(LEFT_BOTTOM, 8);		sprite_eyes->addKeyframe(LEFT_BOTTOM, glm::vec2(0.0f, 0.75f));

	sprite_eyes->setAnimationSpeed(RIGHT_TOP, 8);		sprite_eyes->addKeyframe(RIGHT_TOP, glm::vec2(0.2f, 0.25f));
	sprite_eyes->setAnimationSpeed(RIGHT_CENTER, 8);	sprite_eyes->addKeyframe(RIGHT_CENTER, glm::vec2(0.2f, 0.5f));
	sprite_eyes->setAnimationSpeed(RIGHT_BOTTOM, 8);	sprite_eyes->addKeyframe(RIGHT_BOTTOM, glm::vec2(0.2f, 0.75f));

	sprite_eyes->setAnimationSpeed(TOP, 8);				sprite_eyes->addKeyframe(TOP, glm::vec2(0.4f, 0.25f));
	sprite_eyes->setAnimationSpeed(CENTER, 8);			sprite_eyes->addKeyframe(CENTER, glm::vec2(0.2f, 0.0f));
	sprite_eyes->setAnimationSpeed(BOTTOM, 8);			sprite_eyes->addKeyframe(BOTTOM, glm::vec2(0.0f, 0.0f));

	sprite_eyes->setAnimationSpeed(CLOSE, 8);
		sprite_eyes->addKeyframe(CLOSE, glm::vec2(0.6f, 0.0f));
		sprite_eyes->addKeyframe(CLOSE, glm::vec2(0.6f, 0.25f));
		sprite_eyes->addKeyframe(CLOSE, glm::vec2(0.6f, 0.5f));
		sprite_eyes->addKeyframe(CLOSE, glm::vec2(0.6f, 0.75f));
		sprite_eyes->addKeyframe(CLOSE, glm::vec2(0.8f, 0.0f));
		sprite_eyes->addKeyframe(CLOSE, glm::vec2(0.8f, 0.25f));
		sprite_eyes->addKeyframe(CLOSE, glm::vec2(0.8f, 0.5f));

		sprite_eyes->setAnimationSpeed(END, 8);			sprite_eyes->addKeyframe(END, glm::vec2(0.8f, 0.5f));

	sprite_eyes->changeAnimation(0);


	tileMapDispl = tileMapPos;

	sprite_paddle->setPosition(glm::vec2(float(tileMapDispl.x + posPlayerWindow.x), float(tileMapDispl.y + posPlayerWindow.y)));
	sprite_eyes->setPosition(glm::vec2(float(tileMapDispl.x + posPlayerWindow.x), float(tileMapDispl.y + posPlayerWindow.y)));


	ball = b;
	bright = bdown = bup = gameover = restart = colision = space = godModeBall = false;
	go = true;
	eyesCont = 0;
	///first_not_loss_life = true;

	collisionWithPolice = false;
}

void Player::setPaddlePos(float x, float y) {
	float newx, newy;
	if (numPaddel == 1) {
		if (x <= 16) newx = x + 4;
		else if (x + 32 >= 368) newx = x - 4;
		else newx = x;
	}
	else {
		if (x-16 <= 16) newx = x + 4;
		else if (x+32+16 >= 368) newx = x - 4;
		else newx = x;
	}

	if (y <= 40) newy = y + 4;
	else if (y >= (384 - 52)) newy = y - 4;
	else newy = y;

	posPlayerWindow.x = newx; posPlayerWindow.y = newy;

}


void Player::checkPaddleCollisions() {
	int bx = ball->getPosBall().x; int by = ball->getPosBall().y;
	float bcx = bx + 16 / 2;
	int fact;
	if (numPaddel == 1) fact = 0;
	else fact = 1;
	if (bx + 16 > posPlayerWindow.x-(16*fact) && bx < posPlayerWindow.x+32+(16*fact) && by + 16 > posPlayerWindow.y && by < posPlayerWindow.y + 12) { //hay colision

		//calculo la diferencia de xsize y ysize para determinar en que parte de paddle ha tocado la pelota
		int ymin;
		if (posPlayerWindow.y > by) ymin = posPlayerWindow.y;
		else ymin = by;
		int ymax;
		if (posPlayerWindow.y + 12 < by + 16) ymax = by+16;
		else ymax = by+16;
		int ysize = ymax - ymin;

		int xmin;
		if (posPlayerWindow.x-(16*fact) > bx) xmin = posPlayerWindow.x-(16*fact);
		else xmin = bx;
		int xmax;
		if (posPlayerWindow.x+32+(16*fact) < bx+16) xmax = posPlayerWindow.x+32+(16*fact);
		else xmax = bx + 16;
		int xsize = xmax - xmin;

		float bcy = by + 16 / 2;
		float pPcy = posPlayerWindow.y + 12 / 2;
		if (xsize > ysize) { //toca parte x
			
			if (bcy > posPlayerWindow.y) {
				bdown = true; go = false;
				//cout << "ha tocado la parte baja" << endl;
			}
			else {
				bup = true;
				if (!bdown) go = true;
				else {
					if (posPlayerWindow.y >= by + 14) {
						go = true;
						//cout << "si" << endl;
					}
					//cout << "no" << endl;
				}
				//cout << "ha tocado la parte alta" << endl;
				if (go) colision = true;
				else colision = false;
			}


			if (go && !magnet && colision) {
				AudioEngine::instance().touchPaddelEffect();
				bdown = false; bup = false;
				//get relection
				float hitx = bcx - posPlayerWindow.x;
				if (hitx < 0) hitx = 0;
				else if (hitx > 32) { //no puede se superior al width de la pala
					hitx = 32;
				}
				hitx -= 32 / 2.0f;
				float reflecionScaled = 2.0f*(hitx / (32 / 2.0f));

				if (hitx >= 0 & hitx <= 8) reflecionScaled = 1.0f;
				else if (hitx < 0 && hitx >= -8) reflecionScaled = -1.0f;
				//cout << reflecionScaled << endl;
				ball->setDirection(glm::vec2(reflecionScaled, -1));
			}
			if (magnet && colision) {
				space = true;
				//cout << "antes: " << ball->getDirection().x << " " << ball->getDirection().y << endl;
				ball->setVelocityBall(glm::ivec2(0.0f, 0.0f));
			}



		}
		else {
			//ignorar si toca en left o right de paddle
			if (bcx < posPlayerWindow.x-(16*fact)) {
				//cout << "left" << endl;
				go = false;
			}
			else {
				//cout << "right" << endl;
				go = false;
			}
		}


	}
}


void Player::update(int deltaTime)
{
	posPlayerWindowBefore = posPlayerWindow;
	sprite_paddle->update(deltaTime);
	sprite_eyes->update(deltaTime);
	if (Game::instance().getKey('d')) { //al pulsar entra mas o menos cuatro veces
		if (numPaddel == 1) incresePaddelSizeInX();
		else if(numPaddel == 2) decreasePaddelSizeInX();
	}

	if (Game::instance().getKey('k')) {
		godModeBall = !godModeBall;
		cout << "godModeBall: " << godModeBall << endl;
	}

	if (!gameover && Game::instance().getSpecialKey(GLUT_KEY_LEFT)) posPlayerWindow.x -= 4;
	else if (!gameover && Game::instance().getSpecialKey(GLUT_KEY_RIGHT)) posPlayerWindow.x += 4;
	else if (!gameover && Game::instance().getSpecialKey(GLUT_KEY_UP)) posPlayerWindow.y -= 4;
	else if (!gameover && Game::instance().getSpecialKey(GLUT_KEY_DOWN)) posPlayerWindow.y += 4;


	setPaddlePos(posPlayerWindow.x, posPlayerWindow.y);
	if (!ball->getStartRound()) {
		ball->setPosition(glm::ivec2(posPlayerWindow.x + (32 / 2) - (16 / 2), posPlayerWindow.y - 16));
	}


	checkPaddleCollisions();
	posBall = ball->getPosBall();
	//cout << "ball: " << posBall.x << " " << posBall.y << endl;
	//cout << "player: " << posPlayerWindow.x << " " << posPlayerWindow.y << endl;

	if (!gameover) {
		if (posBall.x <= 112) { //pelota en la parte izq
			int aux = (posPlayerWindow.y) - (posBall.y + 16);
			if (posPlayerWindow.x >= 112) { //si el jugador no esta en la parte izq
				//cout << "1" << endl;
				if (aux > 0 && aux >= 32) sprite_eyes->changeAnimation(LEFT_TOP);
				else if (aux < 0 && aux <= -64) sprite_eyes->changeAnimation(LEFT_BOTTOM);
				else sprite_eyes->changeAnimation(LEFT_CENTER);
			}
			else {
				//cout << "2" << endl;
				if (aux > 0 && aux >= 32) sprite_eyes->changeAnimation(TOP);
				else if (aux < 0 && aux <= -64) sprite_eyes->changeAnimation(BOTTOM);
				else sprite_eyes->changeAnimation(CENTER);

			}
		}
		else if (posBall.x >= 240) { //pelota en la parte dre
			int aux = (posPlayerWindow.y) - (posBall.y + 16);
			if (posPlayerWindow.x < 240) { //si el jugador no esta en la parte izq
				//cout << "3" << endl;
				if (aux > 0 && aux >= 32) sprite_eyes->changeAnimation(RIGHT_TOP);
				else if (aux < 0 && aux <= -64) sprite_eyes->changeAnimation(RIGHT_BOTTOM);
				else sprite_eyes->changeAnimation(RIGHT_CENTER);
			}
			else {
				//cout << "4" << endl;
				if (aux > 0 && aux >= 32) sprite_eyes->changeAnimation(TOP);
				else if (aux < 0 && aux <= -64) sprite_eyes->changeAnimation(BOTTOM);
				else sprite_eyes->changeAnimation(CENTER);

			}
		}
		else { //pelota en el centro
			int aux = (posPlayerWindow.y) - (posBall.y + 16);
			if (posPlayerWindow.x < 112) { //si el jugador esta en la parte izq
				//cout << "5" << endl;
				if (aux > 0 && aux >= 32) sprite_eyes->changeAnimation(RIGHT_TOP);
				else if (aux < 0 && aux <= -64) sprite_eyes->changeAnimation(RIGHT_BOTTOM);
				else sprite_eyes->changeAnimation(RIGHT_CENTER);
			}
			else if (posPlayerWindow.x > 240) {
				//cout << "6" << endl;
				if (aux > 0 && aux >= 32) sprite_eyes->changeAnimation(LEFT_TOP);
				else if (aux < 0 && aux <= -64) sprite_eyes->changeAnimation(LEFT_BOTTOM);
				else sprite_eyes->changeAnimation(LEFT_CENTER);
			}
			else { //tanto la pelota como el jugador estan en region central
				//cout << "7" << endl;
				if (aux > 0 && aux >= 32) sprite_eyes->changeAnimation(TOP);
				else if (aux < 0 && aux <= -64) sprite_eyes->changeAnimation(BOTTOM);
				else sprite_eyes->changeAnimation(CENTER);

			}
		}
	}
	if(Game::instance().getKey('g')) Game::instance().setGameState(1); //probando
	if ((posBall.x+16 >= 128 && posBall.x <= 240 && posBall.y >= 368)||collisionWithPolice) {
		if (gameover) {
			if (collisionWithPolice) collisionWithPolice = false;
			if(eyesCont == 1) Game::instance().stopCoin();
			++eyesCont;
			if (eyesCont == 48) {
				std::this_thread::sleep_for(std::chrono::milliseconds(300));
				if(Game::instance().getLife() == 0) Game::instance().setGameState(1); //gameover
				else {
					Game::instance().restartGame();
					restart = true;
					//gameover = false;
					eyesCont = 0;
				}
			}
			//cout << eyesCont << endl;
		}
		if ((ball->getRoom() == 1 && !gameover && !godModeBall)||collisionWithPolice) {
			//cout << "decrease" << endl;
			Game::instance().decreaseLife();
			gameover = true;
			ball->setVelocityBall(glm::ivec2(0.0, 0.0));
			AudioEngine::instance().losslife();
			/*if (first_not_loss_life) {
			AudioEngine::instance().losslife();
				first_not_loss_life = false;
			}*/
			sprite_eyes->changeAnimation(CLOSE);
			
		}
		else if (ball->getRoom() == 2 && !gameover) {
			posBall.y = posBall.y - 368 + 16 + 6;
			Game::instance().setScroll(0);
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			ball->setPosition(posBall);
		}
		else if (ball->getRoom() == 3 && !gameover) {
			//cout << "room 3 y posBall.y: " << posBall.y;
			if (posBall.y >= 370) posBall.y = 368;
			posBall.y = 32 + 2; //en room 3 aveces funciona mal, es un poco raro
			//cout << " posBall.y cambiado " << posBall.y << endl;
			Game::instance().setScroll(0);
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			ball->setPosition(posBall);
		}

	}
	if (restart) gameover = false;

	if (magnet && colision) {
		//cout << "dentro" << endl;
		int x = posPlayerWindow.x - posPlayerWindowBefore.x;
		int y = posPlayerWindow.y - 16;
		ball->setPosition(glm::vec2(posBall.x + x, y));
	}
	
	if (Game::instance().getKey(32) && space) {
		space = false;
		colision = false;
		ball->setVelocityBall(glm::ivec2(3.0f, 3.0f));
		ball->setDirection(ball->getDirection()*-1);

	}


	if (numPaddel == 1) { sprite_paddle->setPosition(glm::vec2(float(tileMapDispl.x + posPlayerWindow.x), float(tileMapDispl.y + posPlayerWindow.y)));}
	else if (numPaddel == 2) {sprite_paddle->setPosition(glm::vec2(float(tileMapDispl.x + posPlayerWindow.x -16), float(tileMapDispl.y + posPlayerWindow.y)));}
	sprite_eyes->setPosition(glm::vec2(float(tileMapDispl.x + posPlayerWindow.x), float(tileMapDispl.y + posPlayerWindow.y + 12)));
	
}

void Player::render(){
	sprite_paddle->render();
	sprite_eyes->render();
}

void Player::setTileMap(TileMap *tileMap){
	map = tileMap;
}

void Player::setPosition(const glm::vec2 &posWindow){
	//La posicion del player "posPlayer" de la implemntacion dada es respecto solo a un mapa 
	//que puede ser expresado dentro de una sola window, 
	// sin embargo el tama�o de nuestro mapa es igual al de 3 windows 
	//por lo cual solo la sala 3 de un mapa es con la que trabaja.
	//Una solucion es agregar una variable "posPlayerMap" que almacene la posicion real del player en el mapa.
	posPlayerWindow = posWindow;// window posicion
	if (numPaddel == 1) { sprite_paddle->setPosition(glm::vec2(float(tileMapDispl.x + posPlayerWindow.x), float(tileMapDispl.y + posPlayerWindow.y)));}
	else if (numPaddel == 2) {sprite_paddle->setPosition(glm::vec2(float(tileMapDispl.x + posPlayerWindow.x-16), float(tileMapDispl.y + posPlayerWindow.y)));}
	sprite_eyes->setPosition(glm::vec2(float(tileMapDispl.x + posPlayerWindow.x), float(tileMapDispl.y + posPlayerWindow.y + 12)));

	//sprite_paddle->setPosition(glm::vec2(float(tileMapDispl.x + posPlayerWindow.x), float(tileMapDispl.y + posPlayerWindow.y)));
	//sprite_eyes->setPosition(glm::vec2(float(tileMapDispl.x + posPlayerWindow.x), float(tileMapDispl.y + posPlayerWindow.y+12)));

}

bool Player::getStartRound() {
	return ball->getStartRound();
}


