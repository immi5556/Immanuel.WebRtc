#ifndef _H_PUFFIMPL_
#define _H_PUFFIMPL_

class PuffImpl
{
public:
	PuffImpl(void);
	virtual ~PuffImpl(void) = 0;

private:
	PuffImpl(PuffImpl&);

public:
	virtual const std::vector<puff::type_helper::window_t*>& getAvailableWindows(void) = 0;
	virtual const std::vector<puff::type_helper::screen_t*>& getAvailableScreens(void) = 0;
};

#endif
