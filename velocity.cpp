#include "velocity.h"


/**********************************************************************
* Setters
* Sets private variables.
***********************************************************************/
void Velocity::setDx(float force)
{
	deltaX = force;
}

void Velocity::setDy(float force)
{
	deltaY = force;
}