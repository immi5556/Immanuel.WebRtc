#ifndef _H_CAPTURER_
#define _H_CAPTURER_

class Capturer
{
protected:
	Capturer(void);
	Capturer(Capturer&) = delete;
    virtual ~Capturer(void);

public:
	virtual bool isHWAccelerated(void) const = 0;

public:
	virtual bool capture(const SharableTarget* target, std::uint8_t** dstBuffer, std::size_t* byteWritten, PixelType* dstPixelType, const Area& originalArea) = 0;
};

#endif