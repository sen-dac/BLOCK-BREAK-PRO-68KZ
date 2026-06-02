//============================================================================================
// Application : BLOCK BREAK PRO-68KZ
// Platform    : SHARP X68000 / Human68k
// Build       : GCC真里子版(ver1.42) + XCライブラリ + Z-MUSIC(v2.00)
//--------------------------------------------------------------------------------------------
// Filename    : keycode.h
// Version     : 2.0.0
// Author      : SEN::DAC
// Description : X68000用 BITSNS()で使うキーコードグループとキーコードを定義
//--------------------------------------------------------------------------------------------
// Reference   : C-compiler PRO-68K Cライブラリマニュアル(P.176)
//============================================================================================
//--------------------------------------------------------------------------------------------
// 2重インクルード防止
//--------------------------------------------------------------------------------------------
#ifndef _KEYCODE_H_
#define _KEYCODE_H_

//--------------------------------------------------------------------------------------------
// インクルードファイル
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// 定数定義
//--------------------------------------------------------------------------------------------
// キーグループ
#define KG_0    0x00
#define KG_1    0x01
#define KG_2    0x02
#define KG_3    0x03
#define KG_4    0x04
#define KG_5    0x05
#define KG_6    0x06
#define KG_7    0x07
#define KG_8    0x08
#define KG_9    0x09
#define KG_A    0x0A
#define KG_B    0x0B
#define KG_C    0x0C
#define KG_D    0x0D
#define KG_E    0x0E
#define KG_F    0x0F

// グループ 0
////////////////////////////// BIT_0 : 未定義
#define KG_0_KC_ESC    0x02 // BIT_1 : ESC
#define KG_0_KC_K1_    0x04 // BIT_2 : 1 !
#define KG_0_KC_K2_    0x08 // BIT_3 : 2 "
#define KG_0_KC_K3_    0x10 // BIT_4 : 3 #
#define KG_0_KC_K4_    0x20 // BIT_5 : 4 $
#define KG_0_KC_K5_    0x40 // BIT_6 : 5 %
#define KG_0_KC_K6_    0x80 // BIT_7 : 6 &
// グループ 1
#define KG_1_KC_K7_    0x01 // BIT_0 : 7 '
#define KG_1_KC_K8_    0x02 // BIT_1 : 8 (
#define KG_1_KC_K9_    0x04 // BIT_2 : 9 )
#define KG_1_KC_K0_    0x08 // BIT_3 : 0 ﾜ
#define KG_1_KC_KMI    0x10 // BIT_4 : - =
#define KG_1_KC_HAT    0x20 // BIT_5 : ^
#define KG_1_KC_YEN    0x40 // BIT_6 : \ |
#define KG_1_KC_BS_    0x80 // BIT_7 : BS
// グループ 2
#define KG_2_KC_TAB    0x01 // BIT_0 : TAB
#define KG_2_KC_Q__    0x02 // BIT_1 : Q
#define KG_2_KC_W__    0x04 // BIT_2 : W
#define KG_2_KC_E__    0x08 // BIT_3 : E
#define KG_2_KC_R__    0x10 // BIT_4 : R
#define KG_2_KC_T__    0x20 // BIT_5 : T
#define KG_2_KC_Y__    0x40 // BIT_6 : Y
#define KG_2_KC_U__    0x80 // BIT_7 : U
// グループ 3
#define KG_3_KC_I__    0x01 // BIT_0 : I
#define KG_3_KC_O__    0x02 // BIT_1 : O
#define KG_3_KC_P__    0x04 // BIT_2 : P
#define KG_3_KC_AT_    0x08 // BIT_3 : @
#define KG_3_KC_LSB    0x10 // BIT_4 : [
#define KG_3_KC_ENT    0x20 // BIT_5 : CR (テンキーではない方のENTER)
#define KG_3_KC_A__    0x40 // BIT_6 : A
#define KG_3_KC_S__    0x80 // BIT_7 : S
// グループ 4
#define KG_4_KC_D__    0x01 // BIT_0 : D
#define KG_4_KC_F__    0x02 // BIT_1 : F
#define KG_4_KC_G__    0x04 // BIT_2 : G
#define KG_4_KC_H__    0x08 // BIT_3 : H
#define KG_4_KC_J__    0x10 // BIT_4 : J
#define KG_4_KC_K__    0x20 // BIT_5 : K
#define KG_4_KC_L__    0x40 // BIT_6 : L
#define KG_4_KC_SCO    0x80 // BIT_7 : ;
// グループ 5
#define KG_5_KC_COL    0x01 // BIT_0 : :
#define KG_5_KC_RSB    0x02 // BIT_1 : ]
#define KG_5_KC_Z__    0x04 // BIT_2 : Z
#define KG_5_KC_X__    0x08 // BIT_3 : X
#define KG_5_KC_C__    0x10 // BIT_4 : C
#define KG_5_KC_V__    0x20 // BIT_5 : V
#define KG_5_KC_B__    0x40 // BIT_6 : B
#define KG_5_KC_N__    0x80 // BIT_7 : N  (マニュアルの M は誤記)
// グループ 6
#define KG_6_KC_M__    0x01 // BIT_0 : M
#define KG_6_KC_COM    0x02 // BIT_1 : , <
#define KG_6_KC_DOT    0x04 // BIT_2 : . >
#define KG_6_KC_SLA    0x08 // BIT_3 : / ?
#define KG_6_KC_UDB    0x10 // BIT_4 : _
#define KG_6_KC_SP_    0x20 // BIT_5 : SP (スペース)
#define KG_6_KC_HOM    0x40 // BIT_6 : HOME
#define KG_6_KC_DEL    0x80 // BIT_7 : DEL
// グループ 7
#define KG_7_KC_RUP    0x01 // BIT_0 : Roll up
#define KG_7_KC_RDN    0x02 // BIT_1 : Roll down
#define KG_7_KC_UND    0x04 // BIT_2 : UNDO
#define KG_7_KC_LEF    0x08 // BIT_3 : ←
#define KG_7_KC_UP_    0x10 // BIT_4 : ↑
#define KG_7_KC_RIG    0x20 // BIT_5 : →
#define KG_7_KC_DOW    0x40 // BIT_6 : ↓
#define KG_7_KC_CLR    0x80 // BIT_7 : CLR
// グループ 8
#define KG_8_KC_TSL    0x01 // BIT_0 : / (テンキーの /)
#define KG_8_KC_TAS    0x02 // BIT_1 : * (テンキーの *)
#define KG_8_KC_TMI    0x04 // BIT_2 : - (テンキーの -)
#define KG_8_KC_T7_    0x08 // BIT_3 : 7 (テンキーの 7)
#define KG_8_KC_T8_    0x10 // BIT_4 : 8 (テンキーの 8)
#define KG_8_KC_T9_    0x20 // BIT_5 : 9 (テンキーの 9)
#define KG_8_KC_TPL    0x40 // BIT_6 : + (テンキーの +)
#define KG_8_KC_T4_    0x80 // BIT_7 : 4 (テンキーの 4)
// グループ 9
#define KG_9_KC_T5_    0x01 // BIT_0 : 5 (テンキーの 5)
#define KG_9_KC_T6_    0x02 // BIT_1 : 6 (テンキーの 6)
#define KG_9_KC_TEQ    0x04 // BIT_2 : = (テンキーの =)
#define KG_9_KC_T1_    0x08 // BIT_3 : 1 (テンキーの 1)
#define KG_9_KC_T2_    0x10 // BIT_4 : 2 (テンキーの 2)
#define KG_9_KC_T3_    0x20 // BIT_5 : 3 (テンキーの 3)
#define KG_9_KC_TEN    0x40 // BIT_6 : ENTER (テンキーのENTER)
#define KG_9_KC_T0_    0x80 // BIT_7 : 0 (補足：テンキーの 0)
// グループ A
#define KG_A_KC_TCM    0x01 // BIT_0 : , (テンキーの ,)
#define KG_A_KC_TDT    0x02 // BIT_1 : . (テンキーの .)
#define KG_A_KC_SYM    0x04 // BIT_2 : 記号入力
#define KG_A_KC_REG    0x08 // BIT_3 : 登録
#define KG_A_KC_HLP    0x10 // BIT_4 : HELP
#define KG_A_KC_XF1    0x20 // BIT_5 : XF1
#define KG_A_KC_XF2    0x40 // BIT_6 : XF2
#define KG_A_KC_XF3    0x80 // BIT_7 : XF3
// グループ B
#define KG_B_KC_XF4    0x01 // BIT_0 : XF4
#define KG_B_KC_XF5    0x02 // BIT_1 : XF5
#define KG_B_KC_KNA    0x04 // BIT_2 : かな
#define KG_B_KC_RMJ    0x08 // BIT_3 : ローマ字
#define KG_B_KC_COD    0x10 // BIT_4 : コード入力
#define KG_B_KC_CAP    0x20 // BIT_5 : CAPS (補足:マニュアルの GAPS は誤記)
#define KG_B_KC_INS    0x40 // BIT_6 : INS
#define KG_B_KC_HIR    0x80 // BIT_7 : ひらがな (稀にLED点灯時/消灯時にトグル動作する事があった)
// グループ C
#define KG_C_KC_ZEN    0x01 // BIT_0 : 全角
#define KG_C_KC_BRK    0x02 // BIT_1 : BREAK
#define KG_C_KC_CPY    0x04 // BIT_2 : COPY (押された場合の自作処理より「プリンタがつながっていません」が優先して出てきてしまう)
#define KG_C_KC_F1_    0x08 // BIT_3 : F1
#define KG_C_KC_F2_    0x10 // BIT_4 : F2
#define KG_C_KC_F3_    0x20 // BIT_5 : F3
#define KG_C_KC_F4_    0x40 // BIT_6 : F4
#define KG_C_KC_F5_    0x80 // BIT_7 : F5
// グループ D
#define KG_D_KC_F6_    0x01 // BIT_0 : F6
#define KG_D_KC_F7_    0x02 // BIT_1 : F7
#define KG_D_KC_F8_    0x04 // BIT_2 : F8
#define KG_D_KC_F9_    0x08 // BIT_3 : F9
#define KG_D_KC_F10    0x10 // BIT_4 : F10
////////////////////////////// BIT_5 : 未定義
////////////////////////////// BIT_6 : 未定義
////////////////////////////// BIT_7 : 未定義
// グループ E
#define KG_E_KC_SHT    0x01 // BIT_0 : SHIFT (左右どちらのSHIFTも反応する)
#define KG_E_KC_CTR    0x02 // BIT_1 : CTRL  (左上,左下,右,3つあるどれでも反応する)
#define KG_E_KC_OP1    0x04 // BIT_2 : OPT.1 (自作処理中も OPT.1,2同時押しで電卓が出てくる)
#define KG_E_KC_OP2    0x08 // BIT_3 : OPT.2 (自作処理中も OPT.1,2同時押しで電卓が出てくる)
////////////////////////////// BIT_4 : 未定義
////////////////////////////// BIT_5 : 未定義
////////////////////////////// BIT_6 : 未定義
////////////////////////////// BIT_7 : 未定義
// グループ F
////////////////////////////// BIT_0 : 未定義
////////////////////////////// BIT_1 : 未定義
////////////////////////////// BIT_2 : 未定義
////////////////////////////// BIT_3 : 未定義
////////////////////////////// BIT_4 : 未定義
////////////////////////////// BIT_5 : 未定義
////////////////////////////// BIT_6 : 未定義
////////////////////////////// BIT_7 : 未定義

//--------------------------------------------------------------------------------------------
// 型定義
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// extern
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// プロトタイプ宣言
//--------------------------------------------------------------------------------------------

#endif // _KEYCODE_H_
