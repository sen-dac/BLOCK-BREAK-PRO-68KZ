//============================================================================================
// Application : BLOCK BREAK PRO-68KZ
// Platform    : SHARP X68000 / Human68k
// Build       : GCC真里子版(ver1.42) + XCライブラリ + Z-MUSIC(v2.00)
//--------------------------------------------------------------------------------------------
// Filename    : iomap.h
// Version     : 2.0.0
// Author      : SEN::DAC
// Description : MM-I/Oアドレス定義
//--------------------------------------------------------------------------------------------
// Note        :
// アプリケーション層からの直接参照を不可とする。
// コメント内のページ番号は「Inside X68000」に対応する。
//============================================================================================
//--------------------------------------------------------------------------------------------
// 2重インクルード防止
//--------------------------------------------------------------------------------------------
#ifndef _IOMAP_H_
#define _IOMAP_H_

//--------------------------------------------------------------------------------------------
// インクルードファイル
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// 定数定義
//--------------------------------------------------------------------------------------------
// グラフィックVRAM
#define U2_VRAM_GRA0 (*((volatile U2*)0xC00000))  // ページ0 先頭(P.170)
#define U2_VRAM_GRA1 (*((volatile U2*)0xC80000))  // ページ1 先頭
#define U2_VRAM_GRA2 (*((volatile U2*)0xD00000))  // ページ2 先頭
#define U2_VRAM_GRA3 (*((volatile U2*)0xD80000))  // ページ3 先頭

// テキストVRAM
#define U2_VRAM_TXT0 (*((volatile U2*)0xE00000))  // T0(bit:0) 先頭(P.172)
#define U2_VRAM_TXT1 (*((volatile U2*)0xE20000))  // T1(bit:1) 先頭
#define U2_VRAM_TXT2 (*((volatile U2*)0xE40000))  // T2(bit:2) 先頭
#define U2_VRAM_TXT3 (*((volatile U2*)0xE60000))  // T3(bit:3) 先頭

// CRTコントローラ
#define U2_CRTC_R00  (*((volatile U2*)0xE80000))  // 水平トータル (P.230)
#define U2_CRTC_R01  (*((volatile U2*)0xE80002))  // 水平同期終了位置
#define U2_CRTC_R02  (*((volatile U2*)0xE80004))  // 水平表示開始位置
#define U2_CRTC_R03  (*((volatile U2*)0xE80006))  // 水平表示終了位置
#define U2_CRTC_R04  (*((volatile U2*)0xE80008))  // 垂直トータル (P.230)
#define U2_CRTC_R05  (*((volatile U2*)0xE8000A))  // 垂直同期終了位置
#define U2_CRTC_R06  (*((volatile U2*)0xE8000C))  // 垂直表示開始位置
#define U2_CRTC_R07  (*((volatile U2*)0xE8000E))  // 垂直表示終了位置
#define U2_CRTC_R08  (*((volatile U2*)0xE80010))  // 外部同期水平アジャスト(P.230)
#define U2_CRTC_R20  (*((volatile U2*)0xE80028))  // メモリモード/表示モード制御 (P.233)
#define U2_CRTC_R21  (*((volatile U2*)0xE8002A))  // テキストVRAM アクセスページ(P.205)
#define U2_CRTC_R23  (*((volatile U2*)0xE8002E))  // テキストVRAM アクセスマスク(P.205)

// ビデオコントローラ
#define U2_VDCTR_GPL (*((volatile U2*)0xE82000))  // GRAパレット先頭(PB0～15)(P.217)
#define U2_VDCTR_TPL (*((volatile U2*)0xE82200))  // TXTパレット先頭(PB0のみ)(P.215)
#define U2_VDCTR_PPL (*((volatile U2*)0xE82200))  // PCGパレット先頭(PB1～15を使用する)(P.215)
#define U2_VDCTR_R01 (*((volatile U2*)0xE82500))  // TX-GR-SPBG間,GRn間の描画優先度(P.188)

// DMAコントローラ(CH.2)
#define U1_DMAC2_CSR (*((volatile U1*)0xE84080))  // チャンネルステータス(R/W)
#define U1_DMAC2_CER (*((volatile U1*)0xE84081))  // チャンネルエラー(R)
#define U1_DMAC2_DCR (*((volatile U1*)0xE84084))  // デバイスコントロール(R/W)
#define U1_DMAC2_OCR (*((volatile U1*)0xE84085))  // オペレーションコントロール(R/W)
#define U1_DMAC2_SCR (*((volatile U1*)0xE84086))  // シーケンスコントロール(R/W)
#define U1_DMAC2_CCR (*((volatile U1*)0xE84087))  // チャンネルコントロール(R/W)
#define U2_DMAC2_MTC (*((volatile U2*)0xE8408A))  // メモリトランスファ・カウンタ (R/W)
#define VP_DMAC2_MAR (*((volatile VD**)0xE8408C)) // メモリアドレス(R/W)
#define VP_DMAC2_DAR (*((volatile VD**)0xE84094)) // デバイスアドレス(R/W)
#define U2_DMAC2_BTC (*((volatile U2*)0xE8409A))  // ベーストランスファ・カウンタ (R/W)
#define VP_DMAC2_BAR (*((volatile VD**)0xE8409C)) // ベースアドレス(R/W)
#define U1_DMAC2_NIV (*((volatile U1*)0xE840A5))  // ノーマルインタラプトベクタ(R/W)
#define U1_DMAC2_EIV (*((volatile U1*)0xE840A7))  // エラーインタラプトベクタ(R/W)
#define U1_DMAC2_MFC (*((volatile U1*)0xE840A9))  // メモリファンクションコード(R/W)
#define U1_DMAC2_CPR (*((volatile U1*)0xE840AD))  // チャンネルプライオリティ(R/W)
#define U1_DMAC2_DFC (*((volatile U1*)0xE840B1))  // デバイスファンクションコード(R/W)
#define U1_DMAC2_BFC (*((volatile U1*)0xE840B9))  // ベースファンクションコード(R/W)
#define U1_DMAC3_GCR (*((volatile U1*)0xE840FF))  // ジェネラルコントロール(CH.3にある)(R/W)

// MFP
#define U1_MFP_GPIP  (*((volatile U1*)0xE88001))  // V-DISP,OPM割り込みetc...(P.81)

// システムポート
#define U1_SYSP_R04  (*((volatile U1*)0xE8E007))  // #4 キーボード / NMIスイッチ(P.519)

// ジョイスティックポート
#define U2_JOYSTK_1P (*((volatile U2*)0xE9A000))  // 1P (P.380)
#define U2_JOYSTK_2P (*((volatile U2*)0xE9A002))  // 2P (P.380)

// スプライトコントローラ
#define U2_SPC_SPSCR (*((volatile U2*)0xEB0000))  // スプライト・スクロール 先頭
#define U2_SPC_BG0_X (*((volatile U2*)0xEB0800))  // BG0 X スクロール
#define U2_SPC_BG0_Y (*((volatile U2*)0xEB0802))  // BG0 Y スクロール
#define U2_SPC_BG1_X (*((volatile U2*)0xEB0804))  // BG1 X スクロール
#define U2_SPC_BG1_Y (*((volatile U2*)0xEB0806))  // BG1 Y スクロール
#define U2_SPC_BGCTR (*((volatile U2*)0xEB0808))  // BG コントロール(P.190)
#define U2_SPC_M_R00 (*((volatile U2*)0xEB080A))  // 画面モード：水平トータル(P.235)
#define U2_SPC_M_R01 (*((volatile U2*)0xEB080C))  // 画面モード：水平表示位置
#define U2_SPC_M_R02 (*((volatile U2*)0xEB080E))  // 画面モード：垂直表示位置
#define U2_SPC_M_R03 (*((volatile U2*)0xEB0810))  // 画面モード：解像度設定

// スプライトVRAM(PCG)
#define U2_PCG_ARTOP (*((volatile U2*)0xEB8000))  // PCGエリア先頭
#define U2_PCG_BGAR0 (*((volatile U2*)0xEBC000))  // BGデータエリア0の先頭
#define U2_PCG_BGAR1 (*((volatile U2*)0xEBE000))  // BGデータエリア1の先頭

//--------------------------------------------------------------------------------------------
// 型定義
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// extern
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// プロトタイプ宣言
//--------------------------------------------------------------------------------------------

#endif // _IOMAP_H_
