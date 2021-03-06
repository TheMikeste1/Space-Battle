/*****************************************************
 * File: Driver.cpp
 * Author: Br. Burton
 *
 * Description: This file contains the main function
 *  that starts the game and the callback function
 *  that specifies what methods of the game class are
 *  called each time through the game loop.
 ******************************************************/
#include "game.h"
#include "uiInteract.h"
#include <string>
#include <iostream>

using namespace std;


/*************************************
 * All the interesting work happens here, when
 * I get called back from OpenGL to draw a frame.
 * When I am finished drawing, then the graphics
 * engine will wait until the proper amount of
 * time has passed and put the drawing on the screen.
 **************************************/
void callBack(const Interface *pUI, void *p) throw (const char*)
{
   Game *pGame = (Game *)p;
   
   pGame->advance();
   pGame->handleInput(*pUI);
   pGame->draw(*pUI);
	
	
   vector<Ship*> ships = pGame->getShips();
	unsigned int dead = 0;
	int shipAlive = 0;
	for (int i = 0; i < ships.size(); i++)
	{
		if (!ships[i]->isAlive())
			dead++;
		else
			shipAlive = i;
	}

	if (dead >= ships.size() - 1)
	{	
		string winText("Game over! Player " + to_string(shipAlive + 1) + " won!");
		drawText(Point(0,0), winText.c_str());
	}
}


/*********************************
 * Main is pretty sparse.  Just initialize
 * the game and call the display engine.
 * That is all!
 *********************************/
int main(int argc, char * argv[])
{
    Point topLeft(-500, 500);
    Point bottomRight(500, -500);
   
    Game game(topLeft, bottomRight);

	try
	{
		game.gameSetup();
	} catch(const char* error)
	{
		cin.ignore();
		cin.ignore();
		return atoi(error);
	}

	Interface ui(argc, argv, "Space Battle", topLeft, bottomRight);
	ui.run(callBack, &game);

	cin.ignore();
	cin.ignore();

    return 0;
}
