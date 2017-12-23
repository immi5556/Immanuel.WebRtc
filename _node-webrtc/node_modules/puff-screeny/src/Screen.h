#ifndef _H_SCREEN_
#define _H_SCREEN_

class Screen : public SharableTarget
{
protected:
	Screen(void);
	Screen(Screen&) = delete;
	~Screen(void);

public:
	virtual SharableTargetType getType(void) const override { return SharableTargetType::screen; }
	virtual bool isWindow(void) const override { return false; }
	virtual bool isScreen(void) const override { return true; }
};

#endif