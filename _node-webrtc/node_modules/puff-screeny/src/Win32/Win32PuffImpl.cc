#include "../includes.h"

typedef std::vector<puff::type_helper::window_t*> window_array_t;
typedef std::vector<puff::type_helper::screen_t*> screen_array_t;

Win32PuffImpl* Win32PuffImpl::instance = nullptr;

struct GlobalDC
{
public:
    GlobalDC(void) {
        this->dc = GetDC(nullptr);
    }
    ~GlobalDC(void) {
        ReleaseDC(nullptr, this->dc);
    }

private:
    GlobalDC(GlobalDC& /*unused*/) = default;
    GlobalDC& operator = (GlobalDC& /*unused*/) = default;
    GlobalDC& operator = (GlobalDC&& /*unused*/) noexcept = default;

public:
    operator HDC(void) const {
        return this->dc;
    }

private:
    HDC dc;
};

BOOL CALLBACK Win32PuffImpl::procEnumWindows(HWND hwnd, LPARAM param)
{
    window_array_t *list = reinterpret_cast<window_array_t *>(param);

    // Skip windows that are invisible, minimized, have no title, or are owned,
    // unless they have the app window style set.
    int  len     = GetWindowTextLengthA(hwnd);
    HWND owner   = GetWindow(hwnd, GW_OWNER);
    LONG exstyle = GetWindowLongA(hwnd, GWL_EXSTYLE);
    if (len == 0 || IsIconic(hwnd) || !IsWindowVisible(hwnd) || (owner && !(exstyle & WS_EX_APPWINDOW)))
    {
        return TRUE;
    }

    // Skip the Program Manager window and the Start button.
    const size_t kClassLength = 256;
    char         class_name[kClassLength];
    GetClassNameA(hwnd, class_name, kClassLength);

    // Skip Program Manager window and the Start button. This is the same logic
    // that's used in Win32WindowPicker in libjingle. Consider filtering other
    // windows as well (e.g. toolbars).
    if (strcmp(class_name, "Progman") == 0 || strcmp(class_name, "Button") == 0 ||
        strcmp(class_name, "Windows.UI.Core.CoreWindow") == 0 || strcmp(class_name, "ApplicationFrameWindow") == 0)
        return TRUE;

    list->push_back(Win32Window::create(hwnd));

    return TRUE;
}

Win32PuffImpl* Win32PuffImpl::getInstance()
{
	if (Win32PuffImpl::instance == nullptr)
		Win32PuffImpl::instance = new Win32PuffImpl;

	return Win32PuffImpl::instance;
}

Win32PuffImpl::Win32PuffImpl(void) {}
Win32PuffImpl::Win32PuffImpl(Win32PuffImpl&) {}
Win32PuffImpl::~Win32PuffImpl(void) {}

const std::vector<puff::type_helper::window_t*>& Win32PuffImpl::getAvailableWindows()
{
    if (!this->availableWindows.empty())
    {
        for (int i = 0; i < this->availableWindows.size(); ++i)
            this->availableWindows[i]->release();

        this->availableWindows.clear();
    }

    EnumWindows(procEnumWindows, reinterpret_cast<LPARAM>(&this->availableWindows));
    return this->availableWindows;
}
const std::vector<puff::type_helper::screen_t*>& Win32PuffImpl::getAvailableScreens()
{
    static GlobalDC globalDC;

    if (win32::isDXGISupported())
        return DXGICapturer::getInstance()->getAvailableScreens();
    else
    {
        if (!this->availableScreens.empty())
        {
            for (int i = 0; i < this->availableScreens.size(); ++i)
                this->availableScreens[i]->release();

            this->availableScreens.clear();
        }

        DISPLAY_DEVICEA device = {0,};
        device.cb = sizeof(DISPLAY_DEVICEA);
        for (DWORD i = 0; EnumDisplayDevicesA(nullptr, i, &device, NULL); ++i)
        {
            if ((device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) == DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
            {
                DEVMODEA deviceMode = {0,};
                deviceMode.dmSize   = sizeof(DEVMODEA);

                EnumDisplaySettingsA(device.DeviceName, ENUM_CURRENT_SETTINGS, &deviceMode);

                std::string deviceName = device.DeviceName;
                Area        monitorArea(deviceMode.dmPosition.x, deviceMode.dmPosition.y,
                                        static_cast<int>(deviceMode.dmPosition.x + deviceMode.dmPelsWidth),
                                        static_cast<int>(deviceMode.dmPosition.y + deviceMode.dmPelsHeight));

                this->availableScreens.push_back(new Win32Screen(globalDC, monitorArea));
            }
        }

        return this->availableScreens;
    }
}
