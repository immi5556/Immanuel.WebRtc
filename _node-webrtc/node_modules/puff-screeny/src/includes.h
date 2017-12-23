#ifndef _H_INCLUDES_
#define _H_INCLUDES_

#if defined(_MSC_VER)
#	pragma optimize("gty", on)
#endif

#include "Debug/Console.h"

#if defined(_OPENCV_PREBUILT_)
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#endif

#ifdef UNICODE
#undef UNICODE
#endif

#include <iostream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <memory>
#include <thread>

#include "nbind/nbind.h"
#include "nan.h"
#include "v8.h"

#include "TypeHelper.h"

#include "Object.h"
#include "ScopedLock.h"
#include "Area.h"
#include "SharableTarget.h"
#include "Task/CaptureTask.h"
#include "Task/ScreenshotTask.h"
#include "Screen.h"
#include "Window.h"
#include "PuffImpl.h"
#include "Puff.h"

#include "ImageProcessor/ImageProcessor.h"

#if defined(_IPP_)
#	include "ImageProcessor/IPPImageProcessor.h"
#elif defined(_OPENCV_PREBUILT_)
#	include "ImageProcessor/OpenCVImageProcessor.h"
#endif

#include "Capturer/Capturer.h"

#if _TARGET_OS_ == _PUFF_WINDOWS_
#	include "Win32/win32_includes.h"
#endif

#endif