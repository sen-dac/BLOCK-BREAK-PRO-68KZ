//============================================================================================
// Application : BLOCK BREAK PRO-68KZ
// Platform    : SHARP X68000 / Human68k
// Build       : GCC真里子版(ver1.42) + XCライブラリ + Z-MUSIC(v2.00)
//--------------------------------------------------------------------------------------------
// Filename    : sTitle.c
// Version     : 2.0.0
// Author      : SEN::DAC
// Description : シーン[タイトル]
//============================================================================================
//--------------------------------------------------------------------------------------------
// インクルードファイル
//--------------------------------------------------------------------------------------------
#include "sTitle.h"

//--------------------------------------------------------------------------------------------
// 定数定義 (内部用)
//--------------------------------------------------------------------------------------------
#define BRL_BASE_POS_X  (SCREEN_WIDTH  / 2)   // ボールとラケットのベースX座標
#define BRL_BASE_POS_Y  (SCREEN_HEIGHT / 4)   // ボールとラケットのベースY座標
#define BALL_POS_X      (BRL_BASE_POS_X - 4)  // ボール：X座標
#define BALL_POS_Y      (BRL_BASE_POS_Y + 16) // ボール：Y座標
#define RACKET_L_POS_X  (BRL_BASE_POS_X - 16) // ラケット左：X座標
#define RACKET_L_POS_Y  (BRL_BASE_POS_Y + 24) // ラケット左：Y座標
#define RACKET_R_POS_X  BRL_BASE_POS_X        // ラケット右：X座標
#define RACKET_R_POS_Y  (BRL_BASE_POS_Y + 24) // ラケット右：Y座標

#define MENU_START      0                     // メニュー：スタート
#define MENU_EDIT       1                     // メニュー：エディット(未実装)
#define MENU_EXIT       2                     // メニュー：終了

#define MENU_MAX        3                     // 選択できるメニューの最大値
#define CURSOR_POS_X0   84                    // カーソル：X座標(Y0～Y2で共通)
#define CURSOR_POS_Y0   160                   // カーソル：Y座標(メニュー番号：0)
#define CURSOR_POS_Y1   176                   // カーソル：Y座標(メニュー番号：1)
#define CURSOR_POS_Y2   192                   // カーソル：Y座標(メニュー番号：2)
#define CURSOR_REPEAT   15                    // カーソル移動リピートフレーム数

#define WAIT_TIME_ENTER  2000                 // ボタン押下時のウェイト時間(ms単位：2秒)

//--------------------------------------------------------------------------------------------
// 型定義 (内部用)
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// プロトタイプ宣言 (static)
//--------------------------------------------------------------------------------------------
static VD vdf_s_sTitleRun(VD);                // シーンの実行
static VD vdf_s_sTitleRelease(VD);            // シーンの解放
static VD vdf_s_sTitleRunMenuSelect(VD);      // メニュー選択
static VD vdf_s_sTitleRunMenuEnter(VD);       // メニュー決定

//--------------------------------------------------------------------------------------------
// グローバル変数定義 (外部公開)
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// 静的変数定義 (static)
//--------------------------------------------------------------------------------------------
static U2 u2a_s_sTitleGrPal[DR_COL16];        // タイトル背景のパレット(16色)
static S1 s1_s_SelectMenu = 0;                // 選択中のメニュー番号
static S2 s2_s_CursorPosY = CURSOR_POS_Y0;    // カーソルY座標

static const S2 s2ac_s_CursorPosY[MENU_MAX] = // カーソル：Y座標テーブル
{
    CURSOR_POS_Y0,                            // Y座標(メニュー番号：0)
    CURSOR_POS_Y1,                            // Y座標(メニュー番号：1)
    CURSOR_POS_Y2,                            // Y座標(メニュー番号：2)
};

//--------------------------------------------------------------------------------------------
// 関数定義
//--------------------------------------------------------------------------------------------
//============================================================================================
// シーン[タイトル]：シーン内で使用する画像を全てロード
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : BL : (O) TRUE  : 成功
//                      FALSE : 失敗
//============================================================================================
BL blf_g_sTitleLoadImage(VD)
{
    U4  u4a_pcg[PCG_TILE_SIZE_32U4];                           // BMP → PCG変換用
    U2  u2a_pal[DR_COL16];                                     // 16色パレット登録用
    U1* const u1cp_bbuf = u1cpf_g_DrGetBmpBuf();               // BMPロード用バッファ取得
    U2* const u2cp_cbuf = u2cpf_g_DrGetCnvBuf();               // 画像変換用バッファ取得

    // 背景画像(GR)-16色BMP画像ロードと書込み
    if(!blf_g_DrLoadBmp16(SZ_IMGFN_BK_TITLE, u1cp_bbuf)) return FALSE;
    vdf_g_GrBmpToGr(u2cp_cbuf, u1cp_bbuf, CRT_W, CRT_H);       // BMP → GR 変換
    vdf_g_GrDraw(u2cp_cbuf, GP0, CRT_W, CRT_H);                // グラフィック画面に書き込み
    vdf_g_AppPBarAdd();                                        // プログレスバーを進める

    // 背景画像(GR)のパレットのロード(登録はシーン初期化処理で行う)
    if(!blf_g_DrLoadPal16(SZ_IMGFN_BK_TITLE, u2a_s_sTitleGrPal, BRIGHT_OFF)) return FALSE;
    vdf_g_AppPBarAdd();                                        // プログレスバーを進める

    // カーソル(SP)-16色BMP画像ロードと書込み
    if(!blf_g_DrLoadBmp16(SZ_IMGFN_CURSOR, u1cp_bbuf)) return FALSE;
    vdf_g_SpBmpToPcg(u4a_pcg, (U4*)u1cp_bbuf);                 // BMP → PCG 変換
    SP_DEFCG(PCG_CURSOR_TOP, SP_SIZE16, (U1*)u4a_pcg);         // PCGに画像転送
    vdf_g_AppPBarAdd();                                        // プログレスバーを進める

    // カーソル(SP)のパレットのロード(GR以外は登録をここで行っても良い)
    if(!blf_g_DrLoadPal16(SZ_IMGFN_CURSOR, u2a_pal, BRIGHT_OFF)) return FALSE;
    vdf_g_DrSetPal16(DR_PAL_PCG, PB_PCG_CURSOR, u2a_pal);      // 登録
    vdf_g_AppPBarAdd();                                        // プログレスバーを進める

    return TRUE;
}

//============================================================================================
// シーン[タイトル]：初期化処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
VD vdf_g_sTitleInit(VD)
{
    vdf_g_GrPagePri(GP0, GP1, GP2, GP3);                       // GRn画面プライオリティ設定
    vdf_g_DrSetPal16(DR_PAL_GRA, PB_GRA, u2a_s_sTitleGrPal);   // パレット登録
    s1_s_SelectMenu = 0;                                       // 選択メニュー初期化
    s2_s_CursorPosY = s2ac_s_CursorPosY[s1_s_SelectMenu];      // カーソルY座標初期化

    // ボール (動かないので初期化で一度だけ描画すれば良い)
    vdf_g_SpRegst(SPPL_BALL_TOP,                               // プレーン番号
                  BALL_POS_X,                                  // X座標
                  BALL_POS_Y,                                  // Y座標
                  SP_V_INV_OFF,                                // V-垂直反転
                  SP_H_INV_OFF,                                // H-水平反転
                  PB_PCG_BALL,                                 // パレットブロック番号
                  PCG_BALL_TOP,                                // PCG番号
                  SP_PRW_SP_BG0_BG1);                          // 描画優先度

    // ラケット左 (動かないので初期化で一度だけ描画すれば良い)
    vdf_g_SpRegst(SPPL_RACKET_TOP,                             // プレーン番号
                  RACKET_L_POS_X,                              // X座標
                  RACKET_L_POS_Y,                              // Y座標
                  SP_V_INV_OFF,                                // V-垂直反転
                  SP_H_INV_OFF,                                // H-水平反転
                  PB_PCG_RACKET,                               // パレットブロック番号
                  PCG_RACKET_TOP,                              // PCG番号
                  SP_PRW_SP_BG0_BG1);                          // 描画優先度

    // ラケット右 (動かないので初期化で一度だけ描画すれば良い)
    vdf_g_SpRegst(SPPL_RACKET_END,                             // プレーン番号
                  RACKET_R_POS_X,                              // X座標
                  RACKET_R_POS_Y,                              // Y座標
                  SP_V_INV_OFF,                                // V-垂直反転
                  SP_H_INV_ON,                                 // H-水平反転
                  PB_PCG_RACKET,                               // パレットブロック番号
                  PCG_RACKET_TOP,                              // PCG番号
                  SP_PRW_SP_BG0_BG1);                          // 描画優先度

    vdf_g_ObjStageSet(0);                                      // ステージをゼロクリア
    vdf_g_ObjScoreSet(0);                                      // スコアをゼロクリア

    vdf_g_AppSetProc(vdf_s_sTitleRun);                         // 実行に遷移
}

//============================================================================================
// シーン[タイトル]：実行処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
static VD vdf_s_sTitleRun(VD)
{
    vdf_s_sTitleRunMenuSelect();      // メニュー選択処理
    vdf_s_sTitleRunMenuEnter();       // メニュー決定処理

    vdf_g_CrtVblank();                // 描画タイミング待ち

    // カーソル描画
    vdf_g_SpRegst(SPPL_CURSOR_TOP,    // プレーン番号
                  CURSOR_POS_X0,      // X座標
                  s2_s_CursorPosY,    // Y座標
                  SP_V_INV_OFF,       // V-垂直反転
                  SP_H_INV_OFF,       // H-水平反転
                  PB_PCG_CURSOR,      // PB
                  PCG_CURSOR_TOP,     // PCG
                  SP_PRW_SP_BG0_BG1); // PRW
}

//============================================================================================
// シーン[タイトル]：解放処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
static VD vdf_s_sTitleRelease(VD)
{
    U2 u2a_pal[DR_COL16] = {0};                    // 16色全て黒のパレット

    vdf_g_DrSetPal16(DR_PAL_GRA, PB_GRA, u2a_pal); // 全黒パレットでGR画面を非表示

    vdf_g_SpHide(SPPL_BALL_TOP);                   // スプライト消去：ボール
    vdf_g_SpHide(SPPL_RACKET_TOP);                 // スプライト消去：ラケット左
    vdf_g_SpHide(SPPL_RACKET_END);                 // スプライト消去：ラケット右
    vdf_g_SpHide(SPPL_CURSOR_TOP);                 // スプライト消去：カーソル

    vdf_g_AppMoveProc();                           // 次のシーンに遷移
}

//============================================================================================
// メニュー選択処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
static VD vdf_s_sTitleRunMenuSelect(VD)
{
    static S1 cursor_count_up   = 0; // カーソル上移動リピート時間
    static S1 cursor_count_down = 0; // カーソル下移動リピート時間

    // 上ボタンが離された
    if(u2f_g_InputGetState(CONTROLLER_1, BTN_UP, BS_RELEASE)) cursor_count_up = 0;

    // 下ボタンが離された
    if(u2f_g_InputGetState(CONTROLLER_1, BTN_DOWN, BS_RELEASE)) cursor_count_down = 0;

    // 上ボタンが押し続け
    if(u2f_g_InputGetState(CONTROLLER_1, BTN_UP, BS_PRESS)) cursor_count_up ++;

    // 下ボタンが押し続け
    if(u2f_g_InputGetState(CONTROLLER_1, BTN_DOWN, BS_PRESS)) cursor_count_down ++;

    if(u2f_g_InputGetState(CONTROLLER_1, BTN_UP, BS_DOWN) ||       // 上ボタンを押した瞬間 or
       cursor_count_up >= CURSOR_REPEAT)                           // リピート時間を満たした
    {
        cursor_count_up = 0;                                       // リピート時間クリア
        m_pcmplay(SOUND_CHOICE, PCM_PAN_C, PCM_FR15_6KHZ);         // 効果音(カーソル：選択)

        s1_s_SelectMenu --;                                        // メニュー移動
        if(s1_s_SelectMenu < 0) s1_s_SelectMenu = MENU_MAX- 1;     // 一番上なら下に行く
        s2_s_CursorPosY = s2ac_s_CursorPosY[s1_s_SelectMenu];      // カーソル表示座標更新
    }

    if(u2f_g_InputGetState(CONTROLLER_1, BTN_DOWN, BS_DOWN) ||     // 下ボタンを押した瞬間 or
       cursor_count_down >= CURSOR_REPEAT)                         // リピート時間を満たした
    {
        cursor_count_down = 0;                                     // リピート時間クリア
        m_pcmplay(SOUND_CHOICE, PCM_PAN_C, PCM_FR15_6KHZ);         // 効果音(カーソル：選択)

        s1_s_SelectMenu ++;                                        // メニュー移動
        if(s1_s_SelectMenu >= MENU_MAX) s1_s_SelectMenu = 0;       // 一番下なら上に行く
        s2_s_CursorPosY = s2ac_s_CursorPosY[s1_s_SelectMenu];      // カーソル表示座標更新
    }
}

//============================================================================================
// メニュー決定処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
static VD vdf_s_sTitleRunMenuEnter(VD)
{
    if(u2f_g_InputGetState(CONTROLLER_1, BTN_A, BS_DOWN) ||       // Aボタンが押された または
       u2f_g_InputGetState(CONTROLLER_1, BTN_B, BS_DOWN) ||       // Bボタンが押された または
       u2f_g_InputGetState(CONTROLLER_1, BTN_START, BS_DOWN))     // STARTボタンが押された
    {
        switch(s1_s_SelectMenu)
        {
            case MENU_START:                                      // メニュー：スタート
                m_pcmplay(SOUND_ENTER, PCM_PAN_C, PCM_FR15_6KHZ); // 効果音(カーソル：決定)
                vdf_g_TimerWait(WAIT_TIME_ENTER);                 // 指定した時間処理を止める
                vdf_g_AppSetNext(vdf_g_sPlayInit);                // 次のシーンを設定
                vdf_g_AppSetProc(vdf_s_sTitleRelease);            // 解放に遷移
                break;

            case MENU_EDIT:                                       // メニュー：エディット
                m_pcmplay(SOUND_ENTER, PCM_PAN_C, PCM_FR15_6KHZ); // 効果音(カーソル：決定)
                // NOP                                            // エディットモードは未実装
                break;

            case MENU_EXIT:                                       // メニュー：終了
                m_pcmplay(SOUND_ENTER, PCM_PAN_C, PCM_FR15_6KHZ); // 効果音(カーソル：決定)
                vdf_g_AppSetNext(NULL);                           // シーンはNULL(アプリ終了)
                vdf_g_AppSetProc(vdf_s_sTitleRelease);            // 解放に遷移
                break;
        }
    }
}
