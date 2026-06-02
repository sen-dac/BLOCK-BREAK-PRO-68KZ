//============================================================================================
// Application : BLOCK BREAK PRO-68KZ
// Platform    : SHARP X68000 / Human68k
// Build       : GCC真里子版(ver1.42) + XCライブラリ + Z-MUSIC(v2.00)
//--------------------------------------------------------------------------------------------
// Filename    : racket.c
// Version     : 2.0.0
// Author      : SEN::DAC
// Description : ゲームオブジェクト[ラケット]の処理
//============================================================================================
//--------------------------------------------------------------------------------------------
// インクルードファイル
//--------------------------------------------------------------------------------------------
#include "racket.h"

//--------------------------------------------------------------------------------------------
// 定数定義 (内部用)
//--------------------------------------------------------------------------------------------
#define RACKET_DEF_X   ((FIELD_WIDTH / 2) - (RACKET_WIDTH_0 / 2))  // X座標 デフォルト
#define RACKET_DEF_Y   (SCREEN_HEIGHT - 16)                        // Y座標 デフォルト
#define RACKET_DEF_MX  2                                           // X移動量 デフォルト
#define RACKET_DEF_MY  2                                           // Y移動量 デフォルト
#define RACKET_WIDTH_0 32                                          // 横幅 Lv.0
#define RACKET_WIDTH_1 (RACKET_WIDTH_0 + 16)                       // 横幅 Lv.1
#define RACKET_WIDTH_2 (RACKET_WIDTH_1 + 16)                       // 横幅 Lv.2
#define RACKET_HEIGHT  8                                           // 縦幅

#define RACKET_LV_0    0                                           // ラケットレベル：0
#define RACKET_LV_1    1                                           // ラケットレベル：1
#define RACKET_LV_MAX  2                                           // ラケットレベル：2
#define RACKET_LV_NUM  3                                           // ラケットレベル：要素数

#define RACKET_SP_NUM  4                                           // ラケットSP最大枚数

//--------------------------------------------------------------------------------------------
// 型定義 (内部用)
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// プロトタイプ宣言 (static)
//--------------------------------------------------------------------------------------------
static VD vdf_s_ObjRacketRunMove(VD);      // ラケットの移動
static VD vdf_s_ObjRacketRunWallhit(VD);   // ラケットの移動制限

//--------------------------------------------------------------------------------------------
// グローバル変数定義 (外部公開)
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// 静的変数定義 (static)
//--------------------------------------------------------------------------------------------
static const U2 u2ac_s_ObjRacketWidth[RACKET_LV_NUM] = // ラケットの幅 3段階
{
    RACKET_WIDTH_0,
    RACKET_WIDTH_1,
    RACKET_WIDTH_2
};

static const U2 u2ac_s_ObjRacketPcg[RACKET_SP_NUM] =   // PCG
{
    (PCG_RACKET_TOP + 0),                              // 左端末
    (PCG_RACKET_TOP + 1),                              // 左中
    (PCG_RACKET_TOP + 1),                              // 右中
    (PCG_RACKET_TOP + 0)                               // 右端末(H反転表示)
};

static stOBJECT st_s_ObjRacket =                       // オブジェクト
{
    RACKET_DEF_X,                                      // X座標 初期値
    RACKET_DEF_Y,                                      // Y座標 初期値

    RACKET_DEF_MX,                                     // X移動量
    RACKET_DEF_MY,                                     // Y移動量

    RACKET_WIDTH_0,                                    // 横幅 初期値
    RACKET_HEIGHT,                                     // 縦幅 初期値
};

// 現在のラケットレベル(幅)
// オブジェクト毎の用途の違いから演算相手の符号の有無が異なる。
// このため、堅牢性を優先しオブジェクト構造体のメンバに含めず別で持つ方針とした。
static S1 s1_s_ObjRacketLv = 0;

//--------------------------------------------------------------------------------------------
// 関数定義
//--------------------------------------------------------------------------------------------
//============================================================================================
// ゲームオブジェクト[ラケット]の画像とパレットをロード
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : BL : (O) TRUE  : 成功
//                      FALSE : 失敗
//============================================================================================
BL blf_g_ObjRacketLoadImage(VD)
{
    U4  u4a_pcg[PCG_TILE_SIZE_32U4];                       // BMP → PCG変換用
    U2  u2a_pal[DR_COL16];                                 // 16色パレット登録用
    U1* const u1cp_bbuf = u1cpf_g_DrGetBmpBuf();           // BMPロード用バッファ取得

    // 16色BMP画像ロードと書込み
    // ラケット両端(SP)-16色BMP画像ロードと書込み：同じ画像をH反転して使う
    if(!blf_g_DrLoadBmp16(SZ_IMGFN_RACKET_E, u1cp_bbuf)) return FALSE;

    vdf_g_SpBmpToPcg(u4a_pcg, (U4*)u1cp_bbuf);             // BMP → PCG 変換
    SP_DEFCG(PCG_RACKET_TOP + 0, SP_SIZE16, (U1*)u4a_pcg); // PCGに画像転送
    vdf_g_AppPBarAdd();                                    // プログレスバーを進める

    // ラケット中(SP)-16色BMP画像ロードと書込み
    if(!blf_g_DrLoadBmp16(SZ_IMGFN_RACKET_C, u1cp_bbuf)) return FALSE;

    vdf_g_SpBmpToPcg(u4a_pcg, (U4*)u1cp_bbuf);             // BMP → PCG 変換
    SP_DEFCG(PCG_RACKET_TOP + 1, SP_SIZE16, (U1*)u4a_pcg); // PCGに画像転送
    vdf_g_AppPBarAdd();                                    // プログレスバーを進める

    // ラケット(SP)のパレットのロード(GR以外は登録をここで行っても良い)
    if(!blf_g_DrLoadPal16(SZ_IMGFN_RACKET_E, u2a_pal, BRIGHT_OFF)) return FALSE;

    vdf_g_DrSetPal16(DR_PAL_PCG, PB_PCG_RACKET, u2a_pal);  // 登録
    vdf_g_AppPBarAdd();                                    // プログレスバーを進める

    return TRUE;
}

//============================================================================================
// ゲームオブジェクト[ラケット]の初期化処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
VD vdf_g_ObjRacketInit(VD)
{
    s1_s_ObjRacketLv = 0;                                          // ラケットレベル初期化
    st_s_ObjRacket.u2_w = u2ac_s_ObjRacketWidth[s1_s_ObjRacketLv]; // ラケットレベル適用

    st_s_ObjRacket.s2_x = RACKET_DEF_X;                            // X座標初期化
    st_s_ObjRacket.s2_y = RACKET_DEF_Y;                            // Y座標初期化

}

//============================================================================================
// ゲームオブジェクト[ラケット]の実行処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
VD vdf_g_ObjRacketRun(VD)
{
    vdf_s_ObjRacketRunMove();      // ラケットの移動
    vdf_s_ObjRacketRunWallhit();   // ラケットの移動制限 (移動後に行うこと)
}

//============================================================================================
// ゲームオブジェクト[ラケット]の描画処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
VD vdf_g_ObjRacketDraw(VD)
{
    S4 s4_i;

#ifdef _RACKET_DEBUG_
    // デバッグモードでラケットレベルを下げれる状態の場合は毎回全てのスプライトを消す必要あり。
    // 左右端末ほか全ての部品の表示を一度消さないとゴミが残る
    // 他の場所でこの処理を行うとチラつきが起きるので注意
    for(s4_i = 0; s4_i < RACKET_SP_NUM; s4_i ++)
    {
        vdf_g_SpHide(SPPL_RACKET_TOP + s4_i);
    }
#endif

    // 左から順に右端末以外を描画
    for(s4_i = 0; s4_i <= s1_s_ObjRacketLv; s4_i ++ )
    {
        vdf_g_SpRegst(SPPL_RACKET_TOP + s4_i,                    // プレーン番号
                      st_s_ObjRacket.s2_x + (s4_i * SP_WIDTH16), // X座標
                      st_s_ObjRacket.s2_y,                       // Y座標
                      SP_V_INV_OFF,                              // V-垂直反転
                      SP_H_INV_OFF,                              // H-水平反転
                      PB_PCG_RACKET,                             // パレットブロック番号
                      u2ac_s_ObjRacketPcg[s4_i],                 // PCG番号
                      SP_PRW_SP_BG0_BG1);                        // 描画優先度
    }

    // 右端末を描画
    vdf_g_SpRegst(SPPL_RACKET_TOP + s4_i,                        // プレーン番号
                  st_s_ObjRacket.s2_x + (s4_i * SP_WIDTH16),     // X座標
                  st_s_ObjRacket.s2_y,                           // Y座標
                  SP_V_INV_OFF,                                  // V-垂直反転
                  SP_H_INV_ON,                                   // H-水平反転
                  PB_PCG_RACKET,                                 // パレットブロック番号
                  PCG_RACKET_TOP,                                // PCG番号
                  SP_PRW_SP_BG0_BG1);                            // 描画優先度
}

//============================================================================================
// ゲームオブジェクト[ラケット]の解放処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
VD vdf_g_ObjRacketRelease(VD)
{
    S4 s4_i;

    for(s4_i = 0; s4_i < RACKET_SP_NUM; s4_i ++) // ラケットのスプライト表示を全て消す
    {
        vdf_g_SpHide(SPPL_RACKET_TOP + s4_i);
    }
}

//============================================================================================
// ラケットレベルアップ
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
VD vdf_g_ObjRacketLvUp(VD)
{
    stOBJECT* const stcp_racket = &(st_s_ObjRacket);             // ラケットのポインタ取得

    S2 s2_cx = stcp_racket->s2_x + (stcp_racket->u2_w / 2);      // X中心座標を取得

    s1_s_ObjRacketLv ++;                                         // レベルアップ

    // レベル値の範囲外になった場合
    if(s1_s_ObjRacketLv < 0 || s1_s_ObjRacketLv > RACKET_LV_MAX)
    {
        s1_s_ObjRacketLv = RACKET_LV_MAX;                        // レベル最大とする
    }

    m_pcmplay(SOUND_ITEM_RACKET, PCM_PAN_C, PCM_FR15_6KHZ);      // 効果音(アイテム：ラケット)

    stcp_racket->u2_w = u2ac_s_ObjRacketWidth[s1_s_ObjRacketLv]; // 幅をオブジェクトに適用
    stcp_racket->s2_x = s2_cx - (stcp_racket->u2_w / 2);         // X座標の修正
}

//============================================================================================
// ラケットレベルダウン
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
VD vdf_g_ObjRacketLvDown(VD)
{
    stOBJECT* const stcp_racket = &(st_s_ObjRacket);             // ラケットのポインタ取得

    S2 s2_cx = stcp_racket->s2_x + (stcp_racket->u2_w / 2);      // X中心座標を取得

    s1_s_ObjRacketLv --;                                         // レベルダウン

    // レベル値の範囲外になった場合
    if(s1_s_ObjRacketLv < 0 || s1_s_ObjRacketLv > RACKET_LV_MAX)
    {
        s1_s_ObjRacketLv = 0;                                    // レベル最低とする
    }

    m_pcmplay(SOUND_CHOICE, PCM_PAN_C, PCM_FR15_6KHZ);           // 効果音(カーソル：選択)
    stcp_racket->u2_w = u2ac_s_ObjRacketWidth[s1_s_ObjRacketLv]; // 幅をオブジェクトに適用
    stcp_racket->s2_x = s2_cx - (stcp_racket->u2_w / 2);         // X座標の修正
}

//============================================================================================
// ゲームオブジェクト[ラケット]のポインタを返す
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : const stOBJECT* const : (O) ラケットオブジェクトのポインタ
//--------------------------------------------------------------------------------------------
// Note      :
// 想定する呼び出し元の構文例 (ポインタの値と参照先の値の両方がconst)
// const stOBJECT* const stcpc_racket = stcpcf_g_ObjRacketGetObj();
//
// 以下のような構文の場合は警告を発してくれる。
// stOBJECT* stp_racket = stcpcf_g_ObjRacketGetObj();         // const修飾子なし
// stOBJECT* const stcp_racket = stcpcf_g_ObjRacketGetObj();  // ポインタの値のみconst
//
// しかし、以下の構文の場合は警告を発しないので注意すること(costは修飾子であり型ではない)。
// const stOBJECT* stpc_racket = stcpcf_g_ObjRacketGetObj();   // 参照先の値のみconst
//============================================================================================
const stOBJECT* const stcpcf_g_ObjRacketGetObj(VD)
{
    return &(st_s_ObjRacket);
}

//============================================================================================
// ラケットの移動
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
static VD vdf_s_ObjRacketRunMove(VD)
{
    stOBJECT* const stcp_racket = &(st_s_ObjRacket);           // ラケットのポインタ取得

    if(u2f_g_InputGetState(CONTROLLER_1, BTN_LEFT,  BS_PRESS)) // 左ボタンが押された場合
    {
        stcp_racket->s2_x -= stcp_racket->s2_mx;               // 左に移動
        vdf_g_ObjCountStart();                                 // カウント開始
    }

    if(u2f_g_InputGetState(CONTROLLER_1, BTN_RIGHT, BS_PRESS)) // 右ボタンが押された場合
    {
        stcp_racket->s2_x += stcp_racket->s2_mx;               // 右に移動
        vdf_g_ObjCountStart();                                 // カウント開始
    }

#ifdef _RACKET_DEBUG_
    // デバッグ：ラケットレベルの可変
    if(u2f_g_InputGetState(CONTROLLER_1, BTN_UP, BS_DOWN))     // 上ボタンが押された場合
    {
        vdf_g_ObjRacketLvUp();                                 // ラケットレベルアップ
    }
    if(u2f_g_InputGetState(CONTROLLER_1, BTN_DOWN, BS_DOWN))   // 下ボタンが押された場合
    {
        vdf_g_ObjRacketLvDown();                               // ラケットレベルダウン
    }
#endif
}

//============================================================================================
// ラケットの移動制限(移動後に行うこと)
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
static VD vdf_s_ObjRacketRunWallhit(VD)
{
    stOBJECT* const stcp_racket = &(st_s_ObjRacket);                // ラケットのポインタ取得

    if(stcp_racket->s2_y < 0)                                       // 画面上側
    {
        stcp_racket->s2_y = 0;                                      // 座標修正
    }
    else if((stcp_racket->s2_y + stcp_racket->u2_h) > FIELD_HEIGHT) // 画面下側
    {
        stcp_racket->s2_y = (FIELD_HEIGHT - stcp_racket->u2_h);     // 座標修正
    }

    if(stcp_racket->s2_x < 0)                                       // 画面左側
    {
        stcp_racket->s2_x = 0;                                      // 座標修正
    }
    else if((stcp_racket->s2_x + stcp_racket->u2_w) > FIELD_WIDTH)  // 画面右側
    {
        stcp_racket->s2_x = (FIELD_WIDTH - stcp_racket->u2_w);      // 座標修正
    }
}
