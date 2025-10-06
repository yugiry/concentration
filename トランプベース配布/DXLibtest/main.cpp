#include "DxLib.h"
#include <random>
#include <vector>

using namespace std;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	SetOutApplicationLogValidFlag(false);//ログ出力オフ
	ChangeWindowMode(TRUE); //ウィンドウモード切り替え
	SetGraphMode(1280,720, 32); //ウィンドウサイズ

	if (DxLib_Init() == -1) { //DXライブラリ初期化処理
		return -1;			  //エラーが起きたら直ちに終了
	}

	SetDrawScreen(DX_SCREEN_BACK); //描画先を裏画面に変更
	SetWindowText("toranpu"); //ウィンドウの名前

	//定数
	int MAX_CARD{ 53 };
	int CARD_WIDTH{ 64 };
	int CARD_HEIGHT{ 92 };
	int CARD_START_X{ 100 };
	int CARD_START_Y{ 125 };

	//変数
	vector<int> cards;

	//画像---------------------
	int bg;//背景
	//画像読み込み
	bg = LoadGraph("image\\back.png");

	//画像の分割読み込み
	//LoadDivGraph(画像ファイルポインタ、分割総数、横分割数、縦分割数、横サイズ、縦サイズ、保存配列ポインタ)
	//トランプの横サイズ：64、縦サイズ：92
	int tg[53];
	LoadDivGraph("image\\card.png", MAX_CARD, 13, 5, CARD_WIDTH, CARD_HEIGHT, tg);

	int mouseX, mouseY;//カーソル位置保存用

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
		//裏画面のデータを全て削除
		ClearDrawScreen();

		//処理----------------------------------------------------------------

		//マウスカーソルの位置を取得
		GetMousePoint(&mouseX, &mouseY);



		//画像の描画(位置X、位置Y、グラフィックハンドル、透明度の有効無効)
		//背景
		DrawGraph(0, 0, bg, true);

		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 13; x++)
			{
				DrawGraph(x * CARD_WIDTH + 20 * x + CARD_START_X, y * CARD_HEIGHT + 20 * y + CARD_START_Y, tg[52], true);
			}
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
	for (int i = 0; i < MAX_CARD; i++)
		DeleteGraph(tg[i]);

	WaitKey();	 //キー入力待ち
	DxLib_End(); //DXライブラリ使用の終了処理
	return 0;
}