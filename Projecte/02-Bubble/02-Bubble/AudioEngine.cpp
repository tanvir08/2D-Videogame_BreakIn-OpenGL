#include <Windows.h>
#include <mmsystem.h>
#include <iostream>
#include <irrKlang.h>
#include <vector>
#include "AudioEngine.h"

using namespace irrklang;



ISoundEngine* engineMusic;
ISoundEngine* engineEffects;
ISound* music;
ISound* effect;

AudioEngine::AudioEngine(){
	init();
}

AudioEngine::~AudioEngine(){
	music->drop();
	engineEffects->drop();
	engineMusic->drop();

}

void AudioEngine::init() {
	engineMusic = createIrrKlangDevice();
	engineEffects = createIrrKlangDevice();
}

void AudioEngine::playMusic(const char* url){
	engineMusic->stopAllSounds();
	music = engineMusic->play2D(url, true, false, true);
	music->setVolume(1.0f);
}

void AudioEngine::stopMusic() {
	engineMusic->stopAllSounds();
}

void AudioEngine::mainMusic() {
	stopMusic();
	playMusic("sounds/menu.wav");
}

void AudioEngine::LevelWinMusic() {
	stopMusic();
	playMusic("sounds/finish.wav");
}


void AudioEngine::selectEffect(){
	stopMusic();
	effect = engineEffects->play2D("sounds/select.wav", false, false, true);
	effect->setVolume(0.3f);
}

void AudioEngine::touchBrickEffect() {
	effect = engineEffects->play2D("sounds/brick.wav", false, false, true);
	effect->setVolume(0.3f);
}

void AudioEngine::touchBorderEffect() {
	effect = engineEffects->play2D("sounds/border.wav", false, false, true);
	effect->setVolume(0.3f);
}

void AudioEngine::touchPaddelEffect() {
	effect = engineEffects->play2D("sounds/paddle.wav", false, false, true);
	effect->setVolume(0.3f);
}

void AudioEngine::losslife()
{
	stopMusic();
	stopEffect();
	effect = engineEffects->play2D("sounds/losslife.wav", false, false, true);
	effect->setVolume(0.3f);
}

void AudioEngine::alarm()
{
	stopMusic();
	effect = engineMusic->play2D("sounds/alarm.wav", false, false, true);
	effect->setVolume(0.2f);
}


void AudioEngine::stopEffect() {
	engineEffects->stopAllSounds();
}