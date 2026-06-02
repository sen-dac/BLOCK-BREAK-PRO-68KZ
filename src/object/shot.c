//============================================================================================
// Application : BLOCK BREAK PRO-68KZ
// Platform    : SHARP X68000 / Human68k
// Build       : GCC真里子版(ver1.42) + XCライブラリ + Z-MUSIC(v2.00)
//--------------------------------------------------------------------------------------------
// Filename    : shot.c
// Version     : 2.0.0
// Author      : SEN::DAC
// Description : ゲームオブジェクト[ショット]の処理
//============================================================================================
//--------------------------------------------------------------------------------------------
// インクルードファイル
//--------------------------------------------------------------------------------------------
#include "shot.h"

//--------------------------------------------------------------------------------------------
// 定数定義 (内部用)
//--------------------------------------------------------------------------------------------
#define SHOT_DEF_MY  -3   // Y移動量((移動はY-方向のみ)
#define SHOT_WIDTH   8    // 横幅
#define SHOT_HEIGHT  8    // 縦幅
#define SHOT_MAX     10   // 所持最大数
#define SHOT_DEF_STC 1    // ストック初期値

#define SHOT_ST_NON   0   // ショットのステータス：未使用(なし)
#define SHOT_ST_STC   1   // ボールのステータス：ストック
#define SHOT_ST_ACT   2   // ボールのステータス：アクティブ

#define STOCK_SHOT_X  331 // ストックショット：X座標
#define STOCK_SHOT_Y  178 // ストックショット：Y座標 (一番下)
#define STOCK_SHOT_CL 10  // ストックショット：縦の表示間隔

//--------------------------------------------------------------------------------------------
// 型定義 (内部用)
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// プロトタイプ宣言 (static)
//--------------------------------------------------------------------------------------------
static VD vdf_s_ObjShotRunFir(VD);                                 // ショットの発射
static BL blf_s_ObjShotRunMove(S4 s4_id);                          // ショットの移動
static BL blf_s_ObjShotRunBlockhit(S4 s4_id);                      // ブロックと当たり判定
static BL blf_s_ObjShotRunBlockhitChk(S4 s4_id, U2 u2_x, U2 u2_y); // ブロックデータと照合

//--------------------------------------------------------------------------------------------
// グローバル変数定義 (外部公開)
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// 静的変数定義 (static)
//--------------------------------------------------------------------------------------------
// オブジェクト
static stOBJECT sta_s_ObjShot[SHOT_MAX];

// ショットのステータス
// オブジェクト毎の用途の違いから演算相手の符号の有無が異なる。
// このため、堅牢性を優先しオブジェクト構造体のメンバに含めず別で持つ方針とした。
static U1 u1a_s_ObjShotStatus[SHOT_MAX];

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
BL blf_g_ObjShotLoadImage(VD)
{
    U4  u4a_pcg[PCG_TILE_SIZE_32U4];                    // BMP → PCG変換用
    U2  u2a_pal[DR_COL16];                              // 16色パレット登録用
    U1* const u1cp_bbuf = u1cpf_g_DrGetBmpBuf();        // BMPロード用バッファ取得

    // 16色BMP画像ロードと書込み
    if(!blf_g_DrLoadBmp16(SZ_IMGFN_SHOT, u1cp_bbuf)) return FALSE;

    vdf_g_SpBmpToPcg(u4a_pcg, (U4*)u1cp_bbuf);          // BMP → PCG 変換
    SP_DEFCG(PCG_SHOT_TOP, SP_SIZE16, (U1*)u4a_pcg);    // PCGに画像転送
    vdf_g_AppPBarAdd();                                 // プログレスバーを進める

    // ショット(SP)の16色パレットをロード(GR以外は登録をここで行っても良い)
    if(!blf_g_DrLoadPal16(SZ_IMGFN_SHOT, u2a_pal, BRIGHT_OFF)) return FALSE;

    vdf_g_DrSetPal16(DR_PAL_PCG, PB_PCG_SHOT, u2a_pal); // 登録
    vdf_g_AppPBarAdd();                                 // プログレスバーを進める

    return TRUE;
}

//============================================================================================
// ゲームオブジェクト[ショット]の初期化処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
VD vdf_g_ObjShotInit(VD)
{
    S4 s4_i;

    for(s4_i = 0; s4_i < SHOT_MAX; s4_i ++)          // 全てのショット
    {
        sta_s_ObjShot[s4_i].s2_x  = 0;               // X座標
        sta_s_ObjShot[s4_i].s2_y  = 0;               // Y座標
        sta_s_ObjShot[s4_i].s2_mx = 0;               // X移動量
        sta_s_ObjShot[s4_i].s2_my = SHOT_DEF_MY;     // Y移動量
        sta_s_ObjShot[s4_i].u2_w  = SHOT_WIDTH;      // 横幅
        sta_s_ObjShot[s4_i].u2_h  = SHOT_HEIGHT;     // 縦幅
        u1a_s_ObjShotStatus[s4_i] = SHOT_ST_NON;     // ステータス[未使用]
    }

    for(s4_i = 0; s4_i < SHOT_DEF_STC; s4_i ++)      // ストック数初期値の回数だけ処理する
    {
        u1a_s_ObjShotStatus[s4_i] = SHOT_ST_STC;     // テータス[ストック]
    }
}

//============================================================================================
// ゲームオブジェクト[ショット]の実行処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
VD vdf_g_ObjShotRun(VD)
{
    S4 s4_i;

    vdf_s_ObjShotRunFir();                               // 発射の処理

    for(s4_i = 0; s4_i < SHOT_MAX; s4_i ++)              // 全てのショットを巡回
    {
        if(u1a_s_ObjShotStatus[s4_i] == SHOT_ST_ACT)     // ステータス[アクティブ]なら
        {
            // 処理負荷軽減：以降の処理が必要な"可能性が低い"場合「continue」する
            // 各関数に渡す引数(s4_i)がsta_s_ObjShot[]に対して有効なインデックスであることは
            // 呼び出し元であるここで保証する方針とする。
            if(blf_s_ObjShotRunMove(s4_i))     continue; // ショットの移動         (TRUE：消滅)
            if(blf_s_ObjShotRunBlockhit(s4_i)) continue; // ブロックとの当たり判定 (TRUE：Hit)
        }
    }
}

//============================================================================================
// ゲームオブジェクト[ショット]の描画処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
VD vdf_g_ObjShotDraw(VD)
{
    S4 s4_i;
    U2 u2_sts_y = STOCK_SHOT_Y;                      // ストックショットのy座標

    for(s4_i = 0; s4_i < SHOT_MAX; s4_i ++)          // 全てのショット
    {
        // ストックショットの描画
        if(u1a_s_ObjShotStatus[s4_i] == SHOT_ST_STC) // ステータス[ストック]
        {
            vdf_g_SpRegst(SPPL_SHOT_TOP + s4_i,      // プレーン番号
                          STOCK_SHOT_X,              // X座標
                          u2_sts_y,                  // Y座標
                          SP_V_INV_OFF,              // V-垂直反転
                          SP_H_INV_OFF,              // H-水平反転
                          PB_PCG_SHOT,               // パレットブロック番号
                          PCG_SHOT_TOP,              // PCG番号
                          SP_PRW_SP_BG0_BG1);        // 描画優先度

            u2_sts_y -= STOCK_SHOT_CL;
        }

        // アクティブショットの描画
        if(u1a_s_ObjShotStatus[s4_i] == SHOT_ST_ACT) // ステータス[アクティブ]
        {
            vdf_g_SpRegst(SPPL_SHOT_TOP + s4_i,      // プレーン番号
                          sta_s_ObjShot[s4_i].s2_x,  // X座標
                          sta_s_ObjShot[s4_i].s2_y,  // Y座標
                          SP_V_INV_OFF,              // V-垂直反転
                          SP_H_INV_OFF,              // H-水平反転
                          PB_PCG_SHOT,               // パレットブロック番号
                          PCG_SHOT_TOP,              // PCG番号
                          SP_PRW_SP_BG0_BG1);        // 描画優先度
        }
    }
}

//============================================================================================
// ゲームオブジェクト[ショット]の解放処理
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
VD vdf_g_ObjShotRelease(VD)
{
    S4 s4_i;

    for(s4_i = 0; s4_i < SHOT_MAX; s4_i ++) // ショットのスプライト表示を全て消す
    {
        vdf_g_SpHide(SPPL_SHOT_TOP + s4_i);
    }
}

//============================================================================================
// ショットの追加
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
VD vdf_g_ObjShotAdd(VD)
{
    S4 s4_i;

    m_pcmplay(SOUND_ITEM_SHOT, PCM_PAN_C, PCM_FR15_6KHZ); // 効果音(アイテム取得(ショット))

    for(s4_i = 0; s4_i < SHOT_MAX; s4_i ++)               // 全てのショットの中から
    {
        if(u1a_s_ObjShotStatus[s4_i] == SHOT_ST_NON)      // ステータス[未使用]を発見
        {
            u1a_s_ObjShotStatus[s4_i] = SHOT_ST_STC;      // ステータス[ストック]に変更
            return;                                       // 1つでもあれば確定なので抜ける
        }
    }
}

//============================================================================================
// ショットの発射
//--------------------------------------------------------------------------------------------
// Arguments : -
// Return    : -
//============================================================================================
static VD vdf_s_ObjShotRunFir(VD)
{
    S4 s4_i;

    // ラケットのポインタを取得
    const stOBJECT* const stcpc_racket = stcpcf_g_ObjRacketGetObj();

    U2 u2_racket_half_w = stcpc_racket->u2_w / 2;              // ラケットの幅の半分

    // Bボタンが押されたら
    if(u2f_g_InputGetState(CONTROLLER_1, BTN_B, BS_DOWN))
    {
        for(s4_i = 0; s4_i < SHOT_MAX; s4_i ++)               // 全てのショットから
        {
            if(u1a_s_ObjShotStatus[s4_i] == SHOT_ST_ACT)      // ステータス[アクティブ]発見
            break;                                            // 検索終了(生成不可)
        }

        if(s4_i == SHOT_MAX)                                  // アクティブなショットが無いなら
        {
            for(s4_i = 0; s4_i < SHOT_MAX; s4_i ++)           // 全てのショットから
            {
                if(u1a_s_ObjShotStatus[s4_i] == SHOT_ST_STC)  // ステータス[ストック]発見
                {
                    sta_s_ObjShot[s4_i].s2_x =                // X座標をラケット中央に設定
                    stcpc_racket->s2_x + u2_racket_half_w - (sta_s_ObjShot[s4_i].u2_w / 2);
                    sta_s_ObjShot[s4_i].s2_y =                // Y座標をラケット上に設定
                    stcpc_racket->s2_y - sta_s_ObjShot[s4_i].u2_h;

                    sta_s_ObjShot[s4_i].s2_my = SHOT_DEF_MY;  // Y移動量のデフォルト値

                    u1a_s_ObjShotStatus[s4_i]  = SHOT_ST_ACT; // ステータス[アクティブ]

                    // 効果音(ショット発射)
                    m_pcmplay(SOUND_S_FIRING, PCM_PAN_C, PCM_FR15_6KHZ);
                    vdf_g_ObjCountStart();                    // カウント開始
                    break;
                }
            }
        }
    }
}

//============================================================================================
// ショットの移動
//--------------------------------------------------------------------------------------------
// Arguments : S4 s4_id  : (I) 対象ショットID
// Return    : BL bl_ret : (O) TRUE  : 対象IDショット消滅
//                             FALSE : 対象IDショット健在
//--------------------------------------------------------------------------------------------
// Note      :
// 引数(s4_id)については、ループカウンタである事を前提としてガード処理を行わない方針とする。
// for(s4_i = 0; s4_i < SHOT_MAX; s4_i ++){...}
// sta_s_ObjShot[s4_id]が有効なインデックスである事は呼び出し元で保証すること。
//============================================================================================
static BL blf_s_ObjShotRunMove(S4 s4_id)
{
    BL bl_ret = FALSE;                                       // 戻り値格納用

    sta_s_ObjShot[s4_id].s2_y += sta_s_ObjShot[s4_id].s2_my; // Y方向移動

    if(sta_s_ObjShot[s4_id].s2_y < 0 )                       // 上に出た場合消滅
    {
        u1a_s_ObjShotStatus[s4_id] = SHOT_ST_NON;            // ステータス[未使用]
        vdf_g_SpHide(SPPL_SHOT_TOP + s4_id);                 // 表示を消す
        bl_ret = TRUE;
    }

    return bl_ret;
}

//============================================================================================
// ブロックとの当たり判定
//--------------------------------------------------------------------------------------------
// Arguments : S4 s4_id  : (I) 対象ショットID
// Return    : BL bl_ret : (O) TRUE  : 当たった
//                             FALSE : 当たっていない
//--------------------------------------------------------------------------------------------
// Note      :
// 引数(s4_id)については、ループカウンタである事を前提としてガード処理を行わない方針とする。
// for(s4_i = 0; s4_i < SHOT_MAX; s4_i ++){...}
// sta_s_ObjShot[s4_id]が有効なインデックスである事は呼び出し元で保証すること。
//============================================================================================
static BL blf_s_ObjShotRunBlockhit(S4 s4_id)
{
    BL bl_ret = FALSE; // 戻り値格納用

    // 当たり判定を行うショット側の4点：
    // 一番左のブロック列より更に左にショットがある状態で、計算結果がアンダーフローしても
    // 型をU2にする事で当たり判定の条件式(値の範囲の"上限"の方)で除外されるようにしている。
    // これにより、値の範囲の"下限"のチェックは行わない方針とする。

    U2 u2_cx = sta_s_ObjShot[s4_id].s2_x + (sta_s_ObjShot[s4_id].u2_w / 2);
    U2 u2_cy = sta_s_ObjShot[s4_id].s2_y + (sta_s_ObjShot[s4_id].u2_h / 2);

    U2 u2_tx =
    u2_cx / BLOCK_WIDTH - BLOCK_HIT_OFFSET_X;
    U2 u2_ty =
    sta_s_ObjShot[s4_id].s2_y / BLOCK_HEIGHT - BLOCK_HIT_OFFSET_Y;

    U2 u2_bx =
    u2_cx / BLOCK_WIDTH - BLOCK_HIT_OFFSET_X;
    U2 u2_by =
    (sta_s_ObjShot[s4_id].s2_y + sta_s_ObjShot[s4_id].u2_h) / BLOCK_HEIGHT - BLOCK_HIT_OFFSET_Y;

    U2 u2_lx =
    sta_s_ObjShot[s4_id].s2_x / BLOCK_WIDTH - BLOCK_HIT_OFFSET_X;
    U2 u2_ly =
    u2_cy / BLOCK_HEIGHT - BLOCK_HIT_OFFSET_Y;

    U2 u2_rx =
    (sta_s_ObjShot[s4_id].s2_x + sta_s_ObjShot[s4_id].u2_w) / BLOCK_WIDTH - BLOCK_HIT_OFFSET_X;
    U2 u2_ry =
    u2_cy / BLOCK_HEIGHT - BLOCK_HIT_OFFSET_Y;

    // ブロックデータと照合
    if     (blf_s_ObjShotRunBlockhitChk(s4_id, u2_tx, u2_ty)) bl_ret = TRUE; // 上辺照合
    else if(blf_s_ObjShotRunBlockhitChk(s4_id, u2_bx, u2_by)) bl_ret = TRUE; // 下辺照合
    else if(blf_s_ObjShotRunBlockhitChk(s4_id, u2_lx, u2_ly)) bl_ret = TRUE; // 左辺照合
    else if(blf_s_ObjShotRunBlockhitChk(s4_id, u2_rx, u2_ry)) bl_ret = TRUE; // 右辺照合

    return bl_ret;
}

//============================================================================================
// ブロックデータとの照合～当たっている場合の処理
// ・ブロックの破壊または耐久度減
// ・アイテムブロックの場合はアイテム生成
//--------------------------------------------------------------------------------------------
// Arguments : S4 s4_id  : (I) 対象ショットID
//             U2 u2_x   : (I) 照合を行うブロックデータのX要素番号
//             U2 u2_y   : (I) 照合を行うブロックデータのY要素番号
// Return    : BL bl_ret : (O) TRUE  : 当たった
//                             FALSE : 当たっていない
//--------------------------------------------------------------------------------------------
// Note      :
// 引数(s4_id)については、ループカウンタである事を前提としてガード処理を行わない方針とする。
// for(s4_i = 0; s4_i < SHOT_MAX; s4_i ++){...}
// u1a_s_ObjShotStatus[s4_id]が有効なインデックスである事は呼び出し元で保証すること。
//============================================================================================
static BL blf_s_ObjShotRunBlockhitChk(S4 s4_id, U2 u2_x, U2 u2_y)
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

        u1a_s_ObjShotStatus[s4_id] = SHOT_ST_NON;             // ステータス[未使用]
        vdf_g_SpHide(SPPL_SHOT_TOP + s4_id);                  // ショットの表示を消す
        vdf_g_ObjBlockBgUpdate(u2_x, u2_y);                   // ブロック(BG)表示更新
        m_pcmplay(SOUND_S_CONTACT, PCM_PAN_C, PCM_FR15_6KHZ); // 効果音(ショット接触)

        bl_ret = TRUE;
    }

    return bl_ret;
}
