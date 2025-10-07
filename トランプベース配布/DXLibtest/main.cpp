#include "DxLib.h"
#include <random>
#include <vector>

using namespace std;

typedef struct Point
{
	int x;
	int y;
}Point;

typedef struct Memory
{
	Point pos;
	int card_num;
}Memory;

//�}�E�X�J�[�\�����g�����v�̏�ɂ��邩�`�F�b�N����֐�
bool CheckOnTrump(Point, Point, int, int);

//�߂������񖇂̃g�����v�������������`�F�b�N����֐�
bool CheckEqualTrump(int, int, int);

//�����_�������֐�
int Random(int);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	SetOutApplicationLogValidFlag(false);//���O�o�̓I�t
	ChangeWindowMode(TRUE); //�E�B���h�E���[�h�؂�ւ�
	SetGraphMode(1280, 720, 32); //�E�B���h�E�T�C�Y

	if (DxLib_Init() == -1) { //DX���C�u��������������
		return -1;			  //�G���[���N�����璼���ɏI��
	}

	SetDrawScreen(DX_SCREEN_BACK); //�`���𗠉�ʂɕύX
	SetWindowText("toranpu"); //�E�B���h�E�̖��O

	//�萔
	constexpr int MAX_CARD{ 53 };			//�g�����v�̖���
	constexpr int CARD_WIDTH{ 64 };			//�g�����v�P���̉���
	constexpr int CARD_HEIGHT{ 92 };		//�g�����v�P���̗���
	constexpr Point CARD_START{ 100,100 };	//����̃g�����v�̍��W
	constexpr int CARD_SPACE{ 20 };			//�g�����v�ƃg�����v�̊Ԋu
	constexpr int REVERSE_CARD{ 1 };		//���ʂ̃g�����v�̐�
	constexpr int CUT_X{ 13 };				//�������̃g�����v�̐�
	constexpr int CUT_Y{ 5 };				//�c�����̃g�����v�̐�
	constexpr int TURN_TIME{ 2 };			//�P�^�[���ɂ߂���g�����v�̖���
	constexpr int NEXT_INTERVAL{ 60 };		//���̃^�[���܂ł̎���
	constexpr int CPU_TURN_TIME{ 30 };		//�b�o�t�̃g�����v���߂���܂ł̎���

	//�ϐ�
	Point mouse;							//�}�E�X�̍��W
	vector<Memory> cards;						//�g�����v�̏���ۑ��p
	bool player = true;						//�^�[���`�F�b�N
	bool turn[MAX_CARD - REVERSE_CARD];		//�g�����v���߂����Ă��邩
	bool onclick = false;					//���N���b�N������Ă��邩
	int turn_num = 0;						//���̃^�[���ŉ����߂����Ă��邩
	Memory ptm[2];							//�v���C���[�����̃^�[���ɂ߂������g�����v��񖇕��ۑ��p
	Memory ctm[2];							//�b�o�t�����̃^�[���ɂ߂������g�����v��񖇕��ۑ��p
	vector<Memory> ccm;						//�ߋ�10���̂߂���ꂽ�g�����v���L���p
	bool equal_card = false;				//�����g�����v���߂���ꂽ��
	int next_turn = 0;						//���̃^�[���ɍs���܂ł̎��Ԃ̌v���p
	int player_get = 0;						//�v���C���[���������g�����v�̐�
	int cpu_get = 0;						//�b�o�t���������g�����v�̐�
	int cturn_num = 0;						//�b�o�t���g�����v���߂���܂ł̎��Ԃ̌v���p
	int cturn_card = 0;						//�b�o�t���߂���g�����v�̐����̕ۑ��p

	int num = 0;

	//�摜---------------------
	int bg;//�w�i
	int cg;//��
	//�摜�ǂݍ���
	bg = LoadGraph("image\\back.png");
	cg = LoadGraph("image\\circle.png");

	//�摜�̕����ǂݍ���
	//LoadDivGraph(�摜�t�@�C���|�C���^�A���������A���������A�c�������A���T�C�Y�A�c�T�C�Y�A�ۑ��z��|�C���^)
	//�g�����v�̉��T�C�Y�F64�A�c�T�C�Y�F92
	int tg[MAX_CARD];
	LoadDivGraph("image\\card.png", MAX_CARD, CUT_X, CUT_Y, CARD_WIDTH, CARD_HEIGHT, tg);

	//�g�����v�̍��W��������
	Point ini{ 0,0 };

	//�g�����v�̏���������
	for (int i = 0; i < MAX_CARD - REVERSE_CARD; i++)
	{
		Memory m{ ini,i };
		cards.push_back(m);
	}
	//�g�����v�������_���z�u
	random_device rd;
	mt19937 mt(rd());
	shuffle(cards.begin(), cards.end(), mt);

	//�g�����v�̏��Ԃɂ���čēx���W��������
	for (int i = 0; i < MAX_CARD - REVERSE_CARD; i++)
	{
		Point p{ i % CUT_X,i / CUT_X };
		cards[i].pos = p;
	}

	//�g�����v�����ׂĂ߂����Ă��Ȃ���Ԃɂ���
	for (int i = 0; i < MAX_CARD - REVERSE_CARD; i++)
	{
		turn[i] = false;
	}

	while (1)
	{
		//����ʂ̃f�[�^��S�č폜
		ClearDrawScreen();

		//����----------------------------------------------------------------

		//�}�E�X�J�[�\���̈ʒu���擾
		GetMousePoint(&mouse.x, &mouse.y);

		//�g�����v��񖇂߂��������Ɉꎞ�I�ɒ�~���ăg�����v�̐��������₷������
		if (next_turn > 0)
		{
			next_turn--;
		}
		if (next_turn == 0)
		{
			//�Ⴄ�J�[�h���߂����Ă�����߂������g�����v�𗠖ʂɖ߂�
			if (!equal_card && turn_num > TURN_TIME)
			{
				for (int i = 0; i < TURN_TIME; i++)
				{
					if (!player)
						turn[ptm[i].pos.x + ptm[i].pos.y * CUT_X] = false;
					else
						turn[ctm[i].pos.x + ctm[i].pos.y * CUT_X] = false;
				}
			}
			else if (player)
				equal_card = false;
			turn_num = 0;
			next_turn--;
		}
		else if (!player)
		{
			if (equal_card)
				turn_num = 0;

			if (cturn_num > 0)
				cturn_num--;
			if (cturn_num == 0)
			{
				equal_card = false;
				cturn_num--;
			}
		}

		//�Q�[��
		if (player)
		{
			//�v���C���[�̃^�[��
			if (turn_num < TURN_TIME)
			{
				//���N���b�N����
				if ((GetMouseInput() & MOUSE_INPUT_LEFT) && !onclick)
				{
					for (int y = 0; y < CUT_Y - REVERSE_CARD; y++)
					{
						for (int x = 0; x < CUT_X; x++)
						{
							for (auto i = 0; i < cards.size(); i++)
							{
								if (cards[i].pos.x == x && cards[i].pos.y == y)
								{
									Point p{ x * CARD_WIDTH + CARD_SPACE * x + CARD_START.x,y * CARD_HEIGHT + CARD_SPACE * y + CARD_START.y };
									//Point pos{ x,y };
									//�܂��߂����Ă��Ȃ��g�����v���߂���
									if (CheckOnTrump(mouse, p, CARD_WIDTH, CARD_HEIGHT) && !turn[x + y * CUT_X])
									{
										turn[x + y * CUT_X] = true;
										ptm[turn_num] = cards[i];
										turn_num++;
										break;
									}
								}
							}
						}
					}
				}
				onclick = (GetMouseInput() & MOUSE_INPUT_LEFT);
			}
			//�g�����v����߂�����
			else if (turn_num == TURN_TIME)
			{
				//�߂������񖇂̃g�����v�̐���������
				if (CheckEqualTrump(ptm[0].card_num, ptm[1].card_num, CUT_X))
				{
					//���������Ȃ琳���̊ۂ�\������
					equal_card = true;
					next_turn = NEXT_INTERVAL;
					turn_num++;
				}
				else
				{
					//�Ⴄ�����Ȃ牽���\�����Ȃ�
					next_turn = NEXT_INTERVAL;
					turn_num++;
					player = false;
				}
			}
		}
		else
		{
			//CPU�̃^�[��
			if (turn_num < TURN_TIME)
			{
				if (cturn_num < 0)
				{
					//�g�����v���߂���
					do
					{
						cturn_card = Random(cards.size());
						if (cturn_card == cards.size())cturn_card--;
						//�g�����v���߂����Ă��Ȃ�������߂���
						if (!turn[cards[cturn_card].pos.x + cards[cturn_card].pos.y * CUT_X])
						{
							turn[cards[cturn_card].pos.x + cards[cturn_card].pos.y * CUT_X] = true;
							ctm[turn_num] = cards[cturn_card];
							turn_num++;
							cturn_num = CPU_TURN_TIME;
							break;
						}
						else
						{
							num++;
						}
					} while (true);
				}
			}
			//�g�����v���Q��߂�����
			else if (turn_num == TURN_TIME)
			{
				//�߂������񖇂̃g�����v�̐���������
				if (CheckEqualTrump(ctm[0].card_num, ctm[1].card_num, CUT_X))
				{
					//���������Ȃ琳���̊ۂ�\������
					equal_card = true;
					next_turn = NEXT_INTERVAL;
					turn_num++;
				}
				else
				{
					//�Ⴄ�����Ȃ牽���\�����Ȃ�
					next_turn = NEXT_INTERVAL;
					turn_num++;
					player = true;
				}
			}
		}

		//�摜�̕`��(�ʒuX�A�ʒuY�A�O���t�B�b�N�n���h���A�����x�̗L������)
		//�w�i
		DrawGraph(0, 0, bg, true);

		//�g�����v�̕\��
		for (int y = 0; y < CUT_Y - REVERSE_CARD; y++)
		{
			for (int x = 0; x < CUT_X; x++)
			{
				//�߂����ĂȂ������痠�ʂ�\��
				if (!turn[x + y * CUT_X])
					DrawGraph(x * CARD_WIDTH + CARD_SPACE * x + CARD_START.x, y * CARD_HEIGHT + CARD_SPACE * y + CARD_START.y, tg[52], true);
				//�߂����Ă�����\�ʂ�\��
				else
					DrawGraph(x * CARD_WIDTH + CARD_SPACE * x + CARD_START.x, y * CARD_HEIGHT + CARD_SPACE * y + CARD_START.y, tg[cards[x + y * CUT_X].card_num], true);
			}
		}

		//�����g�����v���߂����Ă����琳���̊ۂ�\��
		if (equal_card)
		{
			DrawGraph(0, 0, cg, true);
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
	DeleteGraph(cg);
	for (int i = 0; i < MAX_CARD; i++)
		DeleteGraph(tg[i]);

	WaitKey();	 //�L�[���͑҂�
	DxLib_End(); //DX���C�u�����g�p�̏I������
	return 0;
}

bool CheckOnTrump(Point mp, Point cp, int cw, int ch)
{
	if (mp.x > cp.x && mp.x < cp.x + cw && mp.y>cp.y && mp.y < cp.y + ch)
	{
		return true;
	}

	return false;
}

bool CheckEqualTrump(int c1, int c2, int cutx)
{
	if (c1 % cutx == c2 % cutx)
		return true;

	return false;
}

int Random(int max)
{
	int min = 0;
	return rand() % (max - min + 1) + min;
}