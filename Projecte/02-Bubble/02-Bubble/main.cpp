#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"


//Remove console (only works in Visual Studio)
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")


#define TIME_PER_FRAME 1000.f / 60.f // Approx. 60 fps (tiempo de frame = (1/framerate)*1000, resultaddo en ms)


static int prevTime;
static Game game; // This object represents our whole game


// If a key is pressed this callback is called

static void keyboardDownCallback(unsigned char key, int x, int y)
{
	Game::instance().keyPressed(key);
}

// If a key is released this callback is called

static void keyboardUpCallback(unsigned char key, int x, int y)
{
	Game::instance().keyReleased(key);
}

// If a special key is pressed this callback is called

static void specialDownCallback(int key, int x, int y)
{
	Game::instance().specialKeyPressed(key);
}

// If a special key is released this callback is called

static void specialUpCallback(int key, int x, int y)
{
	Game::instance().specialKeyReleased(key);
}

// Same for changes in mouse cursor position

static void motionCallback(int x, int y)
{
	Game::instance().mouseMove(x, y);
}

// Same for mouse button presses or releases

static void mouseCallback(int button, int state, int x, int y)
{
	if(state == GLUT_DOWN)
		Game::instance().mousePress(button);
	else if(state == GLUT_UP)
		Game::instance().mouseRelease(button);
}

static void drawCallback()
{
	Game::instance().render();
	glutSwapBuffers();
}

static void idleCallback()
{
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	int deltaTime = currentTime - prevTime;
	
	if(deltaTime > TIME_PER_FRAME)
	{
		// Every time we enter here is equivalent to a game loop execution
		/* si el usuario pulsa en ESC (se analiza en game), cerramos el juego */
		if(!Game::instance().update(deltaTime))
			exit(0);
		prevTime = currentTime;
		glutPostRedisplay();
	}
}


int main(int argc, char **argv)
{
	// GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(300, 50);
	glutInitWindowSize(SCREEN_WIDTH*1.5, SCREEN_HEIGHT*1.5);
	//The glut...Func() methods are inicialize with the funcions that will be use in each case.
	glutCreateWindow("BREAK IN");
	glutDisplayFunc(drawCallback);
	glutIdleFunc(idleCallback);
	glutKeyboardFunc(keyboardDownCallback);
	glutKeyboardUpFunc(keyboardUpCallback);
	glutSpecialFunc(specialDownCallback);
	glutSpecialUpFunc(specialUpCallback);
	glutMouseFunc(mouseCallback);
	glutMotionFunc(motionCallback);

	// GLEW(OpenGL Extension Wrangler Library) will take care of OpenGL extension functions
	glewExperimental = GL_TRUE;
	glewInit();
	
	// Game instance initialization
	Game::instance().init();
	prevTime = glutGet(GLUT_ELAPSED_TIME);
	// GLUT(OpenGL Utility Toolkit) gains control of the application
	glutMainLoop();

	return 0;
}



