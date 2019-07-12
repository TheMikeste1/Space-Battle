#ifndef ship_h
#define ship_h

#define SHIP_SIZE 10

#define ROTATE_AMOUNT 6
#define THRUST_AMOUNT 0.5

#include "Object.h"


// Put your Ship class here
class Ship : public Object
{
private:
	bool thrust;
	int size;

public:
//Constructors
	Ship() : thrust(false), size(SHIP_SIZE) {}
	Ship(Point point) : thrust(false), size(SHIP_SIZE) {Object::point = point;}
	~Ship() {}

	//Getters
	float getThrust() { return thrust; }
	int getSize() { return size; }
	//Setters
	void setThrust(bool thrust) { this->thrust = thrust; }


//Others
	//Misc
	void draw();
	void increaseVelocity();
	//Movement
	void rotateLeft();
	void rotateRight();
};

#endif /* ship_h */
