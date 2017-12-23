#ifndef _H_WIN32PUFFIMPL_
#define _H_WIN32PUFFIMPL_

class Win32PuffImpl : public PuffImpl
{
private:
	static Win32PuffImpl* instance;

private:
	static BOOL CALLBACK procEnumWindows(HWND, LPARAM);

public:
	static Win32PuffImpl* getInstance(void);

public:
	Win32PuffImpl(void);
	~Win32PuffImpl(void);

private:
	Win32PuffImpl(Win32PuffImpl&);

public:
	virtual const std::vector<puff::type_helper::window_t*>& getAvailableWindows(void) override;
	virtual const std::vector<puff::type_helper::screen_t*>& getAvailableScreens(void) override;

private:
	std::vector<puff::type_helper::window_t*> availableWindows;
	std::vector<puff::type_helper::screen_t*> availableScreens;
};

#endif