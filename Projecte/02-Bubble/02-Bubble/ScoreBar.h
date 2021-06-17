#ifndef _SCORE_BAR_INCLUDE
#define _SCORE_BAR_INCLUDE
#include "TileMap.h"
#include <cstring>
//#include "ShaderProgram.h"
class ScoreBar {
public:
	
	static ScoreBar *createScoreBar(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram & program);
	
	ScoreBar(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program);
	~ScoreBar();

	void init(int  money, int  points, int  lives, int  bank, int  room, int  batmode);
	void render(int  money, int  points, int  lives, int  bank, int  room, int  batmode, ShaderProgram &program);
private:
	//draws form right to left
	void draw_number(int number, int available_digits, int  pos_x, int pos_y);// pos_x pos_y are the coordinates of the last digit of the number.
	void draw_capital_words(string text, int available_digits, int  pos_x, int pos_y);
	void draw_batmode(int batmode, int  pos_x, int pos_y);
private:
	bool modified;
	int money,points,lives,bank,room;
	int batmode;
	float currentTime;
	TileMap *tilemap;
	glm::vec2 minCoords;
	ShaderProgram program;
};

#endif

