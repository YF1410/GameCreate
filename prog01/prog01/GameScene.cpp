#include "GameScene.h"
#include <cassert>

GameScene::GameScene() {

}

GameScene::~GameScene() {

	safe_delete(playerModel);
	safe_delete(playerJump1Model);
	safe_delete(playerJump2Model);
	safe_delete(skydomeModel);
	safe_delete(groundModel);
	safe_delete(largeCarModel);
	safe_delete(miniCarModel);
	safe_delete(truckModel);
	safe_delete(playerObj);
	safe_delete(playerJump1Obj);
	safe_delete(playerJump2Obj);
	safe_delete(skydomeObj);
	safe_delete(groundObj);
	safe_delete(largeCarObj);
	safe_delete(miniCarObj);
	safe_delete(truckObj);
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio) {
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

	//.objの名前を指定してモデルを読み込む
	playerModel = playerModel->CreateFromObject("player");
	playerJump1Model = playerJump1Model->CreateFromObject("playerJump1");
	playerJump2Model = playerJump2Model->CreateFromObject("playerJump2");
	skydomeModel = skydomeModel->CreateFromObject("skydome");
	groundModel = groundModel->CreateFromObject("ground");
	largeCarModel = largeCarModel->CreateFromObject("largeCar");
	miniCarModel = miniCarModel->CreateFromObject("miniCar");
	truckModel = truckModel->CreateFromObject("truck");
	// 3Dオブジェクト生成
	playerObj = Object3d::Create();
	playerJump1Obj = Object3d::Create();
	playerJump2Obj = Object3d::Create();
	skydomeObj = Object3d::Create();
	groundObj = Object3d::Create();
	largeCarObj = Object3d::Create();
	miniCarObj = Object3d::Create();
	truckObj = Object3d::Create();
	// 3Dオブジェクトにモデルを割り当てる
	playerObj->SetModel(playerModel);
	playerJump1Obj->SetModel(playerJump1Model);
	playerJump2Obj->SetModel(playerJump2Model);
	skydomeObj->SetModel(skydomeModel);
	groundObj->SetModel(groundModel);
	largeCarObj->SetModel(largeCarModel);
	miniCarObj->SetModel(miniCarModel);
	truckObj->SetModel(truckModel);

	playerObj->SetPosition({ 0.0f, 0.0f, -35.0f });
	playerObj->SetScale({ playerScale,playerScale,playerScale });
	playerJump1Obj->SetPosition(playerObj->GetPosition());
	playerJump1Obj->SetScale(playerObj->GetScale());
	playerJump2Obj->SetPosition(playerObj->GetPosition());
	playerJump2Obj->SetScale(playerObj->GetScale());

	groundObj->SetScale({ groundScale,groundScale, groundScale });

	largeCarObj->SetScale({ largeCarScale,largeCarScale,largeCarScale });
	miniCarObj->SetScale({ miniCarScale,miniCarScale,miniCarScale });
	truckObj->SetScale({ truckScale,truckScale,truckScale });

	largeCarObj->SetPosition({ 0.0f, 0.0f, -10.0f });
	miniCarObj->SetPosition({ 0.0f, 0.0f, -20.0f });
	truckObj->SetPosition({ 0.0f, 0.0f, 10.0f });
	//groundObj->SetPosition({0.0f, -2.5f, 0.0f});

	//サウンド再生
	//audio->PlayWave("Resources/Alarm01.wav");
}

void GameScene::Update() {
	//キーが押されているときの処理
	if (input->TriggerKey(DIK_0)) {
		OutputDebugStringA("Hit 0\n");
	}

	//X座標、Y座標を指定して表示
	debugText.Print("Hello,DirectX!!", 200, 100, 1.0f);
	//X座標、Y座標、縮尺を指定して表示
	debugText.Print("Nihon Kogakuin", 200, 200, 2.0f);

	// 現在の座標を取得
	XMFLOAT3 playerPos = playerObj->GetPosition();
	XMFLOAT3 skydomePos = skydomeObj->GetPosition();
	XMFLOAT3 groundPos = groundObj->GetPosition();
	XMFLOAT3 cameraEye = Object3d::GetEye();
	XMFLOAT3 cameraTarget = Object3d::GetTarget();

	// オブジェクト移動
	if (input->PushKey(DIK_UP) || input->PushKey(DIK_DOWN) || input->PushKey(DIK_RIGHT) || input->PushKey(DIK_LEFT)) {
		// 移動後の座標を計算
		if (input->PushKey(DIK_UP)) {
			playerPos.z += zMove;
			skydomePos.z += zMove;
			cameraEye.z += zMove;
			cameraTarget.z += zMove;
		}
		else if (input->PushKey(DIK_DOWN)) {
			playerPos.z -= zMove;
			skydomePos.z -= zMove;
			cameraEye.z -= zMove;
			cameraTarget.z -= zMove;
		}

		if (input->PushKey(DIK_RIGHT)) {
			playerPos.x += zMove;
			skydomePos.x += zMove;
			cameraEye.x += zMove;
			cameraTarget.x += zMove;
		}
		else if (input->PushKey(DIK_LEFT)) {
			playerPos.x -= zMove;
			skydomePos.x -= zMove;
			cameraEye.x -= zMove;
			cameraTarget.x -= zMove;
		}

		// 座標の変更を反映
		playerObj->SetPosition(playerPos);
		playerJump1Obj->SetPosition(playerPos);
		playerJump2Obj->SetPosition(playerPos);
		skydomeObj->SetPosition(skydomePos);
		Object3d::SetEye(cameraEye);
		Object3d::SetTarget(cameraTarget);
	}

	// カメラ移動
	if (input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->PushKey(DIK_D) || input->PushKey(DIK_A)) {
		if (input->PushKey(DIK_W)) {
			Object3d::CameraMoveVector({ 0.0f,+1.0f,0.0f });
			//scroll += 10.0f;
		}
		else if (input->PushKey(DIK_S)) {
			Object3d::CameraMoveVector({ 0.0f,-1.0f,0.0f });
			//scroll -= 10.0f;
		}

		if (input->PushKey(DIK_D)) {
			Object3d::CameraMoveEyeVector({ +1.0f,0.0f,0.0f });
		}
		else if (input->PushKey(DIK_A)) {
			Object3d::CameraMoveEyeVector({ -1.0f,0.0f,0.0f });
		}
	}

	if (input->TriggerKey(DIK_SPACE) && !isJumpUp && !isJumpDown) {
		isJumpUp = true;
	}

	if (isJumpUp == true) {
		// 移動後の座標を計算
		playerPos.z += zMove;
		playerPos.y += yMove;
		skydomePos.z += zMove;
		cameraEye.z += zMove;
		cameraTarget.z += zMove;


		// 座標の変更を反映
		playerObj->SetPosition(playerPos);
		playerJump1Obj->SetPosition(playerPos);
		playerJump2Obj->SetPosition(playerPos);
		skydomeObj->SetPosition(skydomePos);
		Object3d::SetEye(cameraEye);
		Object3d::SetTarget(cameraTarget);

		if (playerPos.y >= 2.5f) {
			isJumpUp = false;
			isJumpDown = true;
		}
	}

	if (isJumpDown == true) {
		// 移動後の座標を計算
		playerPos.z += zMove;
		playerPos.y -= yMove;
		skydomePos.z += zMove;
		cameraEye.z += zMove;
		cameraTarget.z += zMove;

		// 座標の変更を反映
		playerObj->SetPosition(playerPos);
		playerJump1Obj->SetPosition(playerPos);
		playerJump2Obj->SetPosition(playerPos);
		skydomeObj->SetPosition(skydomePos);
		Object3d::SetEye(cameraEye);
		Object3d::SetTarget(cameraTarget);

		if (playerPos.y <= 0.0f) {
			isJumpDown = false;
			playerPos.y = 0;
			count++;
			if (isLeftLeg == true) {
				isLeftLeg = false;
				isRightLeg = true;
			}
			else if (isRightLeg == true) {
				isLeftLeg = true;
				isRightLeg = false;
			}
		}
	}

	if (count >= 3) {
		groundPos.z = playerPos.z + 35.0f;
		groundObj->SetPosition(groundPos);
		count = 0;
	}

	//XMFLOAT3 largeCarPos = largeCarObj->GetPosition();
	//largeCarPos.x += 1.0f;
	//largeCarObj->SetPosition(largeCarPos);

	playerObj->Update();
	playerJump1Obj->Update();
	playerJump2Obj->Update();
	skydomeObj->Update();
	groundObj->Update();
	largeCarObj->Update();
	miniCarObj->Update();
	truckObj->Update();
}

void GameScene::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();
	XMFLOAT3 playerPos = playerObj->GetPosition();
#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());
	// 背景スプライト描画

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon->ClearDepthBuffer();
#pragma endregion 背景スプライト描画
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Object3d::PreDraw(dxCommon->GetCommandList());
	// 3Dオブクジェクトの描画
	if (playerPos.y <= 0) {
		playerObj->Draw();
	}

	if (isJumpUp || isJumpDown) {
		if (isLeftLeg == true) {
			playerJump1Obj->Draw();
		}
		else if (isRightLeg == true) {
			playerJump2Obj->Draw();
		}
	}

	skydomeObj->Draw();
	groundObj->Draw();
	largeCarObj->Draw();
	miniCarObj->Draw();
	truckObj->Draw();
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