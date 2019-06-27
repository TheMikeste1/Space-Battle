#include "ship.h"
#include "uiDraw.h"


/*********************************************
* Constructors
*********************************************/

/*********************************************
* Others
*********************************************/
	/*********************************************
	* Misc
	*********************************************/
void Ship::draw()
{
	drawShip(point, rotation, thrust);
}

void Ship::increaseVelocity()
{
	velocity.setDy((velocity.getDy() - THRUST_AMOUNT *(-cos(M_PI / 180.0 * rotation))));
	velocity.setDx((velocity.getDx() - THRUST_AMOUNT *(sin(M_PI / 180.0 * rotation))));
}

	/*********************************************
	* Movement
	*********************************************/
void Ship::rotateLeft()
{
	rotation += ROTATE_AMOUNT;

	if (rotation > 359)
		rotation -= 360;
}

void Ship::rotateRight()
{
	rotation -= ROTATE_AMOUNT;

	if (rotation < 0)
		rotation += 360;
}