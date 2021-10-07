﻿#include "DirectXCommon.h"
#include <vector>
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

using namespace Microsoft::WRL;

void DirectXCommon::Init(WinApp* winApp) {
	// nullptrチェック
	assert(winApp);

	this->winApp = winApp;

	// DXGIデバイス初期化
	if (!InitDXGIDevice()) {
		assert(0);
	}

	// コマンド関連初期化
	if (!InitCommand()) {
		assert(0);
	}

	// スワップチェーンの生成
	if (!CreateSwapChain()) {
		assert(0);
	}

	// レンダーターゲット生成
	if (!CreateFinalRenderTargets()) {
		assert(0);
	}

	// 深度バッファ生成
	if (!CreateDepthBuffer()) {
		assert(0);
	}

	// フェンス生成
	if (!CreateFence()) {
		assert(0);
	}
}

void DirectXCommon::ClearRenderTarget() {
	UINT bbIndex = swapchain->GetCurrentBackBufferIndex();

	// レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIndex * dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//画面クリア				R	   G	B	  A
	float clearColor[] = { 0.0f, 0.0f, 0.5f, 1.0f };	//青っぽい色
	cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
}

void DirectXCommon::ClearDepthBuffer() {
	//深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void DirectXCommon::PreDraw() {
	//バックバッファの番号を取得（2つなので0番か1番）
	UINT bbIndex = swapchain->GetCurrentBackBufferIndex();

	//リソースバリアで書き込み可能に変更
	//表示状態から描画状態に変更
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	//描画先指定
	//レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIndex * dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	//レンダーターゲットをセット
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

	// 全画面クリア
	ClearRenderTarget();
	// 深度バッファクリア
	ClearDepthBuffer();

	// ビューポートの設定
	cmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, WinApp::WIN_WIDTH, WinApp::WIN_HEIGHT));
	// シザリング矩形の設定
	cmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, WinApp::WIN_WIDTH, WinApp::WIN_HEIGHT));
}

void DirectXCommon::PostDraw() {
	//リソースバリアを戻す
	//描画状態から表示状態に変更
	UINT bbIndex = swapchain->GetCurrentBackBufferIndex();
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	//命令のクローズ
	cmdList->Close();
	//コマンドリストの実行
	ID3D12CommandList* cmdLists[] = { cmdList.Get() };	//コマンドリストの配列
	cmdQueue->ExecuteCommandLists(1, cmdLists);
	//コマンドリストの実行完了を待つ
	cmdQueue->Signal(fence.Get(), ++fenceVal);
	if (fence->GetCompletedValue() != fenceVal) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	cmdAllocator->Reset(); // キューをクリア
	cmdList->Reset(cmdAllocator.Get(), nullptr);		//再びコマンドリストを貯める準備

	//バッファをフリップ（裏表の入替え）
	swapchain->Present(1, 0);
}

bool DirectXCommon::InitDXGIDevice() {
	HRESULT result = S_FALSE;
#ifdef _DEBUG
	//デバッグレイヤーをオンに
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
	}
#endif
	// DXGIファクトリーの生成
	result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	// アダプターの列挙用
	std::vector<ComPtr<IDXGIAdapter1>> adapters;
	// ここに特定の名前を持つアダプターオブジェクトが入る
	ComPtr<IDXGIAdapter1> tmpAdapter;
	for (int i = 0;
		dxgiFactory->EnumAdapters1(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;
		i++) {
		adapters.push_back(tmpAdapter); // 動的配列に追加する
	}

	for (int i = 0; i < adapters.size(); i++) {
		DXGI_ADAPTER_DESC1 adesc;
		adapters[i]->GetDesc1(&adesc);  // アダプターの情報を取得

		// ソフトウェアデバイスを回避
		if (adesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			continue;
		}

		std::wstring strDesc = adesc.Description;   // アダプター名
		// Intel UHD Graphics（オンボードグラフィック）を回避
		if (strDesc.find(L"Intel") == std::wstring::npos) {
			tmpAdapter = adapters[i];   // 採用
			break;
		}
	}


	// 対応レベルの配列
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;


	result = S_FALSE;
	for (int i = 0; i < _countof(levels); i++) {
		// 採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&dev));
		if (result == S_OK) {
			// デバイスを生成できた時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}

	if (result == S_FALSE) {
		assert(0);
		return false;
	}

	return true;
}

bool DirectXCommon::InitCommand() {
	HRESULT result = S_FALSE;

	// コマンドアロケータを生成
	result = dev->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator));

	if (result == S_FALSE) {
		assert(0);
		return result;
	}

	// コマンドリストを生成
	result = dev->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator.Get(), nullptr,
		IID_PPV_ARGS(&cmdList));

	if (result == S_FALSE) {
		assert(0);
		return result;
	}

	// 標準設定でコマンドキューを生成
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	result = dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));

	if (result == S_FALSE) {
		assert(0);
		return result;
	}

	return true;
}

bool DirectXCommon::CreateSwapChain() {
	HRESULT result = S_FALSE;

	// 各種設定をしてスワップチェーンを生成
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = WinApp::WIN_WIDTH;
	swapchainDesc.Height = WinApp::WIN_HEIGHT;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // 色情報の書式
	swapchainDesc.SampleDesc.Count = 1; // マルチサンプルしない
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; // バックバッファ用
	swapchainDesc.BufferCount = 2;  // バッファ数を２つに設定
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // フリップ後は破棄
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//IDXGISwapChain1のComPtrを用意
	ComPtr<IDXGISwapChain1> swapchain1;
	//スワップチェーンを生成
	result = dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue.Get(),
		winApp->GetHwnd(),
		&swapchainDesc,
		nullptr,
		nullptr,
		&swapchain1);

	if (FAILED(result)) {
		assert(0);
		return result;
	}

	/*if (FAILED(dxgiFactory->CreateSwapChainForHwnd(cmdQueue.Get(), winApp->GetHwnd(), &swapchainDesc, nullptr, nullptr, &swapchain1))) {
		return false;
	}*/

	swapchain1.As(&swapchain);

	return true;
}

bool DirectXCommon::CreateFinalRenderTargets() {
	HRESULT result = S_FALSE;

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = swapchain->GetDesc(&swcDesc);
	if (result == S_FALSE) {
		assert(0);
		return result;
	}

	// 各種設定をしてディスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	// レンダーターゲットビュー
	heapDesc.NumDescriptors = swcDesc.BufferCount;
	result = dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));
	if (result == S_FALSE) {
		assert(0);
		return result;
	}

	// 裏表の２つ分について
	backBuffers.resize(swcDesc.BufferCount);
	for (int i = 0; i < backBuffers.size(); i++) 	{
		// スワップチェーンからバッファを取得
		result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
		if (result == S_FALSE) {
			assert(0);
			return result;
		}

		// ディスクリプタヒープのハンドルを取得
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeaps->GetCPUDescriptorHandleForHeapStart(), i, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		// レンダーターゲットビューの生成
		dev->CreateRenderTargetView(
			backBuffers[i].Get(),
			nullptr,
			handle);
	}

	return true;
}

bool DirectXCommon::CreateDepthBuffer() {
	HRESULT result = S_FALSE;

	//深度バッファリソース設定
	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		WinApp::WIN_WIDTH,
		WinApp::WIN_HEIGHT,
		1, 0,
		1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);
	//深度バッファの生成
	result = dev->CreateCommittedResource
	(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,  //深度値書き込みに使用
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuffer)
	);

	if (result == S_FALSE) {
		assert(0);
		return result;
	}

	//深度ビュー用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;  //深度ビューは1つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;  //デプスステンシルビュー
	result = dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	if (result == S_FALSE) {
		assert(0);
		return result;
	}

	//深度ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;  //深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dev->CreateDepthStencilView
	(
		depthBuffer.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);

	return true;
}

bool DirectXCommon::CreateFence() {
	HRESULT result = S_FALSE;

	result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	if (result == S_FALSE) {
		assert(0);
		return result;
	}

	return true;
}
