#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"


void Game::init()
{
	bPlay = true;
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);	/*fondo negro*/
	scene.init();
}

bool Game::update(int deltaTime)
{
	scene.update(deltaTime);
	
	return bPlay;
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene.render();
}

void Game::keyPressed(int key){
	if(key == 27) // Escape code
		bPlay = false;
	keys[key] = true;
}

void Game::keyReleased(int key)
{
	keys[key] = false;
}

void Game::specialKeyPressed(int key)
{
	specialKeys[key] = true;
}

void Game::specialKeyReleased(int key)
{
	specialKeys[key] = false;
}

void Game::mouseMove(int x, int y)
{
}

void Game::mousePress(int button)
{
}

void Game::mouseRelease(int button)
{
}

bool Game::getKey(int key) const{
	return keys[key];
}


bool Game::getSpecialKey(int key) const
{
	return specialKeys[key];
}

void Game::setGameState(int s) {
	scene.setGameState(s);
}

void Game::decreaseLife() {
	scene.decreaseLife();
}

int Game::getLife() {
	return scene.getLife();
}

void Game::restartGame() {
	scene.restartGame();
}

void Game::setScroll(int key) {
	scene.changeScroll(key);
}

void Game::setbOpenDoor(int bopenDoor) {
	scene.setOpenDoor(bopenDoor);
}

void Game::addValue(int value, int type) {
	scene.changeScrollValue(value, type);
}

void Game::turnOnAlarm()
{
	scene.alertPolice();
}
void Game::hitThePhone()
{
	scene.hitThePhone();
}
void Game::stopCoin() {
	scene.stopCoin();
}





