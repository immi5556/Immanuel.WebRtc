#include "../../includes.h"
#include <memory>
#include <ShlObj.h>
#include <shellapi.h>

#define SAFE_RELEASE(x) { x->Release(); x = nullptr; }

DXGICapturer* DXGICapturer::instance = nullptr;
DXGICapturer* DXGICapturer::getInstance()
{
	if (DXGICapturer::instance == nullptr) {
		DXGICapturer::instance = new DXGICapturer;
		
		if (DXGICapturer::instance->failed == true)
			goto failed;
	}

	return DXGICapturer::instance;

failed:
	if (DXGICapturer::instance != nullptr) {
		DXGICapturer::instance->release();
		DXGICapturer::instance = nullptr;
	}

	return nullptr;
}

DXGICapturer::DXGICapturer(void)
	: device(nullptr), deviceContext(nullptr), dxgiDevice(nullptr), dxgiAdapter(nullptr), dxgiFactory(nullptr), currentMonitor(nullptr), duplication(nullptr), cachedDuplication(false), failed(false)
{
	HRESULT hr = S_OK;
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
	};
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_1
	};

	UINT driverTypeCount = ARRAYSIZE(driverTypes);
	D3D_FEATURE_LEVEL featureLevel;

	for (UINT i = 0; i < driverTypeCount; ++i) {
		hr = D3D11CreateDevice(
			nullptr,
			driverTypes[i],
			nullptr,
			0,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			&this->device,
			&featureLevel,
			&this->deviceContext
		);

		if (SUCCEEDED(hr))
			break;

		SAFE_RELEASE(this->device);
		SAFE_RELEASE(this->deviceContext);
	}

	if (FAILED(hr)) {
		this->failed = true;
		return;
	}

	hr = this->device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&this->dxgiDevice));
	if (FAILED(hr)) {
		this->failed = true;
		return;
	}

	hr = this->dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&this->dxgiAdapter));
	if (FAILED(hr)) {
		this->failed = true;
		return;
	}

	for (int i = 0; ; ++i) {
		IDXGIOutput* output = nullptr;
		if ((hr = this->dxgiAdapter->EnumOutputs(i, &output)) == DXGI_ERROR_NOT_FOUND)
			break;
		else if (FAILED(hr)) {
			this->failed = true;
			return;
		}

		DXGI_OUTPUT_DESC desc;
		hr = output->GetDesc(&desc);
		if (FAILED(hr)) {
			this->failed = true;
			return;
		}

		IDXGIOutput1* output1 = nullptr;
		hr = output->QueryInterface(__uuidof(IDXGIOutput1), reinterpret_cast<void**>(&output1));
		if (FAILED(hr)) {
			this->failed = true;
			return;
		}

		this->output[desc.Monitor] = output;
		this->output1[desc.Monitor] = output1;
		this->cachedDuplication[desc.Monitor] = false;

		IDXGIOutputDuplication* duplication = nullptr;
		hr = output1->DuplicateOutput(this->device, &duplication);
		if (FAILED(hr)) {
			this->failed = true;
			return;
		}

		//------------------------------------------------------------
		// Staging Texture 생성
		//------------------------------------------------------------
		DXGI_OUTDUPL_DESC duplicationDescription = { 0, };
		duplication->GetDesc(&duplicationDescription);

		D3D11_TEXTURE2D_DESC textureDescription = { 0, };
		textureDescription.Width = duplicationDescription.ModeDesc.Width;
		textureDescription.Height = duplicationDescription.ModeDesc.Height;
		textureDescription.Format = duplicationDescription.ModeDesc.Format;
		textureDescription.ArraySize = 1;
		textureDescription.BindFlags = 0;
		textureDescription.MiscFlags = 0;
		textureDescription.SampleDesc.Count = 1;
		textureDescription.SampleDesc.Quality = 0;
		textureDescription.MipLevels = 1;
		textureDescription.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		textureDescription.Usage = D3D11_USAGE_STAGING;

		ID3D11Texture2D* texture = nullptr;
		if (FAILED(this->device->CreateTexture2D(&textureDescription, NULL, &texture)) || texture == nullptr) {
			duplication->Release();

			this->failed = true;
			return;
		}

		duplication->Release();

		Win32Screen* screen = new Win32Screen(desc.Monitor);

		this->cpuAccessTexture[desc.Monitor] = texture;
		this->screenInstances[desc.Monitor] = screen;
		this->availableScreens.push_back(screen);
	}

	uv_mutex_init(&this->mutex);
}

DXGICapturer::~DXGICapturer(void)
{
	uv_mutex_destroy(&this->mutex);

	for (auto iter = this->cpuAccessTexture.begin(), iter_end = this->cpuAccessTexture.end(); iter != iter_end; ++iter) {
		iter->second->Release();
	}
	this->cpuAccessTexture.clear();

	if (this->duplication != nullptr)
		SAFE_RELEASE(this->duplication);

	for (auto iter = this->output1.begin(), iter_end = this->output1.end(); iter != iter_end; ++iter) {
		iter->second->Release();
	}
	this->output1.clear();
	
	for (auto iter = this->output.begin(), iter_end = this->output.end(); iter != iter_end; ++iter) {
		iter->second->Release();
	}
	this->output.clear();

	if (this->dxgiFactory != nullptr)
		SAFE_RELEASE(this->dxgiFactory);

	if (this->dxgiAdapter != nullptr)
		SAFE_RELEASE(this->dxgiAdapter);

	if (this->dxgiDevice != nullptr)
		SAFE_RELEASE(this->dxgiDevice);

	if (this->deviceContext != nullptr)
		SAFE_RELEASE(this->deviceContext);

	if (this->device != nullptr)
		SAFE_RELEASE(this->device);
}

const std::vector<puff::type_helper::screen_t*>& DXGICapturer::getAvailableScreens() const
{
	return this->availableScreens;
}

bool DXGICapturer::isHWAccelerated() const { return true; }

bool DXGICapturer::capture(const SharableTarget* target, std::uint8_t** dstBuffer, std::size_t* byteWritten, PixelType* dstPixelType, const Area& originalArea)
{
	SCOPED_LOCK(mutex);

	HMONITOR monitorHandle = nullptr;
	if (target->isWindow())
		monitorHandle = MonitorFromWindow(reinterpret_cast<const Win32Window*>(target)->getHandle(), MONITOR_DEFAULTTONEAREST);
	else if (target->isScreen())
		monitorHandle = reinterpret_cast<const Win32Screen*>(target)->getHandle();
	
	bool isCached = this->cachedDuplication[monitorHandle];
	if (this->currentMonitor != monitorHandle && this->duplication != nullptr)
		SAFE_RELEASE(this->duplication);

	while (true) {
		if (this->currentMonitor != monitorHandle && FAILED(this->output1[monitorHandle]->DuplicateOutput(this->device, &this->duplication)))
			continue;

		break;
	}
	
	ID3D11Texture2D* texture = this->cpuAccessTexture[monitorHandle];

	//------------------------------------------------------------
	// 화면 캡쳐 시도
	//------------------------------------------------------------
	while (true)
	{
		DXGI_OUTDUPL_FRAME_INFO frameInformation = { 0, };
		IDXGIResource* desktopResource = nullptr;
		HRESULT hr = this->duplication->AcquireNextFrame(isCached ? 17 : 1000, &frameInformation, &desktopResource);
		ID3D11Texture2D* frameTexture = nullptr;

		if (FAILED(hr) && hr != DXGI_ERROR_WAIT_TIMEOUT && isCached == false)
			return false;
		else if (SUCCEEDED(hr))
		{
			if (frameInformation.TotalMetadataBufferSize == 0)
			{
				this->duplication->ReleaseFrame();
				continue;
			}

			//------------------------------------------------------------
			// 새로운 화면 갱신이 있었다면 Staging Texture에 캡쳐 내용을
			// 복사하여 저장한다.
			//------------------------------------------------------------
			if (FAILED(desktopResource->QueryInterface(IID_PPV_ARGS(&frameTexture))))
				return false;

			desktopResource->Release();

			this->deviceContext->CopyResource(texture, frameTexture);
			this->cachedDuplication[monitorHandle] = true;

			frameTexture->Release();
		}

		break;
	}

	//------------------------------------------------------------
	// Staging Texture의 Raw data를 버퍼로 복사한다.
	//------------------------------------------------------------
	D3D11_MAPPED_SUBRESOURCE resource;
	UINT subresource = D3D11CalcSubresource(0, 0, 0);
	this->deviceContext->Map(texture, subresource, D3D11_MAP_READ_WRITE, 0, &resource);

	std::uint8_t* data = static_cast<std::uint8_t*>(resource.pData);
	if (target->isWindow()) 
	{
		int pitch = originalArea.getWidth() * 4;
		
		auto screen = this->screenInstances[monitorHandle];
		const Area& screenArea = screen->getArea();
		int width = screenArea.getWidth(), height = screenArea.getHeight();

		const int leftPadding = originalArea.left * 4;
		const int widthPitch = originalArea.getWidth() * 4;

		for (int y = 0, ym = originalArea.getHeight(); y < ym; ++y)
			memcpy((*dstBuffer) + (widthPitch * y), data + (originalArea.top * width * 4) + (y * width * 4) + leftPadding, widthPitch);
	}
	else if (target->isScreen()) 
	{
		const Area& area = target->getArea();

		memcpy((*dstBuffer), data, area.getWidth() * area.getHeight() * 4);
	}

	this->deviceContext->Unmap(texture, subresource);

	//------------------------------------------------------------
	// 사용한 리소스를 정리한다.
	//------------------------------------------------------------
	if (FAILED(this->duplication->ReleaseFrame()))
		return false;

	this->currentMonitor = monitorHandle;
	return true;
}

void DXGICapturer::release()
{
	delete this;
}