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
	safe_delete(title);
	safe_delete(noOnes0);
	safe_delete(noOnes1);
	safe_delete(noOnes2);
	safe_delete(noOnes3);
	safe_delete(noOnes4);
	safe_delete(noOnes5);
	safe_delete(noOnes6);
	safe_delete(noOnes7);
	safe_delete(noOnes8);
	safe_delete(noOnes9);
	safe_delete(noTens0);
	safe_delete(noTens1);
	safe_delete(noTens2);
	safe_delete(noTens3);
	safe_delete(noTens4);
	safe_delete(noTens5);
	safe_delete(noTens6);
	safe_delete(noTens7);
	safe_delete(noTens8);
	safe_delete(noTens9);
	safe_delete(uiBack);
	safe_delete(gameOver);
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input, Audio* audio) {
	// nullptrチェック
	assert(dxCommon);
	assert(input);
	assert(audio);

	this->dxCommon = dxCommon;
	this->input = input;
	this->titleAudio = audio;
	this->playAudio = audio;
	this->gameoverAudio = audio;
	this->carAudio = audio;
	this->dangerAudio = audio;
	this->deadAudio = audio;
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

	// テクスチャ読み込み
	if (!Sprite::LoadTexture(2, L"Resources/title.png")) {
		assert(0);
	}
	// 前景スプライト生成
	//警告(左上)
	title = Sprite::Create(2, { 0.0f,0.0f });
	title->SetSize({ 1280.0f,960.0f });


	// テクスチャ読み込み
	if (!Sprite::LoadTexture(3, L"Resources/No/No0.png")) {
		assert(0);
	}
	noOnes0 = Sprite::Create(3, { 0.0f,0.0f });
	noOnes0->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(4, L"Resources/No/No1.png")) {
		assert(0);
	}
	noOnes1 = Sprite::Create(4, { 0.0f,0.0f });
	noOnes1->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(5, L"Resources/No/No2.png")) {
		assert(0);
	}
	noOnes2 = Sprite::Create(5, { 0.0f,0.0f });
	noOnes2->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(6, L"Resources/No/No3.png")) {
		assert(0);
	}
	noOnes3 = Sprite::Create(6, { 0.0f,0.0f });
	noOnes3->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(7, L"Resources/No/No4.png")) {
		assert(0);
	}
	noOnes4 = Sprite::Create(7, { 0.0f,0.0f });
	noOnes4->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(8, L"Resources/No/No5.png")) {
		assert(0);
	}
	noOnes5 = Sprite::Create(8, { 0.0f,0.0f });
	noOnes5->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(9, L"Resources/No/No6.png")) {
		assert(0);
	}
	noOnes6 = Sprite::Create(9, { 0.0f,0.0f });
	noOnes6->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(10, L"Resources/No/No7.png")) {
		assert(0);
	}
	noOnes7 = Sprite::Create(10, { 0.0f,0.0f });
	noOnes7->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(11, L"Resources/No/No8.png")) {
		assert(0);
	}
	noOnes8 = Sprite::Create(11, { 0.0f,0.0f });
	noOnes8->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(12, L"Resources/No/No9.png")) {
		assert(0);
	}
	noOnes9 = Sprite::Create(12, { 0.0f,0.0f });
	noOnes9->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(3, L"Resources/No/No0.png")) {
		assert(0);
	}
	noTens0 = Sprite::Create(3, { 0.0f,0.0f });
	noTens0->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(4, L"Resources/No/No1.png")) {
		assert(0);
	}
	noTens1 = Sprite::Create(4, { 0.0f,0.0f });
	noTens1->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(5, L"Resources/No/No2.png")) {
		assert(0);
	}
	noTens2 = Sprite::Create(5, { 0.0f,0.0f });
	noTens2->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(6, L"Resources/No/No3.png")) {
		assert(0);
	}
	noTens3 = Sprite::Create(6, { 0.0f,0.0f });
	noTens3->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(7, L"Resources/No/No4.png")) {
		assert(0);
	}
	noTens4 = Sprite::Create(7, { 0.0f,0.0f });
	noTens4->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(8, L"Resources/No/No5.png")) {
		assert(0);
	}
	noTens5 = Sprite::Create(8, { 0.0f,0.0f });
	noTens5->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(9, L"Resources/No/No6.png")) {
		assert(0);
	}
	noTens6 = Sprite::Create(9, { 0.0f,0.0f });
	noTens6->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(10, L"Resources/No/No7.png")) {
		assert(0);
	}
	noTens7 = Sprite::Create(10, { 0.0f,0.0f });
	noTens7->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(11, L"Resources/No/No8.png")) {
		assert(0);
	}
	noTens8 = Sprite::Create(11, { 0.0f,0.0f });
	noTens8->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(12, L"Resources/No/No9.png")) {
		assert(0);
	}
	noTens9 = Sprite::Create(12, { 0.0f,0.0f });
	noTens9->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(3, L"Resources/No/No0.png")) {
		assert(0);
	}
	noHundreds0 = Sprite::Create(3, { 0.0f,0.0f });
	noHundreds0->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(4, L"Resources/No/No1.png")) {
		assert(0);
	}
	noHundreds1 = Sprite::Create(4, { 0.0f,0.0f });
	noHundreds1->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(5, L"Resources/No/No2.png")) {
		assert(0);
	}
	noHundreds2 = Sprite::Create(5, { 0.0f,0.0f });
	noHundreds2->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(6, L"Resources/No/No3.png")) {
		assert(0);
	}
	noHundreds3 = Sprite::Create(6, { 0.0f,0.0f });
	noHundreds3->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(7, L"Resources/No/No4.png")) {
		assert(0);
	}
	noHundreds4 = Sprite::Create(7, { 0.0f,0.0f });
	noHundreds4->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(8, L"Resources/No/No5.png")) {
		assert(0);
	}
	noHundreds5 = Sprite::Create(8, { 0.0f,0.0f });
	noHundreds5->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(9, L"Resources/No/No6.png")) {
		assert(0);
	}
	noHundreds6 = Sprite::Create(9, { 0.0f,0.0f });
	noHundreds6->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(10, L"Resources/No/No7.png")) {
		assert(0);
	}
	noHundreds7 = Sprite::Create(10, { 0.0f,0.0f });
	noHundreds7->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(11, L"Resources/No/No8.png")) {
		assert(0);
	}
	noHundreds8 = Sprite::Create(11, { 0.0f,0.0f });
	noHundreds8->SetSize({ 100.0f,100.0f });
	// テクスチャ読み込み
	if (!Sprite::LoadTexture(12, L"Resources/No/No9.png")) {
		assert(0);
	}
	noHundreds9 = Sprite::Create(12, { 0.0f,0.0f });
	noHundreds9->SetSize({ 100.0f,100.0f });



	// テクスチャ読み込み
	if (!Sprite::LoadTexture(13, L"Resources/uiBack.png")) {
		assert(0);
	}
	uiBack = Sprite::Create(13, { 0.0f,0.0f });
	uiBack->SetSize({ 1280.0f,960.0f });

	// テクスチャ読み込み
	if (!Sprite::LoadTexture(14, L"Resources/GameOver.png")) {
		assert(0);
	}
	gameOver = Sprite::Create(14, { 0.0f,0.0f });
	gameOver->SetSize({ 1280.0f,960.0f });

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

	skydomeObj->SetPosition({0.0f,0.0f,-1000.0f});

	groundObj->SetScale({ groundScale,groundScale, groundScale });

	for (int i = 0; i < 2; i++) {
		largeCarRObj[i]->SetScale({ largeCarScale,largeCarScale,largeCarScale });
		miniCarRObj[i]->SetScale({ miniCarScale,miniCarScale,miniCarScale });
		truckRObj[i]->SetScale({ truckScale,truckScale,truckScale });
		largeCarLObj[i]->SetScale({ largeCarScale,largeCarScale,largeCarScale });
		miniCarLObj[i]->SetScale({ miniCarScale,miniCarScale,miniCarScale });
		truckLObj[i]->SetScale({ truckScale,truckScale,truckScale });
	}

	largeCarRObj[0]->SetPosition({ 45.0f, 0.0f, -25.0f });
	miniCarRObj[0]->SetPosition({ 45.0f, 0.0f, -25.0f });
	truckRObj[0]->SetPosition({ 45.0f, 0.0f, -25.0f });

	largeCarLObj[0]->SetPosition({ -45.0f, 0.0f, -15.0f });
	miniCarLObj[0]->SetPosition({ -45.0f, 0.0f, -15.0f });
	truckLObj[0]->SetPosition({ -45.0f, 0.0f, -15.0f });

	largeCarRObj[1]->SetPosition({ 45.0f, 0.0f, 5.0f });
	miniCarRObj[1]->SetPosition({ 45.0f, 0.0f, 5.0f });
	truckRObj[1]->SetPosition({ 45.0f, 0.0f, 5.0f });

	largeCarLObj[1]->SetPosition({ -45.0f, 0.0f, 15.0f });
	miniCarLObj[1]->SetPosition({ -45.0f, 0.0f, 15.0f });
	truckLObj[1]->SetPosition({ -45.0f, 0.0f, 15.0f });
}

void GameScene::Update() {
	//キーが押されているときの処理
	if (input->TriggerKey(DIK_0)) {
		OutputDebugStringA("Hit 0\n");
	}

	//X座標、Y座標を指定して表示
	//debugText.Print("Hello,DirectX!!", 200, 100, 1.0f);
	//X座標、Y座標、縮尺を指定して表示
	//debugText.Print("Nihon Kogakuin", 200, 200, 2.0f);

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



	if (nowScene == 0) {
		if (input->TriggerKey(DIK_SPACE)) {
			isChange = 1;

			Object3d::SetEye(cameraEye);
			Object3d::SetTarget(cameraTarget);
		}

		if (isChange == true) {
			nowTime += 0.01;
			timeRate = min(nowTime / endTime, 1);

			cameraEye.x = 0 * (1.0f - (timeRate * timeRate) * (3 - (2 * timeRate))) + 15 * (timeRate * timeRate) * (3 - (2 * timeRate));
			cameraEye.y = 10 * (1.0f - (timeRate * timeRate) * (3 - (2 * timeRate))) + 20 * (timeRate * timeRate) * (3 - (2 * timeRate));
			cameraTarget.y = -3 * (1.0f - (timeRate * timeRate) * (3 - (2 * timeRate))) + -10 * (timeRate * timeRate) * (3 - (2 * timeRate));
			Object3d::SetEye(cameraEye);
			Object3d::SetTarget(cameraTarget);
		}


		if (soundCount == 0) {
			//サウンド再生
			titleAudio->PlayWave("Resources/Title.wav");
		}
		else if (soundCount > 5400) {
			soundCount = 0;
		}
		soundCount++;
		if (timeRate == 1) {
			warningRWaitTime = rand() % 30;
			warningLWaitTime = rand() % 29;
			isWarningRWait = true;
			isWarningLWait = true;
			nowScene = 1;
			soundCount = 0;
			isChange = false;
			nowTime = 0;
			timeRate = 0;
			titleAudio->Stop();
		}
	}
	else if (nowScene == 1) {

		if (soundCount == 0) {
			//サウンド再生
			playAudio->PlayWave("Resources/Play.wav");
		}
		else if (soundCount > 4800) {
			soundCount = -10;
		}
		soundCount++;

		if (input->TriggerKey(DIK_K)) {
			nowScene = 2;
			//当たり判定の中に入れるやつ
			playAudio->Stop();
			soundCount = 0;
			deadAudio->PlayWave("Resources/Dead.wav");

			dangerAudio->PlayWave("Resources/Danger.wav");
			carAudio->PlayWave("Resources/Car.wav");
		}
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
			cameraEye.z += zMove;
			cameraTarget.z += zMove;

			// 座標の変更を反映
			playerObj->SetPosition(playerPos);
			playerJumpLObj->SetPosition(playerPos);
			playerJumpRObj->SetPosition(playerPos);
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
			cameraEye.z += zMove;
			cameraTarget.z += zMove;

			// 座標の変更を反映
			playerObj->SetPosition(playerPos);
			playerJumpLObj->SetPosition(playerPos);
			playerJumpRObj->SetPosition(playerPos);
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
			scoreCountOne++;
			ScrollCarPos(activeCarNum);

			if (activeCarNum == 0) {
				activeCarNum = 1;
			}
			else {
				activeCarNum = 0;
				scrollCount++;
			}
		}

		if (scoreCountOne == 10) {
			scoreCountTen++;
			scoreCountOne = 0;
		}
		if (scoreCountTen == 10) {
			scoreCountHundred++;
			scoreCountTen = 0;
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

		XMFLOAT3 playerPosition = playerObj->GetPosition();

		for (int i = 0; i < _countof(largeCarRObj); i++) {
			if (isRunCarR == true) {
				XMFLOAT3 carRObj = largeCarRObj[i]->GetPosition();

				XMVECTOR position_sub = XMVectorSet
				(
					carRObj.x - playerPosition.x,
					carRObj.y - playerPosition.y,
					carRObj.z - playerPosition.z,
					0
				);

				position_sub = XMVector3Length(position_sub);

				float distance = position_sub.m128_f32[2];

				if (distance <= 1 * 3) {
					carRObj.x = 45.0f;
					largeCarRObj[i]->SetPosition(carRObj);
					nowScene = 2;
				}
			}
		}
		for (int i = 0; i < _countof(largeCarLObj); i++) {
			if (isRunCarL == true) {
				XMFLOAT3 carLObj = largeCarLObj[i]->GetPosition();

				XMVECTOR position_sub = XMVectorSet
				(
					carLObj.x - playerPosition.x,
					carLObj.y - playerPosition.y,
					carLObj.z - playerPosition.z,
					0
				);

				position_sub = XMVector3Length(position_sub);

				float distance = position_sub.m128_f32[2];

				if (distance <= 1 * 3) {
					nowScene = 2;
				}
			}
		}

		for (int i = 0; i < _countof(miniCarRObj); i++) {
			if (isRunCarR == true) {
				XMFLOAT3 carRObj = miniCarRObj[i]->GetPosition();

				XMVECTOR position_sub = XMVectorSet
				(
					carRObj.x - playerPosition.x,
					carRObj.y - playerPosition.y,
					carRObj.z - playerPosition.z,
					0
				);

				position_sub = XMVector3Length(position_sub);

				float distance = position_sub.m128_f32[2];

				if (distance <= 1 * 3) {
					carRObj.x = 45.0f;
					miniCarRObj[i]->SetPosition(carRObj);
					nowScene = 2;
				}
			}
		}
		for (int i = 0; i < _countof(miniCarLObj); i++) {
			if (isRunCarL == true) {
				XMFLOAT3 carLObj = miniCarLObj[i]->GetPosition();

				XMVECTOR position_sub = XMVectorSet
				(
					carLObj.x - playerPosition.x,
					carLObj.y - playerPosition.y,
					carLObj.z - playerPosition.z,
					0
				);

				position_sub = XMVector3Length(position_sub);

				float distance = position_sub.m128_f32[2];

				if (distance <= 1 * 2) {
					nowScene = 2;
				}
			}
		}

		for (int i = 0; i < _countof(truckRObj); i++) {
			if (isRunCarR == true) {
				XMFLOAT3 carRObj = truckRObj[i]->GetPosition();

				XMVECTOR position_sub = XMVectorSet
				(
					carRObj.x - playerPosition.x,
					carRObj.y - playerPosition.y,
					carRObj.z - playerPosition.z,
					0
				);

				position_sub = XMVector3Length(position_sub);

				float distance = position_sub.m128_f32[0];

				if (distance <= 1 * 4) {
					carRObj.x = 45.0f;
					truckRObj[i]->SetPosition(carRObj);
					nowScene = 2;
				}
			}
		}
		for (int i = 0; i < _countof(truckLObj); i++) {
			if (isRunCarL == true) {
				XMFLOAT3 carRObj = truckLObj[i]->GetPosition();

				XMVECTOR position_sub = XMVectorSet
				(
					carRObj.x - playerPosition.x,
					carRObj.y - playerPosition.y,
					carRObj.z - playerPosition.z,
					0
				);

				position_sub = XMVector3Length(position_sub);

				float distance = position_sub.m128_f32[0];

				if (distance <= 1 * 4) {
					nowScene = 2;
				}
			}
		}


		if (activeLargeCarRPos.x <= -45.0f || activeMiniCarRPos.x <= -45.0f || activeTruckRPos.x <= -45.0f) {
			blinkRCount = 0;
			isRunCarR = false;

			if (carTypeR == 0) {
				activeLargeCarRPos.x = 45.0f;
				largeCarRObj[activeCarNum]->SetPosition(activeLargeCarRPos);
			}
			if (carTypeR == 1) {
				activeMiniCarRPos.x = 45.0f;
				miniCarRObj[activeCarNum]->SetPosition(activeMiniCarRPos);
			}
			if (carTypeR == 2) {
				activeTruckRPos.x = 45.0f;
				truckRObj[activeCarNum]->SetPosition(activeTruckRPos);
			}

			srand(time(NULL));
			carTypeR = rand() % 3;
			warningRWaitTime = rand() % 30;
			isWarningRWait = true;
		}

		if (isWarningRWait == true) {
			warningRWaitTime--;
			if (warningRWaitTime <= 0) {
				isRightWarning = true;
				isWarningRWait = false;
			}
		}

		if (activeLargeCarLPos.x >= +45.0f || activeMiniCarLPos.x >= +45.0f || activeTruckLPos.x >= +45.0f) {
			blinkLCount = 0;
			isRunCarL = false;

			if (carTypeL == 0) {
				activeLargeCarLPos.x = -45.0f;
				largeCarLObj[activeCarNum]->SetPosition(activeLargeCarLPos);
			}
			if (carTypeL == 1) {
				activeMiniCarLPos.x = -45.0f;
				miniCarLObj[activeCarNum]->SetPosition(activeMiniCarLPos);
			}
			if (carTypeL == 2) {
				activeTruckLPos.x = -45.0f;
				truckLObj[activeCarNum]->SetPosition(activeTruckLPos);
			}

			srand(time(NULL));
			carTypeL = rand() % 3;
			warningLWaitTime = rand() % 29;
			isWarningLWait = true;
		}

		if (isWarningLWait == true) {
			warningLWaitTime--;
			if (warningLWaitTime <= 0) {
				isLeftWarning = true;
				isWarningLWait = false;
			}
		}
	}
	else if (nowScene == 2) {

		if (soundCount == 0) {
			//サウンド再生
			gameoverAudio->PlayWave("Resources/GameOver.wav");
		}
		else if (soundCount > 4800) {
			soundCount = 0;
		}
		soundCount++;

		cameraEye.y = -3.0f;
		cameraEye.z = -800.0f;
		cameraTarget.y = 750.0f;
		Object3d::SetEye(cameraEye);
		Object3d::SetTarget(cameraTarget);
		if (input->TriggerKey(DIK_SPACE)) {
			nowScene = 3;
		}
	}
	else if (nowScene == 3) {
		gameoverAudio->Stop();
		count = 0;
		soundCount = 0;
		scoreCountOne = 0;
		scoreCountTen = 0;
		scoreCountHundred = 0;
		scrollCount = 1;
		activeCarNum = 0;
		blinkingR = 0;
		blinkingL = 0;
		blinkRCount = 0;
		blinkLCount = 0;
		isWarningRWait = false;
		isWarningLWait = false;
		isRightWarning = false;
		isLeftWarning = false;
		isJumpUp = false;
		isJumpDown = false;
		isRightLeg = true;
		isLeftLeg = false;
		isChange = false;
		cameraEye.x = 0;
		cameraEye.y = 10.0f;
		cameraEye.z = -40.0f;
		cameraTarget.y = -3.0f;
		cameraTarget.z = -25.0f;
		Object3d::SetEye(cameraEye);
		Object3d::SetTarget(cameraTarget);
		nowScene = 0;
		resetPos();
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

void GameScene::resetPos() {
	playerObj->SetPosition({ 0.0f, 0.0f, -35.0f });
	playerJumpRObj->SetPosition(playerObj->GetPosition());
	playerJumpLObj->SetPosition(playerObj->GetPosition());

	groundObj->SetPosition({ 0.0f, 0.0f, 0.0f });

	largeCarRObj[0]->SetPosition({ 45.0f, 0.0f, -25.0f });
	miniCarRObj[0]->SetPosition({ 45.0f, 0.0f, -25.0f });
	truckRObj[0]->SetPosition({ 45.0f, 0.0f, -25.0f });

	largeCarLObj[0]->SetPosition({ -45.0f, 0.0f, -15.0f });
	miniCarLObj[0]->SetPosition({ -45.0f, 0.0f, -15.0f });
	truckLObj[0]->SetPosition({ -45.0f, 0.0f, -15.0f });

	largeCarRObj[1]->SetPosition({ 45.0f, 0.0f, 5.0f });
	miniCarRObj[1]->SetPosition({ 45.0f, 0.0f, 5.0f });
	truckRObj[1]->SetPosition({ 45.0f, 0.0f, 5.0f });

	largeCarLObj[1]->SetPosition({ -45.0f, 0.0f, 15.0f });
	miniCarLObj[1]->SetPosition({ -45.0f, 0.0f, 15.0f });
	truckLObj[1]->SetPosition({ -45.0f, 0.0f, 15.0f });
}

void GameScene::ScrollCarPos(int activeCarNum) {
	XMFLOAT3 activeLargeCarRPos = largeCarRObj[activeCarNum]->GetPosition();
	XMFLOAT3 activeMiniCarRPos = miniCarRObj[activeCarNum]->GetPosition();
	XMFLOAT3 activeTruckRPos = truckRObj[activeCarNum]->GetPosition();

	XMFLOAT3 activeLargeCarLPos = largeCarLObj[activeCarNum]->GetPosition();
	XMFLOAT3 activeMiniCarLPos = miniCarLObj[activeCarNum]->GetPosition();
	XMFLOAT3 activeTruckLPos = truckLObj[activeCarNum]->GetPosition();

	activeLargeCarRPos.x = 45.0f;
	activeMiniCarRPos.x = 45.0f;
	activeTruckRPos.x = 45.0f;
	activeLargeCarLPos.x = -45.0f;
	activeMiniCarLPos.x = -45.0f;
	activeTruckLPos.x = -45.0f;

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

	skydomeObj->Draw();
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
	if (nowScene == 0) {
		if (timeRate == 0) {
			title->Draw();
		}
	}
	if (nowScene == 1 || nowScene == 2) {
		if (nowScene == 1) {
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
			noOnes0->SetPosition({ 270.0f,810.0f });
			noOnes1->SetPosition({ 270.0f,810.0f });
			noOnes2->SetPosition({ 270.0f,810.0f });
			noOnes3->SetPosition({ 270.0f,810.0f });
			noOnes4->SetPosition({ 270.0f,810.0f });
			noOnes5->SetPosition({ 270.0f,810.0f });
			noOnes6->SetPosition({ 270.0f,810.0f });
			noOnes7->SetPosition({ 270.0f,810.0f });
			noOnes8->SetPosition({ 270.0f,810.0f });
			noOnes9->SetPosition({ 270.0f,810.0f });
			noTens0->SetPosition({ 210.0f,810.0f });
			noTens1->SetPosition({ 210.0f,810.0f });
			noTens2->SetPosition({ 210.0f,810.0f });
			noTens3->SetPosition({ 210.0f,810.0f });
			noTens4->SetPosition({ 210.0f,810.0f });
			noTens5->SetPosition({ 210.0f,810.0f });
			noTens6->SetPosition({ 210.0f,810.0f });
			noTens7->SetPosition({ 210.0f,810.0f });
			noTens8->SetPosition({ 210.0f,810.0f });
			noTens9->SetPosition({ 210.0f,810.0f });
			noHundreds0->SetPosition({ 150.0f,810.0f });
			noHundreds1->SetPosition({ 150.0f,810.0f });
			noHundreds2->SetPosition({ 150.0f,810.0f });
			noHundreds3->SetPosition({ 150.0f,810.0f });
			noHundreds4->SetPosition({ 150.0f,810.0f });
			noHundreds5->SetPosition({ 150.0f,810.0f });
			noHundreds6->SetPosition({ 150.0f,810.0f });
			noHundreds7->SetPosition({ 150.0f,810.0f });
			noHundreds8->SetPosition({ 150.0f,810.0f });
			noHundreds9->SetPosition({ 150.0f,810.0f });
			uiBack->Draw();
		}
		else if (nowScene == 2) {
			gameOver->Draw();
			noOnes0->SetPosition({ 794.0f,470.0f });
			noOnes1->SetPosition({ 794.0f,470.0f });
			noOnes2->SetPosition({ 794.0f,470.0f });
			noOnes3->SetPosition({ 794.0f,470.0f });
			noOnes4->SetPosition({ 794.0f,470.0f });
			noOnes5->SetPosition({ 794.0f,470.0f });
			noOnes6->SetPosition({ 794.0f,470.0f });
			noOnes7->SetPosition({ 794.0f,470.0f });
			noOnes8->SetPosition({ 794.0f,470.0f });
			noOnes9->SetPosition({ 794.0f,470.0f });
			noTens0->SetPosition({ 734.0f,470.0f });
			noTens1->SetPosition({ 734.0f,470.0f });
			noTens2->SetPosition({ 734.0f,470.0f });
			noTens3->SetPosition({ 734.0f,470.0f });
			noTens4->SetPosition({ 734.0f,470.0f });
			noTens5->SetPosition({ 734.0f,470.0f });
			noTens6->SetPosition({ 734.0f,470.0f });
			noTens7->SetPosition({ 734.0f,470.0f });
			noTens8->SetPosition({ 734.0f,470.0f });
			noTens9->SetPosition({ 734.0f,470.0f });
			noHundreds0->SetPosition({ 674.0f,470.0f });
			noHundreds1->SetPosition({ 674.0f,470.0f });
			noHundreds2->SetPosition({ 674.0f,470.0f });
			noHundreds3->SetPosition({ 674.0f,470.0f });
			noHundreds4->SetPosition({ 674.0f,470.0f });
			noHundreds5->SetPosition({ 674.0f,470.0f });
			noHundreds6->SetPosition({ 674.0f,470.0f });
			noHundreds7->SetPosition({ 674.0f,470.0f });
			noHundreds8->SetPosition({ 674.0f,470.0f });
			noHundreds9->SetPosition({ 674.0f,470.0f });
		}
		//1の位
		if (scoreCountOne == 0) {
			noOnes0->Draw();
		}
		else if (scoreCountOne == 1) {
			noOnes1->Draw();
		}
		else if (scoreCountOne == 2) {
			noOnes2->Draw();
		}
		else if (scoreCountOne == 3) {
			noOnes3->Draw();
		}
		else if (scoreCountOne == 4) {
			noOnes4->Draw();
		}
		else if (scoreCountOne == 5) {
			noOnes5->Draw();
		}
		else if (scoreCountOne == 6) {
			noOnes6->Draw();
		}
		else if (scoreCountOne == 7) {
			noOnes7->Draw();
		}
		else if (scoreCountOne == 8) {
			noOnes8->Draw();
		}
		else if (scoreCountOne == 9) {
			noOnes9->Draw();
		}
		//10の位
		if (scoreCountTen > 0 || scoreCountHundred > 0) {
			if (scoreCountTen == 0) {
				noTens0->Draw();
			}
			else if (scoreCountTen == 1) {
				noTens1->Draw();
			}
			else if (scoreCountTen == 2) {
				noTens2->Draw();
			}
			else if (scoreCountTen == 3) {
				noTens3->Draw();
			}
			else if (scoreCountTen == 4) {
				noTens4->Draw();
			}
			else if (scoreCountTen == 5) {
				noTens5->Draw();
			}
			else if (scoreCountTen == 6) {
				noTens6->Draw();
			}
			else if (scoreCountTen == 7) {
				noTens7->Draw();
			}
			else if (scoreCountTen == 8) {
				noTens8->Draw();
			}
			else if (scoreCountTen == 9) {
				noTens9->Draw();
			}
		}
		//100の位
		if (scoreCountHundred > 0) {
			if (scoreCountHundred == 0) {
				noHundreds0->Draw();
			}
			else if (scoreCountHundred == 1) {
				noHundreds1->Draw();
			}
			else if (scoreCountHundred == 2) {
				noHundreds2->Draw();
			}
			else if (scoreCountHundred == 3) {
				noHundreds3->Draw();
			}
			else if (scoreCountHundred == 4) {
				noHundreds4->Draw();
			}
			else if (scoreCountHundred == 5) {
				noHundreds5->Draw();
			}
			else if (scoreCountHundred == 6) {
				noHundreds6->Draw();
			}
			else if (scoreCountHundred == 7) {
				noHundreds7->Draw();
			}
			else if (scoreCountHundred == 8) {
				noHundreds8->Draw();
			}
			else if (scoreCountHundred == 9) {
				noHundreds9->Draw();
			}
		}
	}
	// デバッグテキストの描画
	debugText.DrawAll(dxCommon->GetCommandList());
	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion 前景スプライト描画
}