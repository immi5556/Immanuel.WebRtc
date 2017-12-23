#ifndef _H_CAPTURETASK_
#define _H_CAPTURETASK_

class CaptureTask : public Nan::AsyncProgressWorkerBase<char>
{
protected:
	CaptureTask(nbind::cbFunction& callback, nbind::Buffer& buffer, float scaleFactor, int fps, SharableTarget* target);
	virtual ~CaptureTask(void) = 0;

private:
	CaptureTask(CaptureTask&) = delete;

public:
	bool isCapturing(void) const;
	bool isFinished(void);
	void stop(void);
	void setFinishedCallback(nbind::cbFunction& callback);

private:
	virtual void takeScreenshot(std::uint8_t*& buffer, std::size_t& length) = 0;
	virtual void preprocessScreenshot(std::uint8_t* buffer, std::size_t length);

protected:
	const SharableTarget* getTarget(void) const;
	const Area& getResizedArea(void) const;
	const Area& getOriginalArea(void) const;

private:
	void Execute(const Nan::AsyncProgressWorker::ExecutionProgress& progress) override;
	virtual void HandleProgressCallback(const char* data, size_t size) override;
	virtual void HandleOKCallback(void) override;

private:
	bool captureStatus, needResizing, finished;
	std::int64_t fpsTime;

	std::uint8_t* resizeBuffer, *originalBuffer;
	std::size_t resizeBufferLength, originalBufferLength;

	float scaleFactor;
	nbind::Buffer buffer;
	Area targetResolution;
	Area originalResolution;
	std::chrono::high_resolution_clock::time_point currentTick, lastTick;
	SharableTarget* target;
	const int fps;

	Nan::Callback* finishedCallback;
	uv_mutex_t mutex, finishMutex;
};

#endif