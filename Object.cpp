#include "Object.h"

/**********************************************************************
* Setters
* Sets private variables.
***********************************************************************/

void Object::setPoint(Point point)
{
	this->point = point;
}

void Object::setVelocity(Velocity velocity)
{
	this->velocity = velocity;
}

void Object::kill()
{
	alive = false;
	
}


/**********************************************************************
* advance()
* Moves the object.
***********************************************************************/
void Object::advance()
{
	point.addX(velocity.getDx());
	point.addY(velocity.getDy());
}