#ifndef _H_WIN32_HELPER_
#define _H_WIN32_HELPER_

namespace win32 {
	enum windows_version
	{
		Unknown				= 0x0000,
		Windows2000			= 0x0500,
		WindowsXP			= 0x0501,
		WindowsXP64			= 0x0502,
		WindowsServer2003	= 0x0502,
		WindowsServer2003R2	= 0x0502,
		WindowsVista		= 0x0600,
		WindowsServer2008	= 0x0600,
		WindowsServer2008R2	= 0x0601,
		Windows7			= 0x0601,
		WindowsServer2012	= 0x0602,
		Windows8			= 0x0602,
		WindowsServer2012R2	= 0x0603,
		Windows8_1			= 0x0603,
		WindowsServer2016	= 0x1000,
		Windows10			= 0x1000
	};

	bool isDXGISupported(void);
	bool isAeroEnabled(void);
	windows_version getWindowsVersion(void);
}

#endif