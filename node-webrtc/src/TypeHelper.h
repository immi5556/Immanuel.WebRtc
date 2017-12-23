#ifndef _H_TYPEHEADER_
#define _H_TYPEHEADER_

#define _PUFF_WINDOWS_	1
#define _PUFF_OSX_		2
#define _PUFF_LINUX_	3

#if !defined(_TARGET_OS_)
#	if defined(_OS_WINDOWS_) || (!defined(_OS_WINDOWS_) && defined(_MSC_VER))
#		define _TARGET_OS_ _PUFF_WINDOWS_
#	endif
#endif

class IPPImageProcessor;
class OpenCVImageProcessor;
class Win32Screen;
class Win32PuffImpl;
class Win32Window;

#if defined(BUILDING_NODE_EXTENSION)
#	include "nbind/api.h"
	using Nan::New;
#else
namespace nbind {
	class cbFunction
	{
	public:
		cbFunction(...) {};

		template <class T>
		T call(...) { return T(); };

		v8::Local<v8::Function> getJsFunction(void) const { return {}; };
	};

	class Buffer
	{
	public:
		Buffer(unsigned char *ptr = nullptr, size_t len = 0) {}
		unsigned char* data() const { return nullptr; }
		size_t length() const { return 0; }
	};
}

template <class T>
v8::MaybeLocal<T> New(...) { return {}; };
#endif

#if defined(_IPP_)
typedef IPPImageProcessor image_processor_t;
#elif defined(_OPENCV_PREBUILT_)
typedef OpenCVImageProcessor image_processor_t;
#endif

namespace puff {
	template <int os>
	struct _type_helper;

	template <>
	struct _type_helper<_PUFF_WINDOWS_>
	{
		typedef Win32PuffImpl puff_impl_t;
		typedef Win32Window window_t;
		typedef Win32Screen screen_t;
	};

	typedef _type_helper<_TARGET_OS_> type_helper;
	
#if defined(BUILDING_NODE_EXTENSION)
	typedef std::string string_t;
#	define MAKE_STRING(x) Puff::getInternalInstance()->encode(std::vector<short>(x, x + wcslen(x)))
#else
	typedef wchar_t* string_t;
#	define MAKE_STRING(x) x
#endif
}

#endif