#include "GameScene.h"
#include <cassert>

GameScene::GameScene() {

}

GameScene::~GameScene() {

	safe_delete(playerModel);
	safe_delete(playerJumpLModel);
	safe_delete(playerJumpRModel);
	safe_delete(skydomeModel);
	safe_delete(groundModel);
	safe_delete(largeCarLModel);
	safe_delete(miniCarLModel);
	safe_delete(truckLModel);
	safe_delete(playerObj);
	safe_delete(playerJumpLObj);
	safe_delete(playerJumpRObj);
	safe_delete(skydomeObj);
	safe_delete(groundObj);
	for (int i = 0; i < 2; i++) {
		safe_delete(largeCarLObj[i]);
		safe_delete(miniCarLObj[i]);
		safe_delete(truckLObj[i]);
		safe_delete(largeCarRObj[i]);
		safe_delete(miniCarRObj[i]);
		safe_delete(truckRObj[i]);
	}
	safe_delete(warningMarkLT);
	safe_delete(warningMarkLM);
	safe_delete(warningMarkLB);
	safe_delete(warningMarkRT);
	safe_delete(warningMarkRM);
	safe_delete(warningMarkRB);
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
	if (!Sprite::LoadTexture(1, L"Resources/warningMark.png")) {
		assert(0);
	}
	// 前景スプライト生成
	//警告(左上)
	warningMarkLT = Sprite::Create(1, { 100.0f,100.0f });
	warningMarkLT->SetSize({ 100.0f,100.0f });
	//(左中)
	warningMarkLM = Sprite::Create(1, { 100.0f,400.0f });
	warningMarkLM->SetSize({ 100.0f,100.0f });
	//(左下)
	warningMarkLB = Sprite::Create(1, { 100.0f,700.0f });
	warningMarkLB->SetSize({ 100.0f,100.0f });

	//(右上)
	warningMarkRT = Sprite::Create(1, { 1024.0f,100.0f });
	warningMarkRT->SetSize({ 100.0f,100.0f });
	//(右中)
	warningMarkRM = Sprite::Create(1, { 1024.0f,400.0f });
	warningMarkRM->SetSize({ 100.0f,100.0f });
	//(右下)
	warningMarkRB = Sprite::Create(1, { 1024.0f,700.0f });
	warningMarkRB->SetSize({ 100.0f,100.0f });

	isRightWarning = false;
	isLeftWarning = false;
	blinkingR = 0;

	//.objの名前を指定してモデルを読み込む
	playerModel = playerModel->CreateFromObject("player");
	playerJumpRModel = playerJumpRModel->CreateFromObject("playerJumpR");
	playerJumpLModel = playerJumpLModel->CreateFromObject("playerJumpL");
	skydomeModel = skydomeModel->CreateFromObject("skydome");
	groundModel = groundModel->CreateFromObject("ground");
	largeCarRModel = largeCarRModel->CreateFromObject("largeCarR");
	miniCarRModel = miniCarRModel->CreateFromObject("miniCarR");
	truckRModel = truckRModel->CreateFromObject("truckR");
	largeCarLModel = largeCarLModel->CreateFromObject("largeCarL");
	miniCarLModel = miniCarLModel->CreateFromObject("miniCarL");
	truckLModel = truckLModel->CreateFromObject("truckL");

	// 3Dオブジェクト生成
	playerObj = Object3d::Create();
	playerJumpRObj = Object3d::Create();
	playerJumpLObj = Object3d::Create();
	skydomeObj = Object3d::Create();
	groundObj = Object3d::Create();
	for (int i = 0; i < 2; i++) {
		largeCarLObj[i] = Object3d::Create();
		miniCarLObj[i] = Object3d::Create();
		truckLObj[i] = Object3d::Create();
		largeCarRObj[i] = Object3d::Create();
		miniCarRObj[i] = Object3d::Create();
		truckRObj[i] = Object3d::Create();
	}
	// 3Dオブジェクトにモデルを割り当てる
	playerObj->SetModel(playerModel);
	playerJumpRObj->SetModel(playerJumpRModel);
	playerJumpLObj->SetModel(playerJumpLModel);
	skydomeObj->SetModel(skydomeModel);
	groundObj->SetModel(groundModel);
	for (int i = 0; i < 2; i++) {
		largeCarRObj[i]->SetModel(largeCarRModel);
		miniCarRObj[i]->SetModel(miniCarRModel);
		truckRObj[i]->SetModel(truckRModel);
		largeCarLObj[i]->SetModel(largeCarLModel);
		miniCarLObj[i]->SetModel(miniCarLModel);
		truckLObj[i]->SetModel(truckLModel);
	}

	playerObj->SetPosition({ 0.0f, 0.0f, -35.0f });
	playerObj->SetScale({ playerScale,playerScale,playerScale });
	playerJumpRObj->SetPosition(playerObj->GetPosition());
	playerJumpRObj->SetScale(playerObj->GetScale());
	playerJumpLObj->SetPosition(playerObj->GetPosition());
	playerJumpLObj->SetScale(playerObj->GetScale());

	groundObj->SetScale({ groundScale,groundScale, groundScale });

	for (int i = 0; i < 2; i++) {
		largeCarRObj[i]->SetScale({ largeCarScale,largeCarScale,largeCarScale });
		miniCarRObj[i]->SetScale({ miniCarScale,miniCarScale,miniCarScale });
		truckRObj[i]->SetScale({ truckScale,truckScale,truckScale });
		largeCarLObj[i]->SetScale({ largeCarScale,largeCarScale,largeCarScale });
		miniCarLObj[i]->SetScale({ miniCarScale,miniCarScale,miniCarScale });
		truckLObj[i]->SetScale({ truckScale,truckScale,truckScale });
	}

	largeCarRObj[0]->SetPosition({ 145.0f, 0.0f, -25.0f });
	miniCarRObj[0]->SetPosition({ 145.0f, 0.0f, -25.0f });
	truckRObj[0]->SetPosition({ 145.0f, 0.0f, -25.0f });

	largeCarLObj[0]->SetPosition({ -145.0f, 0.0f, -15.0f });
	miniCarLObj[0]->SetPosition({ -145.0f, 0.0f, -15.0f });
	truckLObj[0]->SetPosition({ -145.0f, 0.0f, -15.0f });

	largeCarRObj[1]->SetPosition({ 145.0f, 0.0f, 5.0f });
	miniCarRObj[1]->SetPosition({ 145.0f, 0.0f, 5.0f });
	truckRObj[1]->SetPosition({ 145.0f, 0.0f, 5.0f });

	largeCarLObj[1]->SetPosition({ -145.0f, 0.0f, 15.0f });
	miniCarLObj[1]->SetPosition({ -145.0f, 0.0f, 15.0f });
	truckLObj[1]->SetPosition({ -145.0f, 0.0f, 15.0f });
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

	XMFLOAT3 activeLargeCarRPos = largeCarRObj[activeCarNum]->GetPosition();
	XMFLOAT3 activeMiniCarRPos = miniCarRObj[activeCarNum]->GetPosition();
	XMFLOAT3 activeTruckRPos = truckRObj[activeCarNum]->GetPosition();
	XMFLOAT3 activeLargeCarLPos = largeCarLObj[activeCarNum]->GetPosition();
	XMFLOAT3 activeMiniCarLPos = miniCarLObj[activeCarNum]->GetPosition();
	XMFLOAT3 activeTruckLPos = truckLObj[activeCarNum]->GetPosition();

	//largeCarPos.x += 1.0f;
	//largeCarObj->SetPosition(largeCarPos);

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
		playerJumpLObj->SetPosition(playerPos);
		playerJumpRObj->SetPosition(playerPos);
		skydomeObj->SetPosition(skydomePos);
		Object3d::SetEye(cameraEye);
		Object3d::SetTarget(cameraTarget);
	}

	// カメラ移動
	if (input->PushKey(DIK_W) || input->PushKey(DIK_S) || input->PushKey(DIK_D) || input->PushKey(DIK_A)) {
		if (input->PushKey(DIK_W)) {
			Object3d::CameraMoveVector({ 0.0f,+1.0f,0.0f });
		}
		else if (input->PushKey(DIK_S)) {
			Object3d::CameraMoveVector({ 0.0f,-1.0f,0.0f });
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
		playerJumpLObj->SetPosition(playerPos);
		playerJumpRObj->SetPosition(playerPos);
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
		playerJumpLObj->SetPosition(playerPos);
		playerJumpRObj->SetPosition(playerPos);
		skydomeObj->SetPosition(skydomePos);
		Object3d::SetEye(cameraEye);
		Object3d::SetTarget(cameraTarget);

		if (playerPos.y <= 0.0f) {
			isJumpDown = false;
			playerPos.y = 0;
			count++;
			if (isRightLeg == true) {
				isLeftLeg = true;
				isRightLeg = false;
			}
			else if (isLeftLeg == true) {
				isLeftLeg = false;
				isRightLeg = true;
			}
		}
	}

	if (count >= 3) {
		groundPos.z = playerPos.z + scrollGround;
		groundObj->SetPosition(groundPos);
		count = 0;
		ScrollCarPos(activeCarNum);
		if (activeCarNum == 0) {
			activeCarNum = 1;
		}
		else {
			activeCarNum = 0;
			scrollCount++;
		}
	}

	//警告
	if (input->TriggerKey(DIK_1)) {
		isRightWarning = true;
		isLeftWarning = true;
	}
	else if (input->TriggerKey(DIK_2)) {
		isRightWarning = true;
		isLeftWarning = false;
	}
	else if (input->TriggerKey(DIK_3)) {
		isRightWarning = false;
		isLeftWarning = true;
	}
	else if (input->TriggerKey(DIK_4)) {
		isRightWarning = false;
		isLeftWarning = false;
	}

	//警告点滅

	if (isRightWarning == true) {
		blinkingR++;
	}
	else if (isRightWarning == false) {
		blinkingR = 0;
	}
	if (blinkingR >= 20) {
		blinkingR = 0;
		blinkRCount++;
	}
	if (blinkRCount >= 5) {
		isRightWarning = false;
		isRunCarR = true;
	}

	if (isLeftWarning == true) {
		blinkingL++;
	}
	else if (isLeftWarning == false) {
		blinkingL = 0;
	}
	if (blinkingL >= 20) {
		blinkingL = 0;
		blinkLCount++;
	}
	if (blinkLCount >= 5) {
		isLeftWarning = false;
		isRunCarL = true;
	}

	if (isRunCarR == true) {
		if (carTypeR == 0) {
			activeLargeCarRPos.x -= carMove;
			largeCarRObj[activeCarNum]->SetPosition(activeLargeCarRPos);
		}
		else if (carTypeR == 1) {
			activeMiniCarRPos.x -= carMove;
			miniCarRObj[activeCarNum]->SetPosition(activeMiniCarRPos);
		}
		else if (carTypeR == 2) {
			activeTruckRPos.x -= carMove;
			truckRObj[activeCarNum]->SetPosition(activeTruckRPos);
		}
	}

	if (isRunCarL == true) {
		if (carTypeL == 0) {
			activeLargeCarLPos.x += carMove;
			largeCarLObj[activeCarNum]->SetPosition(activeLargeCarLPos);
		}
		else if (carTypeL == 1) {
			activeMiniCarLPos.x += carMove;
			miniCarLObj[activeCarNum]->SetPosition(activeMiniCarLPos);
		}
		else if (carTypeL == 2) {
			activeTruckLPos.x += carMove;
			truckLObj[activeCarNum]->SetPosition(activeTruckLPos);
		}
	}

	if (activeLargeCarRPos.x <= -150.0f || activeMiniCarRPos.x <= -150.0f || activeTruckRPos.x <= -150.0f) {
		blinkRCount = 0;
		isRunCarR = false;

		if (carTypeR == 0) {
			activeLargeCarRPos.x = 150.0f;
			largeCarRObj[activeCarNum]->SetPosition(activeLargeCarRPos);
		}
		if (carTypeR == 1) {
			activeMiniCarRPos.x = 150.0f;
			miniCarRObj[activeCarNum]->SetPosition(activeMiniCarRPos);
		}
		if (carTypeR == 2) {
			activeTruckRPos.x = 150.0f;
			truckRObj[activeCarNum]->SetPosition(activeTruckRPos);
		}

		srand(time(NULL));
		carTypeR = rand() % 3;
		warningRWaitTime = rand() % 50;
		isWarningRWait = true;
	}

	if (isWarningRWait == true) {
		warningRWaitTime--;
		if (warningRWaitTime <= 0) {
			isRightWarning = true;
			isWarningRWait = false;
		}
	}

	if (activeLargeCarLPos.x >= +150.0f || activeMiniCarLPos.x >= +150.0f || activeTruckLPos.x >= +150.0f) {
		blinkLCount = 0;
		isRunCarL = false;

		if (carTypeL == 0) {
			activeLargeCarLPos.x = -150.0f;
			largeCarLObj[activeCarNum]->SetPosition(activeLargeCarLPos);
		}
		if (carTypeL == 1) {
			activeMiniCarLPos.x = -150.0f;
			miniCarLObj[activeCarNum]->SetPosition(activeMiniCarLPos);
		}
		if (carTypeL == 2) {
			activeTruckLPos.x = -150.0f;
			truckLObj[activeCarNum]->SetPosition(activeTruckLPos);
		}

		srand(time(NULL));
		carTypeL = rand() % 3;
		warningLWaitTime = rand() % 50;
		isWarningLWait = true;
	}

	if (isWarningLWait == true) {
		warningLWaitTime--;
		if (warningLWaitTime <= 0) {
			isLeftWarning = true;
			isWarningLWait = false;
		}
	}

	playerObj->Update();
	playerJumpLObj->Update();
	playerJumpRObj->Update();
	skydomeObj->Update();
	groundObj->Update();

	for (int i = 0; i < 2; i++) {
		largeCarLObj[i]->Update();
		miniCarLObj[i]->Update();
		truckLObj[i]->Update();
		largeCarRObj[i]->Update();
		miniCarRObj[i]->Update();
		truckRObj[i]->Update();
	}
}

void GameScene::ScrollCarPos(int activeCarNum) {
	XMFLOAT3 activeLargeCarRPos = largeCarRObj[activeCarNum]->GetPosition();
	XMFLOAT3 activeMiniCarRPos = miniCarRObj[activeCarNum]->GetPosition();
	XMFLOAT3 activeTruckRPos = truckRObj[activeCarNum]->GetPosition();

	XMFLOAT3 activeLargeCarLPos = largeCarLObj[activeCarNum]->GetPosition();
	XMFLOAT3 activeMiniCarLPos = miniCarLObj[activeCarNum]->GetPosition();
	XMFLOAT3 activeTruckLPos = truckLObj[activeCarNum]->GetPosition();

	activeLargeCarRPos.x = 145.0f;
	activeMiniCarRPos.x = 145.0f;
	activeTruckRPos.x = 145.0f;
	activeLargeCarLPos.x = -145.0f;
	activeMiniCarLPos.x = -145.0f;
	activeTruckLPos.x = -145.0f;

	if (activeCarNum == 0) {
		activeLargeCarRPos.z = -25.0f + (scrollCar * scrollCount);
		activeMiniCarRPos.z = -25.0f + (scrollCar * scrollCount);
		activeTruckRPos.z = -25.0f + (scrollCar * scrollCount);
		activeLargeCarLPos.z = -15.0f + (scrollCar * scrollCount);
		activeMiniCarLPos.z = -15.0f + (scrollCar * scrollCount);
		activeTruckLPos.z = -15.0f + (scrollCar * scrollCount);
	}
	else {
		activeLargeCarRPos.z = 5.0f + (scrollCar * scrollCount);
		activeMiniCarRPos.z = 5.0f + (scrollCar * scrollCount);
		activeTruckRPos.z = 5.0f + (scrollCar * scrollCount);
		activeLargeCarLPos.z = 15.0f + (scrollCar * scrollCount);
		activeMiniCarLPos.z = 15.0f + (scrollCar * scrollCount);
		activeTruckLPos.z = 15.0f + (scrollCar * scrollCount);
	}

	largeCarRObj[activeCarNum]->SetPosition(activeLargeCarRPos);
	miniCarRObj[activeCarNum]->SetPosition(activeMiniCarRPos);
	truckRObj[activeCarNum]->SetPosition(activeTruckRPos);

	largeCarLObj[activeCarNum]->SetPosition(activeLargeCarLPos);
	miniCarLObj[activeCarNum]->SetPosition(activeMiniCarLPos);
	truckLObj[activeCarNum]->SetPosition(activeTruckLPos);
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
		if (isRightLeg == true) {
			playerJumpRObj->Draw();
		}
		else if (isLeftLeg == true) {
			playerJumpLObj->Draw();
		}
	}

	//skydomeObj->Draw();
	groundObj->Draw();

	for (int i = 0; i < 2; i++) {
		largeCarLObj[i]->Draw();
		miniCarLObj[i]->Draw();
		truckLObj[i]->Draw();
		largeCarRObj[i]->Draw();
		miniCarRObj[i]->Draw();
		truckRObj[i]->Draw();
	}
	// 3Dオブジェクト描画後処理
	Object3d::PostDraw();
#pragma endregion 3Dオブジェクト描画
#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(dxCommon->GetCommandList());

	// 前景スプライトの描画
	if (isLeftWarning == true && blinkingL <= 10) {
		warningMarkLT->Draw();
		warningMarkLM->Draw();
		warningMarkLB->Draw();
	}
	if (isRightWarning == true && blinkingR <= 10) {
		warningMarkRT->Draw();
		warningMarkRM->Draw();
		warningMarkRB->Draw();
	}
	// デバッグテキストの描画
	debugText.DrawAll(dxCommon->GetCommandList());
	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion 前景スプライト描画
}