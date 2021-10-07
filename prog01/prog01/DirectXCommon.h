#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <d3dx12.h>
#include <cstdlib>

#include "WinApp.h"

// DirectX汎用
class DirectXCommon {
private:
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private: // メンバ変数
// ウィンドウズアプリケーション管理
	WinApp* winApp;

	// Direct3D関連
	ComPtr<ID3D12Device> dev;
	ComPtr<IDXGIFactory6> dxgiFactory;
	ComPtr<IDXGISwapChain4> swapchain;
	ComPtr<ID3D12CommandAllocator> cmdAllocator;
	ComPtr<ID3D12GraphicsCommandList> cmdList;
	ComPtr<ID3D12CommandQueue> cmdQueue;
	std::vector<ComPtr<ID3D12Resource>> backBuffers;
	ComPtr<ID3D12Resource> depthBuffer;
	ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr<ID3D12Fence> fence;
	UINT64 fenceVal = 0;

public: // メンバ関数
	// 初期化
	void Init(WinApp* win);

	// レンダーターゲットのクリア
	void ClearRenderTarget();

	// 深度バッファのクリア
	void ClearDepthBuffer();

	// 描画前処理
	void PreDraw();

	// 描画後処理
	void PostDraw();

	// デバイスの取得
	// <returns>デバイス</returns>
	ID3D12Device* GetDevice() { return dev.Get(); }

	// 描画コマンドリストの取得
	/// <returns>描画コマンドリスト</returns>
	ID3D12GraphicsCommandList* GetCommandList() { return cmdList.Get(); }


	/// <summary>
	/// DXGIデバイス初期化
	/// </summary>
	/// <returns>成否</returns>
	bool InitDXGIDevice();

	/// <summary>
	/// コマンド関連初期化
	/// </summary>
	/// <returns>成否</returns>
	bool InitCommand();

	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	/// <returns>成否</returns>
	bool CreateSwapChain();

	/// <summary>
	/// レンダーターゲット生成
	/// </summary>
	/// <returns>成否</returns>
	bool CreateFinalRenderTargets();

	/// <summary>
	/// 深度バッファ生成
	/// </summary>
	/// <returns>成否</returns>
	bool CreateDepthBuffer();

	/// <summary>
	/// フェンス生成
	/// </summary>
	/// <returns>成否</returns>
	bool CreateFence();


};