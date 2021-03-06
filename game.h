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

//Data Types
#include <vector>

//Data Manipulation
#include <sstream>

//Threading
#include <thread>

//Networking
#include "connection.h"
#include <list>

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
	thread* sendThread    = NULL;
	thread* receiveThread = NULL;

//Functions
	//Advances
	void advanceShip();
	void advanceBullets();
	//Misc
	void handleCollisions();
	void cleanUp();
	bool isOffScreen(const float place);
	void killShip(int);

	friend string concatenateGameData(Game* const game);
	friend void updateGameData(Game* const game, string data);

	//Networking
	void setupConnection() throw(const char*);
	void getPlayerNumber();
	unsigned int getNumPlayers();

	//Threading
	void setupClientSend();
	void setupClientReceive();

public:
	//Constructors
	Game(Point, Point);
	Game(Point, Point, int);
	~Game();
	
	//Other
	float getClosestDistance(const Object &, const Object &) const;
	void advance();
	void draw(const Interface &);
	void handleInput(const Interface &);

	int getNumberPlayers() const { return ships.size(); }
	vector<Ship*> getShips() { return ships; }

	Connection& getConnection() { return connection; }

	void gameSetup() throw (const char*);	
};


#endif /* GAME_H */
