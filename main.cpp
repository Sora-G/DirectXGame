#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>


//ウィンドウプロージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//メッセージに応じてゲームの固有処理を行う
	switch (msg)
	{
		//ウィンドウ破棄された
	case WM_DESTROY:
		//OSに対してアプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}


//出力ウィンドウに文字を出す
void Log(const std::string& message)
{
	OutputDebugStringA(message.c_str());
}


//string->wstringに変換する関数
std::wstring ConvertString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}


//wstring->stringに変換する関数
std::string ConvertString(const std::wstring& str) {
	if (str.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}



//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) 
{
	WNDCLASS wc{};

	//ウィンドウプロージャ
	wc.lpfnWndProc = WindowProc;

	//ウィンドウクラス名(なんでもいい)
	wc.lpszClassName = L"CG2WindowClass";

	//インスタンスバンドル
	wc.hInstance = GetModuleHandle(nullptr);

	//カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);


	//ウィンドウクラスを登録する
	RegisterClass(&wc);


	//クライアント領域のサイズ
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;

	//ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0,0,kClientWidth, kClientHeight };

	//クライアント領域を基に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);


	//ウィンドウの生成
	HWND hwnd = CreateWindow(
		wc.lpszClassName,		//利用するクラス名
		L"CG2",					//タイトルバーの文字(なんでもいい)
		WS_OVERLAPPEDWINDOW,	//よく見るウィンドウスタイル
		CW_USEDEFAULT,			//表示X座標(Windousに任せる)
		CW_USEDEFAULT,			//表示Y座標(WindousOSに任せる)
		wrc.right - wrc.left,	//ウィンドウの横幅
		wrc.bottom - wrc.top,	//ウィンドウの立幅
		nullptr,				//親ウィンドウバンドル
		nullptr,				//メニューバンドル
		wc.hInstance,			//インスタンスバンドル
		nullptr);				//オプション

	//ウィンドウの表示
	ShowWindow(hwnd, SW_SHOW);


	MSG msg{};
	//ウィンドウの×が押されるまでループ
	while (msg.message != WM_QUIT)
	{
		//ウィンドウにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else
		{
			///↓-------ゲームの処理-------↓



			///↑-------ゲームの処理-------↑
		}
	}


	//出力ウィンドウへの文字出力
	OutputDebugStringA("Hello DirectX!\n");

	return 0;
}