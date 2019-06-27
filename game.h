/*********************************************************************
 * File: game.h
 * Description: Defines the game class for Asteroids
 *
 *********************************************************************/

#ifndef GAME_H
#define GAME_H

//UI
#include "uiDraw.h"
#include "uiInteract.h"
//Game Objects
#include "Object.h"
#include "Bullet.h"
#include "ship.h"

#include <vector>

using namespace std;

class Game
{
private:
//Variables
	//UI
	Point topLeft;
	Point bottomRight;
	//Game Objects
	vector<Ship*> ships;
	vector<Bullet> bullets;
	//Server-Client Relationship
	int shipNumber;

//Functions
	//Advances
	void advanceShip();
	void advanceBullets();
	//Misc
	void handleCollisions();
	void cleanUp();
	bool isOffScreen(const float place);

public:
	//Constructors
	Game(Point, Point);
	Game(Point, Point, int, int);
	~Game();
	
	//Other
	float getClosestDistance(const Object &, const Object &) const;
	void advance();
	void draw(const Interface &);
	void handleInput(const Interface &);	
};


#endif /* GAME_H */
