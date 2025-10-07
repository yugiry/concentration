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

//マウスカーソルがトランプの上にあるかチェックする関数
bool CheckOnTrump(Point, Point, int, int);

//めくった二枚のトランプが同じ数字かチェックする関数
bool CheckEqualTrump(int, int, int);

//ランダム生成関数
int Random(int);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	SetOutApplicationLogValidFlag(false);//ログ出力オフ
	ChangeWindowMode(TRUE); //ウィンドウモード切り替え
	SetGraphMode(1280, 720, 32); //ウィンドウサイズ

	if (DxLib_Init() == -1) { //DXライブラリ初期化処理
		return -1;			  //エラーが起きたら直ちに終了
	}

	SetDrawScreen(DX_SCREEN_BACK); //描画先を裏画面に変更
	SetWindowText("toranpu"); //ウィンドウの名前

	//定数
	constexpr int MAX_CARD{ 53 };			//トランプの枚数
	constexpr int CARD_WIDTH{ 64 };			//トランプ１枚の横幅
	constexpr int CARD_HEIGHT{ 92 };		//トランプ１枚の立幅
	constexpr Point CARD_START{ 100,100 };	//左上のトランプの座標
	constexpr int CARD_SPACE{ 20 };			//トランプとトランプの間隔
	constexpr int REVERSE_CARD{ 1 };		//裏面のトランプの数
	constexpr int CUT_X{ 13 };				//横方向のトランプの数
	constexpr int CUT_Y{ 5 };				//縦方向のトランプの数
	constexpr int TURN_TIME{ 2 };			//１ターンにめくるトランプの枚数
	constexpr int NEXT_INTERVAL{ 60 };		//次のターンまでの時間
	constexpr int CPU_TURN_TIME{ 30 };		//ＣＰＵのトランプをめくるまでの時間

	//変数
	Point mouse;							//マウスの座標
	vector<Memory> cards;						//トランプの情報を保存用
	bool player = true;						//ターンチェック
	bool turn[MAX_CARD - REVERSE_CARD];		//トランプがめくられているか
	bool onclick = false;					//左クリックがされているか
	int turn_num = 0;						//そのターンで何枚めくられているか
	Memory ptm[2];							//プレイヤーがそのターンにめくったトランプを二枚分保存用
	Memory ctm[2];							//ＣＰＵがそのターンにめくったトランプを二枚分保存用
	vector<Memory> ccm;						//過去10枚のめくられたトランプを記憶用
	bool equal_card = false;				//同じトランプがめくられたか
	int next_turn = 0;						//次のターンに行くまでの時間の計測用
	int player_get = 0;						//プレイヤーが揃えたトランプの数
	int cpu_get = 0;						//ＣＰＵが揃えたトランプの数
	int cturn_num = 0;						//ＣＰＵがトランプをめくるまでの時間の計測用
	int cturn_card = 0;						//ＣＰＵがめくるトランプの数字の保存用

	int num = 0;

	//画像---------------------
	int bg;//背景
	int cg;//丸
	//画像読み込み
	bg = LoadGraph("image\\back.png");
	cg = LoadGraph("image\\circle.png");

	//画像の分割読み込み
	//LoadDivGraph(画像ファイルポインタ、分割総数、横分割数、縦分割数、横サイズ、縦サイズ、保存配列ポインタ)
	//トランプの横サイズ：64、縦サイズ：92
	int tg[MAX_CARD];
	LoadDivGraph("image\\card.png", MAX_CARD, CUT_X, CUT_Y, CARD_WIDTH, CARD_HEIGHT, tg);

	//トランプの座標を初期化
	Point ini{ 0,0 };

	//トランプの情報を初期化
	for (int i = 0; i < MAX_CARD - REVERSE_CARD; i++)
	{
		Memory m{ ini,i };
		cards.push_back(m);
	}
	//トランプをランダム配置
	random_device rd;
	mt19937 mt(rd());
	shuffle(cards.begin(), cards.end(), mt);

	//トランプの順番によって再度座標を初期化
	for (int i = 0; i < MAX_CARD - REVERSE_CARD; i++)
	{
		Point p{ i % CUT_X,i / CUT_X };
		cards[i].pos = p;
	}

	//トランプをすべてめくられていない状態にする
	for (int i = 0; i < MAX_CARD - REVERSE_CARD; i++)
	{
		turn[i] = false;
	}

	while (1)
	{
		//裏画面のデータを全て削除
		ClearDrawScreen();

		//処理----------------------------------------------------------------

		//マウスカーソルの位置を取得
		GetMousePoint(&mouse.x, &mouse.y);

		//トランプを二枚めくった時に一時的に停止してトランプの数字を見やすくする
		if (next_turn > 0)
		{
			next_turn--;
		}
		if (next_turn == 0)
		{
			//違うカードをめくっていたらめくったトランプを裏面に戻す
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

		//ゲーム
		if (player)
		{
			//プレイヤーのターン
			if (turn_num < TURN_TIME)
			{
				//左クリックする
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
									//まだめくられていないトランプをめくる
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
			//トランプを二回めくった
			else if (turn_num == TURN_TIME)
			{
				//めくった二枚のトランプの数字を見る
				if (CheckEqualTrump(ptm[0].card_num, ptm[1].card_num, CUT_X))
				{
					//同じ数字なら正解の丸を表示する
					equal_card = true;
					next_turn = NEXT_INTERVAL;
					turn_num++;
				}
				else
				{
					//違う数字なら何も表示しない
					next_turn = NEXT_INTERVAL;
					turn_num++;
					player = false;
				}
			}
		}
		else
		{
			//CPUのターン
			if (turn_num < TURN_TIME)
			{
				if (cturn_num < 0)
				{
					//トランプをめくる
					do
					{
						cturn_card = Random(cards.size());
						if (cturn_card == cards.size())cturn_card--;
						//トランプがめくられていなかったらめくる
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
			//トランプを２回めくった
			else if (turn_num == TURN_TIME)
			{
				//めくった二枚のトランプの数字を見る
				if (CheckEqualTrump(ctm[0].card_num, ctm[1].card_num, CUT_X))
				{
					//同じ数字なら正解の丸を表示する
					equal_card = true;
					next_turn = NEXT_INTERVAL;
					turn_num++;
				}
				else
				{
					//違う数字なら何も表示しない
					next_turn = NEXT_INTERVAL;
					turn_num++;
					player = true;
				}
			}
		}

		//画像の描画(位置X、位置Y、グラフィックハンドル、透明度の有効無効)
		//背景
		DrawGraph(0, 0, bg, true);

		//トランプの表示
		for (int y = 0; y < CUT_Y - REVERSE_CARD; y++)
		{
			for (int x = 0; x < CUT_X; x++)
			{
				//めくられてなかったら裏面を表示
				if (!turn[x + y * CUT_X])
					DrawGraph(x * CARD_WIDTH + CARD_SPACE * x + CARD_START.x, y * CARD_HEIGHT + CARD_SPACE * y + CARD_START.y, tg[52], true);
				//めくられていたら表面を表示
				else
					DrawGraph(x * CARD_WIDTH + CARD_SPACE * x + CARD_START.x, y * CARD_HEIGHT + CARD_SPACE * y + CARD_START.y, tg[cards[x + y * CUT_X].card_num], true);
			}
		}

		//同じトランプをめくっていたら正解の丸を表示
		if (equal_card)
		{
			DrawGraph(0, 0, cg, true);
		}

		//--------------------------------------------------------------------

		ScreenFlip(); //裏画面データを表画面へ反映

		//毎ループ呼び出す。エラーになった場合breakする
		if (ProcessMessage() == -1)break;
		//エスケープキーを押したり、エラーになった場合、breakする
		if (CheckHitKey(KEY_INPUT_ESCAPE))break;
	}

	//画像データ削除
	DeleteGraph(bg);
	DeleteGraph(cg);
	for (int i = 0; i < MAX_CARD; i++)
		DeleteGraph(tg[i]);

	WaitKey();	 //キー入力待ち
	DxLib_End(); //DXライブラリ使用の終了処理
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