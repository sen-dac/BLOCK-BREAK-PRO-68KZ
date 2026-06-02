//============================================================================================
// Application : BLOCK BREAK PRO-68KZ
// Platform    : SHARP X68000 / Human68k
// Build       : GCC真里子版(ver1.42) + XCライブラリ + Z-MUSIC(v2.00)
//--------------------------------------------------------------------------------------------
// Filename    : StdAfx.h
// Version     : 2.0.0
// Author      : SEN::DAC
// Description : 共通インクルードファイルの集約
//============================================================================================
//--------------------------------------------------------------------------------------------
// 2重インクルード防止
//--------------------------------------------------------------------------------------------
#ifndef _STDAFX_H_
#define _STDAFX_H_

//--------------------------------------------------------------------------------------------
// インクルードファイル
//--------------------------------------------------------------------------------------------
#include <stdio.h>
#include <iocslib.h>
#include <doslib.h>
#include <basic0.h>
#include <ZMUSIC.H>

// lib
// iomap.h は読み込まないこと
// 理由：アプリケーション層からMM-I/Oへの直接アクセスを不可とするため。
#include "./lib/type.h"
#include "./lib/zmprm.h"
#include "./lib/crt/crt.h"
#include "./lib/dma/dma.h"
#include "./lib/draw/dr_com.h"
#include "./lib/draw/dr_gr.h"
#include "./lib/draw/dr_sp.h"
#include "./lib/draw/dr_tx.h"
#include "./lib/input/input.h"
#include "./lib/input/keycode.h"
#include "./lib/timer/timer.h"

// config
#include "./config/image_filename.h"
#include "./config/pcm_id.h"
#include "./config/pcg_map.h"

// app
#include "main.h"

// scene
#include "./scene/sTitle.h"
#include "./scene/sPlay.h"
#include "./scene/sResult.h"

// object
#include "./object/_obj_com.h"
#include "./object/ball.h"
#include "./object/block.h"
#include "./object/count.h"
#include "./object/item.h"
#include "./object/numfont.h"
#include "./object/racket.h"
#include "./object/score.h"
#include "./object/shot.h"
#include "./object/stage.h"

//--------------------------------------------------------------------------------------------
// 定数定義
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// 型定義
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// extern
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
// プロトタイプ宣言
//--------------------------------------------------------------------------------------------

#endif // _STDAFX_H_
