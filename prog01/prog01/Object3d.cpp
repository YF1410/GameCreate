#include "Object3d.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

// �ÓI�����o�ϐ��̎���
ID3D12Device* Object3d::device = nullptr;
UINT Object3d::descriptorHandleIncrementSize = 0;
ID3D12GraphicsCommandList* Object3d::cmdList = nullptr;
ComPtr<ID3D12RootSignature> Object3d::rootsignature;
ComPtr<ID3D12PipelineState> Object3d::pipelinestate;
ComPtr<ID3D12DescriptorHeap> Object3d::descHeap;
ComPtr<ID3D12Resource> Object3d::vertBuff;
ComPtr<ID3D12Resource> Object3d::indexBuff;
ComPtr<ID3D12Resource> Object3d::texbuff;
CD3DX12_CPU_DESCRIPTOR_HANDLE Object3d::cpuDescHandleSRV;
CD3DX12_GPU_DESCRIPTOR_HANDLE Object3d::gpuDescHandleSRV;
XMMATRIX Object3d::matView{};
XMMATRIX Object3d::matProjection{};
XMFLOAT3 Object3d::eye = { 0, 0, -50.0f };
XMFLOAT3 Object3d::target = { 0, 0, 0 };
XMFLOAT3 Object3d::up = { 0, 1, 0 };
D3D12_VERTEX_BUFFER_VIEW Object3d::vbView{};
D3D12_INDEX_BUFFER_VIEW Object3d::ibView{};
//Object3d::VertexPosNormalUv Object3d::vertices[vertexCount];
//unsigned short Object3d::indices[planeCount * 3];
std::vector<Object3d::VertexPosNormalUv> Object3d::vertices;
std::vector<unsigned short> Object3d::indices;
Object3d::Material Object3d::material;

bool Object3d::StaticInitialize(ID3D12Device* device, int window_width, int window_height)
{
	// nullptr�`�F�b�N
	assert(device);

	Object3d::device = device;

	Model::StaticInitialize(device);

	// �f�X�N���v�^�q�[�v�̏�����
	//InitializeDescriptorHeap();

	// �J����������
	InitializeCamera(window_width, window_height);

	// �p�C�v���C��������
	//InitializeGraphicsPipeline();

	// �e�N�X�`���ǂݍ���
	//LoadTexture();

	// ���f������
	//CreateModel();

	return true;
}

void Object3d::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	// PreDraw��PostDraw���y�A�ŌĂ΂�Ă��Ȃ���΃G���[
	assert(Object3d::cmdList == nullptr);

	// �R�}���h���X�g���Z�b�g
	Object3d::cmdList = cmdList;

	Model::PreDraw(cmdList);
}

void Object3d::PostDraw()
{
	// �R�}���h���X�g������
	Object3d::cmdList = nullptr;

	Model::PostDraw();
}

Object3d* Object3d::Create()
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	Object3d* object3d = new Object3d();
	if (object3d == nullptr)
	{
		return nullptr;
	}

	//�X�P�[�����Z�b�g
	float scale_val = 5;
	object3d->scale = { scale_val, scale_val, scale_val };

	return object3d;
}

void Object3d::SetEye(XMFLOAT3 eye)
{
	Object3d::eye = eye;

	UpdateViewMatrix();
}

void Object3d::SetTarget(XMFLOAT3 target)
{
	Object3d::target = target;

	UpdateViewMatrix();
}

void Object3d::CameraMoveVector(XMFLOAT3 move)
{
	XMFLOAT3 eye_moved = GetEye();
	XMFLOAT3 target_moved = GetTarget();

	eye_moved.x += move.x;
	eye_moved.y += move.y;
	eye_moved.z += move.z;

	target_moved.x += move.x;
	target_moved.y += move.y;
	target_moved.z += move.z;

	SetEye(eye_moved);
	SetTarget(target_moved);
}

void Object3d::CameraMoveEyeVector(XMFLOAT3 move)
{
	XMFLOAT3 eye_moved = GetEye();

	eye_moved.x += move.x;
	eye_moved.y += move.y;
	eye_moved.z += move.z;

	SetEye(eye_moved);
}


void Object3d::InitializeCamera(int window_width, int window_height)
{
	// �r���[�s��̐���
	matView = XMMatrixLookAtLH
	(
		XMLoadFloat3(&eye),
		XMLoadFloat3(&target),
		XMLoadFloat3(&up)
	);

	// ���s���e�ɂ��ˉe�s��̐���
	//constMap->mat = XMMatrixOrthographicOffCenterLH(
	//0, window_width,
	//window_height, 0,
	//0, 1);
	// �������e�ɂ��ˉe�s��̐���
	matProjection = XMMatrixPerspectiveFovLH
	(
		XMConvertToRadians(60.0f),
		(float)window_width / (float)window_height,
		0.1f, 1000.0f
	);
}

void Object3d::UpdateViewMatrix()
{
	// �r���[�s��̍X�V
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
}

void Object3d::Update()
{
	HRESULT result;
	XMMATRIX matScale, matRot, matTrans;

	// �X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	// ���[���h�s��̍���
	matWorld = XMMatrixIdentity(); // �ό`�����Z�b�g
	matWorld *= matScale; // ���[���h�s��ɃX�P�[�����O�𔽉f
	matWorld *= matRot; // ���[���h�s��ɉ�]�𔽉f
	matWorld *= matTrans; // ���[���h�s��ɕ��s�ړ��𔽉f

	// �e�I�u�W�F�N�g�������
	if (parent != nullptr)
	{
		// �e�I�u�W�F�N�g�̃��[���h�s����|����
		matWorld *= parent->matWorld;
	}

	model->Update(matWorld, matView, matProjection);
}

void Object3d::Draw()
{
	// nullptr�`�F�b�N
	assert(device);
	assert(Object3d::cmdList);

	model->Draw();
}

void Object3d::SetModel(Model* model)
{
	this->model = model;
}