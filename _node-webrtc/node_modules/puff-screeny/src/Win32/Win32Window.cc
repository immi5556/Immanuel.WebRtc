#include "../includes.h"

bool									Win32Window::hookEnabled = false;
HWINEVENTHOOK							Win32Window::hookHandle = nullptr;
std::thread*							Win32Window::hookThread = nullptr;
std::unordered_map<HWND, Win32Window*>	Win32Window::instances;

void Win32Window::hookProcedure(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD idEventThread, DWORD dwmsEventTime)
{
	if (Win32Window::instances.find(hwnd) == Win32Window::instances.end())
		return;

	if (event == EVENT_SYSTEM_MOVESIZESTART || event == EVENT_SYSTEM_MOVESIZEEND) {
		Win32Window* window = Win32Window::instances[hwnd];

		RECT rect;
		DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));

		window->area.left = rect.left;
		window->area.top = rect.top;
		window->area.right = rect.right;
		window->area.bottom = rect.bottom;
	}
}

void Win32Window::hookThreadFunction()
{
	Win32Window::hookHandle = SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND, 0, Win32Window::hookProcedure, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (msg.message == WM_QUIT)
			break;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

Win32Window* Win32Window::create(HWND targetHandle)
{
	if (!Win32Window::hookEnabled) {
		Win32Window::hookThread = new std::thread(hookThreadFunction);

		Win32Window::hookEnabled = true;
	}
	
	Win32Window::instances[targetHandle] = new Win32Window(targetHandle);
	return Win32Window::instances[targetHandle];
}

Win32Window::Win32Window(void)
	: targetHandle(nullptr), captureTask(nullptr) {}
Win32Window::Win32Window(HWND targetHandle)
	: targetHandle(targetHandle), captureTask(nullptr)
{
	RECT rect; //, frame;
	// GetWindowRect(targetHandle, &rect);
	DwmGetWindowAttribute(targetHandle, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));

	/*
	RECT border;
	border.left = frame.left - rect.left;
	border.top = frame.top - rect.top;
	border.right = rect.right - frame.right;
	border.bottom = rect.bottom - frame.bottom;

	rect.left -= border.left;
	rect.top -= border.top;
	rect.right += border.left + border.right;
	rect.bottom += border.top + border.bottom;
	*/

	this->area.left = rect.left;
	this->area.top = rect.top;
	this->area.right = rect.right;
	this->area.bottom = rect.bottom;

	this->background = reinterpret_cast<HBRUSH>(GetClassLongPtrA(targetHandle, GCLP_HBRBACKGROUND));
}

Win32Window::Win32Window(Win32Window&)
	: targetHandle(nullptr), captureTask(nullptr) {}
Win32Window::~Win32Window(void) {}

std::uint64_t Win32Window::getUniqueId() const
{
	return reinterpret_cast<std::uint64_t>(this);
}

const Area& Win32Window::getArea(int dummy) const
{
	return this->area;
}
Area& Win32Window::getArea()
{
	return this->area;
}

puff::string_t Win32Window::getTitle()
{
	wchar_t buffer[MAX_PATH] = { 0, };
	GetWindowTextW(this->targetHandle, buffer, MAX_PATH);

	return MAKE_STRING(buffer);
}

bool Win32Window::startCapture(nbind::Buffer buffer, float scaleFactor, int fps, nbind::cbFunction& callback)
{
	if (scaleFactor == 0.0f)
		return false;

	if (this->captureTask != nullptr) {
		if (this->captureTask->isFinished() == false) {
			return false;
		}
	}

	this->captureTask = new Win32CaptureTask(callback, buffer, scaleFactor, fps, this);
	Nan::AsyncQueueWorker(captureTask);
	return true;
}
void Win32Window::stopCapture(nbind::cbFunction& callback)
{
	if (this->captureTask != nullptr) {
		this->captureTask->setFinishedCallback(callback);
		this->captureTask->stop();
		this->captureTask = nullptr;
	}
}

void Win32Window::takeScreenshot(nbind::Buffer buffer, float scaleFactor, nbind::cbFunction& callback)
{
	if (scaleFactor == 0.0f)
		return;

	Nan::AsyncQueueWorker(new Win32ScreenshotTask(callback, buffer, scaleFactor, this));
}

void Win32Window::release()
{
	delete this;
}

#include "nbind/nbind.h"

#if defined(BUILDING_NODE_EXTENSION)
NBIND_CLASS(Win32Window)
{
	multimethod(getArea, args(), "getArea");
	method(getUniqueId);
	method(getTitle);
	method(startCapture);
	method(stopCapture);
	method(takeScreenshot);
}
#endif