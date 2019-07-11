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


/*************************************
 * All the interesting work happens here, when
 * I get called back from OpenGL to draw a frame.
 * When I am finished drawing, then the graphics
 * engine will wait until the proper amount of
 * time has passed and put the drawing on the screen.
 **************************************/
void callBack(const Interface *pUI, void *p)
{
   Game *pGame = (Game *)p;
   
   pGame->advance();
   pGame->handleInput(*pUI);
   pGame->draw(*pUI);
	
	
   vector<Ship*> ships = pGame->getShips();
	int dead = 0;
	int shipAlive = 0;
	for (int i = 0; i < ships.size(); i++)
	{
		if (!ships[i]->isAlive())
			dead++;
		else
			shipAlive = i;
	}

	if (dead == ships.size() - 1)
	{
		string message("Game over! Player " + to_string(shipAlive + 1) + " won!");
		throw message;
	}
}


/*********************************
 * Main is pretty sparse.  Just initialize
 * the game and call the display engine.
 * That is all!
 *********************************/
int main(int argc, char * argv[])
{
	unsigned int numPlayers = 0;
	do
	{
		cout << "How many players? ";
		cin >> numPlayers;
	} while (numPlayers > 4 || numPlayers < 1);

    Point topLeft(-500, 500);
    Point bottomRight(500, -500);
   
    Game game(topLeft, bottomRight, 0, numPlayers);
	
	try
	{
		game.setupConnection();
		game.setupClientSend();
		game.setupClientReceive();
	} catch (const char* error)
	{
		cout << error << endl;
		return 1;
	}

	Interface ui(argc, argv, "Space Battle", topLeft, bottomRight);
	try
	{
		ui.run(callBack, &game);
	} catch (string& error)
	{
		cout << error << endl;
	}

	cin.ignore();
	cin.ignore();

    return 0;
}
