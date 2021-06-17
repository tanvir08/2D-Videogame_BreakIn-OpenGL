#include "ScoreBar.h"

enum BatMode {
	SMALL,
	LARGE,
	STICKY,
	GUN_BAT
};

ScoreBar *ScoreBar::createScoreBar(const string & levelFile, const glm::vec2 & minCoords, ShaderProgram & program)
{
	ScoreBar *scorebar = new  ScoreBar(levelFile, minCoords, program);
	return scorebar;
}

ScoreBar::ScoreBar(const string & levelFile, const glm::vec2 & minCoords, ShaderProgram & program)
{
	tilemap = TileMap::createTileMap(levelFile, minCoords, program);
	this->minCoords = minCoords;
	this->program = program;
}

ScoreBar::~ScoreBar()
{
	if (tilemap != NULL)
		delete tilemap;
}

void ScoreBar::init(int  money, int  points, int  lives, int  bank, int  room, int  batmode)
{
	modified = false;
	//The inicialization is done with wrong values, to cause an inizalization with the values given in the first call of render.
	this -> money = this -> points = this -> lives = this -> bank = this -> room = this -> batmode = batmode = -1;
}

void ScoreBar::render(int  money, int  points, int  lives, int  bank, int  room, int  batmode, ShaderProgram & program )
{	
	
	if (this -> money != money){
		modified=true;
		this->money=money;
		draw_number(money,7,7,1);
		};
	if (this -> points != points){
		modified=true;
		this->points=points;
		draw_number(points,7,7,6);
		};
	if (this -> lives != lives){
		modified=true;
		this->lives=lives;
		draw_number(lives,2,7,12);
		};
	if (this -> bank != bank){
		modified=true;
		this->bank=bank;
		draw_number(bank,2,7,16);
		}
	if (this->batmode != batmode) {
		modified = true;
		this->batmode = batmode;
		draw_batmode(batmode, 7, 20);
	};
	if (this -> room != room){
		modified=true;
		this->room = room ;
		draw_number(room, 2,7, 23);
		};
	if (modified) {
		tilemap->prepareArrays(minCoords, this->program);
		modified = false;
	}
	tilemap->render();
}

void ScoreBar::draw_number(int number,int available_digits, int pos_x, int pos_y)
{
	//In the tilesheet the numbers 0 to 9 are in represented with the tile positions from 49 to 58 
	int *map = tilemap->getMap();
	glm::ivec2  mapSize = tilemap->getMapSize();
	int pos = pos_y*mapSize.x + pos_x;//pos_inicial
	int pos_final = pos - available_digits + 1;// pos_final 
	if (number == 0) map[pos] = 49;
	while(number > 0){
		map[pos] = (number % 10) + 49;
		pos--;
		number = number/10;
	}
	while (pos >= pos_final) {
		map[pos] = 49; // 49 in the tilesheet represents the number 0 
			pos--;
	}
}

void ScoreBar::draw_batmode(int batmode, int pos_x, int pos_y)
{
	switch (batmode) {
			case SMALL:
				draw_capital_words("SMALL", 6,pos_x, pos_y);
			break;
			case LARGE:
				draw_capital_words("LARGE", 6,pos_x, pos_y);
			break;
			case STICKY:
				draw_capital_words("STICKY", 6,pos_x, pos_y);

			break;
			case GUN_BAT:
				draw_capital_words("GUNBAT", 6,pos_x, pos_y);
			break;
	}
}

//"available_digits" has to be greater or equal to the size of text.
void ScoreBar::draw_capital_words( string text, int available_digits,int  pos_x, int pos_y)
{
	//In the spritesheet the letters A to Z are in represented with the tile positions from 80 to 105
	int *map = tilemap->getMap();
	glm::ivec2  mapSize = tilemap->getMapSize();
	int pos = pos_y*mapSize.x + pos_x;//pos_inicial
	int pos_final = pos - available_digits + 1;
	int size =  text.size();
	for (int i = size - 1 ; i >= 0 ; i--) {
		map[pos] = (text[i] - 'A') + 81;
		pos--;
	}
	while (pos >= pos_final) {
		map[pos] = 0; // blank space is represented by 0
		pos--;
	}
}





