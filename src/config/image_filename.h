//============================================================================================
// Application : BLOCK BREAK PRO-68KZ
// Platform    : SHARP X68000 / Human68k
// Build       : GCC真里子版(ver1.42) + XCライブラリ + Z-MUSIC(v2.00)
//--------------------------------------------------------------------------------------------
// Filename    : image_filename.h
// Version     : 2.0.0
// Author      : SEN::DAC
// Description : 画像ファイル名を定義
//============================================================================================
//--------------------------------------------------------------------------------------------
// 2重インクルード防止
//--------------------------------------------------------------------------------------------
#ifndef _IMAGE_FILENAME_H_
#define _IMAGE_FILENAME_H_

//--------------------------------------------------------------------------------------------
// インクルードファイル
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// 定数定義
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
// 各シーンの背景画像
//--------------------------------------------------------------------------------------------
#define SZ_IMGFN_BK_TITLE    "image/back/s_title.bmp"      // シーン[タイトル]
#define SZ_IMGFN_BK_PLAY     "image/back/s_play.bmp"       // シーン[プレイ]
#define SZ_IMGFN_BK_RESULT   "image/back/s_result.bmp"     // シーン[リザルト]

//--------------------------------------------------------------------------------------------
// 単体オブジェクト
//--------------------------------------------------------------------------------------------
#define SZ_IMGFN_BALL        "image/single/ball.bmp"       // ボール
#define SZ_IMGFN_SHOT        "image/single/shot.bmp"       // ショット
#define SZ_IMGFN_CURSOR      "image/single/cursor.bmp"     // カーソル

//--------------------------------------------------------------------------------------------
// ラケット
//--------------------------------------------------------------------------------------------
#define SZ_IMGFN_RACKET_E    "image/racket/racket_e.bmp"   // ラケット(両端)
#define SZ_IMGFN_RACKET_C    "image/racket/racket_c.bmp"   // ラケット(中間)

//--------------------------------------------------------------------------------------------
// アイテム
//--------------------------------------------------------------------------------------------
#define SZ_IMGFN_ITEM_B      "image/item/item_ball.bmp"    // アイテム(ボール)
#define SZ_IMGFN_ITEM_R      "image/item/item_racket.bmp"  // アイテム(ラケット)
#define SZ_IMGFN_ITEM_T      "image/item/item_Through.bmp" // アイテム(貫通)
#define SZ_IMGFN_ITEM_S      "image/item/item_shot.bmp"    // アイテム(ショット)

//--------------------------------------------------------------------------------------------
// 数字フォント
//--------------------------------------------------------------------------------------------
#define SZ_IMGFN_NUMF_0      "image/num/d0.bmp"            // 0
#define SZ_IMGFN_NUMF_1      "image/num/d1.bmp"            // 1
#define SZ_IMGFN_NUMF_2      "image/num/d2.bmp"            // 2
#define SZ_IMGFN_NUMF_3      "image/num/d3.bmp"            // 3
#define SZ_IMGFN_NUMF_4      "image/num/d4.bmp"            // 4
#define SZ_IMGFN_NUMF_5      "image/num/d5.bmp"            // 5
#define SZ_IMGFN_NUMF_6      "image/num/d6.bmp"            // 6
#define SZ_IMGFN_NUMF_7      "image/num/d7.bmp"            // 7
#define SZ_IMGFN_NUMF_8      "image/num/d8.bmp"            // 8
#define SZ_IMGFN_NUMF_9      "image/num/d9.bmp"            // 9

//--------------------------------------------------------------------------------------------
// ブロック
//--------------------------------------------------------------------------------------------
// 0n
#define SZ_IMGFN_BLOCK_00    "image/block/block00.bmp"     // 上：0(空) / 下：0(空) [00]
#define SZ_IMGFN_BLOCK_01    "image/block/block01.bmp"     // 上：0(空) / 下：1(白) [01]
#define SZ_IMGFN_BLOCK_02    "image/block/block02.bmp"     // 上：0(空) / 下：2(灰) [02]
#define SZ_IMGFN_BLOCK_03    "image/block/block03.bmp"     // 上：0(空) / 下：3(黒) [03]
#define SZ_IMGFN_BLOCK_04    "image/block/block04.bmp"     // 上：0(空) / 下：4(赤) [04]
#define SZ_IMGFN_BLOCK_05    "image/block/block05.bmp"     // 上：0(空) / 下：5(緑) [05]
#define SZ_IMGFN_BLOCK_06    "image/block/block06.bmp"     // 上：0(空) / 下：6(青) [06]
#define SZ_IMGFN_BLOCK_07    "image/block/block07.bmp"     // 上：0(空) / 下：7(黄) [07]
// 1n
#define SZ_IMGFN_BLOCK_10    "image/block/block10.bmp"     // 上：1(白) / 下：0(空) [08]
#define SZ_IMGFN_BLOCK_11    "image/block/block11.bmp"     // 上：1(白) / 下：1(白) [09]
#define SZ_IMGFN_BLOCK_12    "image/block/block12.bmp"     // 上：1(白) / 下：2(灰) [10]
#define SZ_IMGFN_BLOCK_13    "image/block/block13.bmp"     // 上：1(白) / 下：3(黒) [11]
#define SZ_IMGFN_BLOCK_14    "image/block/block14.bmp"     // 上：1(白) / 下：4(赤) [12]
#define SZ_IMGFN_BLOCK_15    "image/block/block15.bmp"     // 上：1(白) / 下：5(緑) [13]
#define SZ_IMGFN_BLOCK_16    "image/block/block16.bmp"     // 上：1(白) / 下：6(青) [14]
#define SZ_IMGFN_BLOCK_17    "image/block/block17.bmp"     // 上：1(白) / 下：7(黄) [15]
// 2n
#define SZ_IMGFN_BLOCK_20    "image/block/block20.bmp"     // 上：2(灰) / 下：0(空) [16]
#define SZ_IMGFN_BLOCK_21    "image/block/block21.bmp"     // 上：2(灰) / 下：1(白) [17]
#define SZ_IMGFN_BLOCK_22    "image/block/block22.bmp"     // 上：2(灰) / 下：2(灰) [18]
#define SZ_IMGFN_BLOCK_23    "image/block/block23.bmp"     // 上：2(灰) / 下：3(黒) [19]
#define SZ_IMGFN_BLOCK_24    "image/block/block24.bmp"     // 上：2(灰) / 下：4(赤) [20]
#define SZ_IMGFN_BLOCK_25    "image/block/block25.bmp"     // 上：2(灰) / 下：5(緑) [21]
#define SZ_IMGFN_BLOCK_26    "image/block/block26.bmp"     // 上：2(灰) / 下：6(青) [22]
#define SZ_IMGFN_BLOCK_27    "image/block/block27.bmp"     // 上：2(灰) / 下：7(黄) [23]
// 3n
#define SZ_IMGFN_BLOCK_30    "image/block/block30.bmp"     // 上：3(黒) / 下：0(空) [24]
#define SZ_IMGFN_BLOCK_31    "image/block/block31.bmp"     // 上：3(黒) / 下：1(白) [25]
#define SZ_IMGFN_BLOCK_32    "image/block/block32.bmp"     // 上：3(黒) / 下：2(灰) [26]
#define SZ_IMGFN_BLOCK_33    "image/block/block33.bmp"     // 上：3(黒) / 下：3(黒) [27]
#define SZ_IMGFN_BLOCK_34    "image/block/block34.bmp"     // 上：3(黒) / 下：4(赤) [28]
#define SZ_IMGFN_BLOCK_35    "image/block/block35.bmp"     // 上：3(黒) / 下：5(緑) [29]
#define SZ_IMGFN_BLOCK_36    "image/block/block36.bmp"     // 上：3(黒) / 下：6(青) [30]
#define SZ_IMGFN_BLOCK_37    "image/block/block37.bmp"     // 上：3(黒) / 下：7(黄) [31]
// 4n
#define SZ_IMGFN_BLOCK_40    "image/block/block40.bmp"     // 上：4(赤) / 下：0(空) [32]
#define SZ_IMGFN_BLOCK_41    "image/block/block41.bmp"     // 上：4(赤) / 下：1(白) [33]
#define SZ_IMGFN_BLOCK_42    "image/block/block42.bmp"     // 上：4(赤) / 下：2(灰) [34]
#define SZ_IMGFN_BLOCK_43    "image/block/block43.bmp"     // 上：4(赤) / 下：3(黒) [35]
#define SZ_IMGFN_BLOCK_44    "image/block/block44.bmp"     // 上：4(赤) / 下：4(赤) [36]
#define SZ_IMGFN_BLOCK_45    "image/block/block45.bmp"     // 上：4(赤) / 下：5(緑) [37]
#define SZ_IMGFN_BLOCK_46    "image/block/block46.bmp"     // 上：4(赤) / 下：6(青) [38]
#define SZ_IMGFN_BLOCK_47    "image/block/block47.bmp"     // 上：4(赤) / 下：7(黄) [39]
// 5n
#define SZ_IMGFN_BLOCK_50    "image/block/block50.bmp"     // 上：5(緑) / 下：0(空) [40]
#define SZ_IMGFN_BLOCK_51    "image/block/block51.bmp"     // 上：5(緑) / 下：1(白) [41]
#define SZ_IMGFN_BLOCK_52    "image/block/block52.bmp"     // 上：5(緑) / 下：2(灰) [42]
#define SZ_IMGFN_BLOCK_53    "image/block/block53.bmp"     // 上：5(緑) / 下：3(黒) [43]
#define SZ_IMGFN_BLOCK_54    "image/block/block54.bmp"     // 上：5(緑) / 下：4(赤) [44]
#define SZ_IMGFN_BLOCK_55    "image/block/block55.bmp"     // 上：5(緑) / 下：5(緑) [45]
#define SZ_IMGFN_BLOCK_56    "image/block/block56.bmp"     // 上：5(緑) / 下：6(青) [46]
#define SZ_IMGFN_BLOCK_57    "image/block/block57.bmp"     // 上：5(緑) / 下：7(黄) [47]
// 6n
#define SZ_IMGFN_BLOCK_60    "image/block/block60.bmp"     // 上：6(青) / 下：0(空) [48]
#define SZ_IMGFN_BLOCK_61    "image/block/block61.bmp"     // 上：6(青) / 下：1(白) [49]
#define SZ_IMGFN_BLOCK_62    "image/block/block62.bmp"     // 上：6(青) / 下：2(灰) [50]
#define SZ_IMGFN_BLOCK_63    "image/block/block63.bmp"     // 上：6(青) / 下：3(黒) [51]
#define SZ_IMGFN_BLOCK_64    "image/block/block64.bmp"     // 上：6(青) / 下：4(赤) [52]
#define SZ_IMGFN_BLOCK_65    "image/block/block65.bmp"     // 上：6(青) / 下：5(緑) [53]
#define SZ_IMGFN_BLOCK_66    "image/block/block66.bmp"     // 上：6(青) / 下：6(青) [54]
#define SZ_IMGFN_BLOCK_67    "image/block/block67.bmp"     // 上：6(青) / 下：7(黄) [55]
// 7n
#define SZ_IMGFN_BLOCK_70    "image/block/block70.bmp"     // 上：7(黄) / 下：0(空) [56]
#define SZ_IMGFN_BLOCK_71    "image/block/block71.bmp"     // 上：7(黄) / 下：1(白) [57]
#define SZ_IMGFN_BLOCK_72    "image/block/block72.bmp"     // 上：7(黄) / 下：2(灰) [58]
#define SZ_IMGFN_BLOCK_73    "image/block/block73.bmp"     // 上：7(黄) / 下：3(黒) [59]
#define SZ_IMGFN_BLOCK_74    "image/block/block74.bmp"     // 上：7(黄) / 下：4(赤) [60]
#define SZ_IMGFN_BLOCK_75    "image/block/block75.bmp"     // 上：7(黄) / 下：5(緑) [61]
#define SZ_IMGFN_BLOCK_76    "image/block/block76.bmp"     // 上：7(黄) / 下：6(青) [62]
#define SZ_IMGFN_BLOCK_77    "image/block/block77.bmp"     // 上：7(黄) / 下：7(黄) [63]

//--------------------------------------------------------------------------------------------
// 型定義
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// extern
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// プロトタイプ宣言
//--------------------------------------------------------------------------------------------

#endif // _IMAGE_FILENAME_H_
