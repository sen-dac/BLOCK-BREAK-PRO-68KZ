//============================================================================================
// Application : BLOCK BREAK PRO-68KZ
// Platform    : SHARP X68000 / Human68k
// Build       : GCC真里子版(ver1.42) + XCライブラリ + Z-MUSIC(v2.00)
//--------------------------------------------------------------------------------------------
// Filename    : ball.c
// Version     : 2.0.0
// Author      : SEN::DAC
// Description : ゲームオブジェクト[ボール]の処理
//============================================================================================
//--------------------------------------------------------------------------------------------
// インクルードファイル
//--------------------------------------------------------------------------------------------
#include "ball.h"

//--------------------------------------------------------------------------------------------
// 定数定義 (内部用)
//--------------------------------------------------------------------------------------------
#define BALL_DEF_MX  -1   // 初期X移動量
#define BALL_DEF_MY  -1   // 初期Y移動量
#define BALL_WIDTH   8    // 横幅
#define BALL_HEIGHT  8    // 縦幅
#define BALL_MAX     10   // 所持最大数
#define BALL_DEF_STC 3    // ストック初期値

#define BALL_ST_NON   0   // ボールのステータス：未使用(なし)
#define BALL_ST_STC   1   // ボールのステータス：ストック
#define BALL_ST_ACT   2   // ボールのステータス：アクティブ

#define STOCK_BALL_X  350 // ストックボール：X座標
#define STOCK_BALL_Y  178 // ストックボール：Y座標 (一番下)
#define STOCK_BALL_CL 10  // ストックボール：縦の表示間隔

#define BALL_TH_TIME  60  // ボールの貫通時間(フレーム数で指定)

//--------------------------------------------------------------------------------------------
// 型定義 (内部用)
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// プロトタイプ宣言 (static)
//--------------------------------------------------------------------------------------------
static VD vdf_s_ObjBallRunTh(VD);                               // 貫通状態の処理
static VD vdf_s_ObjBallRunFir(VD);                              // ボールの発射
static BL blf_s_ObjBallRunMove(S4 s4_id);                       // ボールの移動
static BL blf_s_ObjBallRunWallhit(S4 s4_id);                    // 壁と当たり判定
static BL blf_s_ObjBallRunRackethit(S4 s4_id);                  // ラケットと当たり判定
static BL blf_s_ObjBallHitObj(S4 s4_id,                         // オブジェクトと当たり判定
                              const stOBJECT* const stcpc_obj);
static BL blf_s_ObjBallRunBlockhit(S4 s4_id);                   // ブロックと当たり判定
static BL blf_s_ObjBallRunBlockhitChk(U2 u2_x,                  // ブロックデータと照合
                                      U2 u2_y,
                                      S2* const s2cp_dir);

//--------------------------------------------------------------------------------------------
// グローバル変数定義 (外部公開)
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// 静的変数定義 (static)
//--------------------------------------------------------------------------------------------
// ボールパレット保存用(貫通時の点滅のためにRAM保持)
static U2 u2a_s_ObjBallPal[DR_COL16] = {0};

// ボール貫通時間カウント
static S2 s2_s_ObjBallThCount;

// オブジェクト
static stOBJECT sta_s_ObjBall[BALL_MAX];

// ボールのステータス
// オブジェクト毎の用途の違いから演算相手の符号の有無が異なる。
// このため、堅牢性を優先しオブジェクト構造体のメンバに含めず別で持つ方針とした。
static U1 u1a_s_ObjBallStatus[BALL_MAX];

//--------------------------------------------------------------------------------------------
// 関数定義
//--------------------------------------------------------------------------------------------
//============================================================================================
// ゲームオブジェクト[ボール]の画像とパレットをロード
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : BL : (O) TRUE  : 成功
//                      FALSE : 失敗
//============================================================================================
BL blf_g_ObjBallLoadImage(VD)
{
    U4  u4a_pcg[PCG_TILE_SIZE_32U4];                             // BMP → PCG変換用
    U1* const u1cp_bbuf = u1cpf_g_DrGetBmpBuf();                 // BMPロード用バッファ取得

    // 16色BMP画像ロードと書込み
    if(!blf_g_DrLoadBmp16(SZ_IMGFN_BALL, u1cp_bbuf)) return FALSE;

    vdf_g_SpBmpToPcg(u4a_pcg, (U4*)u1cp_bbuf);                   // BMP → PCG 変換
    SP_DEFCG(PCG_BALL_TOP, SP_SIZE16, (U1*)u4a_pcg);             // PCGに画像転送
    vdf_g_AppPBarAdd();                                          // プログレスバーを進める

    // ボール(SP)の16色パレットをロード(GR以外は登録をここで行っても良い)
    // ※ボールは他のSPと異なり貫通時の点滅のためにパレットをRAMに保持する必要がある
    if(!blf_g_DrLoadPal16(SZ_IMGFN_BALL, u2a_s_ObjBallPal, BRIGHT_OFF)) return FALSE;

    vdf_g_DrSetPal16(DR_PAL_PCG, PB_PCG_BALL, u2a_s_ObjBallPal); // 登録
    vdf_g_AppPBarAdd();                                          // プログレスバーを進める

    return TRUE;
}

//============================================================================================
// ゲームオブジェクト[ボール]の初期化処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
VD vdf_g_ObjBallInit(VD)
{
    S4 s4_i;

    for(s4_i = 0; s4_i < BALL_MAX; s4_i ++)              // 全てのボール
    {
        sta_s_ObjBall[s4_i].s2_x  = 0;                   // X座標
        sta_s_ObjBall[s4_i].s2_y  = 0;                   // Y座標
        sta_s_ObjBall[s4_i].s2_mx = BALL_DEF_MX;         // X移動量
        sta_s_ObjBall[s4_i].s2_my = BALL_DEF_MY;         // Y移動量
        sta_s_ObjBall[s4_i].u2_w  = BALL_WIDTH;          // 横幅
        sta_s_ObjBall[s4_i].u2_h  = BALL_HEIGHT;         // 縦幅
        u1a_s_ObjBallStatus[s4_i] = BALL_ST_NON;         // ステータス[未使用]
    }

    for(s4_i = 0; s4_i < BALL_DEF_STC; s4_i ++)          // ストック数初期値の回数だけ処理する
    {
        u1a_s_ObjBallStatus[s4_i] = BALL_ST_STC;         // テータス[ストック]
    }

    s2_s_ObjBallThCount = 0;                             // 貫通カウントをクリア
}

//============================================================================================
// ゲームオブジェクト[ボール]の実行処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
VD vdf_g_ObjBallRun(VD)
{
    S4 s4_i;

    vdf_s_ObjBallRunTh();                                 // 貫通状態の処理
    vdf_s_ObjBallRunFir();                                // 発射の処理

    for(s4_i = 0; s4_i < BALL_MAX; s4_i ++)               // 全てのボールを巡回
    {
        if(u1a_s_ObjBallStatus[s4_i] == BALL_ST_ACT)      // ステータス[アクティブ]なら
        {
            // 処理負荷軽減：以降の処理が必要な"可能性が低い"場合「continue」する。
            // 各関数に渡す引数(s4_i)がsta_s_ObjBall[]に対して有効なインデックスであることは
            // 呼び出し元であるここで保証する方針とする。
            if(blf_s_ObjBallRunMove(s4_i))      continue; // ボールの移動           (TRUE：消滅)
            if(blf_s_ObjBallRunWallhit(s4_i))   continue; // 壁との当たり判定       (TRUE：Hit)
            if(blf_s_ObjBallRunRackethit(s4_i)) continue; // ラケットとの当たり判定 (TRUE：Hit)
            if(blf_s_ObjBallRunBlockhit(s4_i))  continue; // ブロックとの当たり判定 (TRUE：Hit)
        }
    }
}

//============================================================================================
// ゲームオブジェクト[ボール]の描画処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
VD vdf_g_ObjBallDraw(VD)
{
    S4 s4_i;
    U2 u2_stb_y = STOCK_BALL_Y;                      // ストックボールのy座標

    for(s4_i = 0; s4_i < BALL_MAX; s4_i ++)          // 全てのボール
    {
        if(u1a_s_ObjBallStatus[s4_i] == BALL_ST_STC) // ステータス[ストック]の場合
        {
            vdf_g_SpRegst(SPPL_BALL_TOP + s4_i,      // プレーン番号
                          STOCK_BALL_X,              // X座標
                          u2_stb_y,                  // Y座標
                          SP_V_INV_OFF,              // V-垂直反転
                          SP_H_INV_OFF,              // H-水平反転
                          PB_PCG_BALL,               // パレットブロック番号
                          PCG_BALL_TOP,              // PCG番号
                          SP_PRW_SP_BG0_BG1);        // 描画優先度

            u2_stb_y -= STOCK_BALL_CL;
        }

        if(u1a_s_ObjBallStatus[s4_i] == BALL_ST_ACT) // ステータス[アクティブ]の場合
        {
            vdf_g_SpRegst(SPPL_BALL_TOP + s4_i,      // プレーン番号
                          sta_s_ObjBall[s4_i].s2_x,  // X座標
                          sta_s_ObjBall[s4_i].s2_y,  // Y座標
                          SP_V_INV_OFF,              // V-垂直反転
                          SP_H_INV_OFF,              // H-水平反転
                          PB_PCG_BALL,               // パレットブロック番号
                          PCG_BALL_TOP,              // PCG番号
                          SP_PRW_SP_BG0_BG1);        // 描画優先度
        }
    }
}

//============================================================================================
// ゲームオブジェクト[ボール]の解放処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
VD vdf_g_ObjBallRelease(VD)
{
    S4 s4_i;

    for(s4_i = 0; s4_i < BALL_MAX; s4_i ++) // ボールのスプライト表示を全て消す
    {
        vdf_g_SpHide(SPPL_BALL_TOP + s4_i);
    }

    s2_s_ObjBallThCount = 0;                                     // 貫通カウントをクリア
    vdf_g_DrSetPal16(DR_PAL_PCG, PB_PCG_BALL, u2a_s_ObjBallPal); // 標準パレットに戻す
}

//============================================================================================
// ボールを全て失ったか？ (ゲームオーバー判定)
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : BL bl_ret : (O) TRUE  : ボールを全て失った
//                             FALSE : ボールを持っている
//============================================================================================
BL blf_g_ObjBallaIsAllNone(VD)
{
    S4 s4_i;
    BL bl_ret = TRUE;                                // 戻り値格納用

    for(s4_i = 0; s4_i < BALL_MAX; s4_i ++)          // 全てのボールの中に
    {
        if(u1a_s_ObjBallStatus[s4_i] != BALL_ST_NON) // ステータスが[未使用]ではない物を発見
        {
            bl_ret = FALSE;                          // ゲームオーバーではない
            break;                                   // 1つでもあれば確定なのでループ終了
        }
    }

    return bl_ret;
}

//============================================================================================f
// ボールの追加
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
VD vdf_g_ObjBallAdd(VD)
{
    S4 s4_i;

    m_pcmplay(SOUND_ITEM_BALL, PCM_PAN_C, PCM_FR15_6KHZ); // 効果音(アイテム取得(ボール))

    for(s4_i = 0; s4_i < BALL_MAX; s4_i ++)               // 全てのボールの中から
    {
        if(u1a_s_ObjBallStatus[s4_i] == BALL_ST_NON)      // ステータス[未使用]を見つけたら
        {
            u1a_s_ObjBallStatus[s4_i] = BALL_ST_STC;      // ステータス[ストック]に変更
            break;                                        // 1つでもあれば確定なので抜ける
        }
    }
}

//============================================================================================
// ボールの貫通設定
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
VD vdf_g_ObjBallSetThrough(VD)
{
    m_pcmplay(SOUND_ITEM_THROUGH, PCM_PAN_C, PCM_FR15_6KHZ); // 効果音(アイテム取得(貫通))
    s2_s_ObjBallThCount = BALL_TH_TIME;                      // 貫通カウントを設定
}

//============================================================================================
// 貫通状態の処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
static VD vdf_s_ObjBallRunTh(VD)
{
    U2 u2a_whpalette[DR_COL16] = {                                   // 16色全白パレット(点滅)
        0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
        0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE, 0xFFFE,
    };

    s2_s_ObjBallThCount --;                                          // 貫通カウントは常時減る

    if(s2_s_ObjBallThCount < 0) s2_s_ObjBallThCount = 0;             // 0以下にならない

    // 貫通カウント中はボールを点滅させる
    if(s2_s_ObjBallThCount & 1)                                      // 貫通カウント奇数のとき
    {
        vdf_g_DrSetPal16(DR_PAL_PCG, PB_PCG_BALL, u2a_whpalette);    // 全白パレット登録
    }
    else                                                             // 貫通カウント偶数のとき
    {
        vdf_g_DrSetPal16(DR_PAL_PCG, PB_PCG_BALL, u2a_s_ObjBallPal); // 標準パレット登録
    }

}

//============================================================================================
// ボールの発射
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
static VD vdf_s_ObjBallRunFir(VD)
{
    S4 s4_i;

    // ラケットのポインタを取得
    const stOBJECT* const stcpc_racket = stcpcf_g_ObjRacketGetObj();

    U2 u2_racket_half_w = stcpc_racket->u2_w / 2;              // ラケットの幅の半分

    // Aボタンが押されたら
    if(u2f_g_InputGetState(CONTROLLER_1, BTN_A, BS_DOWN))
    {
        for(s4_i = 0; s4_i < BALL_MAX; s4_i ++)                // 全てのボールから
        {
            if(u1a_s_ObjBallStatus[s4_i] == BALL_ST_STC)       // ステータス[ストック]発見
            {
                sta_s_ObjBall[s4_i].s2_x =                     // X座標をラケット中央に設定
                stcpc_racket->s2_x + u2_racket_half_w - (sta_s_ObjBall[s4_i].u2_w / 2);

                sta_s_ObjBall[s4_i].s2_y =                     // Y座標をラケット上に設定
                stcpc_racket->s2_y - sta_s_ObjBall[s4_i].u2_h;

                sta_s_ObjBall[s4_i].s2_my = BALL_DEF_MY;       // Y移動量のデフォルト値

                // 右入力がある場合
                if(u2f_g_InputGetState(CONTROLLER_1, BTN_RIGHT, BS_DOWN)||
                   u2f_g_InputGetState(CONTROLLER_1, BTN_RIGHT, BS_PRESS))
                {
                    sta_s_ObjBall[s4_i].s2_mx  = -BALL_DEF_MX; // 右方向に発射
                }
                // 右入力が無い場合
                else
                {
                    sta_s_ObjBall[s4_i].s2_mx  = BALL_DEF_MX;  // 左方向に発射
                }

                u1a_s_ObjBallStatus[s4_i] = BALL_ST_ACT;       // ステータス[アクティブ]

                // 効果音(ボール発射)
                m_pcmplay(SOUND_B_FIRING, PCM_PAN_C, PCM_FR15_6KHZ);
                vdf_g_ObjCountStart();                         // カウント開始
                break;
            }
        }
    }
}

//============================================================================================
// ボールの移動
//--------------------------------------------------------------------------------------------
// Arguments : S4 s4_id  : (I) 対象ボールID
// Return    : BL bl_ret : (O) TRUE  : 対象IDボール消滅
//                             FALSE : 対象IDボール健在
//--------------------------------------------------------------------------------------------
// Note      :
// 引数(s4_id)については、ループカウンタである事を前提としてガード処理を行わない方針とする。
// for(s4_i = 0; s4_i < BALL_MAX; s4_i ++){...}
// sta_s_ObjBall[s4_id]が有効なインデックスである事は呼び出し元で保証すること。
//============================================================================================
static BL blf_s_ObjBallRunMove(S4 s4_id)
{
    BL bl_ret = FALSE;                                       // 戻り値格納用

    sta_s_ObjBall[s4_id].s2_x += sta_s_ObjBall[s4_id].s2_mx; // X方向移動
    sta_s_ObjBall[s4_id].s2_y += sta_s_ObjBall[s4_id].s2_my; // Y方向移動

    if(sta_s_ObjBall[s4_id].s2_y > FIELD_HEIGHT)             // 下に落ちたら消滅
    {
        u1a_s_ObjBallStatus[s4_id] = BALL_ST_NON;            // ステータス[未使用]
        vdf_g_SpHide(SPPL_BALL_TOP + s4_id);                 // SP表示を消す
        bl_ret = TRUE;
    }

    return bl_ret;
}

//============================================================================================
// 壁との当たり判定と当たった場合の処理
//--------------------------------------------------------------------------------------------
// Arguments : S4 s4_id  : (I) 対象ボールID
// Return    : BL bl_ret : (O) TRUE  : 壁に当たった
//                             FALSE : 壁に当たっていない
//--------------------------------------------------------------------------------------------
// Note      :
// 引数(s4_id)については、ループカウンタである事を前提としてガード処理を行わない方針とする。
// for(s4_i = 0; s4_i < BALL_MAX; s4_i ++){...}
// sta_s_ObjBall[s4_id]が有効なインデックスである事は呼び出し元で保証すること。
//============================================================================================
static BL blf_s_ObjBallRunWallhit(S4 s4_id)
{
    BL bl_ret = FALSE;                                                         // 戻り値格納用

    // 左の壁に当たった
    if(sta_s_ObjBall[s4_id].s2_x < FIELD_LEFT)
    {
        sta_s_ObjBall[s4_id].s2_x  = FIELD_LEFT;                               // 座標の修正
        sta_s_ObjBall[s4_id].s2_mx = -sta_s_ObjBall[s4_id].s2_mx;              // 反射
        bl_ret = TRUE;
    }

    // 右の壁に当たった
    else if(sta_s_ObjBall[s4_id].s2_x + sta_s_ObjBall[s4_id].u2_w > FIELD_RIGHT)
    {
        sta_s_ObjBall[s4_id].s2_x  = FIELD_RIGHT - sta_s_ObjBall[s4_id].u2_w;  // 座標の修正
        sta_s_ObjBall[s4_id].s2_mx = -sta_s_ObjBall[s4_id].s2_mx;              // 反射
        bl_ret = TRUE;
    }

    // 上の壁に当たった
    else if(sta_s_ObjBall[s4_id].s2_y < 0)
    {
        sta_s_ObjBall[s4_id].s2_y  = 0;                                        // 座標の修正
        sta_s_ObjBall[s4_id].s2_my = -sta_s_ObjBall[s4_id].s2_my;              // 反射
        bl_ret = TRUE;
    }

#ifdef _BALL_DEBUG_
    // 下の壁に当たった【デバッグ用】
    else if(sta_s_ObjBall[s4_id].s2_y + sta_s_ObjBall[s4_id].u2_h > FIELD_HEIGHT)
    {
        sta_s_ObjBall[s4_id].s2_y  = FIELD_HEIGHT - sta_s_ObjBall[s4_id].u2_h; // 座標の修正
        sta_s_ObjBall[s4_id].s2_my = -sta_s_ObjBall[s4_id].s2_my;              // 跳ね返り
        bl_ret = TRUE;
    }
#endif

    if(bl_ret)
    {
        m_pcmplay(SOUND_B_CONTACT, PCM_PAN_C, PCM_FR15_6KHZ); // 効果音(ボール接触)
        vdf_g_ObjScoreAdd(SCORE_ADD_HIT_WALL);                // スコア加算
    }

    return bl_ret;
}

//============================================================================================
// ラケットとの当たり判定と当たった場合の処理
//--------------------------------------------------------------------------------------------
// Arguments : S4 s4_id  : (I) 対象ボールID
// Return    : BL bl_ret : (O) TRUE  : 当たった
//                             FALSE : 当たっていない
//--------------------------------------------------------------------------------------------
// Note      :
// 引数(s4_id)については、ループカウンタである事を前提としてガード処理を行わない方針とする。
// for(s4_i = 0; s4_i < BALL_MAX; s4_i ++){...}
// sta_s_ObjBall[s4_id]が有効なインデックスである事は呼び出し元で保証すること。
//============================================================================================
static BL blf_s_ObjBallRunRackethit(S4 s4_id)
{
    BL bl_ret = FALSE;

    if(blf_s_ObjBallHitObj(s4_id, stcpcf_g_ObjRacketGetObj()))     // 当たっているなら
    {
        if(u2f_g_InputGetState(CONTROLLER_1, BTN_RIGHT, BS_DOWN)||
           u2f_g_InputGetState(CONTROLLER_1, BTN_RIGHT, BS_PRESS)) // 右入力がある場合
        {
            sta_s_ObjBall[s4_id].s2_mx  = -BALL_DEF_MX;            // 右方向に反射する
        }
        if(u2f_g_InputGetState(CONTROLLER_1, BTN_LEFT, BS_DOWN)||
           u2f_g_InputGetState(CONTROLLER_1, BTN_LEFT, BS_PRESS))  // 左入力がある場合
        {
            sta_s_ObjBall[s4_id].s2_mx  = BALL_DEF_MX;             // 左方向に反射する
        }

        m_pcmplay(SOUND_B_CONTACT, PCM_PAN_C, PCM_FR15_6KHZ);      // 効果音(ボール接触)
        vdf_g_ObjScoreAdd(SCORE_ADD_HIT_RACKET);                   // スコア加算
        bl_ret = TRUE;
    }

    return bl_ret;
}

//============================================================================================
// ボールとオブジェクト(現状はラケットのみ)の当たり判定～ボールの座標修正と反射まで
//--------------------------------------------------------------------------------------------
// Arguments : S4                    s4_id     : (I) 対象ボールID
//             const stOBJECT* const stcpc_obj : (I) 相手オブジェクトのポインタ
// Return    : BL                    bl_ret    : (O) TRUE  : 当たった
//                                                   FALSE : 当たっていない
//--------------------------------------------------------------------------------------------
// Note      :
// 引数(s4_id)については、ループカウンタである事を前提としてガード処理を行わない方針とする。
// for(s4_i = 0; s4_i < BALL_MAX; s4_i ++){...}
// sta_s_ObjBall[s4_id]が有効なインデックスである事は呼び出し元で保証すること。
//============================================================================================
static BL blf_s_ObjBallHitObj(S4 s4_id, const stOBJECT* const stcpc_obj)
{
    BL bl_ret = FALSE;                                                           // 戻り値格納用

    // 相手オブジェクト側
    S2 s2_obj_left   = stcpc_obj->s2_x;                                          // 左端
    S2 s2_obj_top    = stcpc_obj->s2_y;                                          // 上端
    S2 s2_obj_right  = stcpc_obj->s2_x + stcpc_obj->u2_w;                        // 右端
    S2 s2_obj_bottom = stcpc_obj->s2_y + stcpc_obj->u2_h;                        // 下端

    // ボール[ID]側
    S2 s2_ball_cx = sta_s_ObjBall[s4_id].s2_x + (sta_s_ObjBall[s4_id].u2_w / 2); // X軸中心
    S2 s2_ball_cy = sta_s_ObjBall[s4_id].s2_y + (sta_s_ObjBall[s4_id].u2_h / 2); // Y軸中心

    S2 s2_ball_left   = sta_s_ObjBall[s4_id].s2_x;                               // 左端
    S2 s2_ball_top    = sta_s_ObjBall[s4_id].s2_y;                               // 上端
    S2 s2_ball_right  = sta_s_ObjBall[s4_id].s2_x + sta_s_ObjBall[s4_id].u2_w;   // 右端
    S2 s2_ball_bottom = sta_s_ObjBall[s4_id].s2_y + sta_s_ObjBall[s4_id].u2_h;   // 下端

    // オブジェクトの縦幅内にボールのY軸中心が入っているか
    if(s2_obj_top <= s2_ball_cy && s2_ball_cy <= s2_obj_bottom)
    {
        if(s2_ball_left <= s2_obj_right && s2_obj_right <= s2_ball_right)        // ボール左端Hit
        {
            sta_s_ObjBall[s4_id].s2_x  = s2_obj_right;                           // 座標修正
            sta_s_ObjBall[s4_id].s2_mx = -sta_s_ObjBall[s4_id].s2_mx;            // 反射
            bl_ret = TRUE;
        }

        else if(s2_ball_right >= s2_obj_left && s2_obj_left >= s2_ball_left)     // ボール右端Hit
        {
            sta_s_ObjBall[s4_id].s2_x  = s2_obj_left - sta_s_ObjBall[s4_id].u2_w;// 座標修正
            sta_s_ObjBall[s4_id].s2_mx = -sta_s_ObjBall[s4_id].s2_mx;            // 反射
            bl_ret = TRUE;
        }
    }

    // オブジェクトの横幅内にボールのX軸中心が入っているか
    else if(s2_obj_left <= s2_ball_cx && s2_ball_cx <= s2_obj_right)
    {
        if(s2_ball_top <= s2_obj_bottom && s2_obj_bottom <= s2_ball_bottom)      // ボール上端Hit
        {
            sta_s_ObjBall[s4_id].s2_y  = s2_obj_bottom;                          // 座標修正
            sta_s_ObjBall[s4_id].s2_my = -sta_s_ObjBall[s4_id].s2_my;            // 反射
            bl_ret = TRUE;
        }

        else if(s2_ball_bottom >= s2_obj_top && s2_obj_top >= s2_ball_top)       // ボール下端Hit
        {
            sta_s_ObjBall[s4_id].s2_y  = s2_obj_top - sta_s_ObjBall[s4_id].u2_h; // 座標修正
            sta_s_ObjBall[s4_id].s2_my = -sta_s_ObjBall[s4_id].s2_my;            // 反射
            bl_ret = TRUE;
        }
    }

    return bl_ret;
}

//============================================================================================
// ブロックとの当たり判定
//--------------------------------------------------------------------------------------------
// Arguments : S4 s4_id  : (I) 対象ボールID
// Return    : BL bl_ret : (O) TRUE  : 当たった
//                             FALSE : 当たっていない
//--------------------------------------------------------------------------------------------
// Note      :
// 引数(s4_id)については、ループカウンタである事を前提としてガード処理を行わない方針とする。
// for(s4_i = 0; s4_i < BALL_MAX; s4_i ++){...}
// sta_s_ObjBall[s4_id]が有効なインデックスである事は呼び出し元で保証すること。
//============================================================================================
static BL blf_s_ObjBallRunBlockhit(S4 s4_id)
{
    BL bl_ret = FALSE;                                 // 戻り値格納用
    S2* const s2cp_dy = &(sta_s_ObjBall[s4_id].s2_my); // 対象ボールY移動量のアドレス (Y反射用)
    S2* const s2cp_dx = &(sta_s_ObjBall[s4_id].s2_mx); // 対象ボールX移動量のアドレス (X反射用)

    // 当たり判定を行うボール側の4点：
    // 一番左のブロック列より更に左にボールがある状態で、計算結果がアンダーフローしても
    // 型をU2にする事で当たり判定の条件式(値の範囲の"上限"の方)で除外されるようにしている。
    // これにより、値の範囲の"下限"のチェックは行わない方針とする。

    U2 u2_cx = sta_s_ObjBall[s4_id].s2_x + (sta_s_ObjBall[s4_id].u2_w / 2);
    U2 u2_cy = sta_s_ObjBall[s4_id].s2_y + (sta_s_ObjBall[s4_id].u2_h / 2);

    U2 u2_tx = // ボール上辺中央(ブロックデータXY位置に変換済)
    u2_cx / BLOCK_WIDTH - BLOCK_HIT_OFFSET_X;
    U2 u2_ty =
    sta_s_ObjBall[s4_id].s2_y / BLOCK_HEIGHT - BLOCK_HIT_OFFSET_Y;

    U2 u2_bx = // ボール下辺中央(ブロックデータXY位置に変換済)
    u2_cx / BLOCK_WIDTH - BLOCK_HIT_OFFSET_X;
    U2 u2_by =
    (sta_s_ObjBall[s4_id].s2_y + sta_s_ObjBall[s4_id].u2_h) / BLOCK_HEIGHT - BLOCK_HIT_OFFSET_Y;

    U2 u2_lx =  // ボール左辺中央(ブロックデータXY位置に変換済)
    sta_s_ObjBall[s4_id].s2_x / BLOCK_WIDTH - BLOCK_HIT_OFFSET_X;
    U2 u2_ly =
    u2_cy / BLOCK_HEIGHT - BLOCK_HIT_OFFSET_Y;

    U2 u2_rx =  // ボール右辺中央(ブロックデータXY位置に変換済)
    (sta_s_ObjBall[s4_id].s2_x + sta_s_ObjBall[s4_id].u2_w) / BLOCK_WIDTH - BLOCK_HIT_OFFSET_X;
    U2 u2_ry =
    u2_cy / BLOCK_HEIGHT - BLOCK_HIT_OFFSET_Y;

    // ブロックデータと照合
    if     (blf_s_ObjBallRunBlockhitChk(u2_tx, u2_ty, s2cp_dy)) bl_ret = TRUE; // 上辺照合
    else if(blf_s_ObjBallRunBlockhitChk(u2_bx, u2_by, s2cp_dy)) bl_ret = TRUE; // 下辺照合
    else if(blf_s_ObjBallRunBlockhitChk(u2_lx, u2_ly, s2cp_dx)) bl_ret = TRUE; // 左辺照合
    else if(blf_s_ObjBallRunBlockhitChk(u2_rx, u2_ry, s2cp_dx)) bl_ret = TRUE; // 右辺照合

    return bl_ret;
}

//============================================================================================
// ブロックデータとの照合～当たっている場合の処理
// ・ブロックの破壊または耐久度減
// ・アイテムブロックの場合はアイテム生成
// ・貫通状態でなければボールの反射
//--------------------------------------------------------------------------------------------
// Arguments : U2        u2_x     : (I) 照合を行うブロックデータのX位置
//             U2        u2_y     : (I) 照合を行うブロックデータのY位置
//             S2* const s2cp_dir : (O) 対象ボールの反射方向(XまたはYの移動量)のアドレス
// Return    : BL        bl_ret   : (O) TRUE  : 当たった
//                                      FALSE : 当たっていない
//============================================================================================
static BL blf_s_ObjBallRunBlockhitChk(U2 u2_x, U2 u2_y, S2* const s2cp_dir)
{
    BL bl_ret = FALSE;                                        // 戻り値格納用
    U2 u2_item_x, u2_item_y;                                  // アイテム生成情報：座標
    U1 u1_item_type;                                          // アイテム生成情報：種類

    U1 u1_bdata = u1f_g_ObjBlockGetData(u2_x, u2_y);          // ブロックデータ取得

    // 引数で渡された位置がブロックデータの範囲内かつそこにブロックがあるか
    if(u2_x < BLOCK_DATA_EL_W && u2_y < BLOCK_DATA_EL_H && u1_bdata)
    {
        // 通常ブロック(1～3)の場合
        if(u1_bdata < BLOCK_DATA_BORDER)
        {
            u1_bdata --;
            vdf_g_ObjBlockSetData(u2_x, u2_y, u1_bdata);      // 硬さ -1
        }
        else
        {
            // アイテム生成情報
            u2_item_x =
            (u2_x + BLOCK_HIT_OFFSET_X) * BLOCK_WIDTH + (BLOCK_WIDTH / 2) - (ITEM_WIDTH  / 2);
            u2_item_y =
            (u2_y + BLOCK_HIT_OFFSET_Y) * BLOCK_HEIGHT + BLOCK_HEIGHT;
            u1_item_type = u1_bdata - BLOCK_DATA_BORDER;

            // アイテム生成
            blf_g_ObjItemCreate(u2_item_x, u2_item_y, u1_item_type);

            // アイテムブロックは一撃消滅
            vdf_g_ObjBlockSetData(u2_x, u2_y, BLOCK_DATA_NONE);
        }

        if(s2_s_ObjBallThCount <= 0)                          // 貫通カウントが0以下なら
        {
            *s2cp_dir = -(*s2cp_dir);                         // ボールの跳ね返り
        }

        vdf_g_ObjBlockBgUpdate(u2_x, u2_y);                   // ブロック(BG)表示更新
        m_pcmplay(SOUND_B_CONTACT, PCM_PAN_C, PCM_FR15_6KHZ); // 効果音(ボール接触)
        vdf_g_ObjScoreAdd(SCORE_ADD_HIT_BLOCK);               // スコア加算

        bl_ret = TRUE;
    }

    return bl_ret;
}
