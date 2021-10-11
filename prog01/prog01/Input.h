#pragma once

#include <Windows.h>
#include <wrl.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

class Input
{
private:
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: //メンバ関数
	//初期化
	bool Initialize(HINSTANCE hInstance, HWND hwnd);
	//更新
	void Update();
	// キーの押下をチェック
	bool PushKey(BYTE keyNumber);
	// キーのトリガーをチェック
	bool TriggerKey(BYTE keyNumber);

private: //メンバ変数
	ComPtr<IDirectInput8> dinput;
	ComPtr<IDirectInputDevice8> devkeyboard;
	BYTE key[256] = {};
	BYTE keyPre[256] = {};
};