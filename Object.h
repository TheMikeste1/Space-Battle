/***************************************************************
* File: .h
* Author: Michael Hegerhorst
* Purpose:
***************************************************************/
#ifndef OBJECT_H
#define OBJECT_H

#include "velocity.h"
#include "point.h"

#define M_PI 3.14159265358979323846

class Object
{
protected:
	Point point;
	Velocity velocity;
	bool alive;
	int rotation;

public:
	//Constructors
	Object() : alive(true) {}
	//Deconstructors
	~Object() {};
	
	//Getters
	Point getPoint() const { return point; }
	Velocity getVelocity() const { return velocity; }
	bool isAlive() const { return alive; }
	int getRotation() const { return rotation; }
	//Setters
	void setPoint(Point point);
	void setVelocity(Velocity velocity);
	void kill();
	void setRotation(int rotation) { this->rotation = rotation; }

	//Others
	void advance();
	void move()
	{
		point.addX(velocity.getDx());
		point.addY(velocity.getDy());
	}
	virtual void draw() = 0;
};

#endif