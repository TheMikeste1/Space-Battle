#include "Bullet.h"
#include <cmath>

/**********************************************************************
* fire()
* Fires the weapon.
***********************************************************************/
void Bullet::fire(Point point, float rotation, Velocity velocity)
{
	this->rotation = rotation;

	this->velocity.setDy(velocity.getDy() - BULLET_SPEED * (-cos(M_PI / 180.0 * rotation)));
	this->velocity.setDx(velocity.getDx()  - BULLET_SPEED * (sin(M_PI / 180.0 * rotation)));

	//Make so bullet does not impact ship immediately...
	this->point = Point(point.getX() + this->getVelocity().getDx() * 1.5, point.getY() + this->getVelocity().getDy() * 1.5);
}

/**********************************************************************
* draw()
***********************************************************************/
void Bullet::draw()
{
	if (decay > 40)
	{
		alive = false;
		return;
	}
	drawDot(point);
	decay++;
}
