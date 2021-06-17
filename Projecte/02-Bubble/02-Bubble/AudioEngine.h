#include <string>


class AudioEngine
{
public:
	static AudioEngine& instance()
	{
		static AudioEngine G;

		return G;
	}
	AudioEngine();
	~AudioEngine();
	void playMusic(const char* url);
	void stopMusic();
	void mainMusic();
	void LevelWinMusic();
	void touchBrickEffect();
	void touchBorderEffect();
	void touchPaddelEffect();
	void losslife();
	void alarm();

	void stopEffect();
	void selectEffect();
private:

	void init();

};

