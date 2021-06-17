#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Scene.h"
#include "Game.h"
#include "AudioEngine.h"


/*el punto inicial para dibujar tilemap en window, hay que cambiar: (x,y)= (16, 32)*/
#define SCREEN_X 16
#define SCREEN_Y 32
/*posicion del jugador en la window*/
#define INIT_PLAYER_X_TILES 13
#define INIT_PLAYER_Y_TILES 20

#define INIT_POLICE_X_TILES 1
#define INIT_POLICE_Y_TILES 23

#define INIT_COIN_X_TILES 2
#define INIT_COIN_Y_TILES 22


enum GameStates
{
	MENU,
	GAME_OVER,
	INSTRUCTIONS,
	CREDITS,
	GAME_WIN,
	LEVEL_WIN,
	PLAY
};
enum BatMode {
	SMALL,
	LARGE,
	STICKY,
	GUN_BAT
};

Scene::Scene() {
	currentMap = NULL;
	player = NULL;
	ball = NULL;
	scorebar = NULL;
	coin = NULL;
}

Scene::~Scene() {
	if (currentMap != NULL)
		delete currentMap;
	if (player != NULL)
		delete player;
	if (ball != NULL)
		delete ball;
	if (scorebar != NULL)
		delete scorebar;
	if (coin != NULL)
		delete coin;
}



void Scene::startGame() {
	// needed to inicialicializa when thre ir a restart after a collion of player with police
	scroll = scroll_ini;
	policeAwakeBank[0] = policeAwakeBank[1] = policeAwakeBank[2] = false;
	policeInRoomOfBank[0] = policeInRoomOfBank[1] = policeInRoomOfBank[2] = -1;
	policePausedBank[0] = policePausedBank[1] = policePausedBank[2] = false;
	room = 1;
	policeAwake = false;
	////
	ball = new Ball();
	ball->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	ball->setPosition(glm::vec2(13 * currentMap->getTileSize(), 20 * currentMap->getTileSize()));
	ball->setTileMap(currentMap);
	ball->setRoom(room);


	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, ball);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * currentMap->getTileSize(), INIT_PLAYER_Y_TILES * currentMap->getTileSize() + 10));
	player->setTileMap(currentMap);
	int x = player->getPosPlayer().x + (32 / 2) - (16 / 2);
	int y = player->getPosPlayer().y - 16;
	ball->setPosition(glm::vec2(x, y));

	police = new Police();
	police->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, player);
	police->setPosition(glm::vec2(INIT_POLICE_X_TILES * currentMap->getTileSize(), INIT_POLICE_Y_TILES * currentMap->getTileSize()));
	police->setTileMap(currentMap);

	coin = new Coin();
	coin->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, player);
	coin->setPosition(glm::vec2(INIT_COIN_X_TILES * currentMap->getTileSize(), INIT_COIN_Y_TILES * currentMap->getTileSize()), glm::vec2(INIT_COIN_X_TILES * currentMap->getTileSize(), INIT_COIN_Y_TILES * currentMap->getTileSize() - scroll));
	coin->setRoom(room);

}


void Scene::alertPolice()
{
	if (!policeAwakeBank[bank - 1]) {
		cout << "awake _ 1" << endl;
		AudioEngine::instance().alarm();
		policeAwakeBank[bank - 1] = true;
		policeInRoomOfBank[bank - 1] = room;
	}
}


void Scene::init(){
	
	initShaders();

	// For letters: There is something wrong its not painting
	//if(!text.init("fonts/OpenSans-Bold.ttf"))
	//if (!text.init("fonts/OpenSans-Regular.ttf"))
	if (!text.init("fonts/DroidSerif.ttf"))
		cout << "Could not load font!!!" << endl;

	//Inicialization of state and the variables for each state
	state = MENU;
	//	state = PLAY;
	//state = LEVEL_WIN;
	//var of MENU
	menu_selected_pos = 0;
	//var of PLAY
	is_level_win = is_game_win = is_game_over = false;
	//set to true just for testing  
	//is_level_win = true;
	money = points = 0;
	batmode = SMALL;
	lives = 4;
	room = bank = 1;
	//bank = 3;// second level (level = bank)
	// Inicializations od the backgrounds
	glm::vec2 geom[2] = { glm::vec2(0.f, 0.f), glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT) };
	glm::vec2 texCoords[2] = { glm::vec2(0.f, 0.f), glm::vec2(1.f, 1.f) };

	background = TexturedQuad::createTexturedQuad(geom, texCoords, texProgram);
	//Background constant
	windowsTexs[MENU].loadFromFile("images/backgrounds/menu.png", TEXTURE_PIXEL_FORMAT_RGBA);
	windowsTexs[GAME_OVER].loadFromFile("images/backgrounds/game_final.png", TEXTURE_PIXEL_FORMAT_RGBA);
	windowsTexs[INSTRUCTIONS].loadFromFile("images/backgrounds/instructions.png", TEXTURE_PIXEL_FORMAT_RGBA);
	windowsTexs[CREDITS].loadFromFile("images/backgrounds/credits.png", TEXTURE_PIXEL_FORMAT_RGBA);
	windowsTexs[GAME_WIN].loadFromFile("images/backgrounds/game_win.png", TEXTURE_PIXEL_FORMAT_RGBA);
	windowsTexs[LEVEL_WIN].loadFromFile("images/backgrounds/level_win.png", TEXTURE_PIXEL_FORMAT_RGBA);


	//Background dynamic
	//windowsTexs[PLAY].loadFromFile("images/backgrounds/play", TEXTURE_PIXEL_FORMAT_RGB);// It depends on the level we need at least 3


	geom[0] = glm::vec2(0.f, 0.f); geom[1] = glm::vec2(352.f, 368.f);
	texCoords[0] = glm::vec2(0.f, 0.0f); texCoords[1] = glm::vec2(1.0f, 1.0f);
	texQuad[0] = TexturedQuad::createTexturedQuad(geom, texCoords, texProgram);
	texs[0].loadFromFile("images/bg1.png", TEXTURE_PIXEL_FORMAT_RGBA);
	texs[1].loadFromFile("images/bg2.png", TEXTURE_PIXEL_FORMAT_RGBA);
	texs[2].loadFromFile("images/bg3.png", TEXTURE_PIXEL_FORMAT_RGBA);
	texs[2].loadFromFile("images/bg4.png", TEXTURE_PIXEL_FORMAT_RGBA);
	texs[3].loadFromFile("images/robbery.png", TEXTURE_PIXEL_FORMAT_RGBA);

	maps[0] = TileMap::createTileMap("levels/level01.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);	/*el segundo param es para decir apartir de que punto tiene que construir el mapa de tail y que lo pinto en el shader texProgram*/
	maps[1] = TileMap::createTileMap("levels/level02.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	maps[2] = TileMap::createTileMap("levels/level03.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	currentMap = maps[bank - 1];
	glm::ivec2 mapSize = currentMap->getMapSize();
	//scrolling
	//We need to focus the initially in the first room. But we by default see the last room(3) with scroll = 0, 
	// therefore we need to scroll down, which can be express aplying a negative translation over the postions of the map, this will make aparently the map displayed in the window to go up and then we can see the first room.. 
	scroll_ini = scroll = 0 - (mapSize.y / 3) * 2 * 16;// we need to translate the size of 2 rooms, 16px is the height of a TILE
	scrolling_up = scrolling_down = false;
	is_first = true;
	current_scroll_finish = true;

	scorebar = ScoreBar::createScoreBar("levels/scorebar.txt", glm::vec2(SCREEN_X + (mapSize.x * 16), SCREEN_Y), texProgram);
	scorebar->init(money, points, lives, bank, room, batmode);

	startGame();
	ball->setLevel(bank - 1);
	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
	currentTime = 0.0f;
	goUp = goDown = false;
	policeAwakeBank[0] = policeAwakeBank[1] = policeAwakeBank[2] = false;
	policeInRoomOfBank[0] = policeInRoomOfBank[1] = policeInRoomOfBank[2] = -1;
	win_level[0] = win_level[1] = win_level[2] = false;
	phone_was_hit = false;

	AudioEngine::instance().mainMusic();
	first_game_over = true;
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;
	if (state == PLAY) {
		if (!(scrolling_up || scrolling_down)) {
			/*if (phone_was_hit && Game::instance().getKey((int)13)) {
				phone_was_hit = false;
				//phone_was_hit = true;//temporal missing window of bonus
			}*/
			//if (!phone_was_hit) {

				player->update(deltaTime);
				ball->update(deltaTime);
				if (Game::instance().getKey((int)'a')) { 
					policeAwakeBank[bank - 1] = true;
					AudioEngine::instance().alarm();
				};
				if (policeAwakeBank[bank - 1]) {
					police->update(deltaTime);
				}
				coin->update(deltaTime);
			//}
		}
		else {
			//option 1 simple 
			if (is_first) {
				scrolling_positions = 26;
				is_first = false;
			}
			if (scrolling_positions > 0) {
				if (scrolling_up) {
					scroll += 16;// add 1 position (size of tile) 
				}
				else {
					scroll -= 16;
				}
				scrolling_positions--;
			}
			else {
				//scrolling_positions is 0
				if (policePausedBank[bank - 1] && policeInRoomOfBank[bank - 1] == room) {
					policeAwakeBank[bank - 1] = true;
					AudioEngine::instance().alarm();
					police->restart();//restart because we are awaking the the police of the bank in the previous line.
					policePausedBank[bank - 1] = false;

				}
				current_scroll_finish = true;
				scrolling_down = scrolling_up = false;
				is_first = true;
			}
		}
	}
}


void Scene::setGameState(int s) {
	if (s == GAME_OVER) {
		is_game_over = true;
	}
}

void Scene::decreaseLife() {
	lives -= 1;
}

void Scene::restartGame() {
	startGame();
	ball->setbOpenDoor(bank - 1, openDoor);
}

void Scene::changeScroll(int i) {
	if (i == 0) goDown = true;
	else if (i == 1) goUp = true;
}

void Scene::changeScrollValue(int value, int type) {
	if (type == 1) points += value;
	else if (type == 2) money += value;
}

void Scene::stopCoin() {
	coin->setVelocity(glm::ivec2(0.0f, 0.0f));
}

void Scene::hitThePhone()
{
	phone_was_hit = true;
}



void Scene::render()
{
	glm::mat4 modelview;
	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelview = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
	switch (state) {
	case(MENU):

		background->render(windowsTexs[MENU]);
		text.render("Prova:", glm::vec2(10, 32), 32, glm::vec4(1, 1, 1, 1));

		if (Game::instance().getKey((int)'p')) {
			AudioEngine::instance().selectEffect();
			state = PLAY;
		}
		else if (Game::instance().getKey((int)'i')) {
			AudioEngine::instance().selectEffect();
			state = INSTRUCTIONS;
		}
		else if (Game::instance().getKey((int)'c')) {
			AudioEngine::instance().selectEffect();
			state = CREDITS;
		}
		break;

	case(INSTRUCTIONS):
		background->render(windowsTexs[INSTRUCTIONS]);
		if (Game::instance().getKey((int)'b')) {
			AudioEngine::instance().mainMusic();
			state = MENU;
		}
		break;
	case(CREDITS):
		background->render(windowsTexs[CREDITS]);
		if (Game::instance().getKey((int)'b')) {
			AudioEngine::instance().mainMusic();
			state = MENU;
		}
		break;
	case(GAME_OVER):
		if (first_game_over) { AudioEngine::instance().losslife();  first_game_over = false; }
		//cout << "game:over" << endl;
		background->render(windowsTexs[GAME_OVER]);
		if (Game::instance().getKey((int)'m')) {
			//state = MENU;
			Game::instance().init(); //empezar a cargar de nuevo las cosas
		}
		break;
	case(GAME_WIN):
		background->render(windowsTexs[GAME_WIN]);
		if (Game::instance().getKey((int)'m')) {
			state = MENU;
			Game::instance().init();
		}
		break;
	case(LEVEL_WIN):
		texProgram.use();
		texProgram.setUniformMatrix4f("projection", projection);
		texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
		modelview = glm::mat4(1.0f);
		modelview = glm::translate(modelview, glm::vec3(16 * 2.0f, 16 * 3.0f - scroll_ini + scroll, 0.f));
		texProgram.setUniformMatrix4f("modelview", modelview);
		texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
		texQuad[0]->render(texs[3]);
		//background->render(windowsTexs[LEVEL_WIN]);


		texProgram.use();
		texProgram.setUniformMatrix4f("projection", projection);
		texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
		modelview = glm::mat4(1.0f);
		texProgram.setUniformMatrix4f("modelview", modelview);
		texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
		//cout << lives << endl;

		scorebar->render(money, points, lives, bank, room, batmode, texProgram);
		

		if (Game::instance().getKey((int)'n') || Game::instance().getKey((int)'37')) {
			state = PLAY;
			AudioEngine::instance().stopMusic();
		}

		break;
	case(PLAY):
		/*if (policeAwakeBank[bank - 1] && policeInRoomOfBank[bank - 1] == room) {
			AudioEngine::instance().alarm();
		}
		else { 
			AudioEngine::instance().stopEffect();
		}*/

		if (room == 3 && win_level[bank - 1] == false && currentMap->empty_lastRoom()) {
			win_level[bank - 1] = true;
			if (bank == 3) {
				is_game_win = true;
			}
			else {
				is_level_win = true;
			}
		}
		if (Game::instance().getKey((int)'+') || goUp) {
			goUp = false;
			//as a simple boolean 
			if (room < 3 && current_scroll_finish && !scrolling_down) {
				current_scroll_finish = false;
				scrolling_up = true;
				if (policeAwakeBank[bank - 1] && policeInRoomOfBank[bank - 1] == room) {
					policePausedBank[bank - 1] = true;
					policeAwakeBank[bank - 1] = false;
					AudioEngine::instance().stopMusic();
				}
				room++;
				/*if (policePausedBank[bank - 1] && policeInRoomOfBank[bank - 1] == room) {
				policeAwakeBank[bank - 1] = true;
				policePausedBank[bank - 1] = false;
				}*/
				ball->setRoom(room);
				coin->setPosition(glm::vec2(INIT_COIN_X_TILES * currentMap->getTileSize(), INIT_COIN_Y_TILES * currentMap->getTileSize()), glm::vec2(INIT_COIN_X_TILES * currentMap->getTileSize(), INIT_COIN_Y_TILES * currentMap->getTileSize() - scroll));
				coin->setRoom(room);
				coin->updateStates();

			}
		}
		if (Game::instance().getKey((int)'-') || goDown) {
			goDown = false;
			if (room > 1 && current_scroll_finish && !scrolling_up) {
				current_scroll_finish = false;
				scrolling_down = true;
				if (policeAwakeBank[bank - 1] && policeInRoomOfBank[bank - 1] == room) {
					policePausedBank[bank - 1] = true;
					policeAwakeBank[bank - 1] = false;
					AudioEngine::instance().stopMusic();
				}
				room--;
				/*if ( policePausedBank[bank - 1] && policeInRoomOfBank[bank - 1] == room ) {
				policeAwakeBank[bank-1] = true;
				policePausedBank[bank-1] = false;
				}*/
				ball->setRoom(room);
				coin->setPosition(glm::vec2(INIT_COIN_X_TILES * currentMap->getTileSize(), INIT_COIN_Y_TILES * currentMap->getTileSize()), glm::vec2(INIT_COIN_X_TILES * currentMap->getTileSize(), INIT_COIN_Y_TILES * currentMap->getTileSize() - scroll));
				coin->setRoom(room);
				coin->updateStates();
			}
		}

		if (Game::instance().getKey((int)'l')) {
			if (lives - 1 >= -1)--lives;
		}
		if (lives == -1) {
			is_game_over = true;
		}

		// In order to make the scrolldown work there is the need to have 3 backgrounds, one foreach room
		//For room 3
		texProgram.use();
		texProgram.setUniformMatrix4f("projection", projection);
		texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
		modelview = glm::mat4(1.0f);
		modelview = glm::translate(modelview, glm::vec3(32.f, 48.0f - (0) + scroll, 0.f));
		texProgram.setUniformMatrix4f("modelview", modelview);
		texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
		texQuad[0]->render(texs[bank - 1]);
		// For room 2
		texProgram.use();
		texProgram.setUniformMatrix4f("projection", projection);
		texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
		modelview = glm::mat4(1.0f);
		modelview = glm::translate(modelview, glm::vec3(32.f, 48.0f - scroll_ini / 2 + scroll, 0.f));
		texProgram.setUniformMatrix4f("modelview", modelview);
		texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
		texQuad[0]->render(texs[bank - 1]);
		// For room 1
		texProgram.use();
		texProgram.setUniformMatrix4f("projection", projection);
		texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
		modelview = glm::mat4(1.0f);
		modelview = glm::translate(modelview, glm::vec3(16 * 2.0f, 16 * 3.0f - scroll_ini + scroll, 0.f));
		texProgram.setUniformMatrix4f("modelview", modelview);
		texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
		texQuad[0]->render(texs[bank - 1]);

		texProgram.use();
		texProgram.setUniformMatrix4f("projection", projection);
		texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
		modelview = glm::mat4(1.0f);
		modelview = glm::translate(modelview, glm::vec3(0.f, scroll, 0.f));
		texProgram.setUniformMatrix4f("modelview", modelview);
		texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
		//map->prepareArrays(glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
		currentMap->render();


		texProgram.use();
		texProgram.setUniformMatrix4f("projection", projection);
		texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
		modelview = glm::mat4(1.0f);
		texProgram.setUniformMatrix4f("modelview", modelview);
		texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
		player->render();

		ball->render();
		if (policeAwakeBank[bank - 1]) {
			police->render();
		}
		coin->render();
		texProgram.use();
		texProgram.setUniformMatrix4f("projection", projection);
		texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
		modelview = glm::mat4(1.0f);
		texProgram.setUniformMatrix4f("modelview", modelview);
		texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
		//cout << lives << endl;

		scorebar->render(money, points, lives, bank, room, batmode, texProgram);
		if (phone_was_hit) {
			/*
			texProgram.use();
			texProgram.setUniformMatrix4f("projection", projection);
			texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
			modelview = glm::mat4(1.0f);
			texProgram.setUniformMatrix4f("modelview", modelview);
			texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
			PhoneBar->render();
			*/
		}
		//go to the next bank
		if (Game::instance().getKey((int)'n')) {
			if (time_next_bank < 2) {
				time_next_bank++;
			}
			else {
				time_next_bank = 0;
				if (bank + 1 <= 3) {

					if (policeAwakeBank[bank - 1] && policeInRoomOfBank[bank - 1] == room) {
						policePausedBank[bank - 1] = true;
						policeAwakeBank[bank - 1] = false;
						AudioEngine::instance().stopMusic();
					}
					bank++;
					if (policePausedBank[bank - 1] && policeInRoomOfBank[bank - 1] == room) {
						
						policePausedBank[bank - 1] = false;
						policeAwakeBank[bank - 1] = true;
						police->restart();
						AudioEngine::instance().alarm();
					}
					currentMap = maps[bank - 1];
					ball->setTileMap(currentMap);
					ball->setLevel(bank - 1);
					ball->setbOpenDoor(bank - 1, ball->getbopenDoor()[bank - 1]);
				}
			}
		}
		//go to the previous bank 
		if (Game::instance().getKey((int)'b')) {
			if (time_back_bank < 2) {
				time_back_bank++;
			}
			else {
				time_back_bank = 0;
				if (bank - 1 >= 1) {
					if (policeAwakeBank[bank - 1] && policeInRoomOfBank[bank - 1] == room) {
						policePausedBank[bank - 1] = true;
						policeAwakeBank[bank - 1] = false;
						AudioEngine::instance().stopMusic();
					}
					bank--;
					if (policePausedBank[bank - 1] && policeInRoomOfBank[bank - 1] == room) {
						policePausedBank[bank - 1] = false;
						policeAwakeBank[bank - 1] = true;
						AudioEngine::instance().alarm();
					}
					currentMap = maps[bank - 1];
					//Atencio every one that depends on the map should update their tilemap
					ball->setTileMap(currentMap);
					ball->setLevel(bank - 1);
					ball->setbOpenDoor(bank - 1, ball->getbopenDoor()[bank - 1]);
				}
			}
		}

		//  is_game_over is set ad hoc JUST for testing.
		//is_game_over = true;
		//is_game_win = true;
		//SET just for testing
		if (is_game_over) {
			state = GAME_OVER;
			lives = 4;
			is_game_over = false;
		}
		else if (is_game_win) {
			AudioEngine::instance().LevelWinMusic();
			state = GAME_WIN;
			is_game_win = false;
		}
		else if (is_level_win) {
			state = LEVEL_WIN;
			lives++;
			is_level_win = false;
			if (bank + 1 <= 3) {
				/*if (policeAwakeBank[bank - 1] && policeInRoomOfBank[bank - 1] == room) {
				policePausedBank[bank - 1] = true;
				policeAwakeBank[bank - 1] = false;
				}*/
				bank++;
				/*room = 1;
				if (policePausedBank[bank - 1] && policeInRoomOfBank[bank - 1] == room) {
				policePausedBank[bank - 1] = false;
				policeAwakeBank[bank - 1] = true;
				police->restart();
				}*/
				currentMap = maps[bank - 1];
				startGame();
				ball->setTileMap(currentMap);
				ball->setLevel(bank - 1);
				ball->setbOpenDoor(bank - 1, ball->getbopenDoor()[bank - 1]);
			}
			AudioEngine::instance().LevelWinMusic();
		}
		//text.render("Prova: ", glm::vec2(20, 32), 32, glm::vec4(1, 1, 1, 1));
		break;
	}
}


void Scene::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if (!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if (!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	texProgram.init();
	texProgram.addShader(vShader);
	texProgram.addShader(fShader);
	texProgram.link();
	if (!texProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << texProgram.log() << endl << endl;
	}
	texProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}



