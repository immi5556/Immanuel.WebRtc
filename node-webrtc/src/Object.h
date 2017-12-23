#ifndef _H_OBJECT_
#define _H_OBJECT_

class Object
{
protected:
	Object(void) {}
	~Object(void) {}

private:
	Object(Object&) = delete;
	Object& operator = (const Object&) = delete;

public:
	virtual void release(void) = 0;
};

#endif