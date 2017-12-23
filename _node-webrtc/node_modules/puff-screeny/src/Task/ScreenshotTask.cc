#include "../includes.h"

ScreenshotTask::ScreenshotTask(nbind::cbFunction& callback_, nbind::Buffer& buffer, float scaleFactor, SharableTarget* target) : 
	AsyncWorker(new Nan::Callback(callback_.getJsFunction())), target(target), buffer(buffer), originalResolution(target->getArea()), 
	originalBuffer(nullptr), originalBufferLength(0), resizeBuffer(nullptr), resizeBufferLength(0), scaleFactor(scaleFactor)
{
	this->targetResolution = this->originalResolution * scaleFactor;
	this->originalBufferLength = static_cast<size_t>(this->originalResolution.getWidth() * this->originalResolution.getHeight() * 4);
	this->originalBuffer = new std::uint8_t[this->originalBufferLength];

	if (scaleFactor != 1.0f) {
		this->resizeBufferLength = static_cast<size_t>(this->targetResolution.getWidth() * this->targetResolution.getHeight() * 4);
		this->resizeBuffer = new std::uint8_t[this->resizeBufferLength];
	} else {
		this->resizeBufferLength = this->originalBufferLength;
		this->resizeBuffer = this->originalBuffer;
	}
}
ScreenshotTask::~ScreenshotTask(void)
{
	if (this->scaleFactor != 1.0f)
		delete[] this->resizeBuffer;

	delete[] this->originalBuffer;
}

void ScreenshotTask::preprocessScreenshot(std::uint8_t* buffer, std::size_t length)
{
	if (scaleFactor != 1.0f) {
		image_processor_t::getInstance()->scale(ARGB32, buffer, length, this->originalResolution, this->resizeBuffer, this->resizeBufferLength, this->targetResolution);
	}

	const int order[4] = { 2, 1, 0, 3 };
	image_processor_t::getInstance()->swap(this->resizeBuffer, this->targetResolution, this->buffer.data(), this->targetResolution, PixelType::ARGB32, order);
	image_processor_t::getInstance()->value(RGBA32, this->buffer.data(), this->buffer.length(), this->targetResolution, 3, 255);
}

const SharableTarget* ScreenshotTask::getTarget() const
{
	return this->target;
}
const Area& ScreenshotTask::getResizedArea() const
{
	return this->targetResolution;
}
const Area& ScreenshotTask::getOriginalArea() const
{
	return this->originalResolution;
}

void ScreenshotTask::Execute()
{
	this->takeScreenshot(this->originalBuffer, this->originalBufferLength);
	this->preprocessScreenshot(this->originalBuffer, this->originalBufferLength);
}
void ScreenshotTask::HandleOKCallback()
{
	nbind::cbFunction fn(**this->callback);
	fn.call<void>();
}
