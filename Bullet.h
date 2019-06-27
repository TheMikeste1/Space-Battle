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

#define BULLET_SPEED 5.0

class Bullet : public Object
{
private:
	int decay; //When decay == 40, Bullet dies.

public:
	//Constructors
	Bullet() : decay(0) {}
	//Deconstructors
	~Bullet() {}

	//Getters
	int getDecay() { return decay; }
	//Setters

	//Others
	void fire(Point point, float angle, Velocity velocity);
	void draw();
};

#endif