#ifndef VELOCITY_H
#define VELOCITY_H
#include <iostream>


class Velocity
{
private:
	float deltaX;
	float deltaY;

public:
	//Constructors
	  //Default
	Velocity() : deltaX(0), deltaY(0) {}
	//Enter all
	Velocity(float newDeltaX, float  newDeltaY)
	{setDy(newDeltaY); setDx(newDeltaX);}
	
	//Getters
	float getDx() const { return deltaX; }
	float getDy() const { return deltaY; }

	//Setters
	void setDx(float);
	void setDy(float);

	//Operators

};

inline bool operator==(const Velocity& lhs, const Velocity& rhs)
{
	if (lhs.getDx() == rhs.getDx()
		&& lhs.getDy() == rhs.getDy())
	{
		return true;
	}

	return false;
}

inline std::ostream& operator << (std::ostream & out, Velocity &rhs)
{
	std::cout << "{" << rhs.getDx() << ", " << rhs.getDy() << "}";

	return out;
}

#endif