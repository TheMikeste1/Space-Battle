#ifndef rocks_h
#define rocks_h

#include "Object.h"
#include "uiDraw.h"
#include <vector>

#define BIG_ROCK_SIZE 16
#define MEDIUM_ROCK_SIZE 8
#define SMALL_ROCK_SIZE 4

#define BIG_ROCK_SPIN 2
#define MEDIUM_ROCK_SPIN 5
#define SMALL_ROCK_SPIN 10



// Define the following classes here:

//   Rock
class Rock : public Object
{
private:
	int size;
	int spin;

public:
	Rock()  {}

	//Getters
	int getSize() { return size; }
	int getSpin() { return spin; }
	//Setters
	void setSize(int size) { this->size = size; }
	void setSpin(int spin) { this->spin = spin; }

	//Others
	void draw() {}
};


//   BigRock
class BigRock : public Rock
{
public:
	BigRock(Point point, int rotation, Velocity velocity)
	{
		setPoint(point); setRotation(rotation); setVelocity(velocity); setSize(BIG_ROCK_SIZE); setSpin(BIG_ROCK_SPIN);
	}
	~BigRock();
	
	//Others
	void draw() { drawLargeAsteroid(point, rotation); }
};


//   MediumRock
class MediumRock : public Rock
{
public:
	MediumRock(Point point, int rotation, Velocity velocity)
	{
		setPoint(point); setRotation(rotation); setVelocity(velocity); setSize(MEDIUM_ROCK_SIZE); setSpin(MEDIUM_ROCK_SPIN);
	}

	//Others
	void draw() { drawMediumAsteroid(point, rotation); }
};


//   SmallRock
class SmallRock : public Rock
{
public:
	SmallRock(Point point, int rotation, Velocity velocity)
	{
		setPoint(point); setRotation(rotation); setVelocity(velocity); setSize(SMALL_ROCK_SIZE); setSpin(SMALL_ROCK_SPIN);
	}

	//Others
	void draw() { drawSmallAsteroid(point, rotation); }
};


#endif /* rocks_h */
