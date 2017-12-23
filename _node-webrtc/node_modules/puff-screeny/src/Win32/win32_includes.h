#ifndef _H_WIN32_INCLUDES_
#define _H_WIN32_INCLUDES_

#define WIN32_LEAN_AND_MEAN

#if !defined(min) && !defined(max)
#	define max(a,b) (((a) > (b)) ? (a) : (b))
#	define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#include <Windows.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <WinUser.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <LM.h>

#include "win32_helper.h"
#include "../Capturer/Win32/GdiPlusCapturer.h"
#include "../Capturer/Win32/DXGICapturer.h"

#include "../Task/Win32/Win32CaptureTask.h"
#include "../Task/Win32/Win32ScreenshotTask.h"

#include "Win32PuffImpl.h"
#include "Win32Screen.h"
#include "Win32Window.h"

#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "d3d11.lib")

#endif