#include "WinApp.h"
#include "DirectXCommon.h"
#include "Audio.h"
#include "GameScene.h"

//# Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	//ポインタ置き場
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	Input* input = nullptr;
	Audio* audio = nullptr;
	GameScene* gameScene = nullptr;

	// ゲームウィンドウの作成
	win = new WinApp();
	win->CreateGameWindow();

	//DirectX初期化処理
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(win);

#pragma region 汎用機能初期化

	//入力の初期化
	input = new Input();
	input->Initialize(win->GetInstance(), win->GetHwnd());

	// オーディオの初期化
	audio = new Audio();
	if (!audio->Initialize())
	{
		assert(0);
		return 1;
	}
	// スプライト静的初期化
	if (!Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_height))
	{
		assert(0);
		return 1;
	}

	// 3Dオブジェクト静的初期化
	if (!Object3d::StaticInitialize(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_height))
	{
		assert(0);
		return 1;
	}

#pragma endregion 汎用機能初期化

	// ゲームシーンの初期化
	gameScene = new GameScene();
	gameScene->Initialize(dxCommon, input, audio);

	while (true)  // ゲームループ
	{
		// メッセージ処理
		if (win->ProcessMessage())
		{
			break;
		}

#pragma region DirectX毎フレーム処理

		// 入力関連の毎フレーム処理
		input->Update();
		// ゲームシーンの毎フレーム処理
		gameScene->Update();

#pragma endregion DirectX毎フレーム処理

#pragma region グラフィックスコマンド
		// 描画開始
		dxCommon->PreDraw();

		// ゲームシーンの描画
		gameScene->Draw();

		// 描画終了
		dxCommon->PostDraw();

#pragma endregion グラフィックスコマンド
	}

	//解放
	// 各種解放
	safe_delete(gameScene);
	safe_delete(audio);
	safe_delete(input);
	safe_delete(dxCommon);

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();
	safe_delete(win);

	return 0;
}