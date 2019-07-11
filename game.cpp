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
* Prototypes
*********************************************/
//void sendThread(const Ship* const playerShip, const vector<Bullet>* const bullets, SOCKET socket);

/*********************************************
* Constructors
*********************************************/
Game::Game(Point topLeft, Point bottomRight) 
: topLeft(topLeft), bottomRight(bottomRight), shipNumber(0)
{
	Ship* ship = new Ship();
	ships.push_back(ship);
}

Game::Game(Point topLeft, Point bottomRight, int shipNumber, int numPlayers) 
: topLeft(topLeft), bottomRight(bottomRight), shipNumber(shipNumber)
{
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
}

Game::~Game()
{
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
	for (list<Bullet>::iterator it = bullets.begin(); it != bullets.end(); ++it)
	{
		it->move();

		//If the asteroid is off screen horizontally...
		if (isOffScreen(it->getPoint().getX()))
			it->setPoint(Point(-it->getPoint().getX(), it->getPoint().getY()));
		//If the asteroid is off screen vertically...
		if (isOffScreen(it->getPoint().getY()))
		{
			it->setPoint(Point(it->getPoint().getX(), -it->getPoint().getY()));
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
	for (list<Bullet>::iterator it = bullets.begin(); it != bullets.end(); ++it)
	{
		if (it->isAlive())
		{
			it->draw();
		}
	}
}

void Game::handleCollisions()
{
	for (list<Bullet>::iterator it = bullets.begin(); it != bullets.end(); ++it)
	{
		if (it->isAlive())
		{
			//Bullet hits ship
			for (int j = 0; j < ships.size(); j++)
			{
				if (ships[j]->isAlive())
				{
					if (getClosestDistance(*it, *ships[j]) <= ships[j]->getSize())
					{
						ships[j]->kill();
						ships[j]->setThrust(false);
						ships[j]->setVelocity(Velocity(0, 0));
						it->kill();
						drawLanderFlames(it->getPoint(), 1, 1, 1);
					}
				}
			}
		}
	}
}


void Game::cleanUp()
{
	//Bullets
	list<Bullet>::iterator bulletIt = bullets.begin();
	while (bulletIt != bullets.end())
	{
		Bullet* bullet = &*bulletIt;
		++bulletIt;

		if (!bullet->isAlive())
		{
			bullets.remove(*bullet);
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

/*********************************************
* Networking
*********************************************/
void Game::setupConnection() throw(const char*)
{
	try
	{
		connection.initialize();
	} catch (...)
	{
		throw "Unable to connect to server.";
	}
}

/*********************************************
* Threading
*********************************************/
#define CRLF "\r\n"
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


	const list<Bullet>* const bullets = &game->bullets;
	if (!bullets->empty())
	{
		for (list<Bullet>::const_iterator it = bullets->begin(); it != bullets->end(); ++it)
		{
			const Bullet* const bullet = &*it;
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
	//Add end stream character
	data.append(CRLF);
	return data;
}

void sendToServer(Game* const game, Connection* const connection)
{
	while (connection->connected())
	{
		try
		{
			connection->sendData(concatenateGameData(game));
		} catch (const int error)
		{
			cout << "Error sending info to server: " << error << endl;
		}

		Sleep(20);
	}

	cout << "Done receiving from server\n";
}

void Game::setupClientSend()
{
	sendThread = new thread(sendToServer, this, &this->connection);
}

void updateGameData(Game* const game, string data)
{
	unsigned int pos1 = 0;
	unsigned int pos2 = data.find('|');

	string dataPiece = data.substr(pos1, pos2);	
	

	list<Bullet> newBullets;

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
	} while (pos1 < data.size() && dataPiece != CRLF);

	game->bullets = newBullets;

	if (dataPiece != CRLF)
	{
		throw "Incoming stream ended unexpectedly!";
	}
}

void receiveFromServer(Game* const game, Connection* const connection)
{
	while (connection->connected())
	{
		string data = CRLF;
		try
		{
			data = connection->receiveData();
		} catch (const int error)
		{
			cout << "Error receiving info from server: " << error << endl;
			break;
		}

		updateGameData(game, data);
	}

	cout << "Done receiving from server\n";
}

void Game::setupClientReceive()
{
	receiveThread = new thread(receiveFromServer, this, &this->connection);
}
