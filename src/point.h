// Point is a set of (x, y) coordinates. 

#ifndef POINT_H
#define POINT_H

#include <ostream>

namespace Geometry_Utilities {

/* Point */
// A Point contains an (x, y) pair to represent coordinates
struct Point
{
	double x;
	double y;

	explicit Point (double in_x = 0., double in_y = 0.) :
		x(in_x), y(in_y)
		{}

	// getter/setter for x,y access in pybind11
	double get_x()
	    {return x;}

	double get_y()
	    {return y;}

	void set_x(double in_x)
	    {x = in_x;}

	void set_y(double in_y)
	    {y = in_y;}

	// compare two Points
	bool operator== (const Point& rhs) const
		{return (x == rhs.x && y == rhs.y);}
 
	bool operator!= (const Point& rhs) const
		{return !(*this == rhs);}
		
	bool operator< (const Point& rhs) const
		{return (x == rhs.x) ? (y < rhs.y) : (x < rhs.x);}
		
	bool operator<= (const Point& rhs) const
		{return (*this < rhs) || (*this == rhs);}
		
	bool operator> (const Point& rhs) const
		{return !(*this <= rhs);}

	bool operator>= (const Point& rhs) const
		{return !(*this < rhs);}

};

}	// end namepsace

#endif