#include <Windows.h>



//�E�B���h�E�v���[�W��
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//���b�Z�[�W�ɉ����ăQ�[���̌ŗL�������s��
	switch (msg)
	{
		//�E�B���h�E�j�����ꂽ
	case WM_DESTROY:
		//OS�ɑ΂��ăA�v���̏I����`����
		PostQuitMessage(0);
		return 0;
	}

	//�W���̃��b�Z�[�W�������s��
	return DefWindowProc(hwnd, msg, wparam, lparam);
}



//Windows�A�v���ł̃G���g���[�|�C���g(main�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) 
{

	//�o�̓E�B���h�E�ւ̕����o��
	OutputDebugStringA("Hello DirectX!\n");

	return 0;
}