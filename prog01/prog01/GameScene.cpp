#include "GameScene.h"
#include <cassert>

GameScene::GameScene()
{

}

GameScene::~GameScene()
{
	safe_delete(sprite);
	safe_delete(object3d);
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio)
{
	// nullptrチェック
	assert(dxCommon);
	assert(input);
	assert(audio);

	this->dxCommon = dxCommon;
	this->input = input;
	this->audio = audio;

	// デバッグテキスト用テクスチャ読み込み
	if (!Sprite::LoadTexture(debugTextTexNumber, L"Resources/debugfont.png")) {
		assert(0);
	}
	// デバッグテキスト初期化
	debugText.Initialize(debugTextTexNumber);

	// テクスチャ読み込み
	if (!Sprite::LoadTexture(1, L"Resources/APEX_01.png")) {
		assert(0);
	}

	// 背景スプライト生成
	sprite = Sprite::Create(1, { 0.0f,0.0f });
	sprite->SetSize({ 100.0f,100.0f });
	sprite->SetPosition({ 100.0f,100.0f });

	// 3Dオブジェクト生成
	object3d = Object3d::Create();
	object3d->Update();

	//サウンド再生
	audio->PlayWave("Resources/Alarm01.wav");
}

void GameScene::Update()
{
	//キーが押されているときの処理
	if (input->TriggerKey(DIK_0))
	{
		OutputDebugStringA("Hit 0\n");
	}

	//X座標、Y座標を指定して表示
	debugText.Print("Hello,DirectX!!", 200, 100, 1.0f);
	//X座標、Y座標、縮尺を指定して表示
	debugText.Print("Nihon Kogakuin", 200, 200, 2.0f);

	// オブジェクト移動
	if (input->PushKey(DIK_UP) || input->PushKey(DIK_DOWN) || input->PushKey(DIK_RIGHT) || input->PushKey(DIK_LEFT))
	{
		// 現在の座標を取得
		XMFLOAT3 position = object3d->GetPosition();

		// 移動後の座標を計算
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

		// 座標の変更を反映
		object3d->SetPosition(position);
	}

	// カメラ移動
	if (input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->PushKey(DIK_D) || input->PushKey(DIK_A))
	{
		if (input->PushKey(DIK_W))
		{
			Object3d::CameraMoveEyeVector({ 0.0f,+1.0f,0.0f });
		}
		else if (input->PushKey(DIK_S))
		{
			Object3d::CameraMoveEyeVector({ 0.0f,-1.0f,0.0f });
		}

		if (input->PushKey(DIK_D))
		{
			Object3d::CameraMoveEyeVector({ +1.0f,0.0f,0.0f });
		}
		else if (input->PushKey(DIK_A))
		{
			Object3d::CameraMoveEyeVector({ -1.0f,0.0f,0.0f });
		}
	}

	object3d->Update();
}

void GameScene::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();
#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());
	// 背景スプライト描画
	sprite->Draw();
	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon->ClearDepthBuffer();
#pragma endregion 背景スプライト描画
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(dxCommon->GetCommandList());
	// 3Dオブクジェクトの描画
	object3d->Draw();
	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();
#pragma endregion 3Dオブジェクト描画
#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());
	// デバッグテキストの描画
	debugText.DrawAll(dxCommon->GetCommandList());
	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion 前景スプライト描画
}