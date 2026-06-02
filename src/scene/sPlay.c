//============================================================================================
// Application : BLOCK BREAK PRO-68KZ
// Platform    : SHARP X68000 / Human68k
// Build       : GCC真里子版(ver1.42) + XCライブラリ + Z-MUSIC(v2.00)
//--------------------------------------------------------------------------------------------
// Filename    : sPlay.c
// Version     : 2.0.0
// Author      : SEN::DAC
// Description : シーン[プレイ]
//============================================================================================
//--------------------------------------------------------------------------------------------
// インクルードファイル
//--------------------------------------------------------------------------------------------
#include "sPlay.h"

//--------------------------------------------------------------------------------------------
// 定数定義 (内部用)
//--------------------------------------------------------------------------------------------
#define STAGE_POS_X     370           // ステージ表示 X座標
#define STAGE_POS_Y     5             // ステージ表示 Y座標

#define SCORE_POS_X     328           // スコア表示   X座標
#define SCORE_POS_Y     35            // スコア表示   Y座標

#define COUNT_POS_X     328           // カウント表示 X座標
#define COUNT_POS_Y     65            // カウント表示 Y座標

#define WAIT_TIME_MISS  3000          // ゲームオーバー時のウェイト時間(ms単位：3秒)
#define WAIT_TIME_CLEAR 4000          // ステージクリア時のウェイト時間(ms単位：4秒)

// CONTRAST()のパラメータ
// 参考資料：「C-Compiler PRO-68K Cライブラリマニュアル」P.272 (版・刷不明)
// 注：CONTRAST()は、CRTMOD()より後に呼ぶ事
#define CONTRAST_NOW    -1            // 現在の値を返す
#define CONTRAST_DEF    -2            // 規定値に戻す
#define CONTRAST_PUS     7            // ポーズ中の暗さ(暗0～15明)

//--------------------------------------------------------------------------------------------
// 型定義 (内部用)
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// プロトタイプ宣言 (static)
//--------------------------------------------------------------------------------------------
static VD vdf_s_sPlayRun(VD);         // シーンの実行
static VD vdf_s_sPlayRelease(VD);     // シーンの解放
static BL blf_s_sPlayRunPause(VD);    // ポーズの処理

//--------------------------------------------------------------------------------------------
// グローバル変数定義 (外部公開)
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// 静的変数定義 (static)
//--------------------------------------------------------------------------------------------
static U2 u2a_s_sPlayGrPal[DR_COL16]; // 背景(GR)のパレット(16色)

//--------------------------------------------------------------------------------------------
// 関数定義
//--------------------------------------------------------------------------------------------
//============================================================================================
// シーン[プレイ]：シーン内で使用する画像を全てロード
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : BL : (O) TRUE  : 成功
//                      FALSE : 失敗
//============================================================================================
BL blf_g_sPlayLoadImage(VD)
{
    U1* const u1cp_bbuf = u1cpf_g_DrGetBmpBuf();            // BMPロード用バッファ取得
    U2* const u2cp_cbuf = u2cpf_g_DrGetCnvBuf();            // 画像変換用バッファ取得

    // 背景画像(GR)-16色BMP画像ロードと書込み
    if(!blf_g_DrLoadBmp16(SZ_IMGFN_BK_PLAY, u1cp_bbuf)) return FALSE;

    vdf_g_GrBmpToGr(u2cp_cbuf, u1cp_bbuf, CRT_W, CRT_H);    // BMP → GR 変換
    vdf_g_GrDraw(u2cp_cbuf, GP1, CRT_W, CRT_H);             // グラフィック画面に書き込み
    vdf_g_AppPBarAdd();                                     // プログレスバーを進める

    // 背景画像(GR)のパレットのロード(登録はシーン初期化で行う)
    if(!blf_g_DrLoadPal16(SZ_IMGFN_BK_PLAY, u2a_s_sPlayGrPal, BRIGHT_OFF)) return FALSE;
    vdf_g_AppPBarAdd();                                     // プログレスバーを進める

    // SP/BG：16色BMP画像とパレットロード～PCG書込み
    if(!blf_g_ObjBallLoadImage())    return FALSE; // ボール      (SP)画像とパレットをロード
    if(!blf_g_ObjShotLoadImage())    return FALSE; // ショット    (SP)画像とパレットをロード
    if(!blf_g_ObjRacketLoadImage())  return FALSE; // ラケット    (SP)画像とパレットをロード
    if(!blf_g_ObjBlockLoadImage())   return FALSE; // ブロック    (BG)画像とパレットをロード
    if(!blf_g_ObjItemLoadImage())    return FALSE; // アイテム    (SP)画像とパレットをロード
    if(!blf_g_ObjNumfontLoadImage()) return FALSE; // 数字フォント(SP)画像とパレットをロード

    return TRUE;
}

//============================================================================================
// シーン[プレイ]：初期化処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
VD vdf_g_sPlayInit(VD)
{
    vdf_g_GrPagePri(GP1, GP0, GP2, GP3);                    // GRn画面プライオリティ設定
    vdf_g_DrSetPal16(DR_PAL_GRA, PB_GRA, u2a_s_sPlayGrPal); // パレット登録

    vdf_g_ObjBallInit();                                    // ボールの初期化
    vdf_g_ObjShotInit();                                    // ショットの初期化
    vdf_g_ObjRacketInit();                                  // ラケットの初期化
    vdf_g_ObjItemInit();                                    // アイテムの初期化
    vdf_g_ObjBlockInit(u1f_g_ObjStageGet());                // ブロックの初期化
    vdf_g_ObjCountInit();                                   // カウントの初期化

    // ステージ数の描画 (動かないので初期化で一度だけ描画すれば良い)
    vdf_g_ObjStageDraw(STAGE_POS_X, STAGE_POS_Y);
    vdf_g_AppSetProc(vdf_s_sPlayRun);                       // 実行に遷移
}

//============================================================================================
// シーン[プレイ]：実行処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
static VD vdf_s_sPlayRun(VD)
{
    if(!blf_s_sPlayRunPause())                        // ポーズ中でないなら
    {
        vdf_g_ObjBallRun();                           // ボールの実行
        vdf_g_ObjShotRun();                           // ショットの実行
        vdf_g_ObjItemRun();                           // アイテムの実行
        vdf_g_ObjRacketRun();                         // ラケットの実行
        vdf_g_ObjCountRun();                          // カウントの実行

        // ゲームオーバー判定 (ボールを全て失ったか？)
        if(blf_g_ObjBallaIsAllNone())
        {
            // 効果音(ミス)
            m_pcmplay(SOUND_MISS, PCM_PAN_C, PCM_FR15_6KHZ);
            vdf_g_TimerWait(WAIT_TIME_MISS);          // 指定した時間処理を止める
            vdf_g_AppSetNext(vdf_g_sResultInit);      // 次のシーンを設定
            vdf_g_AppSetProc(vdf_s_sPlayRelease);     // 解放に遷移
        }

        // クリア判定 (ブロックを全て崩したか？)
        if(blf_g_ObjBlockIsAllNone())
        {
            // 効果音(ステージクリア)
            m_pcmplay(SOUND_STAGE_CLR, PCM_PAN_C, PCM_FR15_6KHZ);
            vdf_g_ObjScoreAdd(s4f_g_ObjCountGet());   // 残り時間をスコアに加算
            vdf_g_TimerWait(WAIT_TIME_CLEAR);         // 指定した時間処理を止める

            if(blf_g_ObjStageAdd())                   // 次のステージへ～上限を超えた場合
            {
                vdf_g_AppSetNext(vdf_g_sResultInit);  // 全クリア → シーン[リザルト]
            }
            else
            {
                vdf_g_AppSetNext(vdf_g_sPlayInit);    // 次のステージへ → シーン[プレイ]
            }
            vdf_g_AppSetProc(vdf_s_sPlayRelease);     // 解放に遷移
        }

        vdf_g_CrtVblank();                            // 描画タイミング待ち

        vdf_g_ObjBallDraw();                          // ボールの描画
        vdf_g_ObjShotDraw();                          // ショットの描画
        vdf_g_ObjItemDraw();                          // アイテムの描画
        vdf_g_ObjRacketDraw();                        // ラケットの描画
        vdf_g_ObjScoreDraw(SCORE_POS_X, SCORE_POS_Y); // スコアの描画
        vdf_g_ObjCountDraw(COUNT_POS_X, COUNT_POS_Y); // カウントの描画
    }
}

//============================================================================================
// シーン[タイトル]：解放処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
static VD vdf_s_sPlayRelease(VD)
{
    U2 u2a_pal[DR_COL16] = {0};                             // 16色全て黒のパレット

    vdf_g_DrSetPal16(DR_PAL_GRA, PB_GRA, u2a_pal);          // 全黒パレットでGR画面を非表示

    vdf_g_ObjBallRelease();                                 // ボールの解放
    vdf_g_ObjShotRelease();                                 // ショットの解放
    vdf_g_ObjRacketRelease();                               // ラケットの解放
    vdf_g_ObjItemRelease();                                 // アイテムの解放
    vdf_g_ObjBlockRelease();                                // ブロックの解放
    vdf_g_ObjScoreRelease();                                // スコアの解放
    vdf_g_ObjCountRelease();                                // カウントの解放
    vdf_g_ObjStageRelease();                                // ステージの解放

    vdf_g_AppMoveProc();                                    // 次のシーンに遷移
}

//============================================================================================
// ポーズ(ゲーム一時停止)の処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : BL : (O) TRUE  : ポーズ中
//                      FALSE : ポーズ中ではない
//============================================================================================
static BL blf_s_sPlayRunPause(VD)
{
    static BL bl_pause = FALSE; // FALSE：ポーズ-OFF /  TRUE：ポーズ-ON

    // 既にポーズ中の場合
    if(bl_pause)
    {
        // STARTボタンが押されら
        if(u2f_g_InputGetState(CONTROLLER_1, BTN_START, BS_DOWN))
        {
            m_pcmplay(SOUND_ENTER, PCM_PAN_C, PCM_FR15_6KHZ); // 効果音(カーソル：決定)
            bl_pause = FALSE;                                 // ポーズ OFF
            CONTRAST(CONTRAST_DEF);                           // コントラストを規定値に戻す
        }
    }

    // STARTボタンが押されたら
    else if(u2f_g_InputGetState(CONTROLLER_1, BTN_START, BS_DOWN))
    {
        m_pcmplay(SOUND_ENTER, PCM_PAN_C, PCM_FR15_6KHZ);     // 効果音(カーソル：決定)
        bl_pause = TRUE;                                      // ポーズ ON
        CONTRAST(CONTRAST_PUS);                               // コントラスト設定～暗くする
    }

    return bl_pause;
}
