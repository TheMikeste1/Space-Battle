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
	int decay; //When decay == 40, Bullet dies.

public:
	//Constructors
	Bullet() : decay(0) {}
	Bullet(int decay, Point point, Velocity velocity):
		decay(decay) { setPoint(point); setVelocity(velocity); }
	//Deconstructors
	~Bullet() {}

	//Getters
	int getDecay() const { return decay; }
	//Setters

	//Others
	void fire(Point point, float angle, Velocity velocity);
	void draw() override;
};

inline bool operator==(const Bullet& lhs, const Bullet& rhs)
{
	if (lhs.getDecay() == rhs.getDecay()
		&& lhs.isAlive() == rhs.isAlive()
		&& lhs.getVelocity() == rhs.getVelocity()
		&& lhs.getPoint() == rhs.getPoint())
	{
		return true;
	}

	return false;
}

inline bool operator==(const Bullet* lhs, const Bullet& rhs)
{
	if (lhs->getDecay() == rhs.getDecay()
		&& lhs->isAlive() == rhs.isAlive()
		&& lhs->getVelocity() == rhs.getVelocity()
		&& lhs->getPoint() == rhs.getPoint())
	{
		return true;
	}

	return false;
}

#endif