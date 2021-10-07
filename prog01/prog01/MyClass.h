#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#pragma once

class MyClass
{
public:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	//Microsoft::WRL::ComPtr<ID3D12Device> device;
	//Microsoft::WRL::ComPtr<IDXGIFactory6>dxgiFactory;
	//ComPtr<ID3D12Device> device;
	//ComPtr<IDXGIFactory6> dxgiFactory;
};