#include "../includes.h"

namespace win32 {
	bool isDXGISupported(void)
	{
		return getWindowsVersion() >= Windows8;
	}

	bool isAeroEnabled()
	{
		BOOL status;
		DwmIsCompositionEnabled(&status);

		return status == TRUE;
	}

	windows_version getWindowsVersion()
	{
		static DWORD major = 0, minor = 0;
		if (major == 0) {
			LPWKSTA_INFO_100 info = nullptr;
			if (NetWkstaGetInfo(nullptr, 100, reinterpret_cast<BYTE**>(&info)) != NERR_Success)
				return Unknown;

			major = info->wki100_ver_major;
			minor = info->wki100_ver_minor;
		}

		if (major == 5) {
			switch (minor) 
			{
			case 0:
				return Windows2000;

			case 1:
				return WindowsXP;

			case 2:
				return WindowsXP64;

			default:
				return Unknown;
			}
		} else if (major == 6) {
			switch (minor) 
			{
			case 0:
				return WindowsVista;
			
			case 1:
				return Windows7;
			
			case 2:
				return Windows8;

			case 3:
				return Windows8_1;

			default:
				return Unknown;
			}
		} else if (major == 10) {
			switch (minor) 
			{
			case 0:
				return Windows10;

			default:
				return Unknown;
			}
		} else {
			return Unknown;
		}
	}
}
