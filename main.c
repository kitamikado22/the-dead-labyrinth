#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
//#include <windows.h>
//#include <conio.h>

#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define TERM_RAW_ON 1
#define TERM_RAW_OFF 0


//////////
// 定数定義
//////////
// 画面系
#define SCREEN_WIDTH 32		// スクリーンの幅
#define SCREEN_HEIGHT 18	// スクリーンの高さ
#define TEXTBOX_WIDTH 60	// テキストボックスの
#define TEXTBOX_LINE 4		// テキストボックスの行数
#define VALUE_MAP_REF 9		// マップの画面素材の基準値
#define NUM_PLAYER_MAP 16	// プレイヤーのマップでのグラフィック番号
// 難易度
#define NUM_MODE 3	// 難易度の数
#define EASY 0		//　イージー
#define NORMAL 1	// ノーマル
#define HARD 2		// ハード
// キー
#define ENTER 0x0d	// Enter
#define BACK_SPACE 0x08	// Back Space
#define CTRL_C 0X03	// Ctrl+C
// 方向
#define SOUTH 0		// 北
#define EAST 1		// 東
#define NORTH 2		// 南
#define WEST 3		// 西
// ステージ作成
#define PRIORY_MAX 5	// ステージを作る場合の部屋の優先度のマックス
// オブジェクト
#define COLOR_NUM 4	// カラータイルの種類の数
// プレイヤーステータス
#define START_HP 20		// 初期 HP
#define START_MP 5		// 初期 MP
#define START_POWER 4	// 初期ちから
#define ITEM_NUM_MAX 9	// プレーヤーが持てるアイテムの最大数
#define SKILL_NUM_MAX 5	// プレーヤーのスキル最大数
#define EXP_MAX 15000	// 経験値最大値
#define GOLD_MAX 20000	// お金最大
// コントロールモード
#define DEAD -1	// 死亡
#define FIELD 0	// フィールドモード
#define MENU 1	// メニューモード
#define BATTLE 2	// バトルモード
#define MAP 3	// マップモード
#define TOWN 4	// タウンモード
// スキル系
#define COST_HEEL 3	// ヒールの MP コスト
#define COST_FIRE 2	// ファイアの MP コスト
#define COST_HEEL_P 8	// ヒールプラスの MP コスト
#define COST_BURST 5	// バーストの MP コスト
#define COST_FIRE_M 8	// メガファイアの MP コスト
#define VALUE_HEEL 30	// ヒールの基準値
#define VALUE_FIRE 20	// ファイアの基準値
#define VALUE_HEEL_P 90	// ヒールプラスの基準値
#define VALUE_BURST 40	// バーストの基準値
#define VALUE_FIRE_M 60	// メガファイアの基準値
// アイテム系
#define VALUE_HERB 30	// 薬草の回復基準値
#define VALUE_HERB_U 60	// 上やくそうの回復基準値
#define VALUE_WATER 30	// せいすいの基準値
#define VALUE_STICK 4	// こん棒の攻撃力
#define VALUE_COPPER 10	// どうのつるぎの攻撃力
#define VALUE_SWORD 20	// てつのつるぎの攻撃力
#define VALUE_CLOTH 2	// ぬののふくの防御力
#define VALUE_LEATHER 4	// かわのよろいの防御力
#define VALUE_IRON 16	// てつのよろいの防御力
#define GOLD_HERB 10	// やくそうの買値
#define GOLD_HERB_U 25	// 上やくそうの買値
#define GOLD_WATER 20	// せいすいの買値
#define GOLD_RELEASE 60	// わなかいじょの買値
#define GOLD_STICK 55	// こんぼうの買値
#define GOLD_COPPER 180	// どうのつるぎの買値
#define GOLD_SWORD 500	// てつのつるぎの買値
#define GOLD_CLOTH 20	// ぬののふくの買値
#define GOLD_LEATHER 70	// かわのよろいの買値
#define GOLD_IRON 300	// てつのよろいの買値
// モンスター
#define MONSTER_NUM_MAX 3	// 同時に戦えるモンスターの数
#define MONSTER_KIND_NUM 5	// 敵の種類の数
#define SLIME 0		// スライム
#define GOBLIN 1	// ゴブリン
#define ORC 2		// オーク
#define SKELETON 3	// スケルトン
#define WIZARD 4	// ウィザード
#define FAIRY 5		// 妖精
#define DRAGON 6	// ドラゴン
#define COST_POWER_UP 3	// パワーアップバフのコスト
#define COST_PROVO 4	// 挑発のコスト
#define COST_THUNDER 3	// サンダーのコスト
#define COST_THUNDER_GIGA 7	// ギガサンダーのコスト
#define COST_HYPNOS 4	// 催眠のコスト
#define VALUE_THUNDER 15	// サンダーの基準値
#define VALUE_THUNDER_GIGA 30	// ギガサンダーの基準値
#define COST_HONONOIKI 10	// ほのおのいきコスト
#define VALUE_HONONOIKI 50	// ほのおのいき基準値


#define UNIX
////////////////////
// 条件コンパイルつきの関数
////////////////////
// スリープ関数
void mySleep(int ms)
{
#ifdef UNIX
	usleep(ms * 1000);
#else
	Sleep(ms);
#endif
}

// カーソルの位置を変更
void move_cursor_console(int x, int y)
{
#ifdef UNIX
	printf("\e[%d;%dH", y, x);
#else
	printf("\x1B[%d;%dH", y, x);
#endif
}

void off_cursor_console()
{
#ifdef UNIX
	printf("\e[?25l");
#else
	printf("\x1B[?25l");
#endif
}

void on_cursor_console()
{
#ifdef UNIX
	printf("\e[?25h");
#else
	printf("\x1B[?25h");
#endif	
}

/*
// Windows用カラー表示、位置表示の為の初期化
void enableVT() {
	// Windowsのコンソール画面を以下のように設定すると
	// カラー表示、位置表示ができるようになる
    HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD consoleMode = 0;
    GetConsoleMode(stdOut, &consoleMode);
    SetConsoleMode(stdOut, consoleMode | 0x0004);
}
*/


// ステージ構成の列挙体
typedef enum STAGE_STATE {
	Space,		// 空白
	Wall,		// 壁
	Start,		// スタート地点
	Red,		// 赤色のタイル
	Green,		// 緑色のタイル
	Blue,		// 青色のタイル
	White,		// 白色のタイル
	Treasure,	// 宝箱
	Enemy,		// 固定敵
	Rotate,		// 回転床
	Teleport,	// テレポート
	Landmine,	// 地雷
	Arrow,		// 矢が飛んでくる
	Goal,		// ゴール地点
}Stage_state;

// アイテム構成の列挙体
typedef enum ITEM {
	None,		// なし
	Herb,		// やくそう
	Herb_u,		// 上やくそう
	Water,		// せいすい
	Release,	// 罠解除
	Map,		// 地図
	
	// 武器
	Weapon,		// 武器
	Stick,		// こん棒
	Copper,		// どうのつるぎ
	Sword,		// てつのつるぎ
	
	// 防具
	Armor,		// 防具
	Cloth,		// ぬののふく
	Leather,	// 皮のよろい
	Iron,		// 鉄のよろい
	
	// 大事なもの
	Jewelry,	// 宝石
}Item;

// スキル構成の列挙体
typedef enum SKILL {
	Heel,		// ヒール
	Fire,		// ファイヤ
	Heel_plus,	// ヒールプラス
	Burst,		// バースト
	Fire_mega,	// メガファイヤ
}Skill;

// 二次元座標データの構造体
typedef struct POS {
	char x;		// x座標
	char y;		// y座標
}Pos;

// ステージを作成する際の部屋の構造体
typedef struct ROOM {
	char x;		// x座標
	char y;		// y座標
	char way;	// 通路の数
}Room;

// ウィンドウカーソルの構造体
typedef struct CURSOR {
	char x;		// x座標
	char y;		// y座標
	char num;	// 選んだ番号
	char start_y;	// 最初ののy座標
	char last_y;	// 終端のy座標
	char history;	// ひとつ前のカーソルのy座標
	bool draw_flag;	// カーソルを描画するかどうか
}Cursor;

// モンスターの構造体
typedef struct MONSTER {
	char name[16];	// 名前
	int hp;	// HP
	int mp;	// MP
	int hp_max;	// さいだい HP
	int mp_max;	// さいだい MP
	int attack;	// 攻撃力
	int defense;	// 守備力
	int exp;	// 経験値
	int gold;	// お金
	bool power_buff;	// 力が増えるバフ
	int count;	// カウンター
	char num;	// 識別番号
}Monster;

// プレイヤーの構造体
typedef struct PLAYER {
	char mode;	// モード
	char x;		// x座標
	char y;		// y座標
	char dir;	// プレイヤーの向き
	
	// 基本ステータス
	int hp;		// プレイヤーのHP
	int mp;		// プレイヤーのMP
	int hp_max;	// 最大HP
	int mp_max;	// 最大MP
	int exp;	// 経験値
	int gold;	// お金
	char level;	// レベル
	char power;	// 素の攻撃力
	char physical;	// 素の防御力
	int attack;	// 攻撃力
	int defense;// 防御力
	
	// アイテム
	char item_num;	// 現在所持しているアイテムの数
	Item item[ITEM_NUM_MAX];	// プレイヤーの所持アイテム
	char weapon;	// 装備している武器、所持アイテムの何番目にあるか
	char armor;		// 装備している防具
	
	// スキル
	char skill_num;	// 現在所持しているスキルの数
	Skill skill[SKILL_NUM_MAX];	// プレイヤーの所持スキル
	
	// モード
	char control;	// 何を操作するのか
	
	// カーソル
	Cursor basic[8];	// ウィンドウそれぞれに設定したカーソル
	char sel_cursor;	// 選んでいるカーソル
	
	// マップ
	Pos map_p;	// マップの座標
	
	// モンスター
	char monst_num;	// モンスターの数
	char monst_num_cp;	// コピー
	Monster monster[MONSTER_NUM_MAX];	// 戦うモンスター
	
	// デバフ
	bool anger_debuff;	// 挑発
	bool hypnos_debuff;	// 催眠
	int count;	// カウンター
	
	// フラグ
	bool draw_window_flag;	// ウィンドウを描画
	//bool use_cursor_flag;	// カーソル描画
	bool process_flag;	// 内部処理
	//bool print_screen_flag;	// 画面表示
	//bool monster_dead_flag;	// モンスター死亡フラグ
	//bool monster_turn;	// モンスターのターン
	bool end_flag;	// ゲーム終了
	bool game_over_flag;	// ゲームオーバー
}Player;

// 関数ポインタのtypedef
typedef Stage_state(* fp_modes)();


///////////////
// プロトタイプ宣言
///////////////
void get_treasure(Player *player);
//char input_menu();



//////////////
// グローバル変数
//////////////
// ステージ
Stage_state **Stage;	// ステージのデータ
int Stage_size;		// ステージのサイズ

// マップ
char **Map_white;	// 白紙の地図

// スクリーン
char Screen[SCREEN_HEIGHT][SCREEN_WIDTH];	// グラフィック専用
char Screen_str[SCREEN_HEIGHT][SCREEN_WIDTH * 2];	// 文字列専用

// テキストボックス
char Textbox[TEXTBOX_LINE][64];
//char Cp_textbox[6][SCREEN_WIDTH];	// コピー用
char Text[256];

// 文字素材
//char *Map_material[] = {"  ","■","▲","赤","緑","青","白","宝","敵","回","移","雷","矢","▼"};	// マップで表示する文字
char *Screen_material[] = {"  ","＿","■","□","--","+-","-+","| "," |","  ","■","▲","赤","緑","青","白","＠"};	// スクリーンで表示する文字
char *Item_str[] = {"なし","やくそう","上やくそう","せいすい","わなかいじょ","地図","","こんぼう","どうのつるぎ","てつのつるぎ","","ぬののふく","かわのよろい","てつのよろい","ほうせき"};
char *Skill_str[] = {"ヒール","ファイヤ","ヒールプラス","バースト","メガファイヤ"};

// スクリーンの描画データ
char Way[SCREEN_HEIGHT][SCREEN_WIDTH] = {	// 通路
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};
char Pattern_0[7][10] = {	// パターン０
	{3,3,3,3,3,3,3,2,2,0},
	{3,3,3,3,3,3,3,2,2,2},
	{3,3,3,3,3,3,3,2,2,2},
	{3,3,3,3,3,3,3,2,2,2},
	{3,3,3,3,3,3,3,2,2,2},
	{3,3,3,3,3,3,3,2,2,0},
	{3,3,3,3,3,3,3,2,1,1},
};
char Pattern_1[7][12] = {	// パターン１
	{3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3},
};
char Pattern_2[7][10] = {	// パターン２
	{0,2,2,3,3,3,3,3,3,3},
	{2,2,2,3,3,3,3,3,3,3},
	{2,2,2,3,3,3,3,3,3,3},
	{2,2,2,3,3,3,3,3,3,3},
	{2,2,2,3,3,3,3,3,3,3},
	{0,2,2,3,3,3,3,3,3,3},
	{1,1,2,3,3,3,3,3,3,3},
};
char Pattern_3[16][10] = {	// パターン３
	{3,3,3,3,2,2,0,0,0,0},
	{3,3,3,3,2,2,2,2,0,0},
	{3,3,3,3,2,2,2,2,2,2},
	{3,3,3,3,2,2,2,2,2,2},
	{3,3,3,3,2,2,2,2,2,2},
	{3,3,3,3,2,2,2,2,2,2},
	{3,3,3,3,2,2,2,2,2,2},
	{3,3,3,3,2,2,2,2,2,2},
	{3,3,3,3,2,2,2,2,2,2},
	{3,3,3,3,2,2,2,2,2,2},
	{3,3,3,3,2,2,2,2,2,2},
	{3,3,3,3,2,2,2,2,2,0},
	{3,3,3,3,2,2,2,2,0,0},
	{3,3,3,3,2,2,2,0,0,0},
	{3,3,3,3,2,2,0,0,0,0},
	{3,3,3,3,2,1,1,1,1,1},
};
char Pattern_4[16][24] = {	// パターン４
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
	{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
};
char Pattern_5[16][10] = {	// パターン５
	{0,0,0,0,2,2,3,3,3,3},
	{0,0,2,2,2,2,3,3,3,3},
	{2,2,2,2,2,2,3,3,3,3},
	{2,2,2,2,2,2,3,3,3,3},
	{2,2,2,2,2,2,3,3,3,3},
	{2,2,2,2,2,2,3,3,3,3},
	{2,2,2,2,2,2,3,3,3,3},
	{2,2,2,2,2,2,3,3,3,3},
	{2,2,2,2,2,2,3,3,3,3},
	{2,2,2,2,2,2,3,3,3,3},
	{2,2,2,2,2,2,3,3,3,3},
	{0,2,2,2,2,2,3,3,3,3},
	{0,0,2,2,2,2,3,3,3,3},
	{0,0,0,2,2,2,3,3,3,3},
	{0,0,0,0,2,2,3,3,3,3},
	{1,1,1,1,1,2,3,3,3,3},
};
char Pattern_6[18][4] = {	// パターン６
	{2,2,0,0},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
};
char Pattern_7[18][4] = {	// パターン７
	{0,0,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
	{2,2,2,2},
};
char Textbox_window[6][SCREEN_WIDTH] = {	// テキストボックスのウィンドウ
	{5,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,6},
	{7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8},
	{7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8},
	{7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8},
	{7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8},
	{5,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,6},
};
char Map_window[5][5] = {	// ミニマップウィンドウ
	{5,4,4,4,6},
	{7,0,0,0,8},
	{7,0,0,0,8},
	{7,0,0,0,8},
	{5,4,4,4,6},
};

int termraw(int flag)
{
    static struct termios t_save;
           struct termios t;

    if( flag == 1 )
    {
        if( tcgetattr(0,&t) == -1 ) return -1;

        t_save = t;
        t.c_iflag = ~( BRKINT | ISTRIP | IXON  );
        t.c_lflag = ~( ICANON | IEXTEN | ECHO | ECHOE | ECHOK | ECHONL );
        t.c_cc[VMIN]  = 1;
        t.c_cc[VTIME] = 0;

        if( tcsetattr(0, TCSANOW, &t) == -1 ) return -1;

        return 0;

    }else{

        if( tcsetattr(0, TCSANOW, &t_save) == -1 ) return -1;
        return 0;

    }

}

static const char utf8_bytes[256] = {    
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0
};

char getch()
{
    char ch;
    termraw(TERM_RAW_ON);
    ch = getchar();
    termraw(TERM_RAW_OFF);
    return ch;
}

// ゲームっぽく文字列が出力される
void print_s(const char *str)
{
	// 2 バイト分格納する
	char ch2[3] = {0};
	
	// 何回繰り返すのか、与えられた文字列の長さを計測
	int size = strlen(str);
	
	// 2 バイトづつ出力、その際処理を少しだけ止める
	for (int i = 0; i < size; i += 2) {
		memcpy(ch2, &str[i], 2);	// オリジナルから 2 バイトコピー
		printf("%s", ch2);
		
		mySleep(8);		// 遅延
	}
}

// ゲームや操作の説明
void explain()
{
	system("clear");	// 画面クリア
	printf("\n迷宮を探索してお宝を手に入れましょう。\n家に帰るまでが迷宮探索です。\n\n");
	printf(" 基本操作方法\n");
	printf(" W : カーソルを上に動かす。フィールドで前に進む。\n");
	printf(" A : フィールドで左に90度回転する。\n");
	printf(" S : カーソルを下に動かす。\n");
	printf(" D : フィールドで右に90度回転する。\n");
	printf(" Enter : 決定ボタン。ウィンドウを呼び出したりする。\n");
	printf(" Back Space : キャンセルボタン。ウィンドウを閉じたりする。\n");
	printf(" Ctrl+C : ゲームの終了\n");
	
	printf("\nEnterキーを押して次に進みましょう。\n");
	printf("    ->");
	char input;
	do {
		input = getch();
	} while (input != ENTER);
}

// スタートメニュー難易度を選択できる
int start_menu()
{
	// ゲームや操作の説明
	explain();
	
	int select = 0;	// 選んでいる位置座標
	char *str1[] = {"EASY", "NORMAL", "HARD"};
	char *str2[] = {
		"マップの大きさは 10 × 10\n敵が弱く、フィールドの罠の数が少ない\n",
		"マップの大きさは 20 × 20\nしっかり考えないと死んでしまう\n",
		"マップの大きさは 30 × 30\n敵が強く、フィールドの罠が増加する\n"
	};
	
	// メインループ
	while (true) {
		system("clear");	// 画面クリア
		printf("\n難易度を選択してください。\n\n");
		
		// メニューの描画
		for (int i = 0; i < NUM_MODE; i++) {
			if (i == select) printf("->");
			printf(" %s\n", str1[i]);
		}
		
		// 難易度の説明
		putchar('\n');
		print_s(str2[select]);
		
		// 入力
		bool input_flag;	// 入力フラグ
		while (true) {
			input_flag = true;	// 初期化
			
			// プレイヤーが選ぶ
			char input = getch();
			
			// 難易度選択の確定
			if (input == ENTER) return select;
				
			// カーソルの移動
			else if (input == 'w') select--;	// カーソルを下に移動
			else if (input == 's') select++;	// カーソルを上に移動
			else input_flag = false;	// 入力失敗
			
			// 入力成功したら処理の続行
			if (input_flag) break;
		}
		
		// カーソルはループする
		if (select > NUM_MODE - 1) select = 0;
		else if (select < 0) select = NUM_MODE - 1;
	}
}

// 範囲指定をして乱数を取得
int get_scope_rand(int start, int last)
{
	int size = last - start + 1;
	return rand() % size + start;
}

// 範囲指定をして乱数を取得
Stage_state get_object_rand(Stage_state start, Stage_state last)
{
	int size = last - start + 1;
	return (enum STAGE_STATE)(rand() % size + (int)start);
}

// 基準値の前後の乱数を取得
int get_basic_rand(int num)
{
	return get_scope_rand(num - 2, num + 3);
}

// ステージに設置できる座標でのランダムな値を返す
int get_pos_rand(char start, char last)
{
	char num = get_scope_rand(start, last);
	// 奇数を返す
	if (num % 2 == 1) return num;
	else num++;
	return num;
}

// イージーモードでのオブジェクト配置
Stage_state set_easy()
{	
	// オブジェクトが配置されるかどうか
	int num = get_scope_rand(1, 100);
	if (num > 15) return Space;	// 確率 85 %で何も設置しない
	
	// 選ぶオブジェクト
	Stage_state object;
	
	// 色付きタイルの配列
	Stage_state color[COLOR_NUM] = {Red, Green, Blue, White};
	static char color_num = 0;
	
	// オブジェクトの選択
	num = get_scope_rand(1, 100);
	// 色付きタイル
	if (num <= 20) {	// 確率 20 %
		// 色付きタイルをすべて使っている場合はオブジェクトなし
		if (color_num >= COLOR_NUM) return Space;
		
		object = color[color_num];
		color_num++;
	}
	// 宝箱
	else if (num <= 60) object = Treasure;	// 確率 40 %
	// 敵
	else if (num <= 99) object = Enemy;	// 確率 39 %
	// 罠
	else if (num == 100) object = get_object_rand(Rotate, Arrow);	// 確率 1 %
	
	return object;
}

// ノーマルモードでのオブジェクト配置
Stage_state set_nomal()
{
	// オブジェクトが配置されるかどうか
	int num = get_scope_rand(1, 100);
	if (num > 15) return Space;	// 確率 85 %で何も設置しない
	
	// 選ぶオブジェクト
	Stage_state object;
	
	// 色付きタイルの配列
	Stage_state color[COLOR_NUM] = {Red, Green, Blue, White};
	static char color_num = 0;
	
	// オブジェクトの選択
	num = get_scope_rand(1, 100);
	// 色付きタイル
	if (num <= 20) {	// 確率 20 %
		// 色付きタイルをすべて使っている場合はオブジェクトなし
		if (color_num >= COLOR_NUM) return Space;
		
		object = color[color_num];
		color_num++;
	}
	// 宝箱
	else if (num <= 50) object = Treasure;	// 確率 30 %
	// 敵
	else if (num <= 80) object = Enemy;	// 確率 30 %
	// 罠
	else if (num <= 100) object = get_object_rand(Rotate, Arrow);	// 確率 20 %
	
	return object;
}

// ハードモードでのオブジェクト配置
Stage_state set_hard()
{
	// オブジェクトが配置されるかどうか
	int num = get_scope_rand(1, 100);
	if (num > 10) return Space;	// 確率 90 %で何も設置しない
	
	// 選ぶオブジェクト
	Stage_state object;
	
	// 色付きタイルの配列
	Stage_state color[COLOR_NUM] = {Red, Green, Blue, White};
	static char color_num = 0;
	
	// オブジェクトの選択
	num = get_scope_rand(1, 100);
	// 色付きタイル
	if (num <= 10) {	// 確率 10 %
		// 色付きタイルをすべて使っている場合はオブジェクトなし
		if (color_num >= COLOR_NUM) return Space;
		
		object = color[color_num];
		color_num++;
	}
	// 宝箱
	else if (num <= 50) object = Treasure;	// 確率 40 %
	// 敵
	else if (num <= 60) object = Enemy;	// 確率 10 %
	// 罠
	else if (num <= 100) object = get_object_rand(Rotate, Arrow);	// 確率 40 %
	
	return object;
}

// スタート地点とゴール地点、またはアイテムの設置
void set_object(const int mode)
{
	// 関数のポインタを作成
	fp_modes select_object[NUM_MODE] = {
		set_easy,	// 0 イージーモードでの処理
		set_nomal,	// 1 ノーマルモードでの処理
		set_hard	// 2 ハードモードでの処理
	};
	
	// いちますの部屋を作成
	for (int i = 1; i < Stage_size - 1; i+=2) {
		for (int j = 1; j < Stage_size - 1; j+=2) {
			Stage[i][j] = select_object[mode]();	// 変数 i j が奇数のところに部屋を作成
		}
	}
	
	// スタート地点は左上の端で固定
	Stage[1][1] = Start;
	
	// ゴール地点は右下付近でランダム配置
	char goal_x = get_pos_rand(Stage_size - 5, Stage_size - 2);
	char goal_y = get_pos_rand(Stage_size - 5, Stage_size - 2);
	Stage[goal_y][goal_x] = Goal;
}

// その座標に通路を作ることができるか調べる
bool check_create_way(const Room *room, int addx, int addy)
{
	// 座標を算出
	unsigned char x = room->x + addx;
	unsigned char y = room->y + addy;
	
	// 壁があるか調べる
	if (Stage[y][x] != Wall) {
		return false;
	}
	// アウトラインでないか調べる
	if (x == 0 || x == Stage_size - 1 || y == 0 || y == Stage_size - 1) {
		return false;
	}
	
	// つなげる部屋の座標を算出
	x = room->x + (2 * addx);
	y = room->y + (2 * addy);
	
	// つなげる部屋がどの部屋とも接続されていないか調べる
	if (Stage[y + 1][x] != Wall) return false;
	if (Stage[y - 1][x] != Wall) return false;
	if (Stage[y][x + 1] != Wall) return false;
	if (Stage[y][x - 1] != Wall) return false;
	
	// 数々の厳しい審査を抜けたものだけ
	return true;
}

// 部屋をランダムに決める
bool set_room(Room *candi, Room *final_candi, Room *room, const int room_num)
{
	unsigned char candi_num = 0;	// 候補の数
	
	// 初期化
	for (int i = 0; i < 256; i++) {
		candi[i].way = 0;
	}
	
	// ほかの部屋と接続している部屋を割り出す
	for (int i = 1; i < Stage_size - 1; i+=2) {
		for (int j = 1; j < Stage_size - 1; j+=2) {
			// 四方に壁がない場合カウントアップ
			if (Stage[i + 1][j] != Wall)	candi[candi_num].way++;
			if (Stage[i - 1][j] != Wall)	candi[candi_num].way++;
			if (Stage[i][j + 1] != Wall)	candi[candi_num].way++;
			if (Stage[i][j - 1] != Wall)	candi[candi_num].way++;
			
			// 候補に選ばれた部屋のデータを収集
			if (candi[candi_num].way) {
				candi[candi_num].x = j;
				candi[candi_num].y = i;
				candi_num++;
			}
		}
	}
	// 最初は接続している部屋がないため
	if (candi_num == 0) {
		room->x = 1, room->y = 1;	// 初期化
		return false;
	}
	// すべての部屋が接続さている場合
	if (candi_num == room_num) {
		return true;
	}
	
	// 候補を走査して優先度の高い接続数を割り出す
	char priority = PRIORY_MAX;	// 優先する接続数
	for (int i = 0; i < candi_num; i++) {
		// 優先度の更新
		if (candi[i].way < priority) {
			priority = candi[i].way;
		}
	}
	
	// 優先度の高いもので接続できる部屋をみつける
	// 見つからない場合、優先度を下げてもう一度処理
	bool conect_flag;
	unsigned char final_num;
	while (true) {
		// 初期化
		final_num = 0;
		
		for (int i = 0; i < candi_num; i++) {
			//printf("final_num++ : %d\n", final_num);
			// 優先度の低いものは処理なし
			if (candi[i].way != priority) continue;
			
			// 接続候補があるか四方を調べる
			conect_flag = false;
			if (check_create_way(&candi[i], 1, 0))			conect_flag = true;
			else if (check_create_way(&candi[i], -1, 0))	conect_flag = true;
			else if (check_create_way(&candi[i], 0, 1))		conect_flag = true;
			else if (check_create_way(&candi[i], 0, -1))	conect_flag = true;
			
			// 最終候補を先頭から入れていく、元からそこにあるデータは入れ替え
			if (conect_flag) {
				final_candi[final_num].x = candi[i].x;
				final_candi[final_num].y = candi[i].y;
				final_num++;
			}
		}
		
		// 候補が見つかった場合
		if (final_num) break;
		else priority++;
	}
	
	// 最終候補からランダムに選ぶ
	unsigned char num = rand() % final_num;
	room->x = final_candi[num].x;
	room->y = final_candi[num].y;
	return false;
}

// 接続する方向を決める
void set_direct(const Room *room)
{
	unsigned char dir[4];	// 東西南北の方向カード
	unsigned char dir_num;	// 残っている方向カードの数
	bool dir_flag;	// 方向を決めるかのフラグ
	
	// 初期化
	for (int i = 0; i < 4; i++) {
		dir[i] = i;
	}
	dir_num = 0;
	dir_flag = true;
	
	// 方向のカードをランダムに入れ替えてシャッフル
	for (int i = 0; i < 4; i++) {
		unsigned char tmp, p1, p2;
		p1 = rand() % 4;
		p2 = rand() % 4;
		tmp = dir[p1];
		dir[p1] = dir[p2];
		dir[p2] = tmp;
	}
	
	// 方向が決まるまでループ
	while (dir_flag) {
		dir_flag = false;	// ふつうは一度で方向を決める
		
		// シャッフルカードを上から順番に引いていく
		unsigned char way = dir[dir_num];
		
		if (way == EAST) {
			// その座標に通路を作ることができるか調べる
			if (check_create_way(room, 1, 0)) {
				Stage[room->y][room->x + 1] = Space;
			}
			else dir_flag = true;	// もう一度方向を決める
		}
		else if (way == WEST) {
			// その座標に通路を作ることができるか調べる
			if (check_create_way(room, -1, 0)) {
				Stage[room->y][room->x - 1] = Space;
			}
			else dir_flag = true;	// もう一度方向を決める
		}
		else if (way == NORTH) {
			//printf("N\n");
			// その座標に通路を作ることができるか調べる
			if (check_create_way(room, 0, 1)) {
				Stage[room->y + 1][room->x] = Space;
				//printf("NORTH\n");
			}
			else dir_flag = true;	// もう一度方向を決める
		}
		else if (way == SOUTH) {
			// その座標に通路を作ることができるか調べる
			if (check_create_way(room, 0, -1)) {
				Stage[room->y - 1][room->x] = Space;
			}
			else dir_flag = true;	// もう一度方向を決める
		}
		
		dir_num++;	// カウントアップ、カードを引く
		
		/*
		if (dir_num >= 5) {
			printf("//Errer//\n");
			break;
		}
		*/
	}
}

// ステージの作成
void create_stage(const int mode)
{
	system("cls");
	printf("\n Creating Stage...\n");
	
	// 乱数表初期化
	srand((unsigned)time(NULL));
	
	// ステージのメモリ領域を獲得どうじにマップも
	// bool 型に変更　可能
	Stage = (Stage_state **)malloc(sizeof(Stage_state *) * Stage_size);
	Map_white = (char **)malloc(sizeof(char *) * Stage_size);
	for (int i = 0; i < Stage_size; i++) {
		Stage[i] = (Stage_state *)malloc(sizeof(Stage_state) * Stage_size);
		Map_white[i] = (char *)malloc(sizeof(char) * Stage_size);
	}
	
	// 初期化
	for (int i = 0; i < Stage_size; i++) {
		for (int j = 0; j < Stage_size; j++) {
			Stage[i][j] = Wall;
			Map_white[i][j] = 0;
		}
	}
	
	// 部屋やスタート、ゴール、罠などのオブジェクト配置
	set_object(mode);
	
	// 部屋どうしをつなげていく
	Room candi[256];		// 候補の部屋
	Room final_candi[256];	// 最終候補
	Room room;				// 選択している部屋のデータ
	bool end_flag;			// ステージ作成終了フラグ
	
	// できる部屋の数を算出
	int room_num = (Stage_size / 2) * (Stage_size / 2);
	
	while (true) {
		// 部屋を決める
		end_flag = set_room(candi, final_candi, &room, room_num);
		
		// 終了
		if (end_flag) break;
		
		// 方向を決める
		set_direct(&room);
	}
}

// ステージとマップのメモリ領域の開放
void free_stage_map()
{
	for (int i = 0; i < Stage_size; i++) {
		free(Stage[i]);
		free(Map_white[i]);
	}
	free(Stage);
	free(Map_white);
}

// ステージの表示 デバッグ用
/*void print_stage(Player *player)
{
	//system("cls");
	int data;
	for (int i = 0; i < Stage_size; i++) {
		for (int j = 0; j < Stage_size; j++) {
			if (player->x == j && player->y == i) {
				printf("●");
				continue;
			}
			data = Stage[i][j];
			printf("%s", Map_material[data]);
		}
		putchar('\n');
	}
	//getch();
}*/

// 3D描画するためにプレイヤー中心にステージのデータを調べる
void check_wall_3D(Player *player, bool *walls)
{
	// 壁があるか 8 つの座標から探索
	Pos south[8] = {	// 北向きの場合のプレイヤーの座標を中心に調べる座標のテーブル
		{-1, -2},{0, -2},{1, -2},
		{-1, -1},{0, -1},{1, -1},
		{-1, 0},         {1, 0}
	};
	Pos east[8] = {	// 東向きの場合のプレイヤーの座標を中心に調べる座標のテーブル
		{2, -1},{2, 0},{2, 1},
		{1, -1},{1, 0},{1, 1},
		{0, -1},       {0, 1}
	};
	Pos north[8] = {	// 南向きの場合のプレイヤーの座標を中心に調べる座標のテーブル
		{1, 2},{0, 2},{-1, 2},
		{1, 1},{0, 1},{-1, 1},
		{1, 0},       {-1, 0}
	};
	Pos west[8] = {	// 西向きの場合のプレイヤーの座標を中心に調べる座標のテーブル
		{-2, 1},{-2, 0},{-2, -1},
		{-1, 1},{-1, 0},{-1, -1},
		{0, 1},         {0, -1}
	};
	// ポインタの配列にまとめる
	Pos *search[4] = {
		south,	// 0 北
		east,	// 1 東
		north,	// 2 南
		west,	// 3 西
	};
	
	char x, y;	// x座標、y座標
	for (int i = 0; i < 8; i++) {
		// 座標を算出
		x = player->x + search[player->dir][i].x;
		y = player->y + search[player->dir][i].y;
		
		// 調べる座標がアウトラインをこえていないか調べる
		if (x < 0 || x >= Stage_size || y < 0 || y >= Stage_size) {
			continue;	// 調べるのをやめる
		}
		
		// その座標に壁があるか調べる
		if (Stage[y][x] == Wall) walls[i] = true;
	}
}

// プレイヤーの視界に写るデータからスクリーンに描画
void draw_3D(Player *player)
{
	/* 3D描画するために壁があるかないかの情報が 8 つの座標から必要
	0 1 2
	3 4 5
	6 ▲ 7
	*/
	bool walls[8] = {0};	// プレイヤーの座標と向きから必要な情報を格納する
	
	// 3D描画するのに必要な情報を取得する
	check_wall_3D(player, walls);
	
	// スクリーンを作成
	// 通路を描画してスクリーンを初期化
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		for (int j = 0; j < SCREEN_WIDTH; j++) {
			Screen[i][j] = Way[i][j];
		}
	}
	// 壁の描画
	// 描画する順番に注意！！！
	if (walls[0]) {
		for (int i = 0; i < 7; i++) {
			for (int j = 0; j < 10; j++) {
				Screen[i + 5][j + 3] = Pattern_0[i][j];
			}
		}
	}
	if (walls[2]) {
		for (int i = 0; i < 7; i++) {
			for (int j = 0; j < 10; j++) {
				Screen[i + 5][j + 19] = Pattern_2[i][j];
			}
		}
	}
	if (walls[1]) {
		for (int i = 0; i < 7; i++) {
			for (int j = 0; j < 12; j++) {
				Screen[i + 5][j + 10] = Pattern_1[i][j];
			}
		}
	}
	if (walls[3]) {
		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 10; j++) {
				Screen[i + 2][j] = Pattern_3[i][j];
			}
		}
	}
	if (walls[5]) {
		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 10; j++) {
				Screen[i + 2][j + 22] = Pattern_5[i][j];
			}
		}
	}
	if (walls[4]) {
		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 24; j++) {
				Screen[i + 2][j + 4] = Pattern_4[i][j];
			}
		}
	}
	if (walls[6]) {
		for (int i = 0; i < 18; i++) {
			for (int j = 0; j < 4; j++) {
				Screen[i][j] = Pattern_6[i][j];
			}
		}
	}
	if (walls[7]) {
		for (int i = 0; i < 18; i++) {
			for (int j = 0; j < 4; j++) {
				Screen[i][j + 28] = Pattern_7[i][j];
			}
		}
	}
}

// だたスクリーンのデータを表示、グラフィックのみ
void print_screen_graphic()
{
	// カーソル位置初期化
	move_cursor_console(1, 1);
	
	puts("+----------------------------------------------------------------+");	// 枠
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		putchar('|');	// 枠
		for (int j = 0; j < SCREEN_WIDTH; j++) {
			// グラフィックのみ
			char data = Screen[i][j];	// グラフィックデータを登録
			printf("%s", Screen_material[data]);	// グラフィック出力
		}
		putchar('|');	// 枠
		putchar('\n');	// 改行
	}
	puts("+----------------------------------------------------------------+");	// 枠
}

// だたスクリーンのデータを表示、基本
void print_screen_basic()
{
	// カーソル位置初期化
	move_cursor_console(1, 1);
	
	puts("+----------------------------------------------------------------+");	// 枠
	
	// 2 バイト分格納して出力する
	char ch[3] = {0};
	
	// スクリーンを表示
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		putchar('|');	// 枠
		for (int j = 0; j < SCREEN_WIDTH; j++) {
			// 文字
			if (Screen_str[i][j * 2]) {
				memcpy(ch, &Screen_str[i][j * 2], 2);	// 2 バイトコピー
				printf("%s", ch);	// 文字出力
				continue;
			}
			
			// グラフィック
			char data = Screen[i][j];	// グラフィックデータを登録
			printf("%s", Screen_material[data]);	// グラフィック出力
		}
		putchar('|');	// 枠
		putchar('\n');	// 改行
	}
	puts("+----------------------------------------------------------------+");	// 枠
}

// プレイヤーの目の前の座標を返す
Pos get_pos_advance(const Player *player)
{
	// プレイヤーの向きから進むためのテーブル
	Pos table[4] = {
		{0, -1},	// 北向きに進む
		{1, 0},		// 東向きに進む
		{0, 1},		// 南向きに進む
		{-1, 0}		// 西向きに進む
	};
	
	Pos adv;
	adv.x = player->x + table[player->dir].x;
	adv.y = player->y + table[player->dir].y;
	
	return adv;
}

// プレイヤーの座標前に進ませる
void set_pos_advance(Player *player)
{
	Pos p = get_pos_advance(player);
	player->x = p.x;
	player->y = p.y;
}

// プレイヤーの座標を中心に東西南北に壁があるか調べる
bool check_wall(const Player *player)
{
	// 調べる座標を算出
	Pos p = get_pos_advance(player);
	
	// その座標に壁があるなら false
	if (Stage[p.y][p.x] == Wall) return false;
	else return true;
}

// 指定の座標に指定の大きさのウィンドウを描画、グラフィックのみ
void draw_window_graphic(char x, char y, char width, char height)
{
	// 0    4    5    6    7    8
	// "  " "--" "+-" "-+" "| " " |"
	
	char data;	// ウィンドウを作成するために渡すデータ
	
	// ウィンドウ描画
	for (int i = 0; i < height + 2; i++) {
		for (int j = 0; j < width + 2; j++) {
			// 最初と最後の行
			if (i == 0 || i == height + 1) {
				if (j == 0) data = 5;	// 左端
				else if (j == width + 1) data = 6;	// 右端
				else data = 4;	// それ以外
			}
			
			// 間の行
			else {
				if (j == 0) data = 7;	// 左端
				else if (j == width + 1) data = 8;	// 右端
				else data = 0;	// それ以外
			}
			
			Screen[y + i][x + j] = data;
			
			// ウィンドウと重なる文字を削除
			Screen_str[y + i][(x + j) * 2] = 0;
		}
	}
}

// 指定した座標に文字を描画
void draw_string(char x, char y, const char *str)
{
	int size = strlen(str);
	memcpy(&Screen_str[y][x * 2], str, size);
}

// 文字専用のスクリーンを初期化
void init_string()
{
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		for (int j = 0; j < SCREEN_WIDTH * 2; j++) {
			Screen_str[i][j] = 0;
		}
	}
}

// テキストボックスをスクリーンに描画
void draw_textbox()
{
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < SCREEN_WIDTH; j++) {
			// コピー
			//Cp_textbox[i][j] = Screen[i + 12][j];
			
			// 上書き
			Screen[i + 12][j] = Textbox_window[i][j];
		}
	}
}

// スクリーンにテキストボックスを表示する
void update_textbox()
{
	// カーソルはコンソール画面の座標であることに注意！
	// テキストボックスの描画範囲にカーソルを設定
	for (int i = 0; i < TEXTBOX_LINE; i++) {
		// カーソル移動
		move_cursor_console(4,15 + i);
		
		// テキスト出力
		printf("%s", &Textbox[i][0]);
	}
}

// テキストボックスのデータを初期化
void init_textbox()
{
	for (int i = 0; i < TEXTBOX_LINE; i++) {
		for (int j = 0; j < 64; j++) {
			if (j < 60) Textbox[i][j] = ' ';
			else Textbox[i][j] = 0;
		}
	}
}

// テキストボックスの書き込み可能な行を返す
int get_line_textbox()
{
	// 最後尾から探索
	for (int i = TEXTBOX_LINE - 1; i >= 0; i--) {
		for (int j = 63; j >= 0; j--) {
			// 文字を線形探索
			if (Textbox[i][j] != 0 && Textbox[i][j] != ' ') {
				return i + 1;	// 見つけた次の行を返す
			}
		}
	}
	return 0;	// テキストボックスに履歴なし
}

// テキストボックス内の文字を上に詰める
void set_textbox()
{
	// 下の行を上の行に上書き
	for (int i = 0; i < TEXTBOX_LINE - 1; i++) {
		memcpy(&Textbox[i][0], &Textbox[i + 1][0], 64);
	}
	// 一番下の行をクリア
	for (int i = 0; i < 64; i++) {
		if (i < 60) Textbox[TEXTBOX_LINE - 1][i] = ' ';
		else Textbox[TEXTBOX_LINE - 1][i] = 0;
	}
} 

// コピーを使ってテキストボックスを消す
/*void delete_textbox()
{
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < SCREEN_WIDTH; j++) {
			// 上書き
			Screen[i + 12][j] = Cp_textbox[i][j];
		}
	}
}*/

// テキストボックスの範囲の文字スクリーンを初期化
void init_textbox_str()
{
	for (int i = 0; i < TEXTBOX_LINE; i++) {
		draw_string(1, 13 + i, "                                                            ");
	}
}

// 文字をテキストボックスに少しづつ出力
void print_textbox(const char *str)
{
	// テキストボックスの描画
	draw_textbox();
	
	// テキストボックスの範囲の文字スクリーンを初期化
	init_textbox_str();
	
	// スクリーンを表示
	print_screen_basic();
	
	// 文字数
	int size = strlen(str);
	
	// 書き込める行を取得
	int line_w = get_line_textbox();
	
	// テキストボックスに空きの行が存在しない場合
	if (line_w == TEXTBOX_LINE) {
		set_textbox();
		line_w--;
	}
	
	// オリジナルを少しづつコピーして表示を繰り返してテキストボックスに文字が少しづつ出力されているように見せる
	// 文字数繰り返す
	for (int i = 0, j = 0, k = line_w; i < size; i+=2, j+=2) {
		// テキストボックスの作成
		if (j == TEXTBOX_WIDTH) {	// 文字数がテキストボックスの幅最大になった場合
			k++;	// 次の行に移動
			j = 0;	// 初期化
			
			// テキストボックスに空きの行が存在しない場合
			if (k == TEXTBOX_LINE) {
				set_textbox();
				k--;
			}
		}
		memcpy(&Textbox[k][j], &str[i], 2);	// ループごとに 2 バイトづつコピー
		
		// テキストボックスの画面更新
		update_textbox();
		
		// 遅延
		mySleep(16);
	}
}

// 現在のテキストボックスを文字スクリーンに描画できる
void draw_textbox_str()
{
	for (int i = 0; i < TEXTBOX_LINE; i++) {
		draw_string(1, 13 + i, &Textbox[i][0]);
	}
}

// テキストボックスEnterキー押すまで表示
void enter_textbox()
{
	// 書き込める行を取得
	int line_w = get_line_textbox();
	
	// テキストボックスに空きの行が存在しない場合
	if (line_w == TEXTBOX_LINE) {
		set_textbox();
		line_w--;
	}
	
	// Enter
	char enter[] = {'-','>',' ','E','n','t','e','r'};
	char del[] = {' ',' ',' ',' ',' ',' ',' ',' '};
	
	// テキストボックス更新
	memcpy(&Textbox[line_w][0], enter, 8);
	
	// テキストボックスの画面更新
	update_textbox();
	
	// Enterを消す
	memcpy(&Textbox[line_w][0], del, 8);
	
	// コピーを使ってテキストボックスを消す
	//delete_textbox();
	
	// Enter押すまでループ
	while (true) {
		char ch = getch();
		if (ch == ENTER) break;
	}
	
	// 描画
	draw_textbox_str();
	
	// 画面表示
	print_screen_basic();
}

// ダメージ演出、フラッシュ
void print_damage_flash()
{	
	// 文字スクリーン初期化
	init_string();

	// 交互に繰り返してフラッシュ
	for (int k = 0; k < 5; k++) {
		// 白背景
		move_cursor_console(1, 1);	// カーソル位置初期化
		puts("+----------------------------------------------------------------+");	// 枠
		for (int i = 0; i < SCREEN_HEIGHT; i++) {
			puts("|■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■|");
		}
		puts("+----------------------------------------------------------------+");	// 枠
		mySleep(64);
		
		// 通常画面
		print_screen_basic();
		mySleep(32);
	}
}

// マップ初期化
void init_map_white()
{
	for (int i = 0; i < Stage_size; i++) {
		for (int j = 0; j < Stage_size; j++) {
			Map_white[i][j] = 0;
		}
	}
}

// スキル初期化
void init_skill(Player *player)
{
	for (int i = 0; i < 5; i++) {
		player->skill[i] = (Skill)i;
	}
}

// カーソル構造体初期化
void init_cursor(Player *player)
{
	for (int i = 0; i < 8; i++) {
		player->basic[i].x = 0;
		player->basic[i].y = 0;
		player->basic[i].num = 0;
		player->basic[i].start_y = 0;
		player->basic[i].last_y = 0;
		player->basic[i].history = 0;
		player->basic[i].draw_flag = true;
	}
}

// 変数の初期化処理
void init(int mode, Player *player)
{
	// ステージの大きさ、選んだ難易度によって違う
	Stage_size = (mode + 1) * 10 + 1;
	player->mode = mode;
	player->x = 1;	// 初期座標 x
	player->y = 1;	// 初期座標 y
	player->dir = EAST;	// 最初に向いている方向、東
	player->hp = START_HP;	// 初期 HP 20
	player->mp = START_MP;	// 初期 MP 0
	player->hp_max = START_HP;	// 初期 HP 20
	player->mp_max = START_MP;	// 初期 MP 0
	player->exp = 0;	// 経験値
	player->gold = 20;	// お金
	player->level = 1;	// レベル
	player->power = START_POWER;	// 初期ちから 4
	player->physical = 0;
	player->attack = START_POWER;	// 攻撃力
	player->defense = 0;	// 防御力
	player->item_num = 1;	// アイテムの個数
	player->item[0] = None;	// 0 番目はアイテムなし用
	player->item[1] = Herb;	// やくそう
	player->weapon = 0;	// 装備している武器
	player->armor = 0;	// 装備している防具
	player->skill_num = 0;	// スキルの数
	player->control = FIELD;	// フィールド操作
	player->sel_cursor = 0;	// 選んでいるカーソル
	player->anger_debuff = false;	// 挑発デバフ
	player->hypnos_debuff = false;	// 催眠デバフ
	player->count = 0;	// カウンター
	player->draw_window_flag = false;	// ウィンドウを描画
	//player->use_cursor_flag = true;	// カーソル描画
	player->process_flag = false;	// 内部処理のフラグ
	//player->print_screen_flag = true;	// どうしても画面更新をしたくない
	//player->monster_dead_flag = false;	// モンスター死亡フラグ
	player->end_flag = false;		// ゲーム終了フラグ
	player->game_over_flag = false;	// ゲームオーバー
	
	// スキル初期化
	init_skill(player);
	
	// カーソル初期化
	init_cursor(player);
	
	// 文字専用スクリーン
	init_string();
	
	// テキストボックス
	init_textbox();
	
	// 画面クリア
	system("clear");
}

// はいかいいえの二択ウィンドウを描画
void draw_yes_no_window()
{
	// ウィンドウを描画
	draw_window_graphic(26, 8, 4, 2);
	
	// 内容を描画
	draw_string(27, 9, "　はい");
	draw_string(27, 10, "　いいえ");
}

// プレイヤーの簡易ステータスウィンドウを描画
void draw_simple_status_window(Player *player)
{
	// 簡易ステータスを描画
	draw_window_graphic(23, 2, 6, 5);
	
	// ウィンドウタイトルを描画
	draw_string(25, 2, "プレイヤ");
	
	// 内容を描画
	sprintf(Text, "レベル：　%2d", player->level);
	draw_string(24, 3, Text);
	sprintf(Text, "ＨＰ：%6d", player->hp);
	draw_string(24, 4, Text);
	sprintf(Text, "ＭＰ：%6d", player->mp);
	draw_string(24, 5, Text);
	sprintf(Text, "Ｇ：%8d", player->gold);
	draw_string(24, 6, Text);
	sprintf(Text, "Ｅ：%8d", player->exp);
	draw_string(24, 7, Text);
}

// 基本的な最初のウィンドウを描画
void draw_start_window_menu(Player *player)
{
	//簡易ステータスを描画
	draw_simple_status_window(player);
	
	// コマンドを描画
	draw_window_graphic(1, 1, 6, 3);
	
	// ウィンドウタイトルを描画
	draw_string(3, 1, "コマンド");
	
	// 内容を描画
	draw_string(2, 2, "　ステータス");
	draw_string(2, 3, "　アイテム");
	draw_string(2, 4, "　スキル");
}

// バトルモードでのスタートウィンドウを描画
void draw_start_window_battle(Player *player)
{
	//簡易ステータスを描画
	draw_simple_status_window(player);
	
	// ウィンドウを描画
	draw_window_graphic(1, 1, 5, 4);
	
	// ウィンドウタイトルを描画
	draw_string(2, 1, "コマンド");
	
	// 内容を描画
	draw_string(2, 2, "　たたかう");
	draw_string(2, 3, "　スキル");
	draw_string(2, 4, "　アイテム");
	draw_string(2, 5, "　にげる");
}

// タウンモードでのスタートウィンドウを描画
void draw_start_window_town(Player *player)
{
	//簡易ステータスを描画
	draw_simple_status_window(player);
	
	// ウィンドウを描画
	draw_window_graphic(1, 1, 5, 4);
	
	// ウィンドウタイトルを描画
	draw_string(3, 1, "おみせ");
	
	// 内容を描画
	draw_string(2, 2, "　やどや");
	draw_string(2, 3, "　どうぐや");
	draw_string(2, 4, "　ぶきや");
	draw_string(2, 5, "　ぼうぐや");
}

// メニューモードでのスタートウィンドウのカーソル設定
void set_start_cursor_menu(Player *player)
{
	// スタートウィンドウのカーソル
	player->sel_cursor = 0;
	player->basic[0].x = 2;
	player->basic[0].y = 2;
	player->basic[0].num = 0;
	player->basic[0].start_y = 2;
	player->basic[0].last_y = 4;
	player->basic[0].history = 3;
	player->basic[0].draw_flag = true;
}

// バトルモードでのスタートウィンドウのカーソル設定
void set_start_cursor_battle(Player *player)
{
	// スタートウィンドウのカーソル
	player->sel_cursor = 0;
	player->basic[0].x = 2;
	player->basic[0].y = 2;
	player->basic[0].num = 0;
	player->basic[0].start_y = 2;
	player->basic[0].last_y = 5;
	player->basic[0].history = 3;
	player->basic[0].draw_flag = true;
}

// タウンモードでのスタートウィンドウのカーソル設定
void set_start_cursor_town(Player *player)
{
	player->sel_cursor = 0;
	player->basic[0].x = 2;
	player->basic[0].y = 2;
	player->basic[0].num = 0;
	player->basic[0].start_y = 2;
	player->basic[0].last_y = 5;
	player->basic[0].history = 3;
	player->basic[0].draw_flag = true;
}

// カーソル描画
void draw_cursor(Player *player)
{
	// 矢印
	char arrow[2] = {'-','>'};
	char del[2] = {0};
	
	// 使っているカーソルの数だけ描画
	for (int i = 0; i <= player->sel_cursor; i++) {
		// カーソルを描画するかどうか
		if (player->basic[i].draw_flag == false) continue;
		
		// カーソルの座標を登録
		char x = player->basic[i].x;
		char y = player->basic[i].y;
		
		// 現在使用しているカーソルのひとつ前を削除
		if (i == player->sel_cursor) {
			// カーソルを表示しない
			//if (player->use_cursor_flag == false) return;
			
			// 履歴を確認して前の矢印を削除
			char h = player->basic[player->sel_cursor].history;
			memcpy(&Screen_str[h][x * 2], del, 2);
		}
		
		// 2 バイトだけコピーして隣の文字に影響をおよばさない
		memcpy(&Screen_str[y][x * 2], arrow, 2);
	}
}

// メニューモードカーソル設定
void set_cursor_menu(Player *player)
{
	// 二枚目ウィンドウ
	if (player->sel_cursor == 1) {
		// ステータスウィンドウ
		if (player->basic[0].num == 0) {
			player->basic[1].x = 0;
			player->basic[1].y = 0;
			player->basic[1].num = 0;
			player->basic[1].start_y = 0;
			player->basic[1].last_y = 0;
			player->basic[1].history = 0;
			player->basic[1].draw_flag = false;
		}
		
		// アイテムウィンドウ
		else if (player->basic[0].num == 1) {
			player->basic[1].x = 6;
			player->basic[1].y = 1;
			player->basic[1].num = 0;
			player->basic[1].start_y = 1;
			player->basic[1].last_y = player->item_num;
			player->basic[1].history = 1;
			player->basic[1].draw_flag = true;
		}
		
		// スキルウィンドウ
		else if (player->basic[0].num == 2) {
			player->basic[1].x = 5;
			player->basic[1].y = 1;
			player->basic[1].num = 0;
			player->basic[1].start_y = 1;
			player->basic[1].last_y = player->skill_num;
			player->basic[1].history = 1;
			player->basic[1].draw_flag = true;
		}
	}
	
	// 三枚目のウィンドウ
	else if (player->sel_cursor == 2) {
		// アイテムの行動ウィンドウ
		if (player->basic[0].num == 1) {
			player->basic[2].x = 12;
			player->basic[2].y = 2;
			player->basic[2].num = 0;
			player->basic[2].start_y = 2;
			player->basic[2].last_y = 4;
			player->basic[2].history = 3;
			player->basic[2].draw_flag = true;
		}
	}
}

// バトルモードでのカーソル設定
void set_cursor_battle(Player *player)
{
	// 二枚目ウィンドウ
	if (player->sel_cursor == 1) {
		// モンスターを選ぶ
		if (player->basic[0].num == 0) {
			player->basic[1].x = 21;
			player->basic[1].y = 8;
			player->basic[1].num = 0;
			player->basic[1].start_y = 8;
			player->basic[1].last_y = player->monst_num + 7;
			player->basic[1].history = 8;
			player->basic[1].draw_flag = true;
		}
		
		// スキルウィンドウ
		else if (player->basic[0].num == 1) {
			player->basic[1].x = 5;
			player->basic[1].y = 1;
			player->basic[1].num = 0;
			player->basic[1].start_y = 1;
			player->basic[1].last_y = player->skill_num;
			player->basic[1].history = 1;
			player->basic[1].draw_flag = true;
		}
		
		// アイテムウィンドウ
		else if (player->basic[0].num == 2) {
			player->basic[1].x = 6;
			player->basic[1].y = 1;
			player->basic[1].num = 0;
			player->basic[1].start_y = 1;
			player->basic[1].last_y = player->item_num;
			player->basic[1].history = 1;
			player->basic[1].draw_flag = true;
		}
	}
	
	// 三枚目のウィンドウ
	else if (player->sel_cursor == 2) {
		// モンスターを選ぶ
		player->basic[2].x = 21;
		player->basic[2].y = 8;
		player->basic[2].num = 0;
		player->basic[2].start_y = 8;
		player->basic[2].last_y = player->monst_num + 7;
		player->basic[2].history = 8;
		player->basic[2].draw_flag = true;
	}
}

// タウンモードでのカーソル設定
void set_cursor_town(Player *player)
{
	// 二枚目ウィンドウ
	if (player->sel_cursor == 1) {
		// 宿屋イエスノー
		if (player->basic[0].num == 0) {
			player->basic[1].x = 27;
			player->basic[1].y = 9;
			player->basic[1].num = 0;
			player->basic[1].start_y = 9;
			player->basic[1].last_y = 10;
			player->basic[1].history = 9;
			player->basic[1].draw_flag = true;
		}
		
		// どうぐリスト
		else if (player->basic[0].num == 1) {
			player->basic[1].x = 5;
			player->basic[1].y = 1;
			player->basic[1].num = 0;
			player->basic[1].start_y = 1;
			player->basic[1].last_y = 4;
			player->basic[1].history = 1;
			player->basic[1].draw_flag = true;
		}
		
		// 武器防具リスト
		else if (player->basic[0].num >= 2) {
			player->basic[1].x = 5;
			player->basic[1].y = 1;
			player->basic[1].num = 0;
			player->basic[1].start_y = 1;
			player->basic[1].last_y = 3;
			player->basic[1].history = 1;
			player->basic[1].draw_flag = true;
		}
	}
	
	// 三枚目のウィンドウ
	else if (player->sel_cursor == 2) {
		if (player->basic[0].num >= 1) {
			player->basic[2].x = 27;
			player->basic[2].y = 9;
			player->basic[2].num = 0;
			player->basic[2].start_y = 9;
			player->basic[2].last_y = 10;
			player->basic[2].history = 9;
			player->basic[2].draw_flag = true;
		}
	}
}

// モンスターウィンドウを描画
void draw_monster_window(Player *player)
{
	// ウィンドウを描画
	draw_window_graphic(20, 7, 7, player->monst_num);
	
	// 内容を描画
	for (int i = 0; i < player->monst_num; i++) {
		sprintf(Text, "　%-12s", player->monster[i].name);
		draw_string(21, i + 8, Text);
	}
}

// ステータスウィンドウを描画
void draw_status_window(Player *player)
{
	// ウィンドウを描画
	draw_window_graphic(4, 0, 10, 9);
	
	// ウィンドウタイトルを描画
	draw_string(7, 0, "ステータス");
	
	// 内容を描画
	sprintf(Text, "　　　レベル：　　%2d", player->level);
	draw_string(5, 1, Text);
	sprintf(Text, "　　　ちから：　　%2d", player->power);
	draw_string(5, 2, Text);
	sprintf(Text, "　みのまもり：　　%2d", player->physical);
	draw_string(5, 3, Text);
	sprintf(Text, "さいだいＨＰ：%6d", player->hp_max);
	draw_string(5, 4, Text);
	sprintf(Text, "さいだいＭＰ：%6d", player->mp_max);
	draw_string(5, 5, Text);
	sprintf(Text, "　こうげき力：%6d", player->attack);
	draw_string(5, 6, Text);
	sprintf(Text, "　　しゅび力：%6d", player->defense);
	draw_string(5, 7, Text);
	sprintf(Text, "　ぶき：%12s", Item_str[player->item[player->weapon]]);
	draw_string(5, 8, Text);
	sprintf(Text, "よろい：%12s", Item_str[player->item[player->armor]]);
	draw_string(5, 9, Text);
}

// アイテムを持っていない場合テキスト
void text_item_no(Player *player)
{
	// テキストボックス出力
	print_textbox("アイテムを持っていません。");
	enter_textbox();
	
	// ウィンドウを閉じる
	player->sel_cursor--;
	
	// ウィンドウ描画を許可
	player->draw_window_flag = true;
}

// スキルを持っていない場合テキスト
void text_skill_no(Player *player)
{
	// テキストボックス出力
	print_textbox("スキルを持っていません。");
	enter_textbox();

	// ウィンドウを閉じる
	player->sel_cursor--;

	// ウィンドウ描画を許可
	player->draw_window_flag = true;
}

// アイテムウィンドウを描画
void draw_item_window(Player *player)
{
	// ウィンドウを描画
	draw_window_graphic(4, 0, 8, player->item_num);
	
	// ウィンドウタイトルを描画
	draw_string(6, 0, "アイテム");
	
	// 内容を描画
	for (int i = 1; i <= player->item_num; i++) {
		sprintf(Text, "　　%-12s", Item_str[player->item[i]]);
		draw_string(5, i, Text);
		
		// 装備品がある場合
		if (i == player->weapon || i == player->armor) {
			draw_string(5, i, "Ｅ");
		}
	}
}

// スキルウィンドウを描画
void draw_skill_window(Player *player)
{
	// ウィンドウを描画
	draw_window_graphic(4, 0, 7, player->skill_num);
	
	// ウィンドウタイトルを描画
	draw_string(7, 0, "スキル");
	
	// 内容を描画
	for (int i = 0; i < player->skill_num; i++) {
		sprintf(Text, "　%-12s", Skill_str[player->skill[i]]);
		draw_string(5, i + 1, Text);
	}
}

// アイテムの行動ウィンドウ
void draw_act_item_window()
{
	// ウィンドウを描画
	draw_window_graphic(11, 1, 4, 3);
	
	// ウィンドウタイトルを描画
	draw_string(12, 1, "こうどう");
	
	// 内容を描画
	char str[32];
	draw_string(12, 2, "　つかう");
	draw_string(12, 3, "　そうび");
	draw_string(12, 4, "　すてる");
}

// プレイヤーのアイテムを並べる
void sort_item(Player *player)
{
	int j;
	bool flag = false;
	
	// noneを探索して、見つけた場合入れ替えて最後尾にもっていく
	for (int i = 1; i < player->item_num; i++) {
		// 最初に空白をみつけた場合
		if (player->item[i] == None && flag == false) {
			j = i;	// 座標を保存
			flag = true;
		}
		// 見つけてそれ以降
		if (flag) {
			// swap
			Item tmp = player->item[i];
			player->item[i] = player->item[i + 1];
			player->item[i + 1] = tmp;
		}
	}
	
	// 装備しているものがある場合
	if (player->weapon > j) player->weapon--;
	if (player->armor > j) player->armor--;
}

// さいだいHPで回復しようとした場合
bool recovery_hp_max(Player *player, const char *name)
{
	if (player->hp == player->hp_max) {
		// テキストを更新
		sprintf(Text, "プレイヤーは　%s　を使おうとした。", name);
		print_textbox(Text);
		print_textbox("しかし　回復は　ひつようなかった。");
		return false;
	}
	return true;
}

// プレイヤーが HP を回復した時の共通のテキスト
void recovery_hp_text(int value)
{
	// テキスト更新
	sprintf(Text, "プレイヤーの　ＨＰは　%2d　回復した。", value);
	print_textbox(Text);
}

// アイテムをつかう場合の共通のテキスト
void use_item(Item obj)
{
	// テキストを更新
	sprintf(Text, "プレイヤーは　%s　をつかった。", Item_str[obj]);
	print_textbox(Text);
}

// 薬草をつかう
bool use_herb(Player *player)
{
	// HP確認
	if (recovery_hp_max(player, "やくそう") == false) return false;
	
	// テキスト
	use_item(Herb);
	
	// HP回復
	int value = get_basic_rand(VALUE_HERB);
	player->hp += value;
	
	// HP上限
	if (player->hp > player->hp_max) {
		player->hp = player->hp_max;
	}
	
	// テキスト更新
	recovery_hp_text(value);
	return true;
}

// 上やくそうをつかう
bool use_herb_u(Player *player)
{
	// HP確認
	if (recovery_hp_max(player, "上やくそう") == false) return false;
	
	// テキスト
	use_item(Herb_u);
	
	// HP回復
	int value = get_basic_rand(VALUE_HERB_U);
	player->hp += value;
	
	// HP上限
	if (player->hp > player->hp_max) {
		player->hp = player->hp_max;
	}
	
	// テキスト更新
	recovery_hp_text(value);
	return true;
}

// せいすい
bool use_water(Player *player)
{
	// MP確認
	if (player->mp == player->mp_max) {
		// テキストを更新
		print_textbox("プレイヤーは　せいすい　を使おうとした。");
		print_textbox("しかし　気力は　じゅうぶんだ。");
		return false;
	}
	
	// テキスト
	use_item(Water);
	
	// MP回復
	int value = get_basic_rand(VALUE_WATER);
	player->mp += value;
	
	// MP上限
	if (player->mp > player->mp_max) {
		player->mp = player->mp_max;
	}
	
	// テキスト更新
	sprintf(Text, "プレイヤーの　ＭＰは　%2d　回復した。", value);
	print_textbox(Text);
	return true;
}

// マップを持っているか確認
bool check_item_map(Player *player)
{
	for (int i = 1; i <= player->item_num; i++) {
		if (player->item[i] == Map) return true;
	}
	return false;
}

// マップをスクリーンに描画
void draw_map_whole(Player *player)
{	
	// データ
	char data;
	int sta_x = player->map_p.x;
	int las_x = player->map_p.x + Stage_size;
	int sta_y = player->map_p.y;
	int las_y = player->map_p.x + Stage_size;
	char pla_x = sta_x + player->x;
	char pla_y = sta_y + player->y;
	
	// スクリーン描画
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		for (int j = 0; j < SCREEN_WIDTH; j++) {
			// マップ外は空白
			data = 0;
			
			// マップ描画範囲
			if (sta_y <= i && i < las_y) {
				if (sta_x <= j && j < las_x) {
					data = Map_white[i - sta_y][j - sta_x] + VALUE_MAP_REF;
				}
			}
			
			// プレイヤー
			if (i == pla_y && j == pla_x) {
				data = NUM_PLAYER_MAP;
			}
			
			// 上書き
			Screen[i][j] = data;
		}
	}
}

// 地図
void use_map(Player *player)
{
	// テキスト
	use_item(Map);
	print_textbox("Ｈ：ヘルプ");
	
	// 座標初期化
	player->map_p.x = 0;
	player->map_p.y = 0;
	
	// プレイヤーがスクリーン描画外、難易度がイージーより上の場合
	if (player->y >= SCREEN_HEIGHT && player->mode > EASY) {
		player->map_p.y = SCREEN_HEIGHT - Stage_size;
	}
}

// 罠解除
void use_release(Player *player)
{
	// テキスト
	use_item(Release);
	
	// 目の前に罠がある場合
	Pos p = get_pos_advance(player);
	if (Rotate <= Stage[p.y][p.x] && Stage[p.y][p.x] <= Arrow) {
		Stage[p.y][p.x] = Space;	// 罠解除
		print_textbox("目の前の　わなのかいじょに　せいこうした！");
	}
	// なかった場合
	else {
		print_textbox("しかし　目の前には　わながなかった。");
	}
}

// アイテムを捨てた処理
bool dispose_item(Player *player, int num)
{
	// アイテムの確認
	Item obj = player->item[num];
	
	// 大事なものは捨てられない
	if (obj == Jewelry) {
		print_textbox("これは　捨てられない。");
		return false;
	}
	
	// テキスト更新
	sprintf(Text, "プレイヤーは　%s　を放り投げた。", Item_str[obj]);
	print_textbox(Text);
	
	// 捨てたアイテムが地図の場合
	if (obj == Map) {
		init_map_white();	// 初期化
	}
	
	// 捨てたアイテムが武器
	if (Weapon < obj && obj < Armor) {
		// それを装備しているのなら
		if (num == player->weapon) {
			// ステータス更新
			player->weapon = 0;	// 装備なし
			player->attack = player->power;	// 初期化
		}
	}
	// 捨てたアイテムが防具
	else if (Armor < obj && obj < Jewelry) {
		// それを装備しているのなら
		if (num == player->armor) {
			// ステータス更新
			player->armor = 0;	// 装備なし
			player->defense = player->physical;	// 初期化
		}
	}
	return true;
}

// アイテムの行動処理メニューモード
void act_item_menu(Player *player)
{
	// アイテムを持っているのか確認
	if (player->item_num == 0) {
		// テキストボックス初期化
		init_textbox();
		
		// テキスト更新
		text_item_no(player);
		return;
	}
	
	// 三枚目のウィンドウであるか
	if (player->sel_cursor < 3) return;
	
	// 選んだアイテムを確認
	int num = player->basic[1].num + 1;
	Item obj = player->item[num];
	
	// テキストボックス初期化
	init_textbox();
	
	bool item_consume_flag = false;
	
	// つかう
	if (player->basic[2].num == 0) {
		// やくそう
		if (obj == Herb) {
			item_consume_flag = use_herb(player);
		}
		// 上やくそう
		else if (obj == Herb_u) {
			item_consume_flag = use_herb_u(player);
		}
		// せいすい
		else if (obj == Water) {
			item_consume_flag = use_water(player);
		}
		// 地図
		else if (obj == Map) {
			use_map(player);
			
			// Enter
			enter_textbox();
			
			// ウィンドウをすべて閉じる
			player->sel_cursor = 0;
			
			// 文字専用スクリーン初期化
			init_string();
			
			// マップモードに切り替え
			player->control = MAP;
			
			return;
		}
		// 罠解除
		else if (obj == Release) {
			use_release(player);
			item_consume_flag = true;
		}
		// それ以外
		else {
			use_item(obj);
			print_textbox("しかし　なにも起きなかった。");
			item_consume_flag = false;
		}
	}
	
	// そうび
	else if (player->basic[2].num == 1) {
		// 装備フラグ
		bool flag = false;
		
		// 武器
		if (Weapon < obj && obj < Armor) {
			// 攻撃力テーブル
			int value[] = {VALUE_STICK,VALUE_COPPER,VALUE_SWORD};
			
			// ステータス更新
			player->weapon = num;
			player->attack = player->power + value[obj - Weapon - 1];
			
			// フラグ更新
			flag = true;
		}
		// 防具
		else if (Armor < obj && obj < Jewelry) {
			// 防御力テーブル
			int value[] = {VALUE_CLOTH,VALUE_LEATHER,VALUE_IRON};
			
			// ステータス更新
			player->armor = num;
			player->defense = player->physical + value[obj - Armor - 1];
			
			// フラグ更新
			flag = true;
		}
		// それ以外
		else {
			print_textbox("それは　そうびできない。");
		}
		
		// そうびしたら
		if (flag) {
			sprintf(Text, "プレイヤーは　%s　をそうびした。", Item_str[obj]);
			print_textbox(Text);
		}
	}
	
	// すてる
	else if (player->basic[2].num == 2) {
		// アイテム消費
		item_consume_flag = true;
		
		// アイテムを捨てる
		if (dispose_item(player, num) == false) {
			// 大事なものはすてられない
			item_consume_flag = false;
		}
	}
	
	// Enter
	enter_textbox();
	
	// アイテムウィンドウにもどす
	player->sel_cursor = 1;
	
	// アイテム消費処理
	if (item_consume_flag) {
		// 選んだアイテムを消費
		player->item[num] = None;
		
		// アイテムを並び替える
		sort_item(player);
		
		// プレイヤーのアイテム数を減らす
		player->item_num--;
		
		// カーソル再設定
		set_cursor_menu(player);
	}
	
	// アイテムが一つもない場合
	if (player->item_num == 0) {
		// スタートウィンドウに戻す
		player->sel_cursor = 0;
	}
}

// スキルを使用テンプレート
bool chant_skill(Player *player, Skill act)
{
	// コストテーブル
	int cost[] = {
		COST_HEEL,
		COST_FIRE,
		COST_HEEL_P,
		COST_BURST,
		COST_FIRE_M
	};
	
	// テキストを更新
	sprintf(Text, "プレイヤーは　%sを　となえた。", Skill_str[act]);
	print_textbox(Text);
	
	// MP確認
	if (player->mp < cost[act]) {
		// テキストを更新
		print_textbox("しかし　ＭＰ　が足りない。");
		return false;
	}
	
	// MP消費
	player->mp -= cost[act];
	return true;
}

// 回復系スキル
bool chant_recover_single(Player *player, Skill act)
{
	// HP確認
	char *str = Skill_str[act];
	if (recovery_hp_max(player, str) == false) return false;
	
	// MP確認
	if (chant_skill(player, act) == false) return false;
	
	// モンスターのターン
	player->process_flag = true;
	
	// 回復量テーブル
	int value[] = {VALUE_HEEL,0,VALUE_HEEL_P};
	
	// 回復
	int rec = get_basic_rand(value[act]);
	player->hp += rec;
	
	// HP上限
	if (player->hp > player->hp_max) {
		player->hp = player->hp_max;
	}
	
	// テキスト更新
	recovery_hp_text(rec);
	
	return true;
}

// ヒール
bool chant_heel(Player *player)
{
	return chant_recover_single(player, Heel);
}

// ヒールプラス
bool chant_heel_plus(Player *player)
{
	return chant_recover_single(player, Heel_plus);
}

// スキルの行動処理メニューモード
void act_skill_menu(Player *player)
{
	// スキルを持っていない場合
	if (player->skill_num == 0) {
		// テキストボックス初期化
		init_textbox();
		
		// テキスト更新
		text_skill_no(player);
		return;
	}
	
	// 二枚目ウィンドウを表示してない場合
	if (player->sel_cursor == 1) return;
	
	// 使うスキルを確認
	Skill act = player->skill[player->basic[1].num];
	
	// テキストボックス初期化
	init_textbox();
	
	// ヒールを唱える
	if (act == Heel) {
		chant_heel(player);
	}
	// ヒールプラスを唱える
	else if (act == Heel_plus) {
		chant_heel_plus(player);
	}
	// それ以外
	else {
		// テキスト更新
		print_textbox("ここでは　となえることができない。");
	}
	
	// Enter
	enter_textbox();
	
	// スキルウィンドウに戻す
	player->sel_cursor = 1;
}

// モンスターをソート
void sort_monster(Player *player, int num)
{
	bool flag = false;
	
	// たおしたモンスターは指定されているのでそれだけを最後尾にもっていく
	for (int i = 0; i < player->monst_num - 1; i++) {
		// 指定のモンスターをみつけたら
		if (i == num) {
			flag = true;
		}
		// そのあとはずっと交換処理をする
		if (flag) {
			// swap
			Monster tmp = player->monster[i];
			player->monster[i] = player->monster[i + 1];
			player->monster[i + 1] = tmp;
		}
	}
}

// ダメージがあたえられない場合
bool get_no_damage(int damage)
{
	if (damage <= 0) {
		print_textbox("しかし　ダメージはあたえられなかった。");
		return false;
	}
	else return true;
}

// デバフ初期化
void init_debuff(Player *player)
{
	player->count = 0;
	player->anger_debuff = false;
	player->hypnos_debuff = false;
}

// 逃げた場合のバトル終了
void escape_end_battle(Player *player)
{
	// 確認
	enter_textbox();
	
	// フィールドモードに切り替え
	player->control = FIELD;
	
	// 文字スクリーン初期化
	init_string();
	
	// デバフ初期化
	init_debuff(player);
	
	// モンスターのターンは許可しない
	player->process_flag = false;
}

// レベルアップによるステータスアップ
void update_status(Player *player, int goal_level)
{
	// 何回レベルアップするか求める
	int num = goal_level - player->level;
	
	// レベルアップ
	for (int i = 0; i < num; i++) {
		// レベルアップ
		player->level++;
		
		// テキスト更新
		sprintf(Text, "プレイヤーは　レベル%2d　になった。", player->level);
		print_textbox(Text);
		
		// ステータスはランダムに増加
		int hp = get_scope_rand(4, 6);
		int mp = get_scope_rand(2, 4);
		int att = get_scope_rand(1, 3);
		int def = get_scope_rand(0, 2);
		
		// ステータスアップ
		player->hp_max += hp;
		player->mp_max += mp;
		player->power += att;
		player->physical += def;
		player->attack += att;
		player->defense += def;
		
		// テキスト更新
		sprintf(Text, "さいだいＨＰが　%d　上がった。", hp);
		print_textbox(Text);
		sprintf(Text, "さいだいＭＰが　%d　上がった。", mp);
		print_textbox(Text);
		sprintf(Text, "　　　ちからが　%d　上がった。", att);
		print_textbox(Text);
		sprintf(Text, "　みのまもりが　%d　上がった。", def);
		print_textbox(Text);
		
		// スキルを覚える可能性
		if (player->level == 3) player->skill_num++;
		else if (player->level == 5) player->skill_num++;
		else if (player->level == 7) player->skill_num++;
		else if (player->level == 9) player->skill_num++;
		else if (player->level == 11) player->skill_num++;
		else continue;
		
		// テキスト更新
		sprintf(Text, "プレイヤーは　%sを　おぼえた！", Skill_str[player->skill_num - 1]);
		print_textbox(Text);
	}
}

// レベルアップ
void update_level(Player *player)
{
	// 経験値からレベルアップテーブル
	int exp = player->exp;	// 省略
	int level = player->level;
		
	if (exp >= 13000 && level <= 15) {
		update_status(player, 16);
	}
	else if (exp >= 10000 && level <= 14) {
		update_status(player, 15);
	}
	else if (exp >= 7500 && level <= 13) {
		update_status(player, 14);
	}
	else if (exp >= 5500 && level <= 12) {
		update_status(player, 13);
	}
	else if (exp >= 4000 && level <= 11) {
		update_status(player, 12);
	}
	else if (exp >= 2900 && level <= 10) {
		update_status(player, 11);
	}
	else if (exp >= 2000 && level <= 9) {
		update_status(player, 10);
	}
	else if (exp >= 1300 && level <= 8) {
		update_status(player, 9);
	}
	else if (exp >= 800 && level <= 7) {
		update_status(player, 8);
	}
	else if (exp >= 450 && level <= 6) {
		update_status(player, 7);
	}
	else if (exp >= 220 && level <= 5) {
		update_status(player, 6);
	}
	else if (exp >= 110 && level <= 4) {
		update_status(player, 5);
	}
	else if (exp >= 47 && level <= 3) {
		update_status(player, 4);
	}
	else if (exp >= 23 && level <= 2) {
		update_status(player, 3);
	}
	else if (exp >= 7 && level <= 1) {
		update_status(player, 2);
	}
	else return;
	
	enter_textbox();
}

// バトル終了処理
void end_battle(Player *player)
{
	// フィールドモードに切り替え
	player->control = FIELD;
	
	// デバフ初期化
	init_debuff(player);
	
	// 固定敵の場合、ステージから削除する
	if (Stage[player->y][player->x] == Enemy) {
		Stage[player->y][player->x] = Space;
	}
	
	// モンスターのターンは許可しない
	player->process_flag = false;
	
	// 経験値、お金の換算
	int e = 0, g = 0;
	for (int i = 0; i < player->monst_num_cp; i++) {
		e += player->monster[i].exp;	// 経験値
		g += player->monster[i].gold;	// お金
	}
	
	// テキスト更新
	sprintf(Text, "%4d　のゴールドと", g);
	print_textbox(Text);
	sprintf(Text, "%4d　の経験値を　かくとく！", e);
	print_textbox(Text);
	
	// ステータス更新
	player->exp += e;
	player->gold += g;
	
	// 最大値は越えられない
	if (player->exp > EXP_MAX) player->exp = EXP_MAX;
	if (player->gold > GOLD_MAX) player->gold = GOLD_MAX;
	
	// 確認
	enter_textbox();
	
	// モンスターが宝箱を落とす確率 10%
	int num = get_scope_rand(1, 100);
	if(num <= 10) {
		print_textbox("モンスターは　宝箱を　落とした。");
		get_treasure(player);
	}
	
	// レベルアップ
	update_level(player);
	
	// 文字スクリーン初期化
	init_string();
}

// モンスターの HP がなくなった
void died_monster(Player *player, int num)
{
	// テキスト更新
	sprintf(Text, "%s　をたおした！", player->monster[num].name);
	print_textbox(Text);
	
	// 最後の一匹だった場合
	if (player->monst_num == 1) {
		// バトル終了
		end_battle(player);
		return;
	}
	
	// たおしたモンスターを一番後ろにもっていく
	sort_monster(player, num);
	
	// モンスターの数を減らす
	player->monst_num--;
}

// こうげきの処理
void attack_to_monster(Player *player)
{
	// モンスターの数が複数いるのにモンスターセレクトを行ってない場合
	if (player->monst_num > 1 && player->sel_cursor == 1) {
		// さらにちょうはつにかかってない
		if (player->anger_debuff == false) return;
	}
	
	// モンスターのターン
	player->process_flag = true;
	
	// テキスト更新
	print_textbox("プレイヤーの　こうげき！");
	
	// 攻撃するモンスターをわりだす
	int num;
	if (player->monst_num == 1) num = 0;
	else num = player->basic[1].num;
	
	// ちょうはつにかかっている場合
	if (player->anger_debuff) {
		// スケルトン探索
		int decoy = 0;
		for (int i = 0; i < player->monst_num; i++) {
			if (player->monster[i].num == SKELETON) {
				decoy = i;
				break;
			}
		}
		num = decoy;	// ターゲット変更
	}
	
	// ダメージを計算
	int damage = get_basic_rand(player->attack);
	damage -= player->monster[num].defense;	// 防御力の分減算
	
	// プレイヤーの攻撃力より防御力のほうが大きい場合
	if (get_no_damage(damage) == false) return;
	
	// 敵のHPを減らす
	player->monster[num].hp -= damage;
	
	// テキスト更新
	sprintf(Text, "%s　に　%2d　のダメージ！", player->monster[num].name, damage);
	print_textbox(Text);
	
	// 敵の HP がなくなった場合
	if (player->monster[num].hp <= 0) died_monster(player, num);
}

// にげるの処理
void escape_battle(Player *player)
{
	// ドラゴンは無条件で逃げることが可能
	if (player->monster[0].num == DRAGON) {
		print_textbox("にげることに　成功した！");
		
		// バトル終了処理
		escape_end_battle(player);
		return;
	}
	
	// 固定敵の場合逃げられない
	if (Stage[player->y][player->x] == Enemy) {
		// テキスト更新
		print_textbox("このモンスターとは　戦わなければならない！");
		enter_textbox();
		
		// ウィンドウを戻す
		player->sel_cursor--;
		return;
	}
	
	// モンスターのターン
	player->process_flag = true;
	
	// 確率 50%
	int num = get_scope_rand(1, 100);
	
	// 成功
	if (num <= 50) {
		print_textbox("にげることに　成功した！");
		
		// バトル終了処理
		escape_end_battle(player);
	}
	
	// 失敗
	else {
		print_textbox("モンスターからは　にげきれない！");
		//enter_textbox();
	}
}

// 全体攻撃系スキル
bool chant_attack_all(Player *player, Skill act)
{	
	// MP確認さらに消費
	if (chant_skill(player, act) == false) return false;
	
	// モンスターのターン
	player->process_flag = true;
	
	// 攻撃力テーブル
	int value[] = {0,0,0,VALUE_BURST,0};
	
	// 全体攻撃
	for (int i = 0; i < player->monst_num; i++) {
		// ダメージを計算
		int damage = get_basic_rand(value[act]);
		damage -= player->monster[i].defense;	// 防御力の分減算
		
		// プレイヤーの攻撃力より防御力のほうが大きい場合
		if (get_no_damage(damage) == false) continue;
		
		// 敵のHPを減らす
		player->monster[i].hp -= damage;
		
		// テキスト更新
		sprintf(Text, "%s　に　%2d　のダメージ！", player->monster[i].name, damage);
		print_textbox(Text);
	}
	
	// 複数たおした場合
	int cp = player->monst_num;	// 現在のモンスターの数をコピー
	for (int i = 0, j = 0; i < cp; i++, j++) {
		// 敵を倒していた場合
		if (player->monster[j].hp <= 0) {
			died_monster(player, j);
			j = -1;	// 初期化
		}
	}
	
	return true;
}

// バースト
bool chant_burst(Player *player)
{
	return chant_attack_all(player, Burst);
}

// 単体攻撃系スキル
bool chant_attack_single(Player *player, Skill act)
{
	// モンスターの数が複数いるのにモンスターセレクトを行ってない場合
	if (player->monst_num > 1 && player->sel_cursor == 2) return true;
	
	// MP確認さらに消費
	if (chant_skill(player, act) == false) return false;
	
	// モンスターのターン
	player->process_flag = true;
	
	// 攻撃するモンスターをわりだす
	int num;
	if (player->monst_num == 1) num = 0;
	else num = player->basic[2].num;
	
	// 攻撃力テーブル
	int value[] = {0,VALUE_FIRE,0,0,VALUE_FIRE_M};
	
	// ダメージを計算
	int damage = get_basic_rand(value[act]);
	damage -= player->monster[num].defense;	// 防御力の分減算
	
	// プレイヤーの攻撃力より防御力のほうが大きい場合
	if (get_no_damage(damage) == false) return true;
	
	// 敵のHPを減らす
	player->monster[num].hp -= damage;
	
	// テキスト更新
	sprintf(Text, "%s　に　%2d　のダメージ！", player->monster[num].name, damage);
	print_textbox(Text);
	
	// 敵の HP がなくなった場合
	if (player->monster[num].hp <= 0) died_monster(player, num);
	
	return true;
}

// ファイヤ
bool chant_fire(Player *player)
{
	return chant_attack_single(player, Fire);
}

// メガファイヤ
bool chant_fire_mega(Player *player)
{
	return chant_attack_single(player, Fire_mega);
}

// スキルの処理バトルモード
void act_skill_battle(Player *player)
{
	// スキルを持っていない場合
	if (player->skill_num == 0) {
		// テキスト更新
		text_skill_no(player);
		return;
	}
	
	// 使うスキルを確認
	Skill act = player->skill[player->basic[1].num];
	
	// スキル使用に成功したか
	bool success = true;
	
	// 自分ひとりで完結できる処理の場合
	if (player->sel_cursor == 2) {
		// ヒール
		if (act == Heel) {
			success = chant_heel(player);
		}
		// ヒールプラス
		else if (act == Heel_plus) {
			success = chant_heel_plus(player);
		}
		// バースト
		else if (act == Burst){
			success = chant_burst(player);
		}
	}
	
	// 対象モンスターを選ぶ必要がある場合
	if (player->sel_cursor >= 2) {
		// ファイヤ
		if (act == Fire) {
			success = chant_fire(player);
		}
		// メガファイヤ
		else if (act == Fire_mega) {
			success = chant_fire_mega(player);
		}
	}
	
	// 失敗したらウィンドウを戻す
	if (success == false) {
		player->sel_cursor--;
		enter_textbox();	// 確認
	}
}

// アイテム処理バトルモード
void act_item_battle(Player *player)
{
	// アイテムを持っているのか確認
	if (player->item_num == 0) {
		// テキスト更新
		text_item_no(player);
		return;
	}
	
	// アイテムウィンドウを開いてないと処理しない
	if (player->sel_cursor == 1) return;
	
	// 選んだアイテムを確認
	int num = player->basic[1].num + 1;
	Item obj = player->item[num];
	
	// フラグ
	bool consum_flag = false;
	
	// つかう
	if (player->sel_cursor == 2) {
		// やくそう
		if (obj == Herb) {
			consum_flag = use_herb(player);
		}
		// 上やくそう
		else if (obj == Herb_u) {
			consum_flag = use_herb_u(player);
		}
		// せいすい
		else if (obj == Water) {
			consum_flag = use_water(player);
		}
		// それ以外
		else {
			// テキスト更新
			use_item(obj);
			print_textbox("しかし　今は意味がない。");
		}
	}
	
	// アイテムの使用に失敗した場合
	if (consum_flag == false) {
		// ウィンドウを戻す
		player->sel_cursor--;
		enter_textbox();	// 確認
		return;
	}
	
	// モンスターのターン
	player->process_flag = true;
	
	// 選んだアイテムを消費
	player->item[num] = None;
	
	// アイテムを並び替える
	sort_item(player);
	
	// プレイヤーのアイテム数を減らす
	player->item_num--;
	
	// カーソル再設定
	set_cursor_battle(player);
}

// メニューモードでのウィンドウをすべて描画
void draw_window_menu(Player *player)
{
	// フラグ
	//bool item_window_flag = false;	// アイテムウィンドウ
	
	// 文字スクリーン初期化
	init_string();
	
	// グラフィックスクリーン初期化
	draw_3D(player);
	
	// スタートウィンドウを描画
	draw_start_window_menu(player);
	
	// 二枚目のウィンドウ
	if (player->sel_cursor >= 1) {
		// ステータスウィンドウを描画
		if (player->basic[0].num == 0) {
			draw_status_window(player);
		}
		
		// アイテムウィンドウを描画
		else if (player->basic[0].num == 1) {
			draw_item_window(player);
		}
		
		// スキルウィンドウを描画
		else if (player->basic[0].num == 2) {
			draw_skill_window(player);
		}
	}
	
	// 三枚目のウィンドウ
	if (player->sel_cursor >= 2) {
		// アイテムの行動ウィンドウ
		/*
		if (item_window_flag) {
			draw_act_item_window();
		}
		*/
		draw_act_item_window();
	}
	
	// ウィンドウ描画は許可しない
	player->draw_window_flag = false;
}

// バトルモードでのウィンドウをすべて描画
void draw_window_battle(Player *player)
{
	// 別のウィンドウを呼び出した時だけ描画
	if (player->draw_window_flag == false) return;
	
	// プレイヤーのターンが終わった場合
	if (player->process_flag) player->sel_cursor = 0;
	
	// 文字スクリーン初期化
	init_string();
	
	// グラフィックスクリーン初期化
	draw_3D(player);
	
	// スタートウィンドウを描画
	draw_start_window_battle(player);
	
	// モンスターウィンドウを描画
	draw_monster_window(player);
	
	// 二枚目ウィンドウ
	if (player->sel_cursor >= 1) {
		// スキルウィンドウを描画
		if (player->basic[0].num == 1) {
			draw_skill_window(player);
		}
		
		// アイテムウィンドウを描画
		else if (player->basic[0].num == 2) {
			draw_item_window(player);
		}
	}
	
	// テキストボックスの描画
	draw_textbox();	// グラフィック
	draw_textbox_str();	// 文字
	
	// ウィンドウ描画は許可しない
	player->draw_window_flag = false;
}

// 道具リストウィンドウ
void draw_tool_list_window()
{
	// ウィンドウの描画
	draw_window_graphic(4, 0, 10, 4);
	
	// ウィンドウタイトル
	draw_string(8, 0, "どうぐ");
	
	// 内容を描画
	draw_string(5, 1, "　やくそう　　　10");
	draw_string(5, 2, "　上やくそう　　25");
	draw_string(5, 3, "　せいすい　　　20");
	draw_string(5, 4, "　わなかいじょ　60");
}

// 武器リストウィンドウ
void draw_weapon_list_window()
{
	// ウィンドウの描画
	draw_window_graphic(4, 0, 10, 3);
	
	// ウィンドウタイトル
	draw_string(9, 0, "ぶき");
	
	// 内容描画
	draw_string(5, 1, "　こんぼう　　　55");
	draw_string(5, 2, "　どうのつるぎ　180 ");
	draw_string(5, 3, "　てつのつるぎ　500 ");
}

// 防具リストウィンドウ
void draw_armor_list_window()
{
	// ウィンドウの描画
	draw_window_graphic(4, 0, 10, 3);
	
	// ウィンドウタイトル
	draw_string(8, 0, "ぼうぐ");
	
	// 内容描画
	draw_string(5, 1, "　ぬののふく　　20");
	draw_string(5, 2, "　かわのよろい　70");
	draw_string(5, 3, "　てつのよろい　300 ");
}

// タウンモードでのウィンドウをすべて描画
void draw_window_town(Player *player)
{
	// 別のウィンドウを呼び出した時だけ描画
	if (player->draw_window_flag == false) return;
	
	// 文字スクリーン初期化
	init_string();
	
	// グラフィックスクリーン初期化
	draw_3D(player);
	
	// スタートウィンドウを描画
	draw_start_window_town(player);
	
	// 二枚目ウィンドウ
	if (player->sel_cursor >= 1) {
		// 宿屋イエスノー
		if (player->basic[0].num == 0) {
			draw_yes_no_window();
		}
		
		// 道具リスト
		else if (player->basic[0].num == 1) {
			draw_tool_list_window();
		}
		
		// 武器リスト
		else if (player->basic[0].num == 2) {
			draw_weapon_list_window();
		}
		
		// 防具リスト
		else if (player->basic[0].num == 3) {
			draw_armor_list_window();
		}
	}
	
	// 三枚目のウィンドウ
	if (player->sel_cursor >= 2) {
		// アイテム屋はすべて三枚目のウィンドウは共通
		if (player->basic[0].num >= 1) {
			draw_yes_no_window();
		}
	}
	
	// テキストボックスの描画
	draw_textbox();	// グラフィック
	draw_textbox_str();	// 文字
	
	// ウィンドウ描画は許可しない
	player->draw_window_flag = false;
}

// ミニマップ描画
void draw_minimap(Player *player)
{
	// マップを持っているか確認
	if (check_item_map(player) == false) return;
	
	// プレイヤーの座標を登録
	char x = player->x;
	char y = player->y;
	
	// マップの更新
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			// 色付きタイル以上のオブジェクトだった場合
			if (Stage[y + i][x + j] > White) {
				Map_white[y + i][x + j] = 0;
				continue;
			}
			Map_white[y + i][x + j] = (char)Stage[y + i][x + j];
		}
	}
	
	// ウィンドウ描画
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			Screen[i][j + 27] = Map_window[i][j];
		}
	}
	
	// "  ","＿","■","□","--","+-","-+","| "," |","  ","■","▲","赤","緑","青","白","●"
	
	// ミニマップ描画
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			// マップ素材と通常画面素材を一緒の配列にしているため基準値VALUE_MAP_REF 9 
			Screen[i + 2][j + 29] = Map_white[y + i][x + j] + VALUE_MAP_REF;
		}
	}
	//NUM_PLAYER_MAP 16
	Screen[2][29] = NUM_PLAYER_MAP;	// プレイヤー描画
}

// スクリーンのデータを統合して表示
void print_screen(Player *player)
{
	// フィールドモード
	if (player->control == FIELD) {
		// 3D描画
		draw_3D(player);
		
		// ミニマップ描画
		draw_minimap(player);
		
		// スクリーンを表示
		print_screen_graphic();
	}

	// メニューモード
	else if (player->control == MENU) {
		// ウィンドウ描画
		draw_window_menu(player);
		
		// カーソル描画
		draw_cursor(player);
		
		// スクリーンを表示
		print_screen_basic();
	}
	
	// バトルモード
	else if (player->control == BATTLE) {
		// ウィンドウを描画
		draw_window_battle(player);
		
		// カーソル描画
		draw_cursor(player);
		
		// スクリーンに表示
		print_screen_basic();
	}
	
	// マップモード
	else if (player->control == MAP) {
		// マップ描画
		draw_map_whole(player);
		
		// スクリーンを表示
		print_screen_graphic();
	}
	
	// 拠点モード
	else if (player->control == TOWN) {
		// ウィンドウを描画
		draw_window_town(player);
		
		// カーソル描画
		draw_cursor(player);
		
		// スクリーンに表示
		print_screen_basic();
	}
}

// 敵とのエンカウント演出
void print_encount()
{
	// 真ん中の座標
	int center_x = 34;	// (SCREEN_WIDTH*2 + 2) / 2
	int center_y = 11;	// (SCREEN_HEIGHT + 2) / 2 + 1
	
	// 一回転するのに必要な処理の回数の基準値
	const int ref = 8;
	
	// 実際に表示する座標
	int x, y;
	
	// 真ん中に表示
	move_cursor_console(center_x,center_y);
	printf("    ");
	
	// 一回転で二行追加するため SCREEN_HEIGHT / 2 = 9
	for (int i = 1; i <= 8; i++) {
		// 一回転
		for (int j = 0; j < i*ref; j++) {
			// 基準値コピー
			if (j == 0) {
				x = center_x;
				y = center_y + 1;
			}
			// 左に進む
			else if (j < i*2) {
				x -= 4;
			}
			// 上に進む
			else if (j < i*4) {
				y--;
			}
			// 右に進む
			else if (j < i*6) {
				x += 4;
			}
			// 下に進む
			else if (j < i*ref) {
				y++;
			}
			// 調整
			if (j >= i*6 - 1 && i == 8) {
				y = 2;
				x -= 4;
			}
			// 画面更新
			move_cursor_console(x,y);
			printf("    ");
			
				
		}
		// 基準値更新
		center_x += 4;
		center_y++;
		
		// 遅延
		mySleep(128);
	}
}

// 敵とのバトル初期化
void init_battle(Player *player)
{
	// 演出
	print_encount();
	
	// スタートアップ
	set_start_cursor_battle(player);
	
	// 文字スクリーン初期化
	init_string();
	
	// テキストボックス初期化
	init_textbox();
	
	// ウィンドウの描画を許可
	player->draw_window_flag = true;
	
	// バトルモードに切り替え
	player->control = BATTLE;
	
	// デバフ初期化
	init_debuff(player);
	
	// 乱数を格納する
	int num;
	
	// 特別モンスター
	Monster dragon = {"ドラゴン",210,100,210,100,25,4,2048,1000,false,0,6};
	
	// 特殊エンカウント
	num = get_scope_rand(1, 100);
	if (num == 1) {
		// 設定更新
		player->monst_num = 1;
		player->monster[0] = dragon;
		
		// テキスト更新
		print_textbox("なんと　ドラゴンが　あらわれた！");
		
		return;
	}
	
	// モンスター設定
	Monster enemy[] = {
		//name hp mp hp_max mp_max attack defense exp gold num
		{"スライム", 5, 0, 5, 0, 2, 0, 2, 2, false, 0, 0},
		{"ゴブリン", 9, 0, 9, 0, 3, 0, 4, 5, false, 0, 1},
		{"オーク", 15, 8, 15, 8, 6, 1, 16, 10, false, 0, 2},
		{"スケルトン", 11, 4, 11, 4, 5, 4, 32, 25, false, 0, 3},
		{"ウィザード", 15, 30, 15, 30, 2, 1, 64, 50, false, 0, 4},
		{"フェアリー", 2, 40, 2, 40, 2, 12, 1000, 200, false, 0, 5},
	};
	
	// レベルが高い場合、モンスターを強くする
	if (player->level >= 8) {
		for (int i = 0; i < MONSTER_KIND_NUM; i++) {
			enemy[i].hp *= 3;
			enemy[i].mp *= 3;
			enemy[i].hp_max *= 3;
			enemy[i].mp_max *= 3;
			enemy[i].attack *= 3;
			enemy[i].defense *= 2;
			enemy[i].exp *= 3;
			enemy[i].gold *= 3;
		}
	}
	
	// 敵の数を決める
	num = get_scope_rand(1, 100);
	if (num <= 60)      player->monst_num = 1;	// 確率 60 %
	else if (num <= 90) player->monst_num = 2;	// 確率 30 %
	else                player->monst_num = 3;	// 確率 10 %
	
	// コピー
	player->monst_num_cp = player->monst_num;
	
	// 敵を選ぶ
	int select;	// 選んだモンスター
	for (int i = 0; i < player->monst_num; i++) {
		num = get_scope_rand(1, 100);
		// レベルが高い場合つよいモンスターばかり出る
		if (player->level >= 8) {
			if (num <= 5)       select = 0;		// スライム確率 5%
			else if (num <= 9) select = 1;		// ゴブリン確率 4%
			else if (num <= 39) select = 2;		// オーク確率 30%
			else if (num <= 69) select = 3;		// スケルトン確率 30%
			else if (num <= 99) select = 4;		// ウィザード確率 30%
			else                select = 6;		// フェアリー確率 1%
		}
		
		// レベルが低い場合
		else {
			if (num <= 35)      select = 0;		// スライム確率 35%
			else if (num <= 70) select = 1;		// ゴブリン確率 35%
			else if (num <= 80) select = 2;		// オーク確率 10%
			else if (num <= 90) select = 3;		// スケルトン確率 10%
			else if (num <= 99) select = 4;		// ウィザード確率 9%
			else                select = 5;		// フェアリー確率1%
		}
		
		// モンスターの設定値をコピー
		player->monster[i] = enemy[select];
	}
	
	// 難易度がイージーの場合
	if (player->mode == EASY) {
		for (int i = 0; i < player->monst_num; i++) {
			// HP を減らす
			player->monster[i].hp -= 4;
			player->monster[i].hp_max -= 4;
		}
	}
	
	// 難易度がハードの場合
	else if (player->mode == HARD) {
		for (int i = 0; i < player->monst_num; i++) {
			player->monster[i].hp *= 2;
			player->monster[i].hp_max = player->monster[i].hp;	// コピー
		}
	}
	
	// モンスターが一匹の場合
	if (player->monst_num == 1) {
		sprintf(Text, "%sが　あらわれた！", player->monster[0].name);
		print_textbox(Text);
		
	}
	// モンスターが複数の場合
	else print_textbox("モンスターの群れが　あらわれた！");
}

// 敵とのエンカウント
void act_encount(Player *player)
{
	// オブジェクトがある場合
	if (Stage[player->y][player->x] != Space) return;
	
	// 1 ~ 100 の乱数を取得
	int num = get_scope_rand(1, 100);
	
	// 敵とのエンカウント率 8%
	if (num > 8) return;
	
	// バトルスタート
	init_battle(player);
}

// カーソルループ処理
void loop_cursor(Player *player)
{
	// アクセス省略
	char sel = player->sel_cursor;
	char start = player->basic[sel].start_y;
	char last = player->basic[sel].last_y;
	
	// カーソルループ
	if (player->basic[sel].y > last) player->basic[sel].y = start;
	else if (player->basic[sel].y < start) player->basic[sel].y = last;
	
	// アクセス省略
	char max = last - start;
	
	// 内部データループ
	if (player->basic[sel].num > max) player->basic[sel].num = 0;
	else if (player->basic[sel].num < 0) player->basic[sel].num = max;
}

// カーソル設定アイテム容量オーバー
void set_cursor_item_over(Player *player)
{
	// 二枚目ウィンドウ
	if (player->sel_cursor == 1) {
		// アイテムウィンドウ
		player->basic[1].x = 6;
		player->basic[1].y = 1;
		player->basic[1].num = 0;
		player->basic[1].start_y = 1;
		player->basic[1].last_y = player->item_num;
		player->basic[1].history = 1;
		player->basic[1].draw_flag = true;
		
		// 前のウィンドウのカーソルを消す
		player->basic[0].draw_flag = false;
	}
	
	// 三枚目のウィンドウ
	else if (player->sel_cursor == 2) {
		player->basic[2].x = 27;
		player->basic[2].y = 9;
		player->basic[2].num = 0;
		player->basic[2].start_y = 9;
		player->basic[2].last_y = 10;
		player->basic[2].history = 9;
		player->basic[2].draw_flag = true;
	}
}

// カーソル設定アイテム容量オーバースタート
void set_start_cursor_item_over(Player *player)
{
	player->sel_cursor = 0;
	player->basic[0].x = 27;
	player->basic[0].y = 9;
	player->basic[0].num = 0;
	player->basic[0].start_y = 9;
	player->basic[0].last_y = 10;
	player->basic[0].history = 9;
	player->basic[0].draw_flag = true;
}

// アイテム容量がオーバーしたウィンドウを描画
void draw_window_item_over(Player *player)
{
	// ウィンドウの描画許可
	if (player->draw_window_flag == false) return;
	
	// 文字スクリーン初期化
	init_string();
	
	// グラフィックスクリーン初期化
	draw_3D(player);
	
	// 二択ウィンドウを描画
	if (player->sel_cursor == 0) {
		draw_yes_no_window();
	}
	
	// アイテムウィンドウを描画
	if (player->sel_cursor >= 1) {
		draw_item_window(player);
	}
	
	// 二択ウィンドウを描画
	if (player->sel_cursor == 2) {
		draw_yes_no_window();
	}
	
	// テキストボックスの描画
	draw_textbox();
	draw_textbox_str();
	
	// ウィンドウの描画を許可しない
	player->draw_window_flag = false;
}

// アイテム容量がオーバーした場合の画面表示
void print_item_over(Player *player)
{
	// 画面描画
	draw_window_item_over(player);
	
	// カーソル描画
	draw_cursor(player);
	
	// 画面表示
	print_screen_basic();
}

// アイテム容量オーバーでのキー入力
char input_item_over()
{
	// 入力
	while (true) {
		char input = getch();
		
		// W
		if (input == 'w') {
			return 'w';
		}
		// S
		else if (input == 's') {
			return 's';
		}
		// Enter
		else if (input == ENTER) {
			return ENTER;
		}
		// Back Space
		else if (input == BACK_SPACE) {
			return BACK_SPACE;
		}
	}
}

// アイテム容量がオーバーした際の入力処理
bool control_item_over(Player *player)
{
	// キー入力
	char key = input_item_over();
	
	// アクセス名省略
	char sel = player->sel_cursor;
	
	// 上にカーソルを動かす
	if (key == 'w') {
		// 履歴を残す
		player->basic[sel].history = player->basic[sel].y;
		
		// カーソルの座標を更新
		player->basic[sel].y--;
		player->basic[sel].num--;
	}
	// 下にカーソルを動かす
	else if (key == 's') {		
		// 履歴を残す
		player->basic[sel].history = player->basic[sel].y;
		
		// カーソルの座標を更新
		player->basic[sel].y++;
		player->basic[sel].num++;
	}
	// Enter
	else if (key == ENTER) {
		// ウィンドウを増やす
		player->sel_cursor++;
		
		// カーソル設定
		set_cursor_item_over(player);
		
		// ウィンドウ描画を許可
		player->draw_window_flag = true;
		
		// アイテムを捨てない
		if (player->basic[0].num == 1) {
			return true;
		}
		
		// テキスト更新
		else if (player->sel_cursor == 1) {
			print_textbox("アイテムを選んでください。");
		}
		
		// テキスト更新
		else if (player->sel_cursor == 2) {
			print_textbox("本当に捨てても　よろしいですか？");
		}
		
		// もう一度アイテムを決めなおす
		else if (player->basic[2].num == 1) {
			// ウィンドウを閉じる
			player->sel_cursor--;
		}
		
		// アイテムを選んだ
		else if (player->sel_cursor == 3) {
			bool success;
			
			// 選んだアイテムを捨てる
			success = dispose_item(player, player->basic[1].num + 1);
			enter_textbox();
			
			// 失敗したら
			if (success == false) return false;
			
			// 選んだアイテムを消費
			player->item[player->basic[1].num + 1] = None;
			
			// アイテムを並び替える
			sort_item(player);
			
			// プレイヤーのアイテム数を減らす
			player->item_num--;
			
			return true;
		}
	}
	// Back Space
	else if (key == BACK_SPACE) {
		// スタートウィンドウまでしか戻せない
		if (player->sel_cursor > 0) {
			// ウィンドウを閉じる
			player->sel_cursor--;
			
			// ウィンドウ描画を許可
			player->draw_window_flag = true;
		}
	}
	
	// カーソルループ
	loop_cursor(player);
	
	return false;
}

// アイテム容量オーバー処理
void set_item_over(Player *player)
{
	// ウィンドウ初期化
	set_start_cursor_item_over(player);
	
	// ウィンドウの描画を許可
	player->draw_window_flag == true;
	
	print_textbox("しかし　容量オーバーです。なにか捨てて　アイテムを入手しますか？");
	
	// 終了フラグ
	bool flag;
	
	while (true) {
		// 画面更新
		print_item_over(player);
		
		// 入力と処理
		flag = control_item_over(player);
		
		// 終了
		if (flag) break;
	}
	
	// 文字スクリーン初期化
	init_string();
}

// 宝箱を開ける
void get_treasure(Player *player)
{
	// テキスト更新
	print_textbox("プレイヤーは　宝箱を　あけた。");
	
	// アイテムをランダムに手に入れる
	Item obj;
	int num = get_scope_rand(1, 100);
	
	// 地図はプレイヤーが持っている場合出てこないようにする
	Item consum;
	if (check_item_map(player)) consum = Release;
	else consum = Map;
	
	// 消費アイテム確率 50%
	if (num <= 50)      obj = (Item)get_scope_rand((int)Herb, (int)consum);
	// 武器アイテム確率 25%
	else if (num <= 75) obj = (Item)get_scope_rand((int)Stick, (int)Sword);
	// 防具アイテム確率 25%
	else                obj = (Item)get_scope_rand((int)Cloth, (int)Iron);
	
	// テキスト更新
	sprintf(Text, "なんと　%s　がはいっていた！", Item_str[obj]);
	print_textbox(Text);
	enter_textbox();
	
	// アイテム容量がオーバーする場合
	if (player->item_num == ITEM_NUM_MAX - 1) {
		set_item_over(player);
		
		// もう一度確認
		if (player->item_num == ITEM_NUM_MAX - 1) return;
	}
	
	// アイテムを入手
	player->item_num++;
	player->item[player->item_num] = obj;
}

// テレポート
void act_teleport(Player *player)
{
	// ランダムに移動
	char x = get_pos_rand(0, Stage_size - 2);
	char y = get_pos_rand(0, Stage_size - 2);
	
	// オブジェクトと重なった場合
	if (Stage[y][x] != Space) {
		Pos dir[4] = {
			{0, -1},// 北
			{1, 0},	// 東
			{0, 1},	// 南
			{-1, 0}	// 西
		};
		// 4 方向から通路を探索して座標をずらす
		for (int i = 0; i < 4; i++) {
			if (Stage[y + dir[i].y][x + dir[i].x] == Space) {
				// 座標を確定
				x += dir[i].x;
				y += dir[i].y;
				
				// プレイヤーの方向を更新
				player->dir = i;
			}
		}
	}
	// プレイヤーの座標を更新
	player->x = x;
	player->y = y;
}

// 地雷
void act_landmine(Player *player)
{
	// フラッシュ演出
	print_damage_flash();
	
	// テキストボックスの初期化
	init_textbox();
	
	// ダメージ計算
	int ref = get_scope_rand(3, 5);	// 基準値
	int damage = ref * player->level;	// プレイヤーのレベルによって違う
	player->hp -= damage;	// プレイヤーの HP を減らす
	
	// 文字列作成
	sprintf(Text, "とつぜん　タイルが爆発した！　プレイヤーは %2d のダメージを受けた！", damage);
	print_textbox(Text);
	enter_textbox();
	
	// トラップ削除
	Stage[player->y][player->x] = Space;
}

// 矢
void act_arrow(Player *player)
{
	// テキストボックスの初期化
	init_textbox();
	
	// ダメージ計算
	int ref = get_scope_rand(1, 3);	// 基準値
	int damage = ref * player->level;	// プレイヤーのレベルによって違う
	player->hp -= damage;	// プレイヤーの HP を減らす
	
	// 文字列作成
	sprintf(Text, "とつぜん　矢が飛んできた！　プレイヤーは %2d のダメージを受けた！", damage);
	print_textbox(Text);
	enter_textbox();
}

// ゴール
void act_goal(Player *player)
{
	// テキストボックスの初期化
	init_textbox();
	
	// テキスト更新
	print_textbox("ついに　ほうせき　をみつけた！！");
	print_textbox("あとは　町に戻るだけだ。");
	enter_textbox();
	
	// アイテム容量がオーバーする場合
	if (player->item_num == ITEM_NUM_MAX - 1) {
		set_item_over(player);
		
		// もう一度確認
		if (player->item_num == ITEM_NUM_MAX - 1) return;
	}
	
	// アイテムを入手
	player->item_num++;
	player->item[player->item_num] = Jewelry;
	
	// ゴール地点を削除
	Stage[player->y][player->x] = Space;
}

// 宝石をもっているか確認
bool check_jewelry(Player *player)
{
	for (int i = 1; i <= player->item_num; i++) {
		if (player->item[i] == Jewelry) return true;
	}
	return false;
}

// ゲームクリア
void game_clear(Player *player)
{
	// グラフィックスクリーン初期化
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		for (int j = 0; j < SCREEN_WIDTH; j++) {
			Screen[i][j] = 0;
		}
	}
	
	// 文字の描画
	draw_string(12, 8, "ゲームクリア");
	
	// 画面表示
	print_screen_basic();
	enter_textbox();
	
	for (int i = 0; i < 9; i++) {
		// 文字スクリーン初期化
		init_string();
		
		// 文字の描画
		draw_string(10, 17 - i, "Thank you for playing.");
		
		// 画面表示
		print_screen_basic();
		
		// 遅延
		mySleep(256);
	}
	enter_textbox();
	
	// この後の処理を飛ばすためにモード変更
	player->control = DEAD;
	
	// ゲーム終了フラグ
	player->end_flag = true;
}

// スタート地点
void set_start_town(Player *player)
{
	// スタートアップ
	set_start_cursor_town(player);
	
	// テキストボックス初期化
	init_textbox();
	
	// 文字スクリーン初期化
	init_string();
	
	// ゲームクリアするか確認
	if (check_jewelry(player)) {
		// ゲームクリア
		game_clear(player);
		return;
	}
	
	// ウィンドウの描画を許可
	player->draw_window_flag = true;
	
	// タウンモードに切り替え
	player->control = TOWN;
	
	// テキスト更新
	print_textbox("町に　もどってきた。");
}

// ステージオブジェクトによる処理
void act_object(Player *player)
{
	// プレイヤーの座標に何があるか調べる
	Stage_state obj = Stage[player->y][player->x];
		
	// オブジェクトそれぞれの処理
	switch (obj) {
		// スタート
		case Start :
			set_start_town(player);
			return;
		
		// 赤色のタイル
		case Red : 
			strcpy(Text, "赤色のタイルだ");
			break;
			
		// 緑色のタイル
		case Green :
			strcpy(Text, "緑色のタイルだ");
			break;
			
		// 青色のタイル
		case Blue :
			strcpy(Text, "青色のタイルだ");
			break;
			
		// 白色のタイル
		case White :
			strcpy(Text, "白色のタイルだ");
			break;
			
		// 宝箱
		case Treasure :
			// テキストボックスの初期化
			init_textbox();
		
			// アイテムゲット
			get_treasure(player);
			
			// 宝箱削除
			Stage[player->y][player->x] = Space;
			return;
			
		// 固定敵
		case Enemy :
			// バトルスタート
			init_battle(player);
			return;
			
		// 回転床
		case Rotate : {
			// プレイヤーの現在の向きを反対方向に変えるテーブル
			char rev[4] = {NORTH, WEST, SOUTH, EAST};
			char num = player->dir;
			player->dir = rev[num];
			return;
		}
			
		// テレポート
		case Teleport : 
			act_teleport(player);
			return;
		
		// 地雷
		case Landmine : 
			act_landmine(player);
			return;
		
		// 矢
		case Arrow : 
			act_arrow(player);
			return;
		
		// ゴール
		case Goal : 
			act_goal(player);
			return;
			
		// それ以外
		default :
			return;
	}
	
	// テキストボックスの初期化
	init_textbox();
	
	// テキスト更新
	print_textbox(Text);
	enter_textbox();
}

// モンスターのスキル使用MP確認
bool consum_mp_monster(Player *player, int num, int cost)
{
	// MP確認
	if (player->monster[num].mp < cost) {
		// テキストを更新
		print_textbox("しかし　ＭＰ　が足りない。");
		return false;
	}
	
	// MP消費
	player->monster[num].mp -= cost;
	return true;
}

// プレイヤーへの通常攻撃
void attack_to_player(Player *player, int num)
{	
	// テキスト更新
	sprintf(Text, "%sの　こうげき！", player->monster[num].name);
	print_textbox(Text);
	
	// ダメージを計算
	int value = player->monster[num].attack;
	if (player->monster[num].power_buff) value *= 1.5;	// バフがついている場合
	int damage = get_basic_rand(value);
	damage -= player->defense;	// 防御力の分減算
	
	// 攻撃力より防御力のほうが大きい場合
	if (get_no_damage(damage) == false) return;
	
	// 敵のHPを減らす
	player->hp -= damage;
	
	// テキスト更新
	sprintf(Text, "プレイヤーに　%2d　のダメージ！", damage);
	print_textbox(Text);
}

// 逃げるモンスター
void escape_monster(Player *player, int num)
{
	// テキスト更新
	sprintf(Text, "%s　は逃げ出した！", player->monster[num].name);
	print_textbox(Text);
	
	// モンスターの経験値、お金を削除
	player->monster[num].exp = 0;
	player->monster[num].gold = 0;
	player->monster[num].hp = 0;
	
	// 最後の一匹だった場合
	if (player->monst_num == 1) {
		// 確認ボタン
		enter_textbox();
		
		// バトル終了
		end_battle(player);
		return;
	}
	
	// たおしたモンスターを一番後ろにもっていく
	sort_monster(player, num);
	
	// モンスターの数を減らす
	player->monst_num--;
}

// スライムの行動
void act_slime(Player *player, int num)
{
	// 乱数取得
	int act_num = get_scope_rand(1, 100);
	
	// 通常攻撃確率 95%
	if (act_num <= 95) {
		attack_to_player(player, num);
	}
	// 逃げる確率5%
	else {
		escape_monster(player, num);
	}
}

// ゴブリンの行動処理
void act_goblin(Player *player, int num)
{
	// 乱数取得
	int act_num = get_scope_rand(1, 100);
	
	// 通常攻撃確率 85%
	if (act_num <= 85) {
		attack_to_player(player, num);
	}
	// 何もしない確率 10%
	else if (act_num <= 95) {
		// テキスト更新
		print_textbox("ゴブリンは　下品にわらっている。");
	}
	// 逃げる確率 5%
	else {
		escape_monster(player, num);
	}
}

// オークの行動処理
void act_orc(Player *player, int num)
{
	// 乱数取得
	int act_num = get_scope_rand(1, 100);
	
	// 通常攻撃確率 80%
	if (act_num <= 80) {
		attack_to_player(player, num);
	}
	// 何もしない確率 10%
	else if (act_num <= 90) {
		// テキスト更新
		print_textbox("オークは　舌なめずりをしている。");
	}
	// ちからをためる
	else {
		// テキスト更新
		print_textbox("オークは　ちからをためている。");
		
		// MP消費
		if (consum_mp_monster(player, num, COST_POWER_UP) == false) return;
		player->monster[num].power_buff = true;
		player->monster[num].count = 3;
	}
}

// モンスターのデバフ処理
bool debuff_monster_skill(Player *player, int num, char *str, int cost)
{
	// デバフ成功失敗
	bool success = true;
	
	// テキスト更新
	sprintf(Text, "%sは　%sを　してきた。", player->monster[num].name, str);
	print_textbox(Text);
	
	// MP消費
	if (consum_mp_monster(player, num, cost) == false) return false;
	
	// すでに別のデバフがかかってる場合
	if (player->count > 0) success = false;
	
	// 固定確率 20%
	int rand = get_scope_rand(1, 100);
	if (rand <= 20) {
		sprintf(Text, "プレイヤーは　%sに　かかってしまった。", str);
		print_textbox(Text);
	}
	else success = false;
	
	// デバフ失敗
	if (success == false) {
		sprintf(Text, "しかし　プレイヤーは　%sに　かからなかった。", str);
		print_textbox(Text);
		return false;
	}
	
	// デバフ成功
	return true;
}

// スケルトン行動処理
void act_skeleton(Player *player, int num)
{
	// 乱数取得
	int act_num = get_scope_rand(1, 100);
	
	// 通常攻撃確率 70%
	if (act_num <= 70) {
		attack_to_player(player, num);
	}
	// 何もしない確率 10%
	else if (act_num <= 80) {
		// テキスト更新
		print_textbox("スケルトンは　骨をならしている。");
	}
	// 挑発
	else {
		if (debuff_monster_skill(player, num, "ちょうはつ", COST_PROVO)) {
			player->anger_debuff = true;
			player->count = 3;	// 3ターン
		}
	}
}

// モンスターのこうげきスキル
void act_monster_skill(Player *player, int num, char *str, int cost, int value)
{
	// テキスト更新
	sprintf(Text, "%sの　%s！", player->monster[num].name, str);
	print_textbox(Text);
	
	// MP消費
	if (consum_mp_monster(player, num, cost) == false) return;
	
	// ダメージを計算
	int damage = get_basic_rand(value);
	damage -= player->defense;	// 防御力の分減算
	
	// プレイヤーの攻撃力より防御力のほうが大きい場合
	if (get_no_damage(damage) == false) return;
	
	// プレイヤーのHPを減らす
	player->hp -= damage;
	
	// テキスト更新
	sprintf(Text, "プレイヤー　に　%2d　のダメージ！", damage);
	print_textbox(Text);
}

// 回復するべきモンスターがいるか確認
int check_recover_monster(Player *player)
{
	// HPが最大の半分を下回っている場合
	for (int i = 0; i < player->monst_num; i++) {
		if (player->monster[i].hp < player->monster[i].hp_max / 2) {
			return i;
		}
	}
	
	// いなかった場合
	return -1;
}

// 選んだモンスターを回復
void recover_hp_monster(Player *player, int num, int r_num)
{
	// テキスト更新
	sprintf(Text, "%sの　ヒール！", player->monster[num].name);
	print_textbox(Text);

	// MP消費
	if (consum_mp_monster(player, num, COST_HEEL) == false) return;
	
	// 回復
	int rec = get_basic_rand(VALUE_HEEL);
	player->monster[r_num].hp += rec;
	
	// HP上限
	if (player->monster[r_num].hp > player->monster[r_num].hp_max) {
		player->monster[r_num].hp = player->monster[r_num].hp_max;
	}
}

// ウィザード行動処理
void act_wizard(Player *player, int num)
{
	// 乱数取得
	int act_num = get_scope_rand(1, 100);
	
	// 回復するべきモンスターがいるか確認
	int r_num = check_recover_monster(player);
	if (r_num >= 0) {
		recover_hp_monster(player, num, r_num);
		return;
	}
	
	// 通常攻撃確率 10%
	if (act_num <= 10) {
		attack_to_player(player, num);
	}
	// こうげき魔法確率 70%
	else if (act_num <= 70) {
		// プレイヤーのレベルが高い場合
		if (player->level >= 8) {
			act_monster_skill(player, num, "ギガサンダー", COST_THUNDER_GIGA, VALUE_THUNDER_GIGA);
		}
		// プレイヤーのレベルが低い場合
		else {
			act_monster_skill(player, num, "サンダー", COST_THUNDER, VALUE_THUNDER);
		}
	}
	// 催眠
	else {
		if (debuff_monster_skill(player, num, "さいみん", COST_HYPNOS)) {
			player->hypnos_debuff = true;
			player->count = 3;	// 3ターン
		}
	}
}

// フェアリー
void act_fairy(Player *player, int num)
{
	// 乱数取得
	int act_num = get_scope_rand(1, 100);
	
	// にげる確率 50%
	if (act_num <= 50) {
		escape_monster(player, num);
	}
	// 何もしない確率 19%
	else if (act_num <= 69) {
		// テキスト更新
		print_textbox("フェアリーは　羽を　やすめている。");
	}
	// ファイア確率 30%
	else if (act_num <= 99) {
		act_monster_skill(player, num, "ファイア", COST_FIRE, VALUE_FIRE);
	}
	// メガファイヤ確率 1%
	else {
		act_monster_skill(player, num, "メガファイア", COST_FIRE_M, VALUE_FIRE_M);
	}
}

// ドラゴン
void act_dragon(Player *player, int num)
{
	// 乱数取得
	int act_num = get_scope_rand(1, 100);
	
	// 通常攻撃確率 80%
	if (act_num <= 60) {
		attack_to_player(player, num);
	}
	// 何もしない確率 10%
	else if (act_num <= 70) {
		// テキスト更新
		print_textbox("ドラゴンは　ようすをみている。");
	}
	// ほのおのいき確率 20%
	else if (act_num <= 90) {
		act_monster_skill(player, num, "ほのおのいき", COST_HONONOIKI, VALUE_HONONOIKI);
		
		// フラッシュ演出
		print_damage_flash();
	}
	// ちからをためる
	else {
		// テキスト更新
		print_textbox("ドラゴンは　ちからをためている。");
		
		// MP消費
		if (consum_mp_monster(player, num, COST_POWER_UP) == false) return;
		player->monster[num].power_buff = true;
		player->monster[num].count = 4;
	}
}

// 指定のモンスターを行動させる
void act_monster_all(Player *player, int num)
{
	// なんのモンスターか識別
	int m_num = player->monster[num].num;
	
	// スライム
	if (m_num == SLIME) {
		act_slime(player, num);
	}
	// ゴブリン
	else if (m_num == GOBLIN) {
		act_goblin(player, num);
	}
	// オーク
	else if (m_num == ORC) {
		act_orc(player, num);
	}
	// スケルトン
	else if (m_num == SKELETON) {
		act_skeleton(player, num);
	}
	// ウィザード
	else if (m_num == WIZARD) {
		act_wizard(player, num);
	}
	// フェアリー
	else if (m_num == FAIRY) {
		act_fairy(player, num);
	}
	// ドラゴン
	else if (m_num == DRAGON) {
		act_dragon(player, num);
	}
}

// バフの確認更新
void set_monster_buff(Player *player, int num)
{
	// バフカウントがない場合
	if (player->monster[num].count == 0) {
		// バフ終了
		player->monster[num].power_buff = false;
	}
	
	// パワーバフがついている場合
	if (player->monster[num].power_buff) {
		// カウントダウン
		player->monster[num].count--;
	}
}

// 敵のターン処理
void act_enemy_turn(Player *player)
{	
	// 敵の数だけ繰り返す
	for (int i = 0; i < player->monst_num; i++) {
		// バフ確認
		set_monster_buff(player, i);
		
		// モンスター固有行動処理
		act_monster_all(player, i);
	}
}

// プレイヤーのデバフ処理
bool set_playaer_debuff(Player *player)
{
	// デバフカウントがない場合
	if (player->count == 0) {
		// デバフ終了
		player->anger_debuff = false;
		player->hypnos_debuff = false;
		print_textbox("デバフが　とけた！");
		return true;
	}
	
	// ランダムで早く解ける
	int num = get_scope_rand(1, 100);
	if (num <= 30) {
		// デバフ終了
		init_debuff(player);
		print_textbox("デバフが　とけた！");
		return true;
	}
	
	// デバフ
	if (player->anger_debuff || player->hypnos_debuff) {
		// カウントダウン
		player->count--;
	}
	return false;
}

// お金が足りないテキスト
void text_gold_shortage()
{
	print_textbox("お客さん　お金が足りませんよ。");
}

// アイテムを買う店の処理
void text_buy_item(Player *player, int num)
{
	// 値段テーブル
	int value[] = {
		0,GOLD_HERB,GOLD_HERB_U,GOLD_WATER,GOLD_RELEASE,0,0,	// 消費アイテム
		GOLD_STICK,GOLD_COPPER,GOLD_SWORD,0,	// 武器
		GOLD_CLOTH,GOLD_LEATHER,GOLD_IRON,	// 防具
	};
	//int num = player->basic[1].num;
	
	// 二枚目ウィンドウ
	if (player->sel_cursor == 2) {
		sprintf(Text, "%sは　%4d　ゴールドです。ほんとうに買いますか？", Item_str[num], value[num]);
		print_textbox(Text);
		return;
	}
	
	// 三枚目のウィンドウ
	else if (player->sel_cursor == 3) {
		// 買う
		if (player->basic[2].num == 0) {
			// お金があるか確認
			if (player->gold < value[num]) {
				text_gold_shortage();
			}
			// お金はある
			else {
				// アイテム容量の確認
				if (player->item_num == ITEM_NUM_MAX-1) {
					print_textbox("アイテムがいっぱいですよ。");
				}
				// アイテムを入手
				else {
					player->item_num++;
					player->item[player->item_num] = (Item)num;
					
					// お金を減らす
					player->gold -= value[num];
					print_textbox("お買い上げありがとうございます。");
				}
			}
		}
	}
	else return;
	
	// ウィンドウを戻す
	player->sel_cursor = 1;
}

// 宿屋で寝た場合の演出
void print_inn()
{
	// カーソル位置初期化
	move_cursor_console(2,2);
	
	// 直接画面表示
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		// カーソル設定
		move_cursor_console(2,2 + i);
		
		// 表示
		printf("　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　");
		
		// 半分出力したところで一旦止める
		if (i == 9) mySleep(256);
	}
	mySleep(256);
	// 画面更新
	//print_screen(player);
}

// 宿屋
void act_inn(Player *player)
{
	// 一枚目ウィンドウ
	if (player->sel_cursor == 1) {
		print_textbox("やどやへ　ようこそ");
		print_textbox("ひとばん　10　ゴールドに　なりますが　お泊りになりますか？");
		return;
	}
	
	// 二枚目のウィンドウ
	else if (player->sel_cursor == 2) {
		// 泊まる
		if (player->basic[1].num == 0) {
			// お金があるか確認
			if (player->gold < 10) {
				text_gold_shortage();
			}
			// お金はある
			else {
				// 全回復
				player->hp = player->hp_max;
				player->mp = player->mp_max;
				
				// お金を減らす
				player->gold -= 10;
				
				// 演出
				print_inn();
				
				print_textbox("おはようございます。");
			}
		}
	}
	
	// ウィンドウを戻す
	player->sel_cursor = 0;
}

// どうぐや
void act_tool_shop(Player *player)
{
	text_buy_item(player, player->basic[1].num + 1);
}

// ぶきや
void act_weapon_shop(Player *player)
{
	text_buy_item(player, Weapon + player->basic[1].num + 1);
}

// ぼうぐや
void act_armor_shop(Player *player)
{
	text_buy_item(player, Armor + player->basic[1].num + 1);
}

// フィールド処理のコントロール
void control_field(Player *player, const char key)
{
	// 内部処理を許可する
	player->process_flag = true;
	
	// 前に進む
	if (key == 'w') {
		// 壁判定
		if (check_wall(player)) {
			// プレイヤーの座標を更新
			set_pos_advance(player);
		}
		else {
			// 内部処理は許可しない
			player->process_flag = false;
		}
	}
	// 左に90度回る
	else if (key == 'a') {
		// プレイヤーの向きを更新
		player->dir--;
		
		// 内部処理は許可しない
		player->process_flag = false;
	}
	// 右に90度回る
	else if (key == 'd') {
		// プレイヤーの向きを更新
		player->dir++;
		
		// 内部処理は許可しない
		player->process_flag = false;
	}
	// Enter
	else if (key == ENTER) {
		// スタートウィンドウ設定
		set_start_cursor_menu(player);
		
		// ウィンドウの描画を許可
		player->draw_window_flag = true;
		
		// メニューモードに切り替え
		player->control = MENU;
	}
	// ゲーム強制終了
	else if (key == CTRL_C) {
		player->end_flag = true;
	}
	
	// 回転してループするように
	if (player->dir > 3) player->dir = 0;
	else if (player->dir < 0) player->dir = 3;
}

// メニュー処理のコントロール
void control_menu(Player *player, const char key)
{	
	// アクセス名省略
	char sel = player->sel_cursor;
	
	// 許可しない
	player->process_flag = false;
	
	// 先頭のカーソルが描画許可されていない場合
	if (player->basic[sel].draw_flag == false) {
		// キー入力が Back Space と Ctrl+C ではない場合
		if (key != BACK_SPACE && key != CTRL_C) return;
	}
	
	// 上にカーソルを動かす
	if (key == 'w') {
		// 履歴を残す
		player->basic[sel].history = player->basic[sel].y;
		
		// カーソルの座標を更新
		player->basic[sel].y--;
		player->basic[sel].num--;
	}
	// 下にカーソルを動かす
	else if (key == 's') {		
		// 履歴を残す
		player->basic[sel].history = player->basic[sel].y;
		
		// カーソルの座標を更新
		player->basic[sel].y++;
		player->basic[sel].num++;
	}
	// Enter
	else if (key == ENTER) {
		// ウィンドウを増やす
		player->sel_cursor++;
		
		// カーソル設定
		set_cursor_menu(player);
		
		// ウィンドウ描画を許可
		player->draw_window_flag = true;
		
		// スキルの行動処理
		if (player->basic[0].num == 2) {
			act_skill_menu(player);
		}
		
		// アイテムの行動処理
		else if (player->basic[0].num == 1) {
			act_item_menu(player);
		}
	}
	// Back Space
	else if (key == BACK_SPACE) {
		// 閉じるウィンドウが最後の場合
		if (player->sel_cursor == 0) {
			// フィールドモードに切り替え
			player->control = FIELD;
			
			// 文字専用スクリーン初期化
			init_string();
			
			// テキストボックス初期化
			init_textbox();
		}
		else {
			// ウィンドウを閉じる
			player->sel_cursor--;
		}
		
		// ウィンドウ描画を許可
		player->draw_window_flag = true;
		
		// カーソル描画を許可
		//player->use_cursor_flag = true;
		
		// 内部処理は許可しない
		player->process_flag = false;
	}
	// ゲーム強制終了
	else if (key == CTRL_C) {
		player->end_flag = true;
	}
	
	// カーソルループ
	loop_cursor(player);
}

// バトル処理のコントロール
void control_battle(Player *player, const char key)
{
	// アクセス名省略
	char sel = player->sel_cursor;
	
	// モンスターのターンは許可しない
	player->process_flag = false;
	
	// 上にカーソルを動かす
	if (key == 'w') {
		// 履歴を残す
		player->basic[sel].history = player->basic[sel].y;
		
		// カーソルの座標を更新
		player->basic[sel].y--;
		player->basic[sel].num--;
	}
	// 下にカーソルを動かす
	else if (key == 's') {		
		// 履歴を残す
		player->basic[sel].history = player->basic[sel].y;
		
		// カーソルの座標を更新
		player->basic[sel].y++;
		player->basic[sel].num++;
	}
	// Enter
	else if (key == ENTER) {
		// ウィンドウを増やす
		player->sel_cursor++;
		
		// カーソル設定
		set_cursor_battle(player);
		
		// ウィンドウ描画を許可
		player->draw_window_flag = true;
		
		// こうげき処理
		if (player->basic[0].num == 0) {
			attack_to_monster(player);
		}
		
		// スキル処理
		else if (player->basic[0].num == 1) {
			act_skill_battle(player);
		}
		
		// アイテム処理
		else if (player->basic[0].num == 2) {
			act_item_battle(player);
		}
		
		// にげる処理
		else if (player->basic[0].num == 3) {
			escape_battle(player);
		}
	}
	// Back Space
	else if (key == BACK_SPACE) {
		// スタートウィンドウまでしか戻せない
		if (player->sel_cursor > 0) {
			// ウィンドウを閉じる
			player->sel_cursor--;
			
			// ウィンドウ描画を許可
			player->draw_window_flag = true;
		}
	}
	// ゲーム強制終了
	else if (key == CTRL_C) {
		player->end_flag = true;
	}
	
	// カーソルループ
	loop_cursor(player);
}

// マップ処理のコントロール
void control_map(Player *player, const char key)
{
	// 許可しない
	player->process_flag = false;
	
	// マップが下に移動
	if (key == 'w') {
		player->map_p.y++;
	}
	// マップが上に移動
	else if (key == 's') {
		player->map_p.y--;
	}
	// ヘルプ
	else if (key == 'h') {
		// テキストボックス初期化
		init_textbox();
		
		// テキスト更新
		print_textbox("Ｗ：上にスクロール　Ｓ：下にスクロール");
		print_textbox("Ｂａｃｋ　Ｓｐａｃｅ：マップを見るのをやめる");
		enter_textbox();
	}
	// Back Space
	else if (key == BACK_SPACE) {
		// メニューモードに切り替え
		player->control = MENU;
		
		// ウィンドウを戻す
		player->sel_cursor = 1;
		
		// ウィンドウの描画を許可
		player->draw_window_flag = true;
	}
	// Ctrl + C
	else if (key == CTRL_C) {
		player->end_flag = true;
	}
	
	// 画面のサイズのほうが大きいイージーモード
	if (player->mode == EASY) {
		player->map_p.y = 0;
		return;
	}
	
	// マップを移動できる限界
	int limit = SCREEN_HEIGHT - Stage_size;
	
	// マップの移動制限
	if (player->map_p.y > 0) player->map_p.y = 0;
	else if (player->map_p.y < limit) player->map_p.y = limit;
}

// タウン処理のコントロール
void control_town(Player *player, const char key)
{
	// アクセス名省略
	char sel = player->sel_cursor;
	
	// 内部処理は許可しない
	player->process_flag = false;
	
	// 上にカーソルを動かす
	if (key == 'w') {
		// 履歴を残す
		player->basic[sel].history = player->basic[sel].y;
		
		// カーソルの座標を更新
		player->basic[sel].y--;
		player->basic[sel].num--;
	}
	// 下にカーソルを動かす
	else if (key == 's') {		
		// 履歴を残す
		player->basic[sel].history = player->basic[sel].y;
		
		// カーソルの座標を更新
		player->basic[sel].y++;
		player->basic[sel].num++;
	}
	// Enter
	else if (key == ENTER) {
		// ウィンドウを増やす
		player->sel_cursor++;
		
		// カーソル設定
		set_cursor_town(player);
		
		// ウィンドウ描画を許可
		player->draw_window_flag = true;
		
		// やどや
		if (player->basic[0].num == 0) {
			act_inn(player);
		}
		
		// どうぐや
		else if (player->basic[0].num == 1) {
			act_tool_shop(player);
		}
		
		// ぶきや
		else if (player->basic[0].num == 2) {
			act_weapon_shop(player);
		}
		
		// ぼうぐや
		else if (player->basic[0].num == 3) {
			act_armor_shop(player);
		}
	}
	// Back Space
	else if (key == BACK_SPACE) {
		// 閉じるウィンドウが最後の場合
		if (player->sel_cursor == 0) {
			// フィールドモードに切り替え
			player->control = FIELD;
			
			// 文字専用スクリーン初期化
			init_string();
			
			// テキストボックス初期化
			init_textbox();
		}
		else {
			// ウィンドウを閉じる
			player->sel_cursor--;
		}
		
		// ウィンドウ描画を許可
		player->draw_window_flag = true;
		
		// カーソル描画を許可
		//player->use_cursor_flag = true;
	}
	// ゲーム強制終了
	else if (key == CTRL_C) {
		player->end_flag = true;
	}
	
	// カーソルループ
	loop_cursor(player);
}

// フィールドモードでのキー入力
char input_field()
{
	// 入力
	while (true) {
		char input = getch();
		
		// W
		if (input == 'w') {
			return 'w';
		}
		// A
		else if (input == 'a') {
			return 'a';
		}
		// D
		else if (input == 'd') {
			return 'd';
		}
		// Enter
		else if (input == ENTER) {
			return ENTER;
		}
		// Ctrl + C
		else if (input == CTRL_C) {
			return CTRL_C;
		}
	}
}

// メニューモードでのキー入力
char input_menu()
{
	// 入力
	while (true) {
		char input = getch();
		
		// W
		if (input == 'w') {
			return 'w';
		}
		// S
		else if (input == 's') {
			return 's';
		}
		// Enter
		else if (input == ENTER) {
			return ENTER;
		}
		// Back Space
		else if (input == BACK_SPACE) {
			return BACK_SPACE;
		}
		// Ctrl + C
		else if (input == CTRL_C) {
			return CTRL_C;
		}
	}
}

// マップモードでのキー入力
char input_map()
{
	// 入力
	while (true) {
		char input = getch();
		
		// W
		if (input == 'w') {
			return 'w';
		}
		// S
		else if (input == 's') {
			return 's';
		}
		// H
		else if (input == 'h') {
			return 'h';
		}
		// Back Space
		else if (input == BACK_SPACE) {
			return BACK_SPACE;
		}
		// Ctrl + C
		else if (input == CTRL_C) {
			return CTRL_C;
		}
	}
}

// 指定のキーしか入力されない
char input_key(Player *player)
{
	// フィールドモード
	if (player->control == FIELD) {
		return input_field();
	}
	// メニューモード
	else if (player->control == MENU) {
		return input_menu();
	}
	// マップモード
	else if (player->control == MAP) {
		return input_map();
	}
	// バトルモード　タウンモード
	else {
		// メニューと操作キーは同じ
		return input_menu();
	}
}

// デバフ実行
bool act_debuff(Player *player)
{
	// 関係ない場合
	if (player->control != BATTLE || player->count == 0) return true;
	
	// デバフ解除
	if (set_playaer_debuff(player)) return true;
	
	// 挑発の場合
	if (player->anger_debuff) {
		// テキスト更新
		print_textbox("プレイヤーは　ちょうはつ にかかっている！");
		
		// こうげき
		attack_to_monster(player);
	}
	
	// 催眠の場合
	else if (player->hypnos_debuff) {
		// テキスト更新
		print_textbox("プレイヤーは　さいみん　にかかっている！");
	}
	
	return false;
}

// プレイヤーの操作
void control(Player *player)
{
	// プレイヤーの死亡
	if (player->control == DEAD) return;
	
	// デバフ実行
	if (act_debuff(player) == false) return;
	
	// キー入力
	char key = input_key(player);
	
	// フィールドを操作
	if (player->control == FIELD) {
		control_field(player, key);
	}
	
	// メニューを操作
	else if (player->control == MENU) {
		control_menu(player, key);
	}
	
	// バトルでの操作
	else if (player->control == BATTLE) {
		control_battle(player, key);
	}
	
	// マップを操作
	else if (player->control == MAP) {
		control_map(player, key);
	}
	
	// 拠点での操作
	else if (player->control == TOWN) {
		control_town(player, key);
	}
}

// 復活
void revival(Player *player)
{
	// 1000ゴールド払い復活
	player->gold -= 1000;
	
	// フィールドモードに切り替え
	player->control = FIELD;
	
	// デバフ初期化
	init_debuff(player);
	player->process_flag = false;
	
	// ステータス更新
	player->x = 1;	// 初期座標 x
	player->y = 1;	// 初期座標 y
	player->hp = player->hp_max;
	player->mp = player->mp_max;
	
	print_textbox("復活料金として教会から　1000　ゴールド　ちょうしゅうされました。");
	enter_textbox();
	
	// 画面更新
	print_screen(player);
}

// ゲームオーバー
void game_over(Player *player)
{
	// テキストボックス初期化
	init_textbox();
	
	// テキスト更新
	print_textbox("プレイヤーは　ちからつきてしまった。");
	enter_textbox();
	
	// グラフィックスクリーン初期化
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		for (int j = 0; j < SCREEN_WIDTH; j++) {
			Screen[i][j] = 0;
		}
	}
	
	// 文字スクリーン初期化
	init_string();
	
	// 文字の描画
	draw_string(12, 8, "ゲームオーバー");
	
	// 画面表示
	print_screen_basic();
	
	// テキストボックス初期化
	init_textbox();
	enter_textbox();
	
	// 復活の可能性
	if (player->gold >= 1000) {
		// 復活
		revival(player);
		return;
	}
	
	// ゲームオーバーフラグ更新
	player->game_over_flag = true;
	
	// ゲームモード変更1
	player->control = DEAD;
}

// 画面表示終わりでの処理
void process_back(Player *player)
{
	// 許可されていない場合処理なし
	if (player->process_flag == false) return;
	
	// フィールドモード
	if (player->control == FIELD) {
		// エンカウント処理
		act_encount(player);
		
		// オブジェクト処理
		act_object(player);
	}
	
	// バトルモード
	else if (player->control == BATTLE) {
		// 敵のターン
		act_enemy_turn(player);
		
		// ウィンドウの描画を許可
		player->draw_window_flag = true;
		
		// モンスターが逃げて終了した場合Enterしたくない
		if (player->control == BATTLE) {
			// 確認
			enter_textbox();
			
			// カーソル初期化
			set_start_cursor_battle(player);
		}
	}
	
	// プレイヤーが死んでしまった場合
	if (player->hp <= 0) {
		// ゲームオーバー
		game_over(player);
		return;
	}
		
	// 画面表示
	print_screen(player);
}

int main()
{
	// 画面設定
	//enableVT();
	off_cursor_console();
	
	// スタートメニュー
	int mode = start_menu();
	
	// 同じモードはループ
	while (true) {
		// 変数、オブジェクトの生成
		Player player;
		
		// 変数の初期化処理
		init(mode, &player);
		
		// ステージの作成
		create_stage(mode);
		
		// メインループ
		while (true) {
			// 画面描画表示
			print_screen(&player);
			
			// 画面表示終わりでの処理
			process_back(&player);
			
			// プレイヤーの操作処理
			control(&player);
			
			// ゲームオーバー
			if (player.game_over_flag) break;
			
			// ゲームの終了
			if (player.end_flag) break;
		}
		// ステージの開放
		free_stage_map();
		
		// ゲームの終了
		if (player.end_flag) break;
	}
	
	// カーソルをオン
	on_cursor_console();
	
	return 0;
}