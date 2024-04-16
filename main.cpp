#include <Windows.h>



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



//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) 
{
	WNDCLASS wc{};

	//ウィンドウプロージャ
	wc.lpfnWndProc = WindowProc;

	//ウィンドウクラス名(何でもいい)
	wc.lpszClassName = L"CG2WindowClass";

	//インスタンスバンドル
	wc.hInstance = GetModuleHandle(nullptr);

	//カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);


	//ウィンドウクラスを登録する
	RegisterClass(&wc);




	//出力ウィンドウへの文字出力
	OutputDebugStringA("Hello DirectX!\n");

	return 0;
}