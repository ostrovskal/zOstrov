//
// Created by User on 18.04.2023.
//

#pragma once

#include "zssh/zCommon.h"
#include "android_native.h"
#include "sshApp.h"

extern sshApp*      theApp;

static zStyle themeDark[] = {
        { Z_THEME_NAME,                   z.R.string.themeDark },
        // картинки
        { Z_FONT1,                        z.R.drawable.font1 },
        { Z_FONT2,                        z.R.drawable.font2 },
        { Z_FONT3,                        z.R.drawable.font3 },
        { Z_FONT4,                        z.R.drawable.font4 },
        { Z_BITMAP_COMMON,                z.R.drawable.zssh },
        { Z_BITMAP_ICONS,                 z.R.drawable.zssh },
        { Z_COLOR_COMMON,                 z.R.color.themeDark },
        // цвета
        { Z_COLOR_DIVIDER,                z.R.color.dividerDark },
        { Z_COLOR_SELECTOR,               z.R.color.selectorDark },
        { Z_COLOR_HINT_TEXT,              z.R.color.hintTextDark },
        { Z_COLOR_HIGHLIGHT_TEXT,         z.R.color.highlightTextDark },
        { Z_COLOR_MESSAGE_TEXT,           z.R.color.messageTextDark },
        { Z_COLOR_LINK_TEXT,              z.R.color.linkTextDark },
        { Z_COLOR_TEXT_TEXT,              z.R.color.textTextDark },
        { Z_COLOR_HEADER_TEXT,            z.R.color.headerTextDark },
        { Z_COLOR_TAB,	 		         z.R.color.tabDark },
        { Z_COLOR_TAB_TEXT,		         z.R.color.tabTextDark },
        { Z_COLOR_PROGRESS,               z.R.color.progressDark },
        { Z_COLOR_PROGRESS_TEXT,          z.R.color.progressTextDark },
        { Z_COLOR_PROGRESS_PRIMARY,       z.R.color.progressPrimaryDark },
        { Z_COLOR_SLIDER_TEXT,            z.R.color.sliderTextDark },
        { Z_COLOR_MENU_TEXT,              z.R.color.menuTextDark },
        { Z_COLOR_MENU,                   z.R.color.menuDark },
        // другое
        { Z_PROGRESS_CIRCULAR,            0x00000c15 },
        { Z_PROGRESS_TILES,               0x00001515 },
        { Z_SLIDER_TILES,                 0x00002c2e },
        { Z_FORM_TILE,                    z.R.integer.formBackground },
        { Z_SLIDER_MODE | ZT_END,         ZS_SLIDER_SCALE }
};

static zStyle themeLight[] = {
        { Z_THEME_NAME,                z.R.string.themeLight },
        // картинки
        { Z_FONT1,                     z.R.drawable.font1 },
        { Z_FONT2,                     z.R.drawable.font2 },
        { Z_FONT3,                     z.R.drawable.font3 },
        { Z_FONT4,                     z.R.drawable.font4 },
        { Z_BITMAP_COMMON,             z.R.drawable.zssh },
        { Z_BITMAP_ICONS,              z.R.drawable.zssh },
        { Z_COLOR_COMMON,              z.R.color.themeLight },
        // цвета
        { Z_COLOR_DIVIDER,             z.R.color.dividerLight },
        { Z_COLOR_SELECTOR,            z.R.color.selectorLight },
        { Z_COLOR_HINT_TEXT,           z.R.color.hintTextLight },
        { Z_COLOR_HIGHLIGHT_TEXT,      z.R.color.highlightTextLight },
        { Z_COLOR_MESSAGE_TEXT,        z.R.color.messageTextLight },
        { Z_COLOR_LINK_TEXT,           z.R.color.linkTextLight },
        { Z_COLOR_TEXT_TEXT,           z.R.color.textTextLight },
        { Z_COLOR_HEADER_TEXT,         z.R.color.headerTextLight },
        { Z_COLOR_TAB,		          z.R.color.tabLight },
        { Z_COLOR_TAB_TEXT,	          z.R.color.tabTextLight },
        { Z_COLOR_PROGRESS,            z.R.color.progressLight },
        { Z_COLOR_PROGRESS_TEXT,       z.R.color.progressTextLight },
        { Z_COLOR_PROGRESS_PRIMARY,    z.R.color.progressPrimaryLight },
        { Z_COLOR_SLIDER_TEXT,         z.R.color.sliderTextLight },
        { Z_COLOR_MENU,                z.R.color.menuLight },
        { Z_COLOR_MENU_TEXT,           z.R.color.menuTextLight },
        // другое
        { Z_PROGRESS_CIRCULAR,         0x00000c14 },
        { Z_PROGRESS_TILES,            0x00001414 },
        { Z_SLIDER_TILES,              0x00002d2f },
        { Z_FORM_TILE,                 z.R.integer.formBackground },
        //{ Z_FORM_TILE,                 z.R.integer.rectRound },
        { Z_SLIDER_MODE | ZT_END,      ZS_SLIDER_ROTATE }
};
