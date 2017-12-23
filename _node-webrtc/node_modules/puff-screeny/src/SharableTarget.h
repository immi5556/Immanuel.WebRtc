#ifndef _H_SHARABLETARGET_
#define _H_SHARABLETARGET_

enum SharableTargetType
{
	window = 1,
	screen = 2
};

class SharableTarget : public Object
{
protected:
	SharableTarget(void);
	virtual ~SharableTarget(void) = 0;
	SharableTarget(SharableTarget&);

public:
	virtual std::uint64_t getUniqueId(void) const = 0;
	virtual const Area& getArea(int dummy = 0) const = 0;
	virtual Area& getArea(void) = 0;
	virtual SharableTargetType getType(void) const = 0;
	virtual bool isWindow(void) const = 0;
	virtual bool isScreen(void) const = 0;

public:
	virtual bool startCapture(nbind::Buffer buffer, float scaleFactor, int fps, nbind::cbFunction& callback) = 0;
	virtual void stopCapture(nbind::cbFunction& callback) = 0;
	virtual void takeScreenshot(nbind::Buffer buffer, float scaleFactor, nbind::cbFunction& callback) = 0;
};

#endif
