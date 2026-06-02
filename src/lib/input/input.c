//============================================================================================
// Application : BLOCK BREAK PRO-68KZ
// Platform    : SHARP X68000 / Human68k
// Build       : GCC真里子版(ver1.42) + XCライブラリ + Z-MUSIC(v2.00)
//--------------------------------------------------------------------------------------------
// Filename    : input.c
// Version     : 2.0.0
// Author      : SEN::DAC
// Description : ゲーム用入力処理：キーボード(一部のキー)とジョイカードを扱う
//               押した瞬間 / 押し続け / 離した瞬間 / 離し続け のボタン状態を取得可能
//               過去DirectX7用として作成した物をベースにX68000対応(ジョイカードはZ純正を想定)
//============================================================================================
//--------------------------------------------------------------------------------------------
// インクルードファイル
//--------------------------------------------------------------------------------------------
#include <stdio.h>
#include "../type.h"
#include "../iomap.h"
#include "./keycode.h"
#include "input.h"

//--------------------------------------------------------------------------------------------
// 定数定義 (内部用)
//--------------------------------------------------------------------------------------------
// ジョイスティックレジスタのビット(参考：Inside X68000 P.380) 0:ON / 1:OFF
#define JOYSTICK_BIT_UP     0x01 // 十字ボタン上
#define JOYSTICK_BIT_DOWN   0x02 // 十字ボタン下
#define JOYSTICK_BIT_LEFT   0x04 // 十字ボタン左
#define JOYSTICK_BIT_RIGHT  0x08 // 十字ボタン右
#define JOYSTICK_BIT_A      0x20 // Aボタン(ジョイカードによりA/B表記が異なる → Z純正基準)
#define JOYSTICK_BIT_B      0x40 // Bボタン(ジョイカードによりA/B表記が異なる → Z純正基準)
#define JOYSTICK_BIT_SELECT 0x03 // セレクト(上下同時押し)
#define JOYSTICK_BIT_START  0x0C // スタート(左右同時押し)

#define CONTROLLERMAX 2          // コントローラの数(1～2：1P, 2P)

//--------------------------------------------------------------------------------------------
// 型定義 (内部用)
//--------------------------------------------------------------------------------------------
typedef struct     //コントローラー情報
{
    U2 u2_data;    // 今回のボタン状態
    U2 u2_prev;    // 前回のボタン状態
    U2 u2_change;  // 前回→今回で変更があるボタン

    U2 u2_down;    // 押した瞬間
    U2 u2_press;   // 押し続けている
    U2 u2_up;      // 離した瞬間
    U2 u2_release; // 離している
}
stCON, *stpCON;

//--------------------------------------------------------------------------------------------
// プロトタイプ宣言 (static)
//--------------------------------------------------------------------------------------------
static VD vdf_s_InputMakeJoyData(VD);       // ジョイスティック入力情報作成
static VD vdf_s_InputMakeKeyData(VD);       // キーボード入力情報作成

//--------------------------------------------------------------------------------------------
// グローバル変数定義 (外部公開)
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// 静的変数定義 (static)
//--------------------------------------------------------------------------------------------
static volatile U2* u2pa_s_InputJoyStick[CONTROLLERMAX] =
{
    &U2_JOYSTK_1P,                          // ジョイスティック 1P
    &U2_JOYSTK_2P                           // ジョイスティック 2P
};

static stCON sta_s_InputCtr[CONTROLLERMAX]; // コントローラー情報

//--------------------------------------------------------------------------------------------
// 関数定義
//--------------------------------------------------------------------------------------------
//============================================================================================
// 入力情報作成 (ジョイスティック1Pとキーボードの情報を統合する)
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//--------------------------------------------------------------------------------------------
// Note      :
// メインループ内で毎回呼び出すこと。
//============================================================================================
VD vdf_g_InputMakeData(VD)
{
    S4 s4_i;

    for(s4_i = 0; s4_i < CONTROLLERMAX; s4_i ++)
    {
        sta_s_InputCtr[s4_i].u2_data = 0;        // 入力情報クリア
    }

    vdf_s_InputMakeJoyData();                    // ジョイスティック入力情報作成
    vdf_s_InputMakeKeyData();                    // キーボード入力情報作成 (1Pのみ影響)

    for(s4_i = 0; s4_i < CONTROLLERMAX; s4_i ++) // 入力情報の加工
    {
        sta_s_InputCtr[s4_i].u2_change =         // 変更があった情報を求める
        sta_s_InputCtr[s4_i].u2_data ^ sta_s_InputCtr[s4_i].u2_prev;

        sta_s_InputCtr[s4_i].u2_down =           // 押した瞬間の情報を求める
        sta_s_InputCtr[s4_i].u2_data & sta_s_InputCtr[s4_i].u2_change;

        sta_s_InputCtr[s4_i].u2_press =          // 押し続けている情報を求める
        sta_s_InputCtr[s4_i].u2_data & sta_s_InputCtr[s4_i].u2_prev;

        sta_s_InputCtr[s4_i].u2_up =             // 離した瞬間の情報を求める
        sta_s_InputCtr[s4_i].u2_change & sta_s_InputCtr[s4_i].u2_prev;

        sta_s_InputCtr[s4_i].u2_release =        // 離している情報を求める
        ~(sta_s_InputCtr[s4_i].u2_down  |
        sta_s_InputCtr[s4_i].u2_press |
        sta_s_InputCtr[s4_i].u2_up);

        sta_s_InputCtr[s4_i].u2_prev =           // 情報を取っておく
        sta_s_InputCtr[s4_i].u2_data;
    }
}

//============================================================================================
// 入力情報取得
//--------------------------------------------------------------------------------------------
// Arguments : U2 u2_id    : (I) コントローラID
//                               CONTROLLER_1    0 // コントローラ 1P
//                               CONTROLLER_2    1 // コントローラ 2P
//             U2 u2_btn   : (I) 対象ボタン
//                               BTN_UP     0x0001 // bit00：上ボタン
//                               BTN_DOWN   0x0002 // bit01：下ボタン
//                               BTN_LEFT   0x0004 // bit02：左ボタン
//                               BTN_RIGHT  0x0008 // bit03：右ボタン
//                               BTN_A      0x0010 // bit04：Aボタン
//                               BTN_B      0x0020 // bit05：Bボタン
//                               BTN_SELECT 0x0040 // bit06：SELECTボタン
//                               BTN_START  0x0080 // bit07：START ボタン
//                               etc...
//             U2 u2_state : (I) ボタンの状態
//                               BS_RELEASE      1 // 離している
//                               BS_DOWN         2 // 押した瞬間
//                               BS_PRESS        3 // 押し続けている
//                               BS_UP           4 // 離した瞬間
// Return    : U2 u2_ret   : (O) 0以外 : 引数で与えた条件を満たしている
//                               0     : 引数で与えた条件を満たしていない
//--------------------------------------------------------------------------------------------
// Note      :
// 引数は必ず専用マクロを使用すること。
//============================================================================================
U2 u2f_g_InputGetState(U2 u2_id, U2 u2_btn, U2 u2_state )
{
    U2 u2_ret = 0;

    u2_id &= 0x0001; // コントーラIDは 0 / 1 しか受け取らない

    switch(u2_state) // 対象ステータス
    {
    case BS_RELEASE: u2_ret = sta_s_InputCtr[u2_id].u2_release & u2_btn; break; // 離している
    case BS_DOWN:    u2_ret = sta_s_InputCtr[u2_id].u2_down    & u2_btn; break; // 押した瞬間
    case BS_PRESS:   u2_ret = sta_s_InputCtr[u2_id].u2_press   & u2_btn; break; // 押し続け
    case BS_UP:      u2_ret = sta_s_InputCtr[u2_id].u2_up      & u2_btn; break; // 離した瞬間
    }

    return u2_ret;
}

//============================================================================================
// ジョイスティック入力情報作成
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
static VD vdf_s_InputMakeJoyData(VD)
{
    S4 s4_i;
    U2 u2_joy;

    for(s4_i = 0; s4_i < CONTROLLERMAX; s4_i ++)
    {
        u2_joy = *u2pa_s_InputJoyStick[s4_i];              // ジョイスティックのアドレスを取得

        if(!(u2_joy & JOYSTICK_BIT_SELECT))
        {
            sta_s_InputCtr[s4_i].u2_data |= BTN_SELECT;    // セレクト(上下同時押し)
        }
        else
        {
            if(!(u2_joy & JOYSTICK_BIT_UP))
            {
                sta_s_InputCtr[s4_i].u2_data |= BTN_UP;    // 上
            }
            else if(!(u2_joy & JOYSTICK_BIT_DOWN))
            {
                sta_s_InputCtr[s4_i].u2_data |= BTN_DOWN;  // 下
            }
        }

        if(!(u2_joy & JOYSTICK_BIT_START))
        {
            sta_s_InputCtr[s4_i].u2_data |= BTN_START;     // スタート(左右同時押し)
        }
        else
        {
            if(!(u2_joy & JOYSTICK_BIT_LEFT))
            {
                sta_s_InputCtr[s4_i].u2_data |= BTN_LEFT;  // 左
            }
            else if(!(u2_joy & JOYSTICK_BIT_RIGHT))
            {
                sta_s_InputCtr[s4_i].u2_data |= BTN_RIGHT; // 右
            }
        }

        if(!(u2_joy & JOYSTICK_BIT_A))
        {
            sta_s_InputCtr[s4_i].u2_data |= BTN_A;         // A
        }

        if(!(u2_joy & JOYSTICK_BIT_B))
        {
            sta_s_InputCtr[s4_i].u2_data |= BTN_B;         // B
        }
    }
}

//============================================================================================
// キーボード入力情報作成 (1Pのみ影響)
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
static VD vdf_s_InputMakeKeyData(VD)
{
    // カーソル上キーが押された場合、上ボタンのビットを上げる
    if( BITSNS(KG_7) & KG_7_KC_UP_ ) sta_s_InputCtr[0].u2_data |= BTN_UP;

    // カーソル下キーが押された場合、下ボタンのビットを上げる
    if( BITSNS(KG_7) & KG_7_KC_DOW ) sta_s_InputCtr[0].u2_data |= BTN_DOWN;

    // カーソル左キーが押された場合、左ボタンのビットを上げる
    if( BITSNS(KG_7) & KG_7_KC_LEF ) sta_s_InputCtr[0].u2_data |= BTN_LEFT;

    // カーソル右キーが押された場合、右ボタンのビットを上げる
    if( BITSNS(KG_7) & KG_7_KC_RIG ) sta_s_InputCtr[0].u2_data |= BTN_RIGHT;

    // Xキーが押された場合、ボタンAのビットを上げる
    if( BITSNS(KG_5) & KG_5_KC_X__ ) sta_s_InputCtr[0].u2_data |= BTN_A;

    // Zキーが押された場合、ボタンBのビットを上げる
    if( BITSNS(KG_5) & KG_5_KC_Z__ ) sta_s_InputCtr[0].u2_data |= BTN_B;

    // スペースキーが押された場合、スタートボタンのビットを上げる
    if( BITSNS(KG_6) & KG_6_KC_SP_ ) sta_s_InputCtr[0].u2_data |= BTN_START;

    //リターンキーが押された場合、セレクトボタンのビットを上げる
    if( BITSNS(KG_3) & KG_3_KC_ENT ) sta_s_InputCtr[0].u2_data |= BTN_SELECT;
}
