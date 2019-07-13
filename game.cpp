/*********************************************************************
 * File: game.cpp
 * Description: Contains the implementaiton of the game class
 *  methods.
 *
 *********************************************************************/

//Game
#include "game.h"

//Data
#include <string>
#include <vector>

// These are needed for the getClosestDistance function...
//Math
#include <limits>
#include <algorithm>
#include <cmath>

#define FLT_MAX 3.402823466e+38F

#define OFF_SCREEN_BORDER_AMOUNT 20.0

/*********************************************
* Constructors
*********************************************/
Game::Game(Point topLeft, Point bottomRight) 
: topLeft(topLeft), bottomRight(bottomRight), shipNumber(0)
{}

Game::~Game()
{
 	connection.disconnect();
	connection.cleanUp();	

	if (sendThread != NULL)
	{
		sendThread->join();
		delete sendThread;
		sendThread = NULL;
	}

	if (receiveThread != NULL)
	{
		receiveThread->join();
		delete receiveThread;
		receiveThread = NULL;
	}

	//Clean up
	if (bullets.size() != 0)
		bullets.clear();

	for (int i = 0; i < ships.size(); i++)
	{
		delete ships[i];
		ships[i] = NULL;
	}

	ships.clear();
}

// You may find this function helpful...          Indeed I did. :)
/**********************************************************
 * Function: getClosestDistance
 * Description: Determine how close these two objects will
 *   get in between the frames.
 **********************************************************/
float Game :: getClosestDistance(const Object &obj1, const Object &obj2) const
{
   // find the maximum distance traveled
   float dMax = max(abs(obj1.getVelocity().getDx()), abs(obj1.getVelocity().getDy()));
   dMax = max(dMax, abs(obj2.getVelocity().getDx()));
   dMax = max(dMax, abs(obj2.getVelocity().getDy()));
   dMax = max(dMax, 0.1f); // when dx and dy are 0.0. Go through the loop once.
   
   float distMin = FLT_MAX;
   for (float i = 0.0; i <= dMax; i++)
   {
      Point point1(obj1.getPoint().getX() + (obj1.getVelocity().getDx() * i / dMax),
                     obj1.getPoint().getY() + (obj1.getVelocity().getDy() * i / dMax));
      Point point2(obj2.getPoint().getX() + (obj2.getVelocity().getDx() * i / dMax),
                     obj2.getPoint().getY() + (obj2.getVelocity().getDy() * i / dMax));
      
      float xDiff = point1.getX() - point2.getX();
      float yDiff = point1.getY() - point2.getY();
      
      float distSquared = (xDiff * xDiff) +(yDiff * yDiff);
      
      distMin = min(distMin, distSquared);
   }
   
   return sqrt(distMin);
}

/*********************************************
* Advances
*********************************************/
/*********************************************
* advance()
* Advances the game a frame. Master function.
*********************************************/
void Game::advance()
{
	advanceShip();
	advanceBullets();

	handleCollisions();
	cleanUp();
}

void Game::advanceShip()
{
	//lock_guard<mutex> lock(*mtx);
	for (vector<Ship*>::iterator it = ships.begin(); it != ships.end(); ++it)
	{
		Ship* ship = *it;
		//Thrust
		if (ship->getThrust())
			ship->increaseVelocity();

		ship->move();

		//If the ship is off screen horizontally...
		if (isOffScreen(ship->getPoint().getX()))
			ship->setPoint(Point(-ship->getPoint().getX(), ship->getPoint().getY()));
		//If the ship is off screen vertically...
		if (isOffScreen(ship->getPoint().getY()))
		{
			ship->setPoint(Point(ship->getPoint().getX(), -ship->getPoint().getY()));
		}
	}
}

void Game::advanceBullets()
{
	for (int i = 0; i < bullets.size(); i++)
	{
		bullets[i].move();

		//If the asteroid is off screen horizontally...
		if (isOffScreen(bullets[i].getPoint().getX()))
			bullets[i].setPoint(Point(-bullets[i].getPoint().getX(), bullets[i].getPoint().getY()));
		//If the asteroid is off screen vertically...
		if (isOffScreen(bullets[i].getPoint().getY()))
		{
			bullets[i].setPoint(Point(bullets[i].getPoint().getX(), -bullets[i].getPoint().getY()));
		}

	}
}

/*********************************************
* Misc
*********************************************/
/*********************************************
* draw()
*********************************************/
void Game::draw(const Interface&)
{
	//Ship
	for (int i = 0; i < ships.size(); i++)
	{
		if (ships[i]->isAlive()) //Draw the ships[i] if alive..
			ships[i]->draw();
		else                //Else, draw flames.
		{
			drawLanderFlames(ships[i]->getPoint(), true, true, true);
			if (i == shipNumber)
				drawText(Point(-50, 0), "You have crashed!");
		}
	}

	//Bullets
	for (int i = 0; i < bullets.size(); i++)
	{
		if (bullets[i].isAlive())
		{
			bullets[i].draw();
		}
	}
}

void Game::handleCollisions()
{
	for (int i = 0; i < bullets.size(); i++)
	{
		if (bullets[i].isAlive())
		{
			//Bullet hits ship
			for (int j = 0; j < ships.size(); j++)
			{
				if (ships[j]->isAlive())
				{
					if (getClosestDistance(bullets[i], *ships[j]) <= ships[j]->getSize())
					{
						ships[j]->kill();
						ships[j]->setThrust(false);
						ships[j]->setVelocity(Velocity(0, 0));
						killShip(j);
						drawLanderFlames(bullets[i].getPoint(), 1, 1, 1);
					}
				}
			}
		}
	}
}


void Game::cleanUp()
{
	//Bullets
	for (int i = 0; i < bullets.size(); i++)
	{
		if (!bullets[i].isAlive())
		{
			bullets.erase(bullets.begin() + i);
		}
	}
}

void Game::handleInput(const Interface & ui)
{
	//const before type makes variable unchangeable, const after type makes pointer constant
	static Ship* const ship = ships[shipNumber];
	if (ship->isAlive())
	{
		//Rotate
		if (ui.isLeft())
		{
			ship->rotateLeft();
		}

		if (ui.isRight())
		{
			ship->rotateRight();
		}

		//Thrust
		if (ui.isUp())
			ship->setThrust(true);
		else
			ship->setThrust(false);

		//Fire
		if (ui.isSpace())
		{
			Bullet newBullet;
			newBullet.fire(ship->getPoint(), ship->getRotation(), ship->getVelocity());
			bullets.push_back(newBullet);
		}
	}
}

bool Game::isOffScreen(const float place)
{
	return !(place >= topLeft.getX() - OFF_SCREEN_BORDER_AMOUNT
		&& place <= bottomRight.getX() + OFF_SCREEN_BORDER_AMOUNT);
}

void Game::killShip(int id)
{
	Ship* ship = ships[id];

	ship->kill();

	//This ought to be threaded
	try
	{
		connection.sendDataToServer("S" + to_string(id) + "|0|");
	} catch (...)
	{
		
	}
}

void Game::gameSetup() throw (const char*)
{
	try
	{
		setupConnection();
		getPlayerNumber();		
	} catch (const char* error)
	{
		cout << error << endl;
		throw "1";
	}

	unsigned int numPlayers = 0;
	if (shipNumber == 0)
	{
		do
		{
			cout << "\nHow many players? ";
			cin >> numPlayers;
		} while (numPlayers > 4 || numPlayers < 1);

		connection.sendDataToServer(to_string(numPlayers));
	}
	else
		numPlayers = getNumPlayers();

	cout << "Waiting on players to join. Total number of players: "
	     << numPlayers << endl;

	for (int i = 0; i < numPlayers; i++)
	{
		int rotation = 180;
		Point point(topLeft.getX() / 2, topLeft.getY() / 2);
		if (i == 1)
		{
			point.setX(point.getX() * -1);
			point.setY(point.getY() * -1);
			rotation = 0;
		}
		else if (i == 2)
		{
			point.setY(point.getY() * -1);
			rotation = -90;
		}
		else if (i == 3)
		{
			point.setX(point.getX() * -1);
			rotation = 90;
		}

		Ship* ship = new Ship(point);
		ship->setRotation(ship->getRotation() + rotation);
		ships.push_back(ship);
	}

	string data = connection.receiveDataFromServer();
	if (data != "G")
	{
		connection.disconnect();
		throw "1";
	}

	cout << "Game starting!\n\n";

	try
	{
		setupClientSend();
		setupClientReceive();
	} catch (const char* error)
	{
		cout << error << endl;
		throw "1";
	}
}

/*********************************************
* Networking
*********************************************/
void Game::setupConnection() throw(const char*)
{
	try
	{
		connection.initializeClient();
	} catch (...)
	{
		throw "Unable to connect to server.";
	}
	cout << "Connection to server...\n";
}

void Game::getPlayerNumber()
{
	string data = connection.receiveDataFromServer();
	shipNumber = atoi(data.c_str());

	cout << "You are Player " << shipNumber + 1 << endl;
}

unsigned int Game::getNumPlayers()
{
	string data = connection.receiveDataFromServer();
	return atoi(data.c_str());
}

/*********************************************
* Threading
*********************************************/
string concatenateGameData(Game* const game)
{
	string data;
	Ship* ship = game->ships[game->shipNumber];

	//Send ship info
	//S| == ship data
	data.append("S" + to_string(game->shipNumber) + "|");
	//Order: alive, thrust, point x, point y, velocity dx, velocity dy, rotation
	if (ship->isAlive())
	{
		string alive = "1";
		char thrust = ship->getThrust() ? '1' : '0';
		
		float x = ship->getPoint().getX();
		float y = ship->getPoint().getY();
		
		float dx = ship->getVelocity().getDx();
		float dy = ship->getVelocity().getDy();

		int rotation = ship->getRotation();

		data.append(alive + "," + thrust + ","                //Alive, Thrust
			     + to_string(x) + "," + to_string(y) + ","    //Point
		         + to_string(dx) + "," + to_string(dy) + ","  //Velocity
		         + to_string(rotation));                      //Rotation
	}
	else
		data.append("0"); //Ship is dead, send nothing else.
	//Add end section character
	data.append("|"); //End ship info


	const vector<Bullet>* const bullets = &game->bullets;
	if (!bullets->empty())
	{
		for (int i = 0; i < bullets->size(); i++)
		{
			const Bullet* const bullet = &bullets->at(i);
			//Order: id, decay, point x, point y, velocity dx, velocity dy
			if (bullet != NULL && bullet->isAlive()) //Only send info if bullet is alive
			{
				//B| == bullet info
				data.append("B|");

				int id = bullet->getId();

				int decay = bullet->getDecay();
			
				float x = bullet->getPoint().getX();
				float y = bullet->getPoint().getY();
				
				float dx = bullet->getVelocity().getDx();
				float dy = bullet->getVelocity().getDy();

				data.append(to_string(id) + "," + to_string(decay) + "," //Decay
				     + to_string(x) + "," + to_string(y) +","          //Point
			         + to_string(dx) + "," + to_string(dy));         //Velocity

				//Add end section character
				data.append("|"); //End bullet info
			}
		}
		
	}

	data.append("\r\n");

	return data;
}

void sendToServer(Game* const game, Connection* const connection)
{
	while (connection->connected())
	{
		try
		{
			connection->sendDataToServer(concatenateGameData(game));
		} catch (const int error)
		{
			cout << "Error sending to server: " << error << endl;
			break;
		}

		this_thread::sleep_for(chrono::milliseconds(5));
	}

	cout << "Done sending to server\n";
}

void Game::setupClientSend()
{
	sendThread = new thread(sendToServer, this, &this->connection);
}

void updateGameData(Game* const game, string data) throw (const char*)
{
	unsigned int pos1 = 0;
	unsigned int pos2 = data.find('|');

	string dataPiece = data.substr(pos1, pos2);	
	

	vector<Bullet> newBullets;

	do
	{
		//Ships
		if (dataPiece[0] == 'S')
		{
			//Order: alive, thrust, point x, point y, velocity dx, velocity dy, rotation
			Ship* ship = game->ships[dataPiece[1] - '0'];
			pos1 += 3; //Manually move pos1 to next data piece

			bool alive = (data[pos1] == '1') ? true : false;

			if (alive)
			{
				pos1 += 2; //Move pos1 manually past alive to save on computation

				bool thrust = (data[pos1] == '1') ? true : false;
				pos1 += 2; //Move pos1 manually past thrust to save on computation

				pos2 = data.find(',', pos1);
				dataPiece = data.substr(pos1, pos2 - pos1);
				pos1 = pos2 + 1;

				float x = atof(dataPiece.c_str());

				pos2 = data.find(',', pos1);
				dataPiece = data.substr(pos1, pos2 - pos1);
				pos1 = pos2 + 1;

				float y = atof(dataPiece.c_str());

				pos2 = data.find(',', pos1);
				dataPiece = data.substr(pos1, pos2 - pos1);
				pos1 = pos2 + 1;

				Point point(x, y);

				float dx = atof(dataPiece.c_str());

				pos2 = data.find(',', pos1);
				dataPiece = data.substr(pos1, pos2 - pos1);
				pos1 = pos2 + 1;

				float dy = atof(dataPiece.c_str());

				Velocity velocity(dx, dy);

				pos2 = data.find('|', pos1);
				dataPiece = data.substr(pos1, pos2 - pos1);
				pos1 = pos2 + 1;

				int rotation = atoi(dataPiece.c_str());

				ship->setThrust(thrust);
				ship->setPoint(point);
				ship->setVelocity(velocity);
				ship->setRotation(rotation);
			}
			else
			{
				ship->kill();
			}
		}
		else if (dataPiece[0] == 'B')
		{
			pos1 += 2; //Move manually over to first data piece
			//Order: id, decay, point x, point y, velocity dx, velocity dy
			pos2 = data.find(',', pos1);
			dataPiece = data.substr(pos1, pos2 - pos1);
			pos1 = pos2 + 1;
			
			int id = atoi(dataPiece.c_str());

			pos2 = data.find(',', pos1);
			dataPiece = data.substr(pos1, pos2 - pos1);
			pos1 = pos2 + 1;
			
			int decay = atoi(dataPiece.c_str());

			pos2 = data.find(',', pos1);
			dataPiece = data.substr(pos1, pos2 - pos1);
			pos1 = pos2 + 1;

			float x = atof(dataPiece.c_str());

			pos2 = data.find(',', pos1);
			dataPiece = data.substr(pos1, pos2 - pos1);
			pos1 = pos2 + 1;

			float y = atof(dataPiece.c_str());

			pos2 = data.find(',', pos1);
			dataPiece = data.substr(pos1, pos2 - pos1);
			pos1 = pos2 + 1;
			
			Point point(x, y);

			float dx = atof(dataPiece.c_str());

			pos2 = data.find('|', pos1);
			dataPiece = data.substr(pos1, pos2 - pos1);
			pos1 = pos2 + 1;

			float dy = atof(dataPiece.c_str());

			Velocity velocity(dx, dy);

			Bullet bullet(id, decay, point, velocity);
			newBullets.push_back(bullet);
		}

		dataPiece = data.substr(pos1, pos2 - pos1);
		pos1 = pos2 + 1;
	} while (pos1 < data.size() - 3);

	game->bullets = newBullets;
}

void receiveFromServer(Game* const game, Connection* const connection)
{
	while (connection->connected())
	{
		string data = "";
		try
		{
			data = connection->receiveDataFromServer();
		} catch (const int error)
		{
			cout << "Error receiving from server: " << error << endl;
			break;
		}

		if (data == "G")
			break;

		if (data == "")
			break;

		try
		{
			updateGameData(game, data);
		} catch (const char * error)
		{
			cout << error << endl;
		}
	}

	cout << "Done receiving from server\n";
}

void Game::setupClientReceive()
{
	receiveThread = new thread(receiveFromServer, this, &this->connection);
}
