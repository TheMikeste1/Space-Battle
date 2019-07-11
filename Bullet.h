/***************************************************************
* File: .h
* Author: Michael Hegerhorst
* Purpose:
***************************************************************/
#ifndef BULLET_H
#define BULLET_H

#include <cmath>

#include "Object.h"
#include "uiDraw.h"

#define BULLET_SPEED 7.5

class Bullet : public Object
{
private:
	unsigned int id;
	int decay; //When decay == 40, Bullet dies.

public:
	//Constructors
	Bullet() : decay(0) { id = random(1, 9999999); }
	Bullet(int id, int decay, Point point, Velocity velocity):
		id(id), decay(decay) 
	{ setPoint(point); setVelocity(velocity); }
	//Deconstructors
	~Bullet() {}

	//Getters
	int getDecay() const { return decay; }
	int getId() const { return id; }

	//Setters

	//Others
	void fire(Point point, float angle, Velocity velocity);
	void draw() override;
};

inline bool operator==(unsigned int lhs, const Bullet& rhs)
{
	return lhs = rhs.getId();
}

inline bool operator==(const Bullet& lhs, const Bullet& rhs)
{
	return lhs.getDecay() == rhs.getDecay() 
	    && lhs.isAlive() == rhs.isAlive() 
	    && lhs.getVelocity() == rhs.getVelocity()
		&& lhs.getPoint() == rhs.getPoint();
}

inline bool operator==(const Bullet* lhs, const Bullet& rhs)
{
	return lhs->getDecay() == rhs.getDecay() 
	    && lhs->isAlive() == rhs.isAlive() 
	    && lhs->getVelocity() == rhs.getVelocity() 
	    && lhs->getPoint() == rhs.getPoint();
}

#endif