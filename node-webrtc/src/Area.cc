#include "includes.h"

Area::Area(int l , int t, int r, int b)
	: left(l), top(t), right(r), bottom(b) {}
Area::Area(void)
	: left(0), top(0), right(0), bottom(0) { }
Area::Area(const Area& area)
	: left(area.left), top(area.top), right(area.right), bottom(area.bottom) { }
Area::~Area(void) { }

template <class T>
T even(T v)
{
	return v % 2 != 0 ? v + 1 : v;
}

int Area::getX() const { return this->left; }
int Area::getY() const { return this->top; }
int Area::getWidth() const { return this->right - this->left; }
int Area::getHeight() const { return this->bottom - this->top; }

#include "nbind/nbind.h"

#if defined(BUILDING_NODE_EXTENSION)
NBIND_CLASS(Area)
{
	method(getX);
	method(getY);
	method(getWidth);
	method(getHeight);
}
#endif