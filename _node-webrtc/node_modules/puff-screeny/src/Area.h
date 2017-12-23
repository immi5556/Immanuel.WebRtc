#ifndef _H_AREA_
#define _H_AREA_

class Area
{
public:
	Area(int left, int top, int right, int bottom);
	Area(void);
	Area(const Area& area);
	~Area(void);

public:
	int getX(void) const;
	int getY(void) const;
	int getWidth(void) const;
	int getHeight(void) const;
	
public:
	int left, top, right, bottom;
};

inline std::ostream& operator << (std::ostream& o, Area& area) {
	o	<< "{ left: " << area.left << ", top: " << area.top << ", right: " 
		<< area.right << ", bottom: " << area.bottom << ", width: " << area.getWidth() << ", height: " << area.getHeight()
		<< " }";
	return o;
}

template <typename T>
Area operator + (const Area& area, T value)
{
	return Area(area.left + value, area.top + value, area.right + value, area.bottom + value);
}
template <typename T>
Area operator - (const Area& area, T value)
{
	return Area(area.left - value, area.top - value, area.right - value, area.bottom - value);
}
template <typename T>
Area operator * (const Area& area, T value)
{
	return Area(area.left * value, area.top * value, area.right * value, area.bottom * value);
}
template <typename T>
Area operator / (const Area& area, T value) 
{
	return Area(area.left / value, area.top / value, area.right / value, area.bottom / value);
}

inline Area operator + (const Area& left, const Area& right)
{
	return Area(left.left + right.left, left.top + right.top, right.right + left.right, left.bottom + right.bottom);
}
inline Area operator - (const Area& left, const Area& right)
{
	return Area(left.left - right.left, left.top - right.top, right.right - left.right, left.bottom - right.bottom);
}
inline Area operator * (const Area& left, const Area& right)
{
	return Area(left.left * right.left, left.top * right.top, right.right * left.right, left.bottom * right.bottom);
}
inline Area operator / (const Area& left, const Area& right)
{
	return Area(left.left / right.left, left.top / right.top, right.right / left.right, left.bottom / right.bottom);
}

inline bool operator == (const Area& left, const Area& right)
{
	return (left.left == right.left && left.top == right.top && left.right == right.right && left.bottom == right.bottom);
}
inline bool operator != (const Area& left, const Area& right)
{
	return (left.left != right.left || left.top != right.top || left.right != right.right || left.bottom != right.bottom);
}

#endif
