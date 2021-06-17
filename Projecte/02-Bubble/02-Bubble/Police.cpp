#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Police.h"
#include "Game.h"
#include <chrono>
#include <thread>




enum EyeStates {
	LEFT_TOP, LEFT_CENTER, LEFT_BOTTOM,
	RIGHT_TOP, RIGHT_CENTER, RIGHT_BOTTOM, TOP, CENTER, BOTTOM, LOOK_AROUND
};

/*procedimiento: inicializar - como actualizo al jugador - como lo muestro */
void Police::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Player * player) {
	width = 32;
	height = 16;
	matrix_positions[0][0] = LEFT_TOP;
	matrix_positions[0][1] = TOP;
	matrix_positions[0][2] = RIGHT_TOP;
	matrix_positions[1][0] = LEFT_CENTER;
	matrix_positions[1][1] = CENTER;
	matrix_positions[1][2] = RIGHT_CENTER;
	matrix_positions[2][0] = LEFT_BOTTOM;
	matrix_positions[2][1] = BOTTOM;
	matrix_positions[2][2] = RIGHT_BOTTOM;
	spritesheet_eyes.loadFromFile("images/police_eyes.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite_eyes = Sprite::createSprite(glm::ivec2(width, height), glm::vec2(0.25, 0.125), &spritesheet_eyes, &shaderProgram);

	sprite_eyes->setNumberAnimations(10);

	sprite_eyes->setAnimationSpeed(LEFT_TOP, 8);		sprite_eyes->addKeyframe(LEFT_TOP, glm::vec2(0.0f, 0.0f));
	sprite_eyes->setAnimationSpeed(LEFT_CENTER, 8);		sprite_eyes->addKeyframe(LEFT_CENTER, glm::vec2(0.0f, 0.125f));
	sprite_eyes->setAnimationSpeed(LEFT_BOTTOM, 8);		sprite_eyes->addKeyframe(LEFT_BOTTOM, glm::vec2(0.0f, 0.25f));

	sprite_eyes->setAnimationSpeed(RIGHT_TOP, 8);		sprite_eyes->addKeyframe(RIGHT_TOP, glm::vec2(0.5f, 0.0f));
	sprite_eyes->setAnimationSpeed(RIGHT_CENTER, 8);	sprite_eyes->addKeyframe(RIGHT_CENTER, glm::vec2(0.5f, 0.125f));
	sprite_eyes->setAnimationSpeed(RIGHT_BOTTOM, 8);	sprite_eyes->addKeyframe(RIGHT_BOTTOM, glm::vec2(0.5f, 0.25f));

	sprite_eyes->setAnimationSpeed(TOP, 8);				sprite_eyes->addKeyframe(TOP, glm::vec2(0.25f, 0.0f));
	sprite_eyes->setAnimationSpeed(CENTER, 8);			sprite_eyes->addKeyframe(CENTER, glm::vec2(0.25f, 0.125f));
	sprite_eyes->setAnimationSpeed(BOTTOM, 8);			sprite_eyes->addKeyframe(BOTTOM, glm::vec2(0.25f, 0.25f));

	sprite_eyes->setAnimationSpeed(LOOK_AROUND, 4);
	sprite_eyes->addKeyframe(LOOK_AROUND, glm::vec2(0.25f, 0.0f));
	sprite_eyes->addKeyframe(LOOK_AROUND, glm::vec2(0.5f, 0.0f));
	sprite_eyes->addKeyframe(LOOK_AROUND, glm::vec2(0.5f, 0.125f));
	sprite_eyes->addKeyframe(LOOK_AROUND, glm::vec2(0.5f, 0.25f));
	sprite_eyes->addKeyframe(LOOK_AROUND, glm::vec2(0.25f, 0.25f));
	sprite_eyes->addKeyframe(LOOK_AROUND, glm::vec2(0.0f, 0.25f));
	sprite_eyes->addKeyframe(LOOK_AROUND, glm::vec2(0.0f, 0.125f));
	sprite_eyes->addKeyframe(LOOK_AROUND, glm::vec2(0.0f, 0.0f));
	sprite_eyes->addKeyframe(LOOK_AROUND, glm::vec2(0.25f, 0.125f));

	current_looking_sprite = CENTER;
	sprite_eyes->changeAnimation(current_looking_sprite);
	
	tileMapDispl = tileMapPos;
	arrive_destine = true;
	eyesCont = 0;
	counter = 0;
	posIni.x = 0; posIni.y = 0;
	posPolice = posIni;
	sprite_eyes->setPosition(glm::vec2(float(tileMapDispl.x + posPolice.x), float(tileMapDispl.y + posPolice.y)));
	this->player = player;
	lastposPlayer = player->getPosPlayer();
	playerwidth = player->getWidth();
	playerHeight = player->getHeight();
	acumulado.x = 0.f , acumulado.y = 0.f;

}

void Police::restart() {
	posPolice = posIni;//It's not 0,0 always because the posIni can be change
	arrive_destine = true;
	eyesCont = 0;
	counter = 0;
}


int Police::relativeHorizontalPosition() {
	// calcualtes the realtive position of player in reference to police.
	if (lastposPlayer.x + playerwidth > posPolice.x){
		if (posPolice.x + width > lastposPlayer.x) {
			// horizonal middle, 
			return 1; // MIDDLE;
		}
		else
			return 2;//player is in the RIGHT of the police; 
	}
	else return 0;// player is in the LEFT of the police;
}

int Police::relativeVerticalPosition() {
	if (lastposPlayer.y + playerHeight/2 > posPolice.y){
		if (posPolice.y + height > lastposPlayer.y) {
			// vertical middle(x)
			return 1;//MIDDLE;
		}
		else return 2;// DOWN;
	}
	else return 0; //UP;
}

int Police::calculateEyeSprite() {

	// width and height 32 x 16
	 //   _ _
	//  | _ _ |
	int relativeHorizontal = relativeHorizontalPosition();
	int relativeVertical = relativeVerticalPosition();
	row = relativeVertical; col = relativeHorizontal;
	cout << "row :" << relativeVertical << " col: " << relativeHorizontal << endl;
	return matrix_positions[relativeVertical][relativeHorizontal];
}

void Police::changeAnimation(int animId) {
	current_looking_sprite = animId;
	sprite_eyes->changeAnimation(current_looking_sprite);
}

bool Police::checkPlayerCollisions() {
	//posBall.x <= brx + 16 && posBall.x + 16 >= brx && posBall.y <= bry + 16 && posBall.y + 16 >= bry
	int current_play_pos_x = player->getPosPlayer().x; int current_play_pos_y = player->getPosPlayer().y;
	if ((posPolice.x <= current_play_pos_x + playerwidth) && (posPolice.x + width >= current_play_pos_x) && (posPolice.y <= current_play_pos_y + playerHeight) && (posPolice.y + height >= current_play_pos_y)) {
		player->setCollisionWithpolice(true);
		changeAnimation(calculateEyeSprite());
		arrive_destine = true;
		return true;
	}
	else return false;
}

void Police::iniMovement()
{
	lastposPlayer = player->getPosPlayer();
	glm::vec2 posCenterPlayer = { glm::vec2(lastposPlayer.x + width / 2 , lastposPlayer.y + height / 2) };

	int dif_x = (posPolice.x + width / 2 - posCenterPlayer.x);
	int dif_y = (posPolice.y + width / 2 - posCenterPlayer.y);


	int aux_x = dif_x;
	int aux_y = dif_y;

	if (aux_y < 0) aux_y *= -1;
	if (aux_x < 0) aux_y *= -1;
	float angulo;
	if (dif_x > dif_y) angulo = M_PI/2 - atan(aux_x / aux_y);
	else angulo = atan(aux_y / aux_x);

	cout << "diferencia_x: " << dif_x << "diferencia_y:" << dif_y << endl;
	/*if (aux < 0) aux *= -1;*/
	//aux = 1;
	cout << "angulo:(grados) " << (angulo * 1.f / M_PI) * 180.f << endl;
	//angulo =  45.f / 180.f * M_PI;
	/*int xa = cos(angulo) * dis_parcial;
	int ya = sin(angulo) * dis_parcial;*/
	float angulo_grados = (angulo  * 1.f )/ M_PI * 180.f;
	if (angulo_grados < 30) {
		cout << "set 0 dregress" << endl;
		angulo_modificado = true;
		angulo = 0; // 0 grados
		vy_zero = true;
		vx_zero = false;
		
	}
	else if (angulo_grados > 60) {
		angulo_modificado = true;
		cout << "set 90 dregress" << endl;
		angulo = M_PI/2.0f;// 90 grados
		cout << "entro_aqui" << endl;
		vx_zero = true;
		vy_zero = false;
	}
	else {
		angulo_modificado = false;
	}
	velocity = 5;
	velocidad.x = cos(angulo) * velocity; velocidad.y = sin(angulo) * velocity;// velocidad no es necesariamente un vector unitario
	direccion.x = cos(angulo) * 1.f; direccion.y = sin(angulo) * 1.f;// direccion es un vector unitario

	if (velocidad.x < 0 ) velocidad.x *= -1;
	if (velocidad.y < 0) velocidad.y *= -1; 

	if (dif_x < 0) {
		//cout << "moving right" << endl;
		move_in_x = 1;  // going to right
		direccion.x  *= +1;
	}
	else if (dif_x > 0) {
		//cout << "moving left" << endl;
		move_in_x = -1; //going to left
		direccion.x  *= -1;
	}
	else {
		move_in_x = 0; // don't move in x
		direccion.x=0;
	}

	if (dif_y < 0) {
		move_in_y = 1; //going down hacia posiciones mayores
		direccion.y *= 1;
	}
	else if (dif_y > 0) {
		move_in_y = -1; // going up  hacia posiciones menores
		direccion.y *= -1;
	}
	else {
		move_in_y = 0;// don't move in y
		direccion.y = 0;
	}

	//cout << velocity << endl;
	//cout << "direccion x:" << direccion.x  <<"  y: "<< direccion .y << endl;
	//cout << "velocidad x:" << velocidad.x << "  y: " << velocidad.y << endl;
	arrive_pos_in_y = arrive_pos_in_x = false;
}
void Police::move() {
	//cout << "Moving" << endl;
	if (!arrive_pos_in_x) {
		//cout << "arrive in final Y: " << arrive_pos_in_y << "angulo modificado: " << angulo_modificado << " vy_zero: " << vy_zero << " vx_zero:  " <<vx_zero  <<" velocidad.x " << velocidad.x << endl;
		if (!angulo_modificado || (angulo_modificado && vy_zero && !vx_zero)) {
			posPolice.x += velocidad.x * direccion.x;
		}
		else if (arrive_pos_in_y && angulo_modificado && !vy_zero && vx_zero) {
			posPolice.x += velocity * move_in_x;
		}
	}
	if (!arrive_pos_in_y) {

		if ( !angulo_modificado || (angulo_modificado && !vy_zero) ) {
			posPolice.y += velocidad.y * direccion.y;
		}
		if (arrive_pos_in_x && angulo_modificado && vy_zero && !vx_zero) {
			posPolice.y += velocity * move_in_y;
		}
	}
    
	glm::vec2 c_pl = { lastposPlayer.x + playerwidth / 2 ,lastposPlayer.y + playerHeight / 2 };
	glm::vec2 c_po = { posPolice.x + width / 2, posPolice.y + height / 2 };
	//cout << "c_po  x:" << c_po.x <<" y: " << c_po.y << endl;
	//cout << "move in x : " << move_in_x <<" in y "<< move_in_y <<endl;
	if ((move_in_x == 1 && c_po.x >= c_pl.x) ||( move_in_x == -1 && c_po.x <= c_pl.x) || move_in_x == 0){
			arrive_pos_in_x = true;
	}
	if ((move_in_y == 1 && c_po.y >= c_pl.y) || (move_in_y == -1 && c_po.y <= c_pl.y) || move_in_y == 0) {
		arrive_pos_in_y = true;
	}
	if (arrive_pos_in_x && arrive_pos_in_y) {
		arrive_pos_in_y = arrive_pos_in_x = false;
		arrive_destine = true;
	}
}

void Police::update(int deltaTime)
{
	if (!checkPlayerCollisions()) {
		if (arrive_destine) {
			++eyesCont;
			if (eyesCont == 50) changeAnimation(LOOK_AROUND);
			if (eyesCont == 150) {
				eyesCont = 0;
				arrive_destine = false;
				iniMovement();
				changeAnimation(calculateEyeSprite());
			}
		}
		else {
			timeBetweenMovement += deltaTime;
			/*if (counter || Game::instance().getKey((int)'q') && timeBetweenMovement >= 1 / 2){
				timeBetweenMovement = 0;
				move();
			}*/
			if (timeBetweenMovement >= 100.f) {
				// this controls the posicion of the police
				timeBetweenMovement = 0.f;
				move();
			}
		}
	}
	
	sprite_eyes->update(deltaTime);

	sprite_eyes->setPosition(glm::vec2(float(tileMapDispl.x + posPolice.x), float(tileMapDispl.y + posPolice.y)));
}

void Police::render() {
	sprite_eyes->render();
}

void Police::setTileMap(TileMap *tileMap)
{
	map = tileMap;
}

void Police::setPosition(const glm::vec2 &posWindow) {

	posIni = posPolice = posWindow;// window posicion
	sprite_eyes->setPosition(glm::vec2(float(tileMapDispl.x + posPolice.x), float(tileMapDispl.y + posPolice.y)));
}




