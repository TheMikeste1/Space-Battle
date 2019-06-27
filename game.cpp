/*********************************************************************
 * File: game.cpp
 * Description: Contains the implementaiton of the game class
 *  methods.
 *
 *********************************************************************/

#include "game.h"

// These are needed for the getClosestDistance function...
#include <limits>
#include <algorithm>


#include <vector>
#include <cmath>

#define OFF_SCREEN_BORDER_AMOUNT 20.0

/*********************************************
* Constructors
*********************************************/
Game::Game(Point topLeft, Point bottomRight) 
: topLeft(topLeft), bottomRight(bottomRight)
{
	while (asteroids.size() < 5)
	{
		asteroids.push_back(createBigAsteroid());
	}
}

Game::Game(Point topLeft, Point bottomRight, int numAsteroids)
	: topLeft(topLeft), bottomRight(bottomRight)
{
	while (asteroids.size() < numAsteroids)
	{
		asteroids.push_back(createBigAsteroid());
	}
}

Game::~Game()
{
	//Clean up
	if (asteroids.size() != 0)
		asteroids.clear();

	if (bullets.size() != 0)
		bullets.clear();
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
   
   float distMin = std::numeric_limits<float>::max();
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
	advanceRocks();
	advanceBullets();

	handleCollisions();
	cleanUp();
}

void Game::advanceShip()
{
	//Thrust
	if (ship.getThrust())
		ship.increaseVelocity();

	ship.move();

	//If the asteroid is off screen horizontally...
	if (isOffScreen(ship.getPoint().getX()))
		ship.setPoint(Point(-ship.getPoint().getX(), ship.getPoint().getY()));
	//If the asteroid is off screen vertically...
	if (isOffScreen(ship.getPoint().getY()))
	{
		ship.setPoint(Point(ship.getPoint().getX(), -ship.getPoint().getY()));
	}
}

void Game::advanceRocks()
{
	static int timer = 0;
	if (timer <= 100)
		for (int i = 0; i < asteroids.size(); i++)
			drawSacredBird(asteroids[i]->getPoint(), 16);
	timer++;
	if (timer > 100)
	{
		for (int i = 0; i < asteroids.size(); i++)
		{
			asteroids[i]->move();

			//If the asteroid is off screen horizontally...
			if (isOffScreen(asteroids[i]->getPoint().getX()))
				asteroids[i]->setPoint(Point(-asteroids[i]->getPoint().getX(), asteroids[i]->getPoint().getY()));
			//If the asteroid is off screen vertically...
			if (isOffScreen(asteroids[i]->getPoint().getY()))
			{
				asteroids[i]->setPoint(Point(asteroids[i]->getPoint().getX(), -asteroids[i]->getPoint().getY()));
			}

			asteroids[i]->setRotation(asteroids[i]->getRotation() + asteroids[i]->getSpin());
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
* Creation
*********************************************/
BigRock * Game::createBigAsteroid()
{
	BigRock *newAsteroid = NULL;

	Point point(random(topLeft.getX(), bottomRight.getX() + 1), random(bottomRight.getY(), topLeft.getY() + 1));
	//Avoid insta-deaths
	while (point.getX() < 30 && point.getX() > -30)
		point.setX(random(topLeft.getX(), bottomRight.getX() + 1));
	while (point.getY() < 30 && point.getY() > -30)
		point.setY(random(bottomRight.getY(), topLeft.getY() + 1));

	int rotation = random(0, 360);
	Velocity velocity((-cos(M_PI / 180.0 * rotation)), (sin(M_PI / 180.0 * rotation)));

	BigRock *asteroid = new BigRock(point, rotation, velocity);
	newAsteroid = asteroid;

	return newAsteroid;
}

MediumRock * Game::createMediumAsteroid(Rock oldRock, int i)
{
	MediumRock *newAsteroid = NULL;

	int rotation = random(0, 360);
	Velocity velocity = oldRock.getVelocity();
	velocity.setDy(velocity.getDy() + i);

	MediumRock *asteroid = new MediumRock(oldRock.getPoint(), rotation, velocity);
	newAsteroid = asteroid;

	return newAsteroid;
}

SmallRock * Game::createSmallAsteroid(Rock oldRock, int i, char direction)
{
	SmallRock *newAsteroid = NULL;

	int rotation = random(0, 360);
	Velocity velocity = oldRock.getVelocity();
	
	if (direction == 'y')
		velocity.setDy(velocity.getDy() + i);
	else
		velocity.setDx(velocity.getDx() + i);

	SmallRock *asteroid = new SmallRock(oldRock.getPoint(), rotation, velocity);
	newAsteroid = asteroid;

	return newAsteroid;
}

/*********************************************
* Destruction
*********************************************/
void Game:: bigRockDeath(Rock asteroid)
{
	asteroids.push_back(createMediumAsteroid(asteroid, 1));
	asteroids.push_back(createMediumAsteroid(asteroid, -1));
	asteroids.push_back(createSmallAsteroid(asteroid, 2, 'x'));
}

void Game::mediumRockDeath(Rock asteroid)
{
	asteroids.push_back(createSmallAsteroid(asteroid, -3, 'x'));
	asteroids.push_back(createSmallAsteroid(asteroid, 3, 'x'));
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
	if (ship.isAlive()) //Draw the ship if alive..
		ship.draw();
	else                //Else, draw flames.
	{
		drawLanderFlames(ship.getPoint(), true, true, true);
		drawText(Point(-50, 0), "You have crashed!");
	}

	//Asteroids
	static int timer = 0;
	if (timer > 100)
	{
		for (int i = 0; i < asteroids.size(); i++)
		{
			if (asteroids[i]->isAlive())
			{
				asteroids[i]->draw();
			}
		}
	}
	timer++;

	static int ticks = 0;
	if (asteroids.size() == 0 && ticks < 300)
	{
		drawText(Point(-50, 0), "You have won!");
		ticks++;
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
	static int timer = 0;
	if (timer > 100)
	{
		for (int i = 0; i < asteroids.size(); i++)
		{
			if (asteroids[i]->isAlive())
				//Ship crashes into asteroid
				if (ship.isAlive())
				{
					if (getClosestDistance(*asteroids[i], ship) <= asteroids[i]->getSize() + ship.getSize())
					{
						ship.kill();
						ship.setThrust(false);
						ship.setVelocity(Velocity(0, 0));
						asteroids[i]->kill();
						if (asteroids[i]->getSize() == BIG_ROCK_SIZE)
							bigRockDeath(*asteroids[i]);
						else if (asteroids[i]->getSize() == MEDIUM_ROCK_SIZE)
							mediumRockDeath(*asteroids[i]);
						else
							drawLanderFlames(asteroids[i]->getPoint(), 1, 1, 1);
					}
				}
			//Bullet hits asteroid
			for (int t = 0; t < bullets.size(); t++)
				if (bullets[t].isAlive())
					if (getClosestDistance(*asteroids[i], bullets[t]) <= asteroids[i]->getSize() + 3)
					{
						bullets[t].kill();
						asteroids[i]->kill();
						if (asteroids[i]->getSize() == BIG_ROCK_SIZE)
							bigRockDeath(*asteroids[i]);
						else if (asteroids[i]->getSize() == MEDIUM_ROCK_SIZE)
							mediumRockDeath(*asteroids[i]);
						else
							drawLanderFlames(asteroids[i]->getPoint(), 1, 1, 1);
					}
		}
	}
	timer++;
}


void Game::cleanUp()
{
	//Asteroids
	vector<Rock*> :: iterator rockIt = asteroids.begin(); //Haha, rockIt == rocket.
	while (rockIt != asteroids.end())
	{
		Rock* asteroid = *rockIt;

		if (!asteroid->isAlive())
			rockIt = asteroids.erase(rockIt);
		else
			++rockIt;
	}

	//Bullets
	vector<Bullet>::iterator bulletIt = bullets.begin();
	while (bulletIt != bullets.end())
	{
		Bullet bullet = *bulletIt;

		if (!bullet.isAlive())
		{
			bulletIt = bullets.erase(bulletIt);
		}
		else
		{
			++bulletIt;
		}
	}

}

void Game::handleInput(const Interface & ui)
{
	if (ship.isAlive())
	{
		//Rotate
		if (ui.isLeft())
		{
			ship.rotateLeft();
		}

		if (ui.isRight())
		{
			ship.rotateRight();
		}

		//Thrust
		if (ui.isUp())
			ship.setThrust(true);
		else
			ship.setThrust(false);

		//Fire
		if (ui.isSpace())
		{
			Bullet newBullet;
			newBullet.fire(ship.getPoint(), ship.getRotation(), ship.getVelocity());
			bullets.push_back(newBullet);
		}
	}
}

bool Game::isOffScreen(const float place)
{
	return !(place >= topLeft.getX() - OFF_SCREEN_BORDER_AMOUNT
		&& place <= bottomRight.getX() + OFF_SCREEN_BORDER_AMOUNT);
}