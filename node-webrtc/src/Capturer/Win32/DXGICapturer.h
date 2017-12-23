#ifndef _H_DXGICAPTURER_
#define _H_DXGICAPTURER_

#define __DXGI__

class DXGICapturer : public Capturer
{
private:
	static DXGICapturer* instance;

public:
	static DXGICapturer* getInstance(void);

private:
	DXGICapturer(void);	
	DXGICapturer(DXGICapturer&) = delete;
	~DXGICapturer(void);

public:
	const std::vector<puff::type_helper::screen_t*>& getAvailableScreens(void) const;

public:
	virtual bool isHWAccelerated(void) const override;

public:
	bool capture(const SharableTarget* target, std::uint8_t** dstBuffer, std::size_t* byteWritten, PixelType* dstPixelType, const Area& originalArea) override;

private:
	void release(void);

private:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	IDXGIDevice* dxgiDevice;
	IDXGIAdapter* dxgiAdapter;
	IDXGIFactory2* dxgiFactory;
	std::unordered_map<HMONITOR, IDXGIOutput*> output;
	std::unordered_map<HMONITOR, IDXGIOutput1*> output1;
	HMONITOR currentMonitor;
	IDXGIOutputDuplication* duplication;
	std::unordered_map<HMONITOR, ID3D11Texture2D*> cpuAccessTexture;
	std::unordered_map<HMONITOR, puff::type_helper::screen_t*> screenInstances;
	std::vector<puff::type_helper::screen_t*> availableScreens;
	std::unordered_map<HMONITOR, bool> cachedDuplication;
	uv_mutex_t mutex;
	
	bool failed;
};

#endif
