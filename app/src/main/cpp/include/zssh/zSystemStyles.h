
#pragma once

static zStyle styles_default[] = { { Z_PADDING | ZT_END, 0 } };

static zStyle styles_z_formcaption[] = {
	{ Z_SIZE, 0x32001400 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_PADDING, 0x01010101 },
	{ Z_TEXT_STYLE, ZS_TEXT_ITALIC },
	{ Z_FOREGROUND_TILES, z.R.integer.header },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_SCALE, 21626 },
	{ Z_TEXT_SIZE, z.R.dimen.headerText },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME | ZT_END, Z_THEME_COLOR_TEXT_HEADER }
};

static zStyle styles_z_keyboardbase[] = {
	{ Z_FOREGROUND_TILES, z.R.integer.butKeyboardSmall },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_FOREGROUND_COLOR, 0xffffffff },
	{ Z_TEXT_STYLE, ZS_TEXT_BOLD },
	{ Z_TEXT_SIZE, 23 },
	{ Z_TEXT_LINES, 1 },
	{ Z_PADDING, 0x00000101 },
	{ Z_IPADDING, 0x02020202 },
	{ Z_VISIBLED, false },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_ICON, z.R.drawable.zssh },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_ICON_COLOR, 0xffffffff },
	{ Z_ICON_SCALE | ZT_END, 26214 }
};

static zStyle styles_z_keyboardalt[] = {
	{ Z_VISIBLED, false },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_FONT, z.R.drawable.font1 },
	{ Z_TEXT_LINES, 1 },
	{ Z_TEXT_FOREGROUND_COLOR, 0xffafafaf },
	{ Z_TEXT_STYLE, ZS_TEXT_BOLD },
	{ Z_TEXT_SIZE, 14 },
	{ Z_PADDING, 0x00000101 },
	{ Z_IPADDING, 0x01010101 },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_END | ZS_GRAVITY_BOTTOM }
};

static zStyle styles_z_scrollbar[] = {
	{ Z_BACKGROUND, 0xffafafaf },
	{ Z_BACKGROUND_TILES, z.R.integer.rect },
	{ Z_SCROLLBAR_SIZE, 2 },
	{ Z_SCROLLBAR_FADE | ZT_END, true }
};

static zStyle styles_z_glow[] = {
	{ Z_BACKGROUND | ZT_END, 0xffffffff }
};

static zStyle styles_z_caret[] = {
	{ Z_BACKGROUND, 0xffffffff },
	{ Z_DURATION | ZT_END, 500 }
};

static zStyle styles_z_editbutclear[] = {
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TAP | ZT_END, ZS_HOVERED }
};

static zStyle styles_z_tabcontent[] = {
	{ Z_PADDING, 0x02020202 },
	{ Z_BACKGROUND | ZT_END, 0x50000000 }
};

static zStyle styles_z_formfooterlyt[] = {
	{ Z_PADDING, 0x01010101 },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_CENTER }
};

static zStyle styles_z_formcontentlyt[] = {
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_PADDING | ZT_END, 0x02020202 }
};

static zStyle styles_z_toast[] = {
	{ Z_BACKGROUND | ZT_THEME, Z_THEME_COLOR_MENU },
	{ Z_BACKGROUND_TILES, z.R.integer.rectRound },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_MENU },
	{ Z_BEHAVIOR, 0 },
	{ Z_DURATION, 30 },
	{ Z_PADDING, 0x05050505 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_MARGINS | ZT_END, 0x00000003 }
};

static zStyle styles_z_bar[] = {
	{ Z_FBO, true },
	{ Z_BACKGROUND | ZT_THEME, Z_THEME_COLOR_MENU },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_DURATION, 60 },
	{ Z_PADDING | ZT_END, 0x02020202 }
};

static zStyle styles_z_barbutton[] = {
	{ Z_SIZE, 0x18001800 },
	{ Z_FOREGROUND, z.R.drawable.zx_icons },
	{ Z_FOREGROUND_COLOR, 0xffffffff },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_SIZE, 15 },
	{ Z_TEXT_STYLE, ZS_TEXT_NORMAL },
	{ Z_TEXT_FOREGROUND_COLOR, 0xffffffff },
	{ Z_FOREGROUND_SCALE, 32767 },
	{ Z_PADDING | ZT_END, 0x01010101 }
};

static zStyle styles_z_baroverflow[] = {
	{ Z_STYLES, z.R.style.barButton },
	{ Z_FOREGROUND | ZT_END, z.R.drawable.zssh }
};

static zStyle styles_z_menuimage[] = {
	{ Z_FOREGROUND, z.R.drawable.zx_icons },
	{ Z_FOREGROUND_COLOR, 0xffffffff },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_BEHAVIOR, 0 },
	{ Z_FOREGROUND_SCALE, 32767 },
	{ Z_PADDING | ZT_END, 0x02020202 }
};

static zStyle styles_z_barpopup[] = {
	{ Z_FBO, true },
	{ Z_DIVIDER_SIZE, 0x00000002 },
	{ Z_DIVIDER_TYPE, ZS_DIVIDER_BEGIN | ZS_DIVIDER_MIDDLE | ZS_DIVIDER_END },
	{ Z_BACKGROUND | ZT_THEME, Z_THEME_COLOR_MENU },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_SCROLLBAR_FADE, true },
	{ Z_DECORATE, ZS_VSCROLLBAR },
	{ Z_SELECTOR | ZT_THEME, Z_THEME_COLOR_SELECTOR },
	{ Z_PADDING | ZT_END, 0x02020202 }
};

static zStyle styles_z_menupopup[] = {
	{ Z_FBO, true },
	{ Z_DIVIDER_SIZE, 0x00000002 },
	{ Z_DIVIDER_TYPE, ZS_DIVIDER_MIDDLE },
	{ Z_BACKGROUND | ZT_THEME, Z_THEME_COLOR_MENU },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_SELECTOR | ZT_THEME, Z_THEME_COLOR_SELECTOR },
	{ Z_PADDING | ZT_END, 0x02020202 }
};

static zStyle styles_z_formfooter[] = {
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_TILES, z.R.integer.button2 },
	{ Z_ICON_SCALE, 43253 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TAP, ZS_PRESSED },
	{ Z_PADDING, 0x02020202 },
	{ Z_FOREGROUND_SCALE, 39321 },
	{ Z_ICON_GRAVITY | ZT_END, ZS_GRAVITY_CENTER | ZS_SCALE_HEIGHT }
};

static zStyle styles_z_spin_item[] = {
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_TEXT },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_LINES, 1 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TAP, ZS_HOVERED },
	{ Z_IPADDING, 0x03030303 },
	{ Z_FOREGROUND_TILES | ZT_END, z.R.integer.selectItem }
};

static zStyle styles_z_list_item[] = {
	{ Z_FBO, true },
	{ Z_TEXT_STYLE, ZS_TEXT_BOLD_ITALIC },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_LINES, 1 },
	{ Z_IPADDING, 0x03030303 },
	{ Z_FOREGROUND_TILES | ZT_END, z.R.integer.selectItem }
};

static zStyle styles_z_menu_item[] = {
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_MENU },
	{ Z_TEXT_SIZE, z.R.dimen.menuText },
	{ Z_GRAVITY, ZS_GRAVITY_START | ZS_GRAVITY_VCENTER },
	{ Z_TEXT_STYLE, ZS_TEXT_BOLD },
	{ Z_FOREGROUND_TILE1, z.R.integer.iconGroup },
	{ Z_FOREGROUND_TILE2, z.R.integer.iconGroup },
	{ Z_TEXT_LINES, 1 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_END | ZS_GRAVITY_VCENTER },
	{ Z_FOREGROUND_SCALE, 32767 },
	{ Z_TEXT_DISTANCE, 7 },
	{ Z_PADDING | ZT_END, 0x01010101 }
};

static zStyle styles_z_edit_menu_item[] = {
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_MENU },
	{ Z_TEXT_SIZE, z.R.dimen.menuText },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_STYLE, ZS_TEXT_BOLD },
	{ Z_TEXT_LINES, 1 },
	{ Z_FOREGROUND_SCALE, 32767 },
	{ Z_TEXT_DISTANCE, 7 },
	{ Z_PADDING | ZT_END, 0x01010101 }
};

static zStyle styles_z_spin_capt[] = {
	{ Z_TEXT_LINES, 1 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_TILES, z.R.integer.selectCapt },
	{ Z_TEXT_FONT, z.R.drawable.font1 },
	{ Z_IPADDING, 0x03030303 },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_HIGHLIGHT },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_CENTER }
};

static zStyle styles_z_button[] = {
	{ Z_FBO, true },
	{ Z_ICON | ZT_THEME, Z_THEME_ICONS },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_DISTANCE, 3 },
	{ Z_FOREGROUND_SCALE, 49151 },
	{ Z_TEXT_FONT, z.R.drawable.font1 },
	{ Z_PADDING, 0x03030303 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_END | ZS_GRAVITY_VCENTER },
	{ Z_TAP, ZS_HOVERED },
	{ Z_FOREGROUND_TILES, z.R.integer.gradientRadial },
	{ Z_ICON_SCALE | ZT_END, 32767 }
};

static zStyle styles_z_tools[] = {
	{ Z_FBO, true },
	{ Z_SIZE, 0x20002000 },
	{ Z_ICON | ZT_THEME, Z_THEME_ICONS },
	{ Z_ICON_COLOR, 0xffffffff },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_DISTANCE, 0 },
	{ Z_FOREGROUND_SCALE, 32767 },
	{ Z_PADDING, 0x03030303 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TAP, ZS_HOVERED },
	{ Z_FOREGROUND_TILES, z.R.integer.tools },
	{ Z_ICON_SCALE | ZT_END, 21626 }
};

static zStyle styles_z_checkbox[] = {
	{ Z_FBO, true },
	{ Z_FOREGROUND_TILE1, z.R.integer.checkOff },
	{ Z_FOREGROUND_TILE2, z.R.integer.checkOn },
	{ Z_FOREGROUND_SCALE, 32767 },
	{ Z_TEXT_DISTANCE, 2 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_VCENTER | ZS_GRAVITY_START },
	{ Z_PADDING, 0x02020202 },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_VCENTER | ZS_GRAVITY_START }
};

static zStyle styles_z_radiobutton[] = {
	{ Z_FBO, true },
	{ Z_FOREGROUND_TILE1, z.R.integer.radioOff },
	{ Z_FOREGROUND_TILE2, z.R.integer.radioOn },
	{ Z_FOREGROUND_SCALE, 32767 },
	{ Z_TEXT_DISTANCE, 5 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_END | ZS_GRAVITY_VCENTER },
	{ Z_GRAVITY, ZS_GRAVITY_VCENTER | ZS_GRAVITY_END },
	{ Z_PADDING | ZT_END, 0x03030303 }
};

static zStyle styles_z_edittext[] = {
	{ Z_FBO, false },
	{ Z_FOREGROUND_TILES, z.R.integer.edit },
	{ Z_FOREGROUND_SCALE, 49151 },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_END | ZS_GRAVITY_VCENTER },
	{ Z_ICON, z.R.drawable.zssh },
	{ Z_ICON_TILES, z.R.integer.iconEditEx },
	{ Z_ICON_SCALE, 49151 },
	{ Z_TEXT_DISTANCE, 2 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_PADDING, 0x05050505 },
	{ Z_IPADDING, 0x05050505 },
	{ Z_MODE, ZS_EDIT_TEXT },
	{ Z_TEXT_LINES, 3 },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_START | ZS_GRAVITY_VCENTER }
};

static zStyle styles_z_ribbon[] = {
	{ Z_FBO, true },
	{ Z_SELECTOR | ZT_THEME, Z_THEME_COLOR_SELECTOR },
	{ Z_SCROLLBAR_FADE, false },
	{ Z_SCROLLBAR_TILES, z.R.integer.rect },
	{ Z_DURATION, 30 },
	{ Z_DECORATE, ZS_GLOW | ZS_SCROLLBAR },
	{ Z_BACKGROUND_TILES, z.R.integer.selectDrop },
	{ Z_PADDING, 0x05050505 },
	{ Z_DIVIDER_SIZE | ZT_END, 0x02000202 }
};

static zStyle styles_z_grid[] = {
	{ Z_FBO, true },
	{ Z_SELECTOR | ZT_THEME, Z_THEME_COLOR_SELECTOR },
	{ Z_SCROLLBAR_FADE, false },
	{ Z_DURATION, 30 },
	{ Z_DECORATE, ZS_GLOW | ZS_SCROLLBAR },
	{ Z_BACKGROUND_TILES, z.R.integer.selectDrop },
	{ Z_PADDING, 0x05050505 },
	{ Z_SPACING_LINE, 5 },
	{ Z_SPACING_CELL, 2 },
	{ Z_CELL_SIZE, 90 },
	{ Z_MODE, ZS_GRID_UNIFORM },
	{ Z_LINES, 0 },
	{ Z_DIVIDER_SIZE, 0x03000302 },
	{ Z_SCROLLBAR_TILES | ZT_END, z.R.integer.rect }
};

static zStyle styles_z_select[] = {
	{ Z_FBO, true },
	{ Z_DIVIDER_SIZE, 0x02000202 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_BACKGROUND_TILES, z.R.integer.selectDrop },
	{ Z_SCROLLBAR_FADE, false },
	{ Z_DECORATE, ZS_VSCROLLBAR | ZS_GLOW },
	{ Z_PADDING, 0x02020202 },
	{ Z_SELECTOR | ZT_THEME | ZT_END, Z_THEME_COLOR_SELECTOR }
};

static zStyle styles_z_msgform[] = {
	{ Z_FBO, true },
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_COLOR, 0xffffffff },
	{ Z_FOREGROUND_TILES | ZT_THEME, Z_THEME_TILES_FORM },
	{ Z_TEXT_SIZE, z.R.dimen.messageText },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_MESSAGE },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_MODE, ZS_FORM_OK },
	{ Z_PADDING | ZT_END, 0x03030303 }
};

static zStyle styles_z_slider[] = {
	{ Z_FBO, true },
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_TILE1 | ZT_THEME, Z_THEME_TILE1_SLIDER },
	{ Z_FOREGROUND_TILE2 | ZT_THEME, Z_THEME_TILE2_SLIDER },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_SCALE, 65535 },
	{ Z_TEXT_SIZE, z.R.dimen.hintText },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_PROGRESS },
	{ Z_DURATION, 40 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_PADDING, 0x03030303 },
	{ Z_MODE | ZT_THEME, Z_THEME_MODE_SLIDER },
	{ Z_SPEED_TRACK, 131070 },
	{ Z_DECORATE | ZT_END, ZS_TIPS }
};

static zStyle styles_z_linearprogress[] = {
	{ Z_FBO, true },
	{ Z_FOREGROUND_TILE1 | ZT_THEME, Z_THEME_TILE1_PROGRESS },
	{ Z_FOREGROUND_TILE2 | ZT_THEME, Z_THEME_TILE2_PROGRESS },
	{ Z_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_PROGRESS },
	{ Z_PRIMARY | ZT_THEME, Z_THEME_COLOR_PRIMARY },
	{ Z_PADDING, 0x01010101 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_SIZE, z.R.dimen.hintText },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_PROGRESS },
	{ Z_MODE, ZS_SLIDER_NONE },
	{ Z_DECORATE, ZS_TIPS },
	{ Z_FOREGROUND_SCALE | ZT_END, 111409 }
};

static zStyle styles_z_circularprogress[] = {
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_COLOR, 0xffffffff },
	{ Z_TEXT_FONT, z.R.drawable.font1 },
	{ Z_FOREGROUND_TILES | ZT_THEME, Z_THEME_TILES_WAIT },
	{ Z_MODE, ZS_SLIDER_ROTATE },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER | ZS_SCALE_HEIGHT },
	{ Z_FOREGROUND_SCALE, 131070 },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_PROGRESS },
	{ Z_DECORATE, ZS_TIPS },
	{ Z_TEXT_SIZE | ZT_END, z.R.dimen.hintText }
};

static zStyle styles_z_switchbutton[] = {
	{ Z_FBO, true },
	{ Z_TEXT_SIZE, z.R.dimen.textText },
	{ Z_FOREGROUND_TILE1, z.R.integer.switchTrack },
	{ Z_FOREGROUND_TILE2, z.R.integer.switchTrumb },
	{ Z_PADDING, 0x05050505 },
	{ Z_TEXT_FONT, z.R.drawable.font1 },
	{ Z_GRAVITY, ZS_GRAVITY_START | ZS_GRAVITY_VCENTER },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_END | ZS_GRAVITY_VCENTER },
	{ Z_DURATION, 20 },
	{ Z_FOREGROUND_SCALE | ZT_END, 65535 }
};

static zStyle styles_z_llinear[] = {
	{ Z_DIVIDER_TYPE, ZS_DIVIDER_BEGIN | ZS_DIVIDER_MIDDLE | ZS_DIVIDER_END },
	{ Z_DIVIDER_SIZE, 0x03000303 },
	{ Z_PADDING, 0x05050505 },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_CENTER }
};

static zStyle styles_z_tabwidget[] = {
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_ICON, z.R.drawable.zx_icons },
	{ Z_SIZE, 0x10001000 },
	{ Z_ICON_SCALE, 32767 },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_ICON_COLOR, 0xffffffff },
	{ Z_TEXT_FONT, z.R.drawable.font1 },
	{ Z_TEXT_SIZE, z.R.dimen.textText },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_TABBED },
	{ Z_PADDING, 0x06020602 },
	{ Z_IPADDING, 0x02020202 },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_CENTER }
};

static zStyle styles_z_tabhost[] = {
	{ Z_BACKGROUND | ZT_THEME, Z_THEME_COLOR_TABBED },
	{ Z_BACKGROUND_TILES, z.R.integer.gradientDiag },
	{ Z_SELECTOR | ZT_THEME, Z_THEME_COLOR_SELECTOR },
	{ Z_SELECTOR_TILES | ZT_END, z.R.integer.rect }
};

static zStyle styles_z_ccontroller[] = {
	{ Z_FBO, true },
	{ Z_GRAVITY, ZS_GRAVITY_START | ZS_GRAVITY_BOTTOM },
	{ Z_FOREGROUND_TILE1, z.R.integer.ccontrolU },
	{ Z_FOREGROUND_TILE2, z.R.integer.ccontrolL },
	{ Z_FOREGROUND_TILE3, z.R.integer.ccontrolR },
	{ Z_FOREGROUND_TILE4, z.R.integer.ccontrolD },
	{ Z_FOREGROUND_COLOR | ZT_END, 0xffffffff }
};

static zStyle styles_z_acontroller[] = {
	{ Z_FBO, true },
	{ Z_GRAVITY, ZS_GRAVITY_END | ZS_GRAVITY_BOTTOM },
	{ Z_FOREGROUND_TILE1, z.R.integer.acontrolY },
	{ Z_FOREGROUND_TILE2, z.R.integer.acontrolA },
	{ Z_FOREGROUND_TILE3, z.R.integer.acontrolX },
	{ Z_FOREGROUND_TILE4, z.R.integer.acontrolB },
	{ Z_FOREGROUND_COLOR | ZT_END, 0xffffffff }
};

static zStyle styles_z_form[] = {
	{ Z_FBO, true },
	{ Z_BACKGROUND_COLOR, 0xffffffff },
	{ Z_BACKGROUND_TILES | ZT_THEME, Z_THEME_TILES_FORM },
	{ Z_MODE, ZS_FORM_YES_NO_DEF },
	{ Z_MASK, z.R.drawable.zssh },
	{ Z_MASK_COLOR, 0xff7f7f7f },
	{ Z_MASK_TILES | ZT_END, z.R.integer.rectRound }
};

static zStyle styles_z_chart[] = {
	{ Z_FBO, true },
	{ Z_BACKGROUND, z.R.drawable.zssh },
	{ Z_BACKGROUND_TILES, z.R.integer.selectDrop },
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_TILES, z.R.integer.rect },
	{ Z_FOREGROUND_COLOR, 0xffffffff },
	{ Z_DECORATE, ZS_SCROLLBAR },
	{ Z_SCROLLBAR_FADE, false },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_PADDING | ZT_END, 0x05050505 }
};

static zStyle styles_z_chartdiagramm[] = {
	{ Z_STYLES, z.R.style.chart },
	{ Z_MODE | ZT_END, ZS_CHART_DIAGRAMM }
};

static zStyle styles_z_chartgraph[] = {
	{ Z_STYLES, z.R.style.chart },
	{ Z_MODE | ZT_END, ZS_CHART_GRAPH }
};

static zStyle styles_z_themedark[] = {
	{ Z_THEME_NAME, z.R.string.themeDark },
	{ Z_THEME_FONT, z.R.drawable.font1 },
	{ Z_THEME_BITMAP, z.R.drawable.zssh },
	{ Z_THEME_ICONS, z.R.drawable.zssh },
	{ Z_THEME_COLOR, z.R.color.themeDark },
	{ Z_THEME_COLOR_DIVIDER, z.R.color.dividerDark },
	{ Z_THEME_COLOR_SELECTOR, z.R.color.selectorDark },
	{ Z_THEME_COLOR_PROGRESS, z.R.color.progressDark },
	{ Z_THEME_COLOR_PRIMARY, z.R.color.primaryDark },
	{ Z_THEME_COLOR_MENU, z.R.color.menuDark },
	{ Z_THEME_COLOR_TABBED, z.R.color.tabbedDark },
	{ Z_THEME_COLOR_TEXT_TEXT, z.R.color.textDark },
	{ Z_THEME_COLOR_TEXT_HIGHLIGHT, z.R.color.textHighlightDark },
	{ Z_THEME_COLOR_TEXT_SHADOW, z.R.color.textShadowDark },
	{ Z_THEME_COLOR_TEXT_MESSAGE, z.R.color.textMessageDark },
	{ Z_THEME_COLOR_TEXT_HINT, z.R.color.textHintDark },
	{ Z_THEME_COLOR_TEXT_HEADER, z.R.color.textHeaderDark },
	{ Z_THEME_COLOR_TEXT_URL, z.R.color.textUrlDark },
	{ Z_THEME_COLOR_TEXT_TABBED, z.R.color.textTabbedDark },
	{ Z_THEME_COLOR_TEXT_PROGRESS, z.R.color.textProgressDark },
	{ Z_THEME_COLOR_TEXT_MENU, z.R.color.textMenuDark },
	{ Z_THEME_TILE1_PROGRESS, z.R.integer.rectRound },
	{ Z_THEME_TILE2_PROGRESS, z.R.integer.rectRound },
	{ Z_THEME_TILE1_SLIDER, z.R.integer.sliderTrack1 },
	{ Z_THEME_TILE2_SLIDER, z.R.integer.sliderTrumb1 },
	{ Z_THEME_TILES_FORM, z.R.integer.formBackground },
	{ Z_THEME_TILES_WAIT, z.R.integer.progressWait },
	{ Z_THEME_MODE_SLIDER, ZS_SLIDER_ROTATE },
	{ Z_THEME_SIZE_TEXT_TEXT | ZT_END, z.R.dimen.textText }
};

static zStyle styles_z_themelight[] = {
	{ Z_THEME_NAME, z.R.string.themeLight },
	{ Z_THEME_FONT, z.R.drawable.font1 },
	{ Z_THEME_BITMAP, z.R.drawable.zssh },
	{ Z_THEME_ICONS, z.R.drawable.zssh },
	{ Z_THEME_COLOR, z.R.color.themeLight },
	{ Z_THEME_COLOR_DIVIDER, z.R.color.dividerLight },
	{ Z_THEME_COLOR_SELECTOR, z.R.color.selectorLight },
	{ Z_THEME_COLOR_PROGRESS, z.R.color.progressLight },
	{ Z_THEME_COLOR_PRIMARY, z.R.color.primaryLight },
	{ Z_THEME_COLOR_MENU, z.R.color.menuLight },
	{ Z_THEME_COLOR_TABBED, z.R.color.tabbedLight },
	{ Z_THEME_COLOR_TEXT_TEXT, z.R.color.textLight },
	{ Z_THEME_COLOR_TEXT_HIGHLIGHT, z.R.color.textHighlightLight },
	{ Z_THEME_COLOR_TEXT_SHADOW, z.R.color.textShadowLight },
	{ Z_THEME_COLOR_TEXT_MESSAGE, z.R.color.textMessageLight },
	{ Z_THEME_COLOR_TEXT_HINT, z.R.color.textHintLight },
	{ Z_THEME_COLOR_TEXT_HEADER, z.R.color.textHeaderLight },
	{ Z_THEME_COLOR_TEXT_URL, z.R.color.textUrlLight },
	{ Z_THEME_COLOR_TEXT_TABBED, z.R.color.textTabbedLight },
	{ Z_THEME_COLOR_TEXT_PROGRESS, z.R.color.textProgressLight },
	{ Z_THEME_COLOR_TEXT_MENU, z.R.color.textMenuLight },
	{ Z_THEME_TILE1_PROGRESS, z.R.integer.gradientRadial },
	{ Z_THEME_TILE2_PROGRESS, z.R.integer.gradientRadial },
	{ Z_THEME_TILE1_SLIDER, z.R.integer.sliderTrack2 },
	{ Z_THEME_TILE2_SLIDER, z.R.integer.sliderTrumb2 },
	{ Z_THEME_TILES_FORM, z.R.integer.formBackground },
	{ Z_THEME_TILES_WAIT, z.R.integer.progressWait },
	{ Z_THEME_MODE_SLIDER, ZS_SLIDER_SCALE },
	{ Z_THEME_SIZE_TEXT_TEXT | ZT_END, z.R.dimen.textText }
};

static zStyle styles_z_zcelllayout1[] = {
	{ Z_GRAVITY, ZS_GRAVITY_START },
	{ Z_PADDING | ZT_END, 0x01010101 }
};

static zStyle styles_z_list1[] = {
	{ Z_STYLES, z.R.style.ribbon },
	{ Z_BACKGROUND_TILES | ZT_END, z.R.integer.selectDrop }
};

static zStyle styles_z_edit1[] = {
	{ Z_MODE, ZS_EDIT_TEXT },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_TEXT },
	{ Z_TEXT_SIZE, 20 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_ICON, z.R.drawable.zssh },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_END },
	{ Z_ICON_COLOR, 0xffffffff },
	{ Z_ICON_TILES, z.R.integer.iconEdit },
	{ Z_TEXT_STYLE, ZS_TEXT_BOLD_ITALIC | ZS_TEXT_UNDERLINE },
	{ Z_PADDING, 0x0A0A0A0A },
	{ Z_GRAVITY, ZS_GRAVITY_VCENTER },
	{ Z_FOREGROUND | ZT_THEME, Z_THEME_BITMAP },
	{ Z_FOREGROUND_TILES, z.R.integer.edit },
	{ Z_TEXT_LINES | ZT_END, 1 }
};

static zStyle styles_z_button1[] = {
	{ Z_FOREGROUND_TILES, z.R.integer.gradientRadial },
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_SCALE, 65535 },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_TEXT },
	{ Z_TEXT_SIZE, 40 },
	{ Z_PADDING, 0x05050505 },
	{ Z_TAP | ZT_END, ZS_HOVERED }
};

static zStyle styles_z_switch1[] = {
	{ Z_FOREGROUND | ZT_THEME, Z_THEME_BITMAP },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_END | ZS_GRAVITY_VCENTER },
	{ Z_FOREGROUND_TILE1, z.R.integer.switchTrack },
	{ Z_FOREGROUND_TILE2, z.R.integer.switchTrumb },
	{ Z_TEXT_SIZE, 20 },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_TEXT },
	{ Z_GRAVITY, ZS_GRAVITY_START | ZS_GRAVITY_VCENTER },
	{ Z_PADDING, 0x05050505 },
	{ Z_DURATION | ZT_END, 30 }
};

static zStyle styles_z_seek[] = {
	{ Z_FOREGROUND | ZT_THEME, Z_THEME_BITMAP },
	{ Z_FOREGROUND_SCALE, 65535 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_SPEED_TRACK, 13107 },
	{ Z_TEXT_SIZE, 50 },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_PROGRESS },
	{ Z_FOREGROUND_TILE1, z.R.integer.sliderTrack1 },
	{ Z_FOREGROUND_TILE2, z.R.integer.sliderTrumb1 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_MODE | ZT_THEME, Z_THEME_MODE_SLIDER },
	{ Z_DURATION, 40 },
	{ Z_DECORATE, ZS_TIPS },
	{ Z_PADDING | ZT_END, 0x01010101 }
};

static zStyle styles_z_progress1[] = {
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_TILE1, z.R.integer.rectRound },
	{ Z_FOREGROUND_TILE2, z.R.integer.rectRound },
	{ Z_FOREGROUND_COLOR, 0x7fffffff },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_PRIMARY | ZT_THEME, Z_THEME_COLOR_PRIMARY },
	{ Z_TEXT_FOREGROUND_COLOR, 0xff000000 },
	{ Z_TEXT_SIZE, 20 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_SHADOW_OFFS, 0x0000 },
	{ Z_PADDING, 0x01020304 },
	{ Z_DECORATE, ZS_TIPS },
	{ Z_FOREGROUND_SCALE | ZT_END, 163837 }
};

static zStyle styles_z_formopen[] = {
	{ Z_STYLES, z.R.style.form },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_MODE | ZT_END, ZS_FORM_NONE }
};

static zStyle styles_z_zlinearlayout1[] = {
	{ Z_PADDING | ZT_END, 0x03030303 }
};

static zStyle styles_z_zviewbutton1[] = {
	{ Z_STYLES, z.R.style.tools },
	{ Z_ICON | ZT_END, z.R.drawable.zx_icons }
};

static zStyle styles_z_zscrolllayout1[] = {
	{ Z_DECORATE, ZS_SCROLLBAR | ZS_GLOW },
	{ Z_BEHAVIOR, ZS_CLICKABLE },
	{ Z_PADDING | ZT_END, 0x03030303 }
};

static zStyle styles_z_tv1[] = {
	{ Z_FOREGROUND_TILE1, z.R.integer.formBackground },
	{ Z_FBO, true },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_SCALE | ZT_END, 32767 }
};

static zStyle styles_z_zviewedit1[] = {
	{ Z_FBO, true },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_END },
	{ Z_ICON_TILES, z.R.integer.iconEditEx },
	{ Z_MODE, ZS_EDIT_TEXT },
	{ Z_PADDING, 0x01010101 },
	{ Z_GRAVITY, ZS_GRAVITY_VCENTER | ZS_GRAVITY_HCENTER },
	{ Z_FOREGROUND_TILES, z.R.integer.edit },
	{ Z_TEXT_LINES | ZT_END, 3 }
};

static zStyle styles_z_grid1[] = {
	{ Z_STYLES, z.R.style.grid },
	{ Z_BACKGROUND_TILES | ZT_END, z.R.integer.selectDrop }
};

static zStyle styles_z_radiodark[] = {
	{ Z_STYLES, z.R.style.radiobutton },
	{ Z_DECORATE, ZS_CHECKED | ZS_ELLIPSIS },
	{ Z_TEXT_LINES | ZT_END, 1 }
};

static zStyle styles_z_chkbox[] = {
	{ Z_DECORATE, ZS_CHECKED },
	{ Z_STYLES | ZT_END, z.R.style.checkbox }
};

static zStyle styles_z_radiolight[] = {
	{ Z_DECORATE, ZS_CHECKED | ZS_ELLIPSIS },
	{ Z_STYLES | ZT_END, z.R.style.radiobutton }
};

static zStyle styles_z_zviewbutton2[] = {
	{ Z_FOREGROUND_TILES, z.R.integer.gradientRadial },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_SCALE, 32767 },
	{ Z_ICON_SCALE, 32767 },
	{ Z_TEXT_SIZE, 25 },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_END | ZS_GRAVITY_VCENTER },
	{ Z_PADDING, 0x05050505 },
	{ Z_TAP | ZT_END, ZS_PRESSED }
};

static zStyle styles_z_zviewswitch1[] = {
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_END | ZS_GRAVITY_VCENTER },
	{ Z_FOREGROUND_TILE1, z.R.integer.switchTrack },
	{ Z_FOREGROUND_TILE2, z.R.integer.switchTrumb },
	{ Z_GRAVITY, ZS_GRAVITY_START | ZS_GRAVITY_VCENTER },
	{ Z_PADDING, 0x05050505 },
	{ Z_DURATION | ZT_END, 30 }
};

static zStyle styles_z_button2[] = {
	{ Z_FOREGROUND_TILES, z.R.integer.gradientRadial },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_SCALE, 32767 },
	{ Z_ICON_SCALE, 32767 },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_END | ZS_GRAVITY_VCENTER },
	{ Z_PADDING, 0x05050505 },
	{ Z_TAP | ZT_END, ZS_PRESSED }
};

static zStyle styles_z_switch2[] = {
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_END | ZS_GRAVITY_VCENTER },
	{ Z_FOREGROUND_TILE1, z.R.integer.switchTrack },
	{ Z_FOREGROUND_TILE2, z.R.integer.switchTrumb },
	{ Z_TEXT_STYLE, ZS_TEXT_BOLD },
	{ Z_GRAVITY, ZS_GRAVITY_START | ZS_GRAVITY_VCENTER },
	{ Z_PADDING, 0x05050505 },
	{ Z_DURATION | ZT_END, 30 }
};

static zStyle styles_z_switch3[] = {
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_END | ZS_GRAVITY_VCENTER },
	{ Z_FOREGROUND_TILE1, z.R.integer.switchTrack },
	{ Z_FOREGROUND_TILE2, z.R.integer.switchTrumb },
	{ Z_TEXT_STYLE, ZS_TEXT_ITALIC | ZS_TEXT_STRIKE | ZS_TEXT_UNDERLINE },
	{ Z_GRAVITY, ZS_GRAVITY_START | ZS_GRAVITY_VCENTER },
	{ Z_DECORATE, ZS_CHECKED },
	{ Z_PADDING, 0x05050505 },
	{ Z_DURATION | ZT_END, 30 }
};

static zStyle styles_z_zviewslider1[] = {
	{ Z_STYLES, z.R.style.slider },
	{ Z_SPEED_TRACK, 65535 },
	{ Z_DECORATE | ZT_END, ZS_TIPS }
};

static zStyle styles_z_seek1[] = {
	{ Z_STYLES, z.R.style.slider },
	{ Z_SPEED_TRACK, 78642 },
	{ Z_MARGINS, 0x0A030000 },
	{ Z_DECORATE | ZT_END, ZS_TIPS }
};

static zStyle styles_z_button11[] = {
	{ Z_FOREGROUND_TILES, z.R.integer.gradientRadial },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_SCALE, 32767 },
	{ Z_ICON_SCALE, 32767 },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_START | ZS_GRAVITY_VCENTER },
	{ Z_TEXT_STYLE, ZS_TEXT_UNDERLINE | ZS_TEXT_BOLD_ITALIC },
	{ Z_PADDING, 0x05050505 },
	{ Z_TAP | ZT_END, ZS_PRESSED }
};

static zStyle styles_z_seek2[] = {
	{ Z_STYLES, z.R.style.slider },
	{ Z_SPEED_TRACK, 65535 },
	{ Z_MODE, ZS_SLIDER_SCALE },
	{ Z_DECORATE, ZS_TIPS },
	{ Z_PADDING | ZT_END, 0x01010101 }
};

static zStyle styles_z_edit2[] = {
	{ Z_STYLES, z.R.style.edittext },
	{ Z_MODE, ZS_EDIT_TEXT },
	{ Z_TEXT_STYLE, ZS_TEXT_NORMAL },
	{ Z_TEXT_LINES | ZT_END, 1 }
};

static zStyle styles_z_formsettings[] = {
	{ Z_STYLES, z.R.style.form },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_CENTER }
};

static zStyle styles_z_zlinearlayout2[] = {
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_VCENTER }
};

static zStyle styles_z_zviewtext1[] = {
	{ Z_TEXT_STYLE | ZT_END, ZS_TEXT_BOLD }
};

static zStyle styles_z_zlinearlayout3[] = {
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_CENTER }
};

static zStyle styles_z_ztabwidget1[] = {
	{ Z_ICON | ZT_END, z.R.drawable.zx_icons }
};

