#define DIRECTINPUT_VERSION		0x0800

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <dinput.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

using namespace DirectX;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	//���b�Z�[�W�ŕ���
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//�E�C���h�E�T�C�Y
	const int window_width = 1280;							//����
	const int window_height = 720;							//�c��

	WNDCLASSEX w{};											//�E�C���h�E�N���X�̐ݒ�
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;					//�E�C���h�E�v���V�[�W����ݒ�
	w.lpszClassName = L"DirectXGame";						//�E�B���h�E�N���X��
	w.hInstance = GetModuleHandle(nullptr);					//�E�C���h�E�n���h��
	w.hCursor = LoadCursor(NULL, IDC_ARROW);			//�J�[�\���w��

	//�E�C���h�E�N���X��OS�ɓo�^
	RegisterClassEx(&w);
	//�E�C���h�E�T�C�Y	{X���W Y���W ���� �c��}
	RECT wrc = { 0,0,window_width,window_height };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);	//�����ŃT�C�Y�␳

	//�E�C���h�E�I�u�W�F�N�g�̐���
	HWND hwnd = CreateWindow(w.lpszClassName,		//�N���X��
		L"DirectXGame",								//�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,						//�W���I�ȃE�C���h�E�X�^�C��
		CW_USEDEFAULT,								//�\��X���W(OS�ɔC����)
		CW_USEDEFAULT,								//�\��Y���W(OS�ɔC����)
		wrc.right - wrc.left,						//�E�C���h�E����
		wrc.bottom - wrc.top,						//�E�C���h�E�c��
		nullptr,									//�e�E�C���h�E�n���h��
		nullptr,									//���j��

		w.hInstance,								//�Ăяo���A�v���P�[�V�����n���h��
		nullptr);									//�I�v�V����

	//�E�C���h�E�\��
	ShowWindow(hwnd, SW_SHOW);

	MSG msg;		//���b�Z�[�W

	float R, G, B;
	R = 0.0f;
	G = 0.0f;
	B = 1.0f;

	float xMove = 0;

	//DirectX������������������
#ifdef _DEBUG
//�f�o�b�O���C���[���I����
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
	}
#endif

	HRESULT result;
	ID3D12Device* dev = nullptr;
	IDXGIFactory6* dxgiFactory = nullptr;
	IDXGISwapChain4* swapchain = nullptr;
	ID3D12CommandAllocator* cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* cmdList = nullptr;
	ID3D12CommandQueue* cmdQueue = nullptr;
	ID3D12DescriptorHeap* rtvHeaps = nullptr;

	// DXGI�t�@�N�g���[�̐���
	result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	// �A�_�v�^�[�̗񋓗p
	std::vector<IDXGIAdapter1*> adapters;
	// �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	IDXGIAdapter1* tmpAdapter = nullptr;

	for (int i = 0;
		dxgiFactory->EnumAdapters1(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;
		i++) {
		adapters.push_back(tmpAdapter); // ���I�z��ɒǉ�����
	}

	for (int i = 0; i < adapters.size(); i++) {
		DXGI_ADAPTER_DESC1 adesc;
		adapters[i]->GetDesc1(&adesc);  // �A�_�v�^�[�̏����擾

		// �\�t�g�E�F�A�f�o�C�X�����
		if (adesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			continue;
		}

		std::wstring strDesc = adesc.Description;   // �A�_�v�^�[��

		// Intel UHD Graphics�i�I���{�[�h�O���t�B�b�N�j�����
		if (strDesc.find(L"Intel") == std::wstring::npos) {
			tmpAdapter = adapters[i];   // �̗p
			break;
		}
	}

	// �Ή����x���̔z��
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (int i = 0; i < _countof(levels); i++) {
		// �̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		result = D3D12CreateDevice(tmpAdapter, levels[i], IID_PPV_ARGS(&dev));
		if (result == S_OK) {
			// �f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
			featureLevel = levels[i];
			break;
		}
	}

	// �R�}���h�A���P�[�^�𐶐�
	result = dev->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator));

	// �R�}���h���X�g�𐶐�
	result = dev->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator, nullptr,
		IID_PPV_ARGS(&cmdList));

	// �W���ݒ�ŃR�}���h�L���[�𐶐�
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};

	dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));

	// �e��ݒ�����ăX���b�v�`�F�[���𐶐�
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = 1280;
	swapchainDesc.Height = 720;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // �F���̏���
	swapchainDesc.SampleDesc.Count = 1; // �}���`�T���v�����Ȃ�
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; // �o�b�N�o�b�t�@�p
	swapchainDesc.BufferCount = 2;  // �o�b�t�@�����Q�ɐݒ�
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // �t���b�v��͔j��
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue,
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)&swapchain);

	// �e��ݒ�����ăf�X�N���v�^�q�[�v�𐶐�
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // �����_�[�^�[�Q�b�g�r���[
	heapDesc.NumDescriptors = 2;    // ���\�̂Q��
	dev->CreateDescriptorHeap(&heapDesc,
		IID_PPV_ARGS(&rtvHeaps));

	// ���\�̂Q���ɂ���
	std::vector<ID3D12Resource*> backBuffers(2);
	for (int i = 0; i < 2; i++) {
		// �X���b�v�`�F�[������o�b�t�@���擾
		result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
		// �f�X�N���v�^�q�[�v�̃n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		// �����\���ŃA�h���X�������
		handle.ptr += i * dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
		// �����_�[�^�[�Q�b�g�r���[�̐���
		dev->CreateRenderTargetView(
			backBuffers[i],
			nullptr,
			handle);
	}

	// �t�F���X�̐���
	ID3D12Fence* fence = nullptr;
	UINT64 fenceVal = 0;

	result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	IDirectInput8* dinput = nullptr;
	result = DirectInput8Create(
		w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr
	);

	IDirectInputDevice8* devkeyboard = nullptr;
	result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);
	result = devkeyboard->SetDataFormat(&c_dfDIKeyboard);
	result = devkeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	//DirectX���������������܂�

	XMFLOAT3 vertices[] = {
	{ -0.5f, -0.5f, 0.0f}, // ����
	{ -0.5f, +0.5f, 0.0f}, // ����
	{ +0.5f, -0.5f, 0.0f}, // �E��
	};
	// ���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(XMFLOAT3) * _countof(vertices));

	// ���_�o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapprop{};   // �q�[�v�ݒ�
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p

	D3D12_RESOURCE_DESC resdesc{};  // ���\�[�X�ݒ�
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = sizeVB; // ���_�f�[�^�S�̂̃T�C�Y
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.SampleDesc.Count = 1;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// ���_�o�b�t�@�̐���
	ID3D12Resource* vertBuff = nullptr;
	result = dev->CreateCommittedResource(
		&heapprop, // �q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resdesc, // ���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	// GPU��̃o�b�t�@�ɑΉ��������z���������擾
	XMFLOAT3* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);

	// �S���_�ɑ΂���
	for (int i = 0; i < _countof(vertices); i++) {
		vertMap[i] = vertices[i];   // ���W���R�s�[
	}

	// �}�b�v������
	vertBuff->Unmap(0, nullptr);

	// ���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeVB;
	vbView.StrideInBytes = sizeof(XMFLOAT3);

	ID3DBlob* vsBlob = nullptr; // ���_�V�F�[�_�I�u�W�F�N�g
	ID3DBlob* psBlob = nullptr; // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ID3DBlob* errorBlob = nullptr; // �G���[�I�u�W�F�N�g

	// ���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"BasicVS.hlsl",  // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "vs_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&vsBlob, &errorBlob);

	// �s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"BasicPS.hlsl",   // �V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�\�ɂ���
		"main", "ps_5_0", // �G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�ݒ�
		0,
		&psBlob, &errorBlob);

	if (FAILED(result)) {
		// errorBlob����G���[���e��string�^�ɃR�s�[
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// �G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// ���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}, // (1�s�ŏ������ق������₷��)
	};

	// �O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	gpipeline.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	gpipeline.VS.BytecodeLength = vsBlob->GetBufferSize();
	gpipeline.PS.pShaderBytecode = psBlob->GetBufferPointer();
	gpipeline.PS.BytecodeLength = psBlob->GetBufferSize();

	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // �W���ݒ�
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;  // �J�����O���Ȃ�
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // �|���S�����h��Ԃ�
	gpipeline.RasterizerState.DepthClipEnable = true; // �[�x�N���b�s���O��L����

	//gpipeline.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;  // RBGA�S�Ẵ`�����l����`��

	//�����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = gpipeline.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;		//�W���ݒ�

	blenddesc.BlendEnable = true;										//�u�����h��L���ɂ���
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;						//���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;							//�\�[�X�̒l��100%�g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;						//�f�X�g�̒l��0%�g��

	/*
	//���Z����
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;								//���Z
	blenddesc.SrcBlend = D3D12_BLEND_ONE;								//�\�[�X�̒l��100%�g��
	blenddesc.DestBlend = D3D12_BLEND_ONE;								//�f�X�g�̒l��100%�g��
	*/

	/*
	//���Z����
	blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;					//�f�X�g����\�[�X�����Z
	blenddesc.SrcBlend = D3D12_BLEND_ONE;								//�\�[�X�̒l��100%�g��
	blenddesc.DestBlend = D3D12_BLEND_ONE;								//�f�X�g�̒l��100%�g��
	*/

	/*
	//�F���]
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;								//���Z
	blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;					//1.0f-�f�X�g�J���[�̒l
	blenddesc.DestBlend = D3D12_BLEND_ZERO;								//�g��Ȃ�
	*/

	//����������
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;								//���Z
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;							//�\�[�X�̃A���t�@�l
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;					//1.0f-�\�[�X�̃A���t�@�l
	
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1; // �`��Ώۂ�1��
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0�`255�w���RGBA
	gpipeline.SampleDesc.Count = 1; // 1�s�N�Z���ɂ�1��T���v�����O

	ID3D12RootSignature* rootsignature;

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	result = dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootsignature));
	rootSigBlob->Release();

	// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	gpipeline.pRootSignature = rootsignature;

	ID3D12PipelineState* pipelinestate = nullptr;
	result = dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate));



	while (true) {	//�Q�[�����[�v
		//���b�Z�[�W������H
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);	//�L�[���̓��b�Z�[�W�̏���
			DispatchMessage(&msg);	//�v���V�[�W���Ƀ��b�Z�[�W�𑗂�
		}

		//�~�{�^���ŏI�����b�Z�[�W��������Q�[�����[�v�𔲂���
		if (msg.message == WM_QUIT) {
			break;
		}

		//DirectX���t���[��������������

		result = devkeyboard->Acquire();
		BYTE key[256] = {};
		result = devkeyboard->GetDeviceState(sizeof(key), key);
		if (key[DIK_0]) {
			OutputDebugStringA("Hit 0\n");
		}

		if (key[DIK_SPACE]) {
			R = 0.5f;
			G = 0.0f;
			B = 0.0f;
		}

		if (key[DIK_RIGHT]) {
			xMove += 0.1f;
		}

		XMFLOAT3 vertices[] = {
				{ -0.5f + xMove, -0.5f, 0.0f}, // ����
				{ -0.5f + xMove, +0.5f, 0.0f}, // ����
				{ +0.5f + xMove, -0.5f, 0.0f}, // �E��
		};
		
		XMFLOAT3* vertMap = nullptr;
		result = vertBuff->Map(0, nullptr, (void**)&vertMap);

		// �S���_�ɑ΂���
		for (int i = 0; i < _countof(vertices); i++) {
			vertMap[i] = vertices[i];   // ���W���R�s�[
		}

		// �}�b�v������
		vertBuff->Unmap(0, nullptr);

		// �o�b�N�o�b�t�@�̔ԍ����擾�i2�Ȃ̂�0�Ԃ�1�ԁj
		UINT bbIndex = swapchain->GetCurrentBackBufferIndex();

		// �P�D���\�[�X�o���A�ŏ������݉\�ɕύX
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = backBuffers[bbIndex]; // �o�b�N�o�b�t�@���w��
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // �\/ ������
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`��
		cmdList->ResourceBarrier(1, &barrierDesc);

		// �Q�D�`���w��
		// �����_�[�^�[�Q�b�g�r���[�p�f�B�X�N���v�^�q�[�v�̃n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr += bbIndex * dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
		cmdList->OMSetRenderTargets(1, &rtvH, false, nullptr);

		// �R�D��ʃN���A           R     G     B    A
		float clearColor[] = { R,G,B,0.0f }; // ���ۂ��F
		cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

		// �S�D�`��R�}���h��������
		cmdList->SetPipelineState(pipelinestate);
		cmdList->SetGraphicsRootSignature(rootsignature);
		D3D12_VIEWPORT viewport{};

		viewport.Width = window_width;
		viewport.Height = window_height;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		cmdList->RSSetViewports(1, &viewport);

		D3D12_RECT scissorrect{};

		scissorrect.left = 0;                                       // �؂蔲�����W��
		scissorrect.right = scissorrect.left + window_width;        // �؂蔲�����W�E
		scissorrect.top = 0;                                        // �؂蔲�����W��
		scissorrect.bottom = scissorrect.top + window_height;       // �؂蔲�����W��

		cmdList->RSSetScissorRects(1, &scissorrect);

		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		cmdList->IASetVertexBuffers(0, 1, &vbView);

		cmdList->DrawInstanced(3, 1, 0, 0);

		// �T�D���\�[�X�o���A��߂�
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`��
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;   // �\����
		cmdList->ResourceBarrier(1, &barrierDesc);

		// ���߂̃N���[�Y
		cmdList->Close();
		// �R�}���h���X�g�̎��s
		ID3D12CommandList* cmdLists[] = { cmdList }; // �R�}���h���X�g�̔z��
		cmdQueue->ExecuteCommandLists(1, cmdLists);
		// �R�}���h���X�g�̎��s������҂�
		cmdQueue->Signal(fence, ++fenceVal);
		if (fence->GetCompletedValue() != fenceVal) {
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			fence->SetEventOnCompletion(fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

		cmdAllocator->Reset(); // �L���[���N���A
		cmdList->Reset(cmdAllocator, nullptr);  // �ĂуR�}���h���X�g�𒙂߂鏀��

		// �o�b�t�@���t���b�v�i���\�̓��ւ��j
		swapchain->Present(1, 0);

		//DirectX���t���[�����������܂�
	}

	UnregisterClass(w.lpszClassName, w.hInstance);

	OutputDebugStringA("Hello,DirectX!!\n");

	return 0;
}
