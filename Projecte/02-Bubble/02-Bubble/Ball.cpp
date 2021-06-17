#include "Ball.h"
#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"
#include <chrono>
#include <thread>
#include "AudioEngine.h"

enum HitInCollision {
	LEFT, TOP, RIGHT, BOTTOM
};


void Ball::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram) {
	texProgram = shaderProgram;
	spritesheet.loadFromFile("images/ball.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(1, 1), &spritesheet, &shaderProgram);

	sprite->setNumberAnimations(1);
	sprite->setAnimationSpeed(0, 1);
	sprite->addKeyframe(0, glm::vec2(0.f, 0.f));

	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;	/*punto inicial de tile map o coord minima de tilemap (32,16)*/

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBall.x), float(tileMapDispl.y + posBall.y)));
	startRound = level = false;
	bopenDoor[level] = 0;
	velocity = velocity_aux = glm::vec2(3.0f, 3.0f);
	direction = glm::vec2(-1.f, -1.f);
}



void Ball::checkBorderCollisions() {
	bool col = false;
	// eje x
	if (map->collisionMoveLeft(toPosMap(posBall), glm::ivec2(16, 16)) /*posBall.x <= 16*/) { //izquierda
		col = true;
		posBall.x = 16;
		direction.x *= -1;
	}
	else if (posBall.x + 16 >= 372) {
		col = true;
		posBall.x = 372 - 16;
		direction.x *= -1;
	}

	//eje y
	if (posBall.y <= 16) { //arriba
		if (posBall.x >= 112 && posBall.x <= 240 && bopenDoor[level]) {
				if (bopenDoor[level] == 2 && room == 2) {
					//cout << "bopenDoor 2: " << posBall.x << " " << posBall.y << endl;
					posBall.y = posBall.y + 738; // 368 + 368, cout << posBall.y  --> 16 ?
					Game::instance().setScroll(1);
					setPosition(toPosWindow(posBall));
					std::this_thread::sleep_for(std::chrono::milliseconds(20));
					//cout << posBall.x << " " << posBall.y << endl;
					
				}
				else if (room == 1) {
					//cout << posBall.x << " " << posBall.y << endl;
					posBall.y = posBall.y + 1152; //368 + 448 + 368 - 32;
					Game::instance().setScroll(1);
					std::this_thread::sleep_for(std::chrono::milliseconds(20));
					setPosition(toPosWindow(posBall));
					//cout << posBall.x << " " << posBall.y << endl;
				}
				else { //para room 3
					posBall.y = 16;
					direction.y *= -1;
				}
				
		}
		else {
			col = true;
			posBall.y = 16;
			direction.y *= -1;
		}
	}
	else if (posBall.y + 16 >= 384) {
		if (posBall.x < 112 || posBall.x > 240) col = true;
		posBall.y = 384 - 16;
		direction.y *= -1;
	}

	if (col) AudioEngine::instance().touchBorderEffect();

}


void Ball::openDoor() {
	tileMap = map->getMap();
	int mapx = map->getMapSize().x;
	int r = getRoom();
	int i;
	if (r == 1) {
		i = 52 * mapx + 8;
		if(bopenDoor[level] == 2) bopenDoor[level] = 2;
		else bopenDoor[level] = 1;
		//cout << "setbopendoor: 1" << endl;
		Game::instance().setbOpenDoor(bopenDoor[level]);
	}
	else if (r == 2) {
		i = 26 * mapx + 8;
		bopenDoor[level] = 2;
		//cout << "setbopendoor: 2" << endl;
		Game::instance().setbOpenDoor(bopenDoor[level]);
	}
	//else if (r == 3) i = 8;
	else return;

	tileMap[i] = 37; tileMap[i + 1] = 38;
	tileMap[i + 2] = 37; tileMap[i + 3] = 38;
	tileMap[i + 4] = 37; tileMap[i + 5] = 38;
	tileMap[i + 6] = 37; tileMap[i + 7] = 38;

}


bool Ball::checkBricksCollisions() {
	tileMap = map->getMap();
	mapy = map->getMapSize().y; mapx = map->getMapSize().x;
	
	glm::vec2 posBall = toPosMap(this->posBall);// cambio a posicion del mapa y usa la variable temporal posBall
	for (int j = 0; j < mapy; j++) {
		for (int i = 0; i < mapx; i++) {
			int tile = tileMap[j*mapx + i];
			bool brick = (tile >= 11 && tile <= 30);
			bool brickSpecial = (tile == 31 || tile == 32);
			bool brickSpecial2 = (tile == 47 || tile == 48);
			bool key = (tile >= 113 && tile <= 116);
			bool wall = (tile >= 33 && tile <= 36); bool ignore = false;
			bool dolar = (tile >= 117 && tile <= 120);
			bool money = (tile >= 121 && tile <= 124);
			bool diamond = (tile >= 125 && tile <= 128);
			bool grape = (tile >= 137 && tile <= 140);
			bool doorbell = (tile >= 129 && tile <= 132);
			bool phone = (tile >= 133 && tile <= 136);

			if (brick || key || wall || dolar || money || diamond || grape || brickSpecial|| brickSpecial2 || doorbell||phone) { //11 al 18 tenemos determinados que son numeros para tiles tipo brick
				int brx = i * 16; int bry = j * 16;
				int brcx = (int)(brx + 0.5 * 16); int brcy = (int)(bry + 0.5 * 16); //16 no es width de brick pero lo pongo porque analizo como tile 
				int ballcx = (int)(posBall.x + 0.5 * 16); int ballcy = (int)(posBall.y + 0.5 * 16); //16 = ball width
;
				if (posBall.x <= brx + 16 && posBall.x + 16 >= brx && posBall.y <= bry + 16 && posBall.y + 16 >= bry) { //AABB collisions, comprobo si hay colision
					if (!wall && !doorbell) {
						if (tile % 2 != 0) {
							//cout << "comprobando1" << endl;
							if ((key && tileMap[j*mapx + i] == 113) || (dolar && tileMap[j*mapx + i] == 117) || (money && tileMap[j*mapx + i] == 121) || (diamond && tileMap[j*mapx + i] == 125) || (grape && tileMap[j*mapx + i] == 137) || (phone && tileMap[j*mapx + i] == 133)) {
								//cout << "detectando1" << endl;
								tileMap[j*mapx + i + 24] = 0; tileMap[j*mapx + i + 25] = 0;
							}
							else if(key || dolar || money || diamond || grape || phone){
								//cout << "detectando2" << endl;
								tileMap[j*mapx + i - 24] = 0; tileMap[j*mapx + i - 23] = 0;
							}
							
							if (!brickSpecial && !brickSpecial2) {
								tileMap[j*mapx + i] = 0; tileMap[j*mapx + i + 1] = 0;
							}
							else if (brickSpecial2) {
								tileMap[j*mapx + i] = 31; tileMap[j*mapx + i + 1] = 32;
							}
							else {
								tileMap[j*mapx + i] = 29; tileMap[j*mapx + i + 1] = 30;
							}
							if (key) openDoor();
							

						}
						else {
							if (brick) {
								int dfl = posBall.x - (brx + 16); //next_brx
								int dfr = 16 - dfl;
								if (dfr > dfl && tileMap[j*mapx + i + 1] % 2 != 0 && tileMap[j*mapx + i + 1]>=11 && tileMap[j*mapx + i + 1] <=21) { //caso excepcional
									brx = (i + 1) * 16; brcx = (int)(brx + 0.5 * 16);
									tileMap[j*mapx + i + 1] = 0; tileMap[j*mapx + i + 2] = 0;
								}
								else {
									tileMap[j*mapx + i - 1] = 0; tileMap[j*mapx + i] = 0;

								}
							}
							else{
								//cout << "comprobando2" << endl;
								if ((key && tileMap[j*mapx + i] == 114) || (dolar && tileMap[j*mapx + i] == 118) || (money && tileMap[j*mapx + i] == 122) || (diamond && tileMap[j*mapx + i] == 126) || (grape && tileMap[j*mapx + i] == 138) || (phone && tileMap[j*mapx + i] == 134)) {
									tileMap[j*mapx + i + 23] = 0;  tileMap[j*mapx + i + 24] = 0;
								}
								else {
									tileMap[j*mapx + i - 24] = 0; tileMap[j*mapx + i - 25] = 0;
								}
								
								if (!brickSpecial && !brickSpecial2) {
									tileMap[j*mapx + i - 1] = 0; tileMap[j*mapx + i] = 0;
								}
								else if (brickSpecial2) {
									tileMap[j*mapx + i - 1] = 31; tileMap[j*mapx + i] = 32;
								}
								else {
									tileMap[j*mapx + i - 1] = 29; tileMap[j*mapx + i] = 30;
								}
								if(key)openDoor();

							}


						}
					}
					else {

						//esta funcionando un poco raro
						//cout << "org y: " << posBall.y << endl;;
						int y = posBall.y; y = (y % 416); 
						if (posBall.x <= 16) ignore = true;
						else if (posBall.x >= 352) {
							ignore = true;
							posBall.x = 352;
						}
						if (y <= 16) ignore = true;
						else if (posBall.y+16 >= 384*(4-room)) {
							posBall.y = 384 * room - 16;
							//cout << "dentro" << endl;
							ignore = true;
						}
						//cout << "ignore: " << ignore << endl;
						//cout << posBall.x  << " " << posBall.y << " " << y << endl;
						if (doorbell) {
							Game::instance().turnOnAlarm();
						}
					}

					if (brick || key || (wall && !ignore) || dolar || money || diamond || grape || brickSpecial || brickSpecial2 || doorbell || phone) {

						if (phone) {
							Game::instance().hitThePhone();
						}
						//calculo la diferencia de xsize y ysize para determinar en que parte de brick ha tocado la pelota
						int ymin;
						if (bry > posBall.y) ymin = bry;
						else ymin = posBall.y;
						int ymax;
						if (bry + 16 < posBall.y + 16) ymax = bry + 16;
						else ymax = posBall.y + 16;
						int ysize = ymax - ymin;

						int xmin;
						if (brx > posBall.x) xmin = brx;
						else xmin = posBall.x;
						int xmax;
						if (brx + 16 < posBall.x + 16) xmax = brx + 16;
						else xmax = posBall.x + 16;
						int xsize = xmax - xmin;

						if (xsize >= ysize) {		//significa que hay mas parte de x que es tocando y puede ser que toque en brick abajo o arriba
							if (ballcy > brcy) posBall.y += ysize + 0.001;	// despues ir abajo
							else posBall.y -= ysize + 0.001;				// despues ir arriba
							direction.y *= -1;

						}
						else {
							if (ballcx < brcx) posBall.x -= xsize + 0.001;	//despues ir izq
							else posBall.x += xsize + 0.001;
							direction.x *= -1;
						}
						this->posBall = toPosWindow(posBall); //Debido a que de ha podido haber modificado posBall (varible temporal) la paso de poscion de Mapa a posiciones de window y los dejo en la variable global "posBall"
						if (brick) {
							Game::instance().addValue(10, 1);
							AudioEngine::instance().touchBrickEffect();
						}
						else if(dolar) Game::instance().addValue(100, 2);
						else if(money) Game::instance().addValue(200, 2);
						else if(grape) Game::instance().addValue(500, 2);
						else if (diamond) Game::instance().addValue(1000, 2);
						return true;
					}
				}

			}


		}
	}
	return false;
}



void Ball::update(int deltaTime) {
	sprite->update(deltaTime);

	bool colision = checkBricksCollisions();
	if (Game::instance().getKey(32) || colision) startRound = true;
	if (Game::instance().getKey('s') && startRound) { //probando
		if (velocity.x == 0.0f && velocity.y == 0.0f) velocity = velocity_aux;
		else velocity = glm::vec2(0.0f, 0.0f);
	}
	if (startRound) {
		checkBorderCollisions();

		posBall += velocity * direction;

		if (colision) map->prepareArrays(glm::vec2(16, 32), texProgram);

	}

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBall.x), float(tileMapDispl.y + posBall.y)));
}

void Ball::render() {
	sprite->render();
}

void Ball::setRoom(int room)
{
	cout << "changed room from " << this->room <<" to "<< room << endl; 
	this->room = room;
}
glm::vec2 Ball::toPosMap(glm::vec2 pos) {
	pos.y = pos.y +  (3 - room) * 16 * 26 ;  // tile_size_pixel * height_room_tile =  height_room_pixel
	return pos;
}

glm::vec2 Ball::toPosWindow(glm::vec2 pos)
{
	pos.y = pos.y - (3 - room) * 16 * 26;
	return pos;
}

void Ball::setTileMap(TileMap *tileMap) { map = tileMap; }
void Ball::setPosition(const glm::vec2 &pos) {
	posBall = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posBall.x), float(tileMapDispl.y + posBall.y)));
}