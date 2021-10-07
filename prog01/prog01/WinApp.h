#pragma once
#include <Windows.h>

/// <summary>
/// ウィンドウズアプリケーション
/// </summary>
class WinApp
{
public: // 静的メンバ変数
	// ウィンドウサイズ
	static const int WIN_WIDTH = 1280;	// 横幅
	static const int WIN_HEIGHT = 720;	// 縦幅
	static const wchar_t winClassName[];

public: // 静的メンバ関数
	/// <summary>
	/// ウィンドウプロシージャ
	/// </summary>
	/// <param name="hwnd">ウィンドウハンドル</param>
	/// <param name="msg">メッセージ番号</param>
	/// <param name="wparam">メッセージ情報1</param>
	/// <param name="lparam">メッセージ情報2</param>
	/// <returns>成否</returns>
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public: // メンバ関数
	/// <summary>
	/// ゲームウィンドウの作成
	/// </summary>
	void CreateGameWindow();

	/// <summary>
	/// ゲームウィンドウの破棄
	/// </summary>
	void DeleteGameWindow();

	/// <summary>
	/// メッセージの処理
	/// </summary>
	/// <returns>終了かどうか</returns>
	bool ProcessMessage();

	/// <summary>
	/// ウィンドウハンドルの取得
	/// </summary>
	/// <returns></returns>
	HWND GetHwnd() { return hwnd; }

	HINSTANCE GetInstance() { return w.hInstance; }

private: // メンバ変数
	// Window関連
	HWND hwnd = nullptr;	// ウィンドウハンドル
	WNDCLASSEX w{};			// ウィンドウクラス
};

