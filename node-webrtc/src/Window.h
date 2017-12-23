#ifndef _H_WINDOW_
#define _H_WINDOW_

class Window : public SharableTarget
{
protected:
	Window(void);
	Window(Window&);
	~Window(void);

public:
	virtual SharableTargetType getType(void) const override { return SharableTargetType::window; }
	virtual bool isWindow(void) const override { return true; }
	virtual bool isScreen(void) const override { return false; }

public:
	virtual puff::string_t getTitle(void) = 0;
};

#endif