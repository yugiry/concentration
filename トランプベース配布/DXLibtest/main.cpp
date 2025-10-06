#include "DxLib.h"
#include <random>
#include <vector>

using namespace std;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	SetOutApplicationLogValidFlag(false);//���O�o�̓I�t
	ChangeWindowMode(TRUE); //�E�B���h�E���[�h�؂�ւ�
	SetGraphMode(1280,720, 32); //�E�B���h�E�T�C�Y

	if (DxLib_Init() == -1) { //DX���C�u��������������
		return -1;			  //�G���[���N�����璼���ɏI��
	}

	SetDrawScreen(DX_SCREEN_BACK); //�`���𗠉�ʂɕύX
	SetWindowText("toranpu"); //�E�B���h�E�̖��O

	//�萔
	int MAX_CARD{ 53 };
	int CARD_WIDTH{ 64 };
	int CARD_HEIGHT{ 92 };
	int CARD_START_X{ 100 };
	int CARD_START_Y{ 125 };

	//�ϐ�
	vector<int> cards;

	//�摜---------------------
	int bg;//�w�i
	//�摜�ǂݍ���
	bg = LoadGraph("image\\back.png");

	//�摜�̕����ǂݍ���
	//LoadDivGraph(�摜�t�@�C���|�C���^�A���������A���������A�c�������A���T�C�Y�A�c�T�C�Y�A�ۑ��z��|�C���^)
	//�g�����v�̉��T�C�Y�F64�A�c�T�C�Y�F92
	int tg[53];
	LoadDivGraph("image\\card.png", MAX_CARD, 13, 5, CARD_WIDTH, CARD_HEIGHT, tg);

	int mouseX, mouseY;//�J�[�\���ʒu�ۑ��p

	//
	for (int i = 0; i < MAX_CARD - 1; i++)
	{
		cards.push_back(i);
	}

	random_device rd;
	mt19937 mt(rd());
	shuffle(cards.begin(), cards.end(), mt);

	while (1) 
	{
		//����ʂ̃f�[�^��S�č폜
		ClearDrawScreen();

		//����----------------------------------------------------------------

		//�}�E�X�J�[�\���̈ʒu���擾
		GetMousePoint(&mouseX, &mouseY);



		//�摜�̕`��(�ʒuX�A�ʒuY�A�O���t�B�b�N�n���h���A�����x�̗L������)
		//�w�i
		DrawGraph(0, 0, bg, true);

		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 13; x++)
			{
				DrawGraph(x * CARD_WIDTH + 20 * x + CARD_START_X, y * CARD_HEIGHT + 20 * y + CARD_START_Y, tg[52], true);
			}
		}

		//--------------------------------------------------------------------

		ScreenFlip(); //����ʃf�[�^��\��ʂ֔��f

		//�����[�v�Ăяo���B�G���[�ɂȂ����ꍇbreak����
		if (ProcessMessage() == -1)break;
		//�G�X�P�[�v�L�[����������A�G���[�ɂȂ����ꍇ�Abreak����
		if (CheckHitKey(KEY_INPUT_ESCAPE))break;
	}

	//�摜�f�[�^�폜
	DeleteGraph(bg);
	for (int i = 0; i < MAX_CARD; i++)
		DeleteGraph(tg[i]);

	WaitKey();	 //�L�[���͑҂�
	DxLib_End(); //DX���C�u�����g�p�̏I������
	return 0;
}