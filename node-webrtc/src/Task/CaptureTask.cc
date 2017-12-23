#include "../includes.h"

using namespace std::chrono;

CaptureTask::CaptureTask(nbind::cbFunction& callback, nbind::Buffer& buffer, float scaleFactor, int fps, SharableTarget* target) :
	Nan::AsyncProgressWorkerBase<char>(new Nan::Callback(callback.getJsFunction())), buffer(buffer), fps(fps), finishedCallback(nullptr), target(target),
	originalResolution(target->getArea()), resizeBuffer(nullptr), resizeBufferLength(0), captureStatus(true), originalBuffer(nullptr), originalBufferLength(0),
	fpsTime(1000000000 / fps), needResizing(false), finished(false), scaleFactor(scaleFactor)
{
	this->targetResolution = originalResolution;
	this->targetResolution.right *= scaleFactor;
	this->targetResolution.bottom *= scaleFactor;

	this->originalBufferLength = this->originalResolution.getWidth() * this->originalResolution.getHeight() * 4;
	this->originalBuffer = new std::uint8_t[this->originalBufferLength];

	if (this->targetResolution == this->originalResolution) {
		this->resizeBufferLength = this->originalBufferLength;
		this->resizeBuffer = this->originalBuffer;
	} else {
		this->resizeBufferLength = this->targetResolution.getWidth() * this->targetResolution.getHeight() * 4;
		this->resizeBuffer = new std::uint8_t[this->resizeBufferLength];
	}

	uv_mutex_init(&this->mutex);
	uv_mutex_init(&this->finishMutex);
}

CaptureTask::~CaptureTask(void)
{
}

bool CaptureTask::isCapturing() const
{
	return this->captureStatus;
}
bool CaptureTask::isFinished()
{
	SCOPED_LOCK(finishMutex);
	return this->finished;
}

void CaptureTask::stop()
{
	SCOPED_LOCK(finishMutex);
	this->captureStatus = false;
}
void CaptureTask::setFinishedCallback(nbind::cbFunction& callback)
{
	if (this->finishedCallback != nullptr)
		delete finishedCallback;

	this->finishedCallback = new Nan::Callback(callback.getJsFunction());
}

void CaptureTask::preprocessScreenshot(std::uint8_t* buffer, std::size_t length)
{
	if (this->originalResolution != this->targetResolution) {
		image_processor_t::getInstance()->scale(ARGB32, buffer, length, this->originalResolution, this->resizeBuffer, this->resizeBufferLength, this->targetResolution);
	}

	const int order[4] = { 2, 1, 0, 3 };
	image_processor_t::getInstance()->swap(this->resizeBuffer, this->targetResolution, this->buffer.data(), this->targetResolution, PixelType::ARGB32, order);
	image_processor_t::getInstance()->value(RGBA32, this->buffer.data(), this->buffer.length(), this->targetResolution, 3, 255);
}

const SharableTarget* CaptureTask::getTarget() const
{
	return this->target;
}

const Area& CaptureTask::getResizedArea() const
{
	return this->targetResolution;
}
const Area& CaptureTask::getOriginalArea() const
{
	return this->originalResolution;
}

void CaptureTask::Execute(const Nan::AsyncProgressWorker::ExecutionProgress& progress)
{
	int frame = 0;
	this->currentTick = high_resolution_clock::now();
	while (this->captureStatus == true) {
		SCOPED_TRYLOCK(finishMutex);
		{
			nanoseconds duration = this->currentTick - this->lastTick;
			if (duration.count() > fpsTime && this->needResizing == false) {
				uv_mutex_lock(&this->mutex);

				// prevent update capture target area during async working in progress.
				// because that behavior can make program really busy for it
				const Area& newArea = this->target->getArea();
				if (newArea != this->originalResolution) { // if there was moving or size changing
					if (newArea.getWidth() != this->originalResolution.getWidth() || newArea.getHeight() != this->originalResolution.getHeight())
						this->needResizing = true;

					this->originalResolution = newArea;
					this->targetResolution = newArea;
					this->targetResolution.right *= scaleFactor;
					this->targetResolution.bottom *= scaleFactor;

					if (this->needResizing == true) { // if there was resizing
						delete[] this->originalBuffer;

						this->originalBufferLength = this->originalResolution.getWidth() * this->originalResolution.getHeight() * 4;
						this->originalBuffer = new std::uint8_t[this->originalBufferLength];

						if (this->scaleFactor != 1.0f) { // if resized size was not same with original size
							delete[] this->resizeBuffer;

							this->resizeBufferLength = this->targetResolution.getWidth() * this->targetResolution.getHeight() * 4;
							this->resizeBuffer = new std::uint8_t[this->resizeBufferLength];

						}
						else {
							this->resizeBuffer = this->originalBuffer;
							this->resizeBufferLength = this->originalBufferLength;
						}

						progress.Send(nullptr, 0);
						uv_mutex_unlock(&this->mutex);
						continue;
					}
				}

				if (this->needResizing == false) {
					this->takeScreenshot(this->originalBuffer, this->originalBufferLength);
					this->preprocessScreenshot(this->originalBuffer, this->originalBufferLength);

					progress.Send(nullptr, 0);
					++frame;

					this->lastTick = high_resolution_clock::now();
				}

				uv_mutex_unlock(&this->mutex);
			}
			else {
				Sleep(1);
			}

			this->currentTick = high_resolution_clock::now();
		}
	}

	if (this->originalResolution != this->targetResolution)
		delete[] this->resizeBuffer;

	delete[] this->originalBuffer;

	uv_mutex_destroy(&this->finishMutex);
	uv_mutex_destroy(&this->mutex);
}

const char* ToCString(const v8::String::Utf8Value& value) {
	return *value ? *value : "<string conversion failed>";
}

void CaptureTask::HandleProgressCallback(const char* data, size_t size)
{
	Nan::HandleScope scope;
	Nan::Callback fn(**this->callback);

	if (this->needResizing == true) {
		uv_mutex_lock(&this->mutex);

		const int argc = 2;
		v8::Local<v8::Value> argv[argc] = {
			Nan::New(this->targetResolution.getWidth()),
			Nan::New(this->targetResolution.getHeight())
		};

		v8::Local<v8::Value> returnRaw = fn.Call(argc, argv);
		v8::Local<v8::Object> returnValue = returnRaw->ToObject();

		v8::Local<v8::Uint8ClampedArray> array = v8::Local<v8::Uint8ClampedArray>::Cast(returnValue);
		unsigned char* data = static_cast<unsigned char*>(array->Buffer()->GetContents().Data());
		std::size_t length = array->Buffer()->GetContents().ByteLength();

		// delete this->buffer.data();
		this->buffer = nbind::Buffer(data, length);
		uv_mutex_unlock(&this->mutex);

		this->needResizing = false;
	} else {
		fn.Call(0, {});
	}
}

void CaptureTask::HandleOKCallback()
{
	Nan::HandleScope scope;
	Nan::Callback fn(**this->finishedCallback);

	fn.Call(0, {});

	this->finished = true;
}