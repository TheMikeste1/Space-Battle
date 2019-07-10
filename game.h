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

//Threading
#include <thread>

//Networking
#include "connection.h"

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
	Connection connection;

	//Threading
	thread* clientSend;
	thread* clientReceive;

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

	//Networking
	void setupConnection() throw(const char*);

	//Threading
	void setupClientSend() throw(const char*);
	void setupClientReceive() throw(const char*);
};


#endif /* GAME_H */
