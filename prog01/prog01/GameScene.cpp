#include "GameScene.h"
#include <cassert>

GameScene::GameScene()
{

}

GameScene::~GameScene()
{
	safe_delete(sprite);
	safe_delete(back1);
	safe_delete(back2);
	safe_delete(back3);
	safe_delete(object3d);
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio)
{
	// nullptr�`�F�b�N
	assert(dxCommon);
	assert(input);
	assert(audio);

	this->dxCommon = dxCommon;
	this->input = input;
	this->audio = audio;

	// �f�o�b�O�e�L�X�g�p�e�N�X�`���ǂݍ���
	if (!Sprite::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png")) {
		assert(0);
	}
	// �f�o�b�O�e�L�X�g������
	debugText.Initialize(debugTextTexNumber);

	// �e�N�X�`���ǂݍ���
	if (!Sprite::LoadTexture(1, L"Resources/APEX_01.png")) {
		assert(0);
	}

	if (!Sprite::LoadTexture(2, L"Resources/back/back1.png")) {
		assert(0);
	}
	if (!Sprite::LoadTexture(3, L"Resources/back/back2.png")) {
		assert(0);
	}
	if (!Sprite::LoadTexture(4, L"Resources/back/back3.png")) {
		assert(0);
	}

	// �w�i�X�v���C�g����
	/*sprite = Sprite::Create(1, { 0.0f,0.0f });
	sprite->SetSize({ WinApp::window_width,WinApp::window_height });
	sprite->SetPosition({ 0.0f,0.0f });*/

	back1 = Sprite::Create(2, { 0.0f,0.0f });
	back1->SetSize({ 1600,1600 });
	back1->SetRotation(45.0f);

	back2 = Sprite::Create(3, { 0.0f,-1600.0f });
	back2->SetSize({ 1600,1600 });
	back2->SetRotation(45.0f);

	back3 = Sprite::Create(4, { 0.0f,-3200.0f });
	back3->SetSize({ 1600,1600 });
	back3->SetRotation(45.0f);

	// 3D�I�u�W�F�N�g����
	object3d = Object3d::Create();
	object3d->SetPosition({-30.0f,-15.0f,0.0f});

	//�T�E���h�Đ�
	audio->PlayWave("Resources/Alarm01.wav");
}

void GameScene::Update()
{
	//�L�[��������Ă���Ƃ��̏���
	if (input->TriggerKey(DIK_0))
	{
		OutputDebugStringA("Hit 0\n");
	}

	//X���W�AY���W���w�肵�ĕ\��
	debugText.Print("Hello,DirectX!!", 200, 100, 1.0f);
	//X���W�AY���W�A�k�ڂ��w�肵�ĕ\��
	debugText.Print("Nihon Kogakuin", 200, 200, 2.0f);

	// �I�u�W�F�N�g�ړ�
	if (input->PushKey(DIK_UP) || input->PushKey(DIK_DOWN) || input->PushKey(DIK_RIGHT) || input->PushKey(DIK_LEFT))
	{
		// ���݂̍��W���擾
		XMFLOAT3 position = object3d->GetPosition();

		// �ړ���̍��W���v�Z
		if (input->PushKey(DIK_UP))
		{
			position.y += 1.0f;
		}
		else if (input->PushKey(DIK_DOWN))
		{
			position.y -= 1.0f;
		}

		if (input->PushKey(DIK_RIGHT))
		{
			position.x += 1.0f;
		}
		else if (input->PushKey(DIK_LEFT))
		{
			position.x -= 1.0f;
		}

		// ���W�̕ύX�𔽉f
		object3d->SetPosition(position);
	}

	// �J�����ړ�
	if (input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->PushKey(DIK_D) || input->PushKey(DIK_A))
	{
		if (input->PushKey(DIK_W))
		{
			//Object3d::CameraMoveEyeVector({ 0.0f,+1.0f,0.0f });
			scroll += 10.0f;
		}
		else if (input->PushKey(DIK_S))
		{
			//Object3d::CameraMoveEyeVector({ 0.0f,-1.0f,0.0f });
			scroll -= 10.0f;
		}

		if (input->PushKey(DIK_D))
		{
			//Object3d::CameraMoveEyeVector({ +1.0f,0.0f,0.0f });
		}
		else if (input->PushKey(DIK_A))
		{
			//Object3d::CameraMoveEyeVector({ -1.0f,0.0f,0.0f });
		}
	}

	if (scroll >= 1710.0f) {
		scroll = 0;
	}

	back1->SetPosition({ 600.0f -scroll,-600.0f +scroll});
	back2->SetPosition({ 1730.0f - scroll,-1730.0f + scroll });
	back3->SetPosition({ 2860.0f - scroll,-2860.0f + scroll });

	object3d->Update();
}

void GameScene::Draw()
{
	// �R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();
#pragma region �w�i�X�v���C�g�`��
	// �w�i�X�v���C�g�`��O����
	Sprite::PreDraw(dxCommon->GetCommandList());
	// �w�i�X�v���C�g�`��
	//sprite->Draw();
	back1->Draw();
	back2->Draw();
	back3->Draw();
	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();
	// �[�x�o�b�t�@�N���A
	dxCommon->ClearDepthBuffer();
#pragma endregion �w�i�X�v���C�g�`��
#pragma region 3D�I�u�W�F�N�g�`��
	// 3D�I�u�W�F�N�g�`��O����
	Object3d::PreDraw(dxCommon->GetCommandList());
	// 3D�I�u�N�W�F�N�g�̕`��
	object3d->Draw();
	// 3D�I�u�W�F�N�g�`��㏈��
	Object3d::PostDraw();
#pragma endregion 3D�I�u�W�F�N�g�`��
#pragma region �O�i�X�v���C�g�`��
	// �O�i�X�v���C�g�`��O����
	Sprite::PreDraw(dxCommon->GetCommandList());
	// �f�o�b�O�e�L�X�g�̕`��
	debugText.DrawAll(dxCommon->GetCommandList());
	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();
#pragma endregion �O�i�X�v���C�g�`��
}