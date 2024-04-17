#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

//libのリンク
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")


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


//出力ウィンドウに文字を出す
void Log(const std::string& message)
{
	OutputDebugStringA(message.c_str());
}

void Log(const std::wstring& message)
{
	OutputDebugStringA(ConvertString(message).c_str());
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


	//DXGIファクトリーの生成
	IDXGIFactory7* dxgiFactory = nullptr;

	//HRESULTはWindows系のエラーコードであり
	//関数が成功したかどうかをSUCCEEDEDマクロで判定できる
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));

	//初期化の根本的な部分でエラーが出た場合はコードが間違っているか
	//どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr));


	//使用するアダプタ用の変数　最初にnullptrを代入しておく
	IDXGIAdapter4* useAdapter = nullptr;

	//良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(
		i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
		IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		//アダプタの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));//取得できないのは一大事
		//ソフトウェアアダプタでなければ採用！
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			//採用したアダプタの情報をログに出力　wstringの方なので注意
			Log(std::format(L"Use Adapter:{}\n", adapterDesc.Description));
			break;
		}
		useAdapter = nullptr;//ソフトウェアアダプタの場合は見なかったことにする
	}
	//適切なアダプタが見つからなかったので起動できない
	assert(useAdapter != nullptr);


	ID3D12Device* device = nullptr;
	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i)
	{
		//採用したアダプタでデバイスを生成
		hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));
		//指定した機能レベルでデバイスが生成できたか確認
		if (SUCCEEDED(hr))
		{
			//生成できたのでログ出力を行ってループを抜ける
			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}

	//デバイスの生成が上手くいかなかったので起動できない
	assert(device != nullptr);
	Log("Comprete Create D3D12Device!\n");


	//コマンドキューを生成する
	ID3D12CommandQueue* commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	//コマンドキューの生成が上手く行かなかったので起動できない
	assert(SUCCEEDED(hr));


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