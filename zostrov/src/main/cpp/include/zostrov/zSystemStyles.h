
#pragma once

static zStyle styles_default[] = { { Z_PADDING | ZT_END, 0 } };

static zStyle styles_z_formcaption[] = {
	{ Z_SIZE, 0x50003200 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_PADDING, 0x01010101 },
	{ Z_IPADDING, 0x00040004 },
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
	{ Z_TEXT_FOREGROUND_COLOR, 0xFFFFFFFF },
	{ Z_TEXT_STYLE, ZS_TEXT_BOLD },
	{ Z_TEXT_SIZE, 20 },
	{ Z_TEXT_LINES, 1 },
	{ Z_PADDING, 0x00000102 },
	{ Z_IPADDING, 0x03030303 },
	{ Z_VISIBLED, false },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_ICON_COLOR, 0xFFFFFFFF },
	{ Z_ICON_SCALE | ZT_END, 26214 }
};

static zStyle styles_z_keyboardalt[] = {
	{ Z_VISIBLED, false },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_LINES, 1 },
	{ Z_TEXT_FOREGROUND_COLOR, 0xFFAFAFAF },
	{ Z_TEXT_STYLE, ZS_TEXT_BOLD },
	{ Z_TEXT_SIZE, 14 },
	{ Z_PADDING, 0x00000202 },
	{ Z_IPADDING, 0x02020202 },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_END | ZS_GRAVITY_BOTTOM }
};

static zStyle styles_z_scrollbar[] = {
	{ Z_BACKGROUND, 0xFFAFAFAF },
	{ Z_BACKGROUND_TILES, z.R.integer.rect },
	{ Z_SCROLLBAR_SIZE, 4 },
	{ Z_SCROLLBAR_FADE | ZT_END, true }
};

static zStyle styles_z_glow[] = {
	{ Z_BACKGROUND | ZT_END, 0xFFFFFFFF }
};

static zStyle styles_z_caret[] = {
	{ Z_BACKGROUND, 0xFFFFFFFF },
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
	{ Z_PADDING, 0x02020202 },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_CENTER }
};

static zStyle styles_z_formcontentlyt[] = {
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_PADDING | ZT_END, 0x01010101 }
};

static zStyle styles_z_toast[] = {
	{ Z_FBO, true },
	{ Z_BACKGROUND | ZT_THEME, Z_THEME_COLOR_MENU },
	{ Z_BACKGROUND_TILES, z.R.integer.rectRoundSm },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_MENU },
	{ Z_BEHAVIOR, 0 },
	{ Z_DURATION, 30 },
	{ Z_TEXT_LINES, 2 },
	{ Z_PADDING, 0x06060606 },
	{ Z_IPADDING, 0x00000000 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_MARGINS | ZT_END, 0x00000005 }
};

static zStyle styles_z_bar[] = {
	{ Z_FBO, true },
	{ Z_BACKGROUND | ZT_THEME, Z_THEME_COLOR_MENU },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_DURATION | ZT_END, 40 }
};

static zStyle styles_z_barbutton[] = {
	{ Z_SIZE, 0x20002000 },
	{ Z_FOREGROUND | ZT_THEME, Z_THEME_ICONS },
	{ Z_FOREGROUND_COLOR, 0xFFFFFFFF },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_SIZE, 20 },
	{ Z_TEXT_STYLE, ZS_TEXT_NORMAL },
	{ Z_TEXT_FOREGROUND_COLOR, 0xFFFFFFFF },
	{ Z_FOREGROUND_SCALE, 32767 },
	{ Z_PADDING | ZT_END, 0x02020202 }
};

static zStyle styles_z_baroverflow[] = {
	{ Z_STYLES, z.R.style.barButton },
	{ Z_FOREGROUND | ZT_END, z.R.drawable.zssh }
};

static zStyle styles_z_menuimage[] = {
	{ Z_FOREGROUND | ZT_THEME, Z_THEME_ICONS },
	{ Z_FOREGROUND_COLOR, 0xFFFFFFFF },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_BEHAVIOR, 0 },
	{ Z_FOREGROUND_SCALE, 43253 },
	{ Z_PADDING | ZT_END, 0x04040404 }
};

static zStyle styles_z_barpopup[] = {
	{ Z_FBO, true },
	{ Z_DIVIDER_SIZE, 0x00000004 },
	{ Z_BACKGROUND | ZT_THEME, Z_THEME_COLOR_MENU },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_SCROLLBAR_FADE, true },
	{ Z_DECORATE, ZS_VSCROLLBAR },
	{ Z_SELECTOR | ZT_THEME, Z_THEME_COLOR_SELECTOR },
	{ Z_PADDING | ZT_END, 0x03030303 }
};

static zStyle styles_z_menupopup[] = {
	{ Z_FBO, true },
	{ Z_DIVIDER_SIZE, 0x00000004 },
	{ Z_BACKGROUND | ZT_THEME, Z_THEME_COLOR_MENU },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_SELECTOR | ZT_THEME, Z_THEME_COLOR_SELECTOR },
	{ Z_PADDING, 0x04040404 },
	{ Z_IPADDING | ZT_END, 0x04040404 }
};

static zStyle styles_z_formfooter[] = {
	{ Z_FOREGROUND_TILES, z.R.integer.button2 },
	{ Z_ICON_SCALE, 43253 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TAP, ZS_PRESSED },
	{ Z_PADDING, 0x06020602 },
	{ Z_FOREGROUND_SCALE, 39321 },
	{ Z_ICON_GRAVITY | ZT_END, ZS_GRAVITY_CENTER | ZS_SCALE_HEIGHT }
};

static zStyle styles_z_spin_item[] = {
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_TEXT },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_LINES, 1 },
	{ Z_TEXT_SIZE, 20 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TAP, ZS_HOVERED },
	{ Z_IPADDING, 0x04040404 },
	{ Z_ICON | ZT_THEME, Z_THEME_ICONS },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_ICON_SCALE, 32767 },
	{ Z_ICON_COLOR, 0xFFFFFFFF },
	{ Z_FOREGROUND_TILES | ZT_END, z.R.integer.selectItem }
};

static zStyle styles_z_list_item[] = {
	{ Z_FBO, true },
	{ Z_TEXT_STYLE, ZS_TEXT_BOLD_ITALIC },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_LINES, 1 },
	{ Z_TEXT_SIZE, 20 },
	{ Z_IPADDING, 0x04040404 },
	{ Z_ICON | ZT_THEME, Z_THEME_ICONS },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_ICON_SCALE, 32767 },
	{ Z_ICON_COLOR, 0xFFFFFFFF },
	{ Z_FOREGROUND_TILES | ZT_END, z.R.integer.selectItem }
};

static zStyle styles_z_grid_item[] = {
	{ Z_FBO, true },
	{ Z_TEXT_SIZE, 20 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_LINES, 100 },
	{ Z_IPADDING, 0x04040404 },
	{ Z_ICON | ZT_THEME, Z_THEME_ICONS },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_ICON_SCALE, 32767 },
	{ Z_ICON_COLOR, 0xFFFFFFFF },
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
	{ Z_FOREGROUND_SCALE, 49151 },
	{ Z_TEXT_DISTANCE, 8 },
	{ Z_PADDING, 0x02020202 },
	{ Z_IPADDING | ZT_END, 0x04040404 }
};

static zStyle styles_z_edit_menu_item[] = {
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_MENU },
	{ Z_TEXT_SIZE, z.R.dimen.menuText },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_STYLE, ZS_TEXT_BOLD },
	{ Z_TEXT_LINES, 1 },
	{ Z_FOREGROUND_SCALE, 32767 },
	{ Z_PADDING, 0x03030303 },
	{ Z_IPADDING | ZT_END, 0x04040404 }
};

static zStyle styles_z_spin_capt[] = {
	{ Z_TEXT_LINES, 1 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_TILES, z.R.integer.selectCapt },
	{ Z_TEXT_SIZE, 23 },
	{ Z_IPADDING, 0x0A051605 },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_HIGHLIGHT },
	{ Z_FOREGROUND_SCALE, 49151 },
	{ Z_ICON | ZT_THEME, Z_THEME_ICONS },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_ICON_SCALE, 32767 },
	{ Z_ICON_COLOR, 0xFFFFFFFF },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_CENTER }
};

static zStyle styles_z_button[] = {
	{ Z_FBO, true },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_DISTANCE, 5 },
	{ Z_FOREGROUND_SCALE, 32767 },
	{ Z_TEXT_SHADOW_OFFS, 0x00020002 },
	{ Z_PADDING, 0x04040404 },
	{ Z_IPADDING, 0x05050505 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_END | ZS_GRAVITY_VCENTER },
	{ Z_TAP, ZS_HOVERED },
	{ Z_FOREGROUND_TILES, z.R.integer.gradientRadial },
	{ Z_ICON_SCALE | ZT_END, 42597 }
};

static zStyle styles_z_tools[] = {
	{ Z_FBO, true },
	{ Z_SIZE, 0x20002000 },
	{ Z_ICON_COLOR, 0xFFFFFFFF },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_DISTANCE, 0 },
	{ Z_FOREGROUND_SCALE, 39321 },
	{ Z_PADDING, 0x05050505 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TAP, ZS_HOVERED },
	{ Z_FOREGROUND_TILES, z.R.integer.tools },
	{ Z_ICON_SCALE | ZT_END, 32767 }
};

static zStyle styles_z_checkbox[] = {
	{ Z_FBO, true },
	{ Z_FOREGROUND_TILE1, z.R.integer.checkOff },
	{ Z_FOREGROUND_TILE2, z.R.integer.checkOn },
	{ Z_FOREGROUND_SCALE, 49151 },
	{ Z_TEXT_DISTANCE, 5 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_VCENTER | ZS_GRAVITY_START },
	{ Z_PADDING, 0x03030303 },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_VCENTER | ZS_GRAVITY_START }
};

static zStyle styles_z_radiobutton[] = {
	{ Z_FBO, true },
	{ Z_FOREGROUND_TILE1, z.R.integer.radioOff },
	{ Z_FOREGROUND_TILE2, z.R.integer.radioOn },
	{ Z_FOREGROUND_SCALE, 49151 },
	{ Z_TEXT_DISTANCE, 5 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_END | ZS_GRAVITY_VCENTER },
	{ Z_GRAVITY, ZS_GRAVITY_VCENTER | ZS_GRAVITY_END },
	{ Z_PADDING | ZT_END, 0x03030303 }
};

static zStyle styles_z_edittext[] = {
	{ Z_FBO, true },
	{ Z_FOREGROUND_TILES, z.R.integer.edit },
	{ Z_FOREGROUND_SCALE, 49151 },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_END | ZS_GRAVITY_VCENTER },
	{ Z_ICON_TILES, z.R.integer.iconEditEx },
	{ Z_ICON_SCALE, 49151 },
	{ Z_TEXT_DISTANCE, 3 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_PADDING, 0x03030303 },
	{ Z_IPADDING, 0x05050505 },
	{ Z_MODE, ZS_EDIT_TEXT },
	{ Z_TEXT_LINES, 1 },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_START | ZS_GRAVITY_VCENTER }
};

static zStyle styles_z_edittext_multi[] = {
	{ Z_STYLES, z.R.style.edittext },
	{ Z_TEXT_LINES | ZT_END, 10000 }
};

static zStyle styles_z_ribbon[] = {
	{ Z_FBO, true },
	{ Z_SELECTOR | ZT_THEME, Z_THEME_COLOR_SELECTOR },
	{ Z_SCROLLBAR_FADE, false },
	{ Z_SCROLLBAR_TILES, z.R.integer.rect },
	{ Z_DURATION, 30 },
	{ Z_DECORATE, ZS_OVERSCROLL | ZS_SCROLLBAR },
	{ Z_BACKGROUND_TILES, z.R.integer.selectDrop },
	{ Z_PADDING, 0x05050505 },
	{ Z_DIVIDER_SIZE | ZT_END, 0x03000304 }
};

static zStyle styles_z_grid[] = {
	{ Z_FBO, true },
	{ Z_SELECTOR | ZT_THEME, Z_THEME_COLOR_SELECTOR },
	{ Z_SCROLLBAR_FADE, false },
	{ Z_DURATION, 30 },
	{ Z_DECORATE, ZS_OVERSCROLL | ZS_SCROLLBAR },
	{ Z_BACKGROUND_TILES, z.R.integer.selectDrop },
	{ Z_PADDING, 0x06060606 },
	{ Z_SPACING_LINE, 6 },
	{ Z_SPACING_CELL, 6 },
	{ Z_CELL_SIZE, 160 },
	{ Z_MODE, ZS_GRID_UNIFORM },
	{ Z_LINES, 0 },
	{ Z_DIVIDER_SIZE, 0x04000404 },
	{ Z_SCROLLBAR_TILES | ZT_END, z.R.integer.rect }
};

static zStyle styles_z_spinner[] = {
	{ Z_FBO, true },
	{ Z_DIVIDER_SIZE, 0x02000204 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_BACKGROUND_TILES, z.R.integer.selectDrop },
	{ Z_SCROLLBAR_FADE, false },
	{ Z_DECORATE, ZS_VSCROLLBAR | ZS_GLOW },
	{ Z_MARGINS, 0x05050505 },
	{ Z_PADDING, 0x04040404 },
	{ Z_SELECTOR | ZT_THEME | ZT_END, Z_THEME_COLOR_SELECTOR }
};

static zStyle styles_z_slider[] = {
	{ Z_FBO, true },
	{ Z_FOREGROUND_TILE1 | ZT_THEME, Z_THEME_TILE1_SLIDER },
	{ Z_FOREGROUND_TILE2 | ZT_THEME, Z_THEME_TILE2_SLIDER },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_SCALE, 65535 },
	{ Z_TEXT_SIZE, z.R.dimen.hintText },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_PROGRESS },
	{ Z_DURATION, 40 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_IPADDING, 0x05050505 },
	{ Z_MODE | ZT_THEME, Z_THEME_MODE_SLIDER },
	{ Z_SPEED_TRACK, 131070 },
	{ Z_DECORATE | ZT_END, ZS_TIPS }
};

static zStyle styles_z_circularprogress[] = {
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_COLOR, 0xFFFFFFFF },
	{ Z_FOREGROUND_TILE1 | ZT_THEME, Z_THEME_TILES_WAIT },
	{ Z_FOREGROUND_TILE2 | ZT_THEME, Z_THEME_TILES_WAIT },
	{ Z_MODE, ZS_SLIDER_ROTATE },
	{ Z_SPEED_TRACK, 65535 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_SCALE, 131070 },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_PROGRESS },
	{ Z_TEXT_SIZE | ZT_END, z.R.dimen.hintText }
};

static zStyle styles_z_linearprogress[] = {
	{ Z_FBO, true },
	{ Z_FOREGROUND_TILE1 | ZT_THEME, Z_THEME_TILE1_PROGRESS },
	{ Z_FOREGROUND_TILE2 | ZT_THEME, Z_THEME_TILE2_PROGRESS },
	{ Z_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_PROGRESS },
	{ Z_PRIMARY | ZT_THEME, Z_THEME_COLOR_PRIMARY },
	{ Z_PADDING, 0x03030303 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_SIZE, z.R.dimen.hintText },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_PROGRESS },
	{ Z_MODE, ZS_SLIDER_NONE },
	{ Z_DECORATE, ZS_TIPS },
	{ Z_FOREGROUND_SCALE, 131070 },
	{ Z_TEXT_SHADOW_OFFS | ZT_END, 0x00020002 }
};

static zStyle styles_z_switchbutton[] = {
	{ Z_FBO, true },
	{ Z_TEXT_SIZE, z.R.dimen.textText },
	{ Z_FOREGROUND_TILE1, z.R.integer.switchTrack },
	{ Z_FOREGROUND_TILE2, z.R.integer.switchTrumb },
	{ Z_PADDING, 0x04040404 },
	{ Z_GRAVITY, ZS_GRAVITY_START | ZS_GRAVITY_VCENTER },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_END | ZS_GRAVITY_VCENTER },
	{ Z_DURATION, 30 },
	{ Z_FOREGROUND_SCALE | ZT_END, 98302 }
};

static zStyle styles_z_llinear[] = {
	{ Z_DIVIDER_TYPE, ZS_DIVIDER_BEGIN | ZS_DIVIDER_MIDDLE | ZS_DIVIDER_END },
	{ Z_DIVIDER_SIZE, 0x03000303 },
	{ Z_PADDING, 0x03030303 },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_CENTER }
};

static zStyle styles_z_tabwidget[] = {
	{ Z_ICON | ZT_THEME, Z_THEME_ICONS },
	{ Z_SIZE, 0x28002800 },
	{ Z_ICON_SCALE, 32767 },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_ICON_COLOR, 0xFFFFFFFF },
	{ Z_TEXT_SIZE, z.R.dimen.textText },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_TABBED },
	{ Z_PADDING, 0x06030603 },
	{ Z_IPADDING, 0x02020202 },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_CENTER }
};

static zStyle styles_z_tabhost[] = {
	{ Z_BACKGROUND | ZT_THEME, Z_THEME_COLOR_TABBED },
	{ Z_BACKGROUND_TILES, z.R.integer.gradientDiag },
	{ Z_SELECTOR | ZT_THEME, Z_THEME_COLOR_SELECTOR },
	{ Z_SELECTOR_TILES | ZT_END, z.R.integer.rect }
};

static zStyle styles_z_butcontroller[] = {
	{ Z_ICON | ZT_THEME, Z_THEME_ICONS },
	{ Z_ICON_COLOR, 0xFFFFFFFF },
	{ Z_ICON_SCALE, 49151 },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_SIZE, 15 },
	{ Z_TEXT_STYLE, ZS_TEXT_BOLD },
	{ Z_TEXT_FOREGROUND_COLOR, 0xFFFFFFFF },
	{ Z_TEXT_LINES, 1 },
	{ Z_VISIBLED, false },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_CENTER }
};

static zStyle styles_z_ccontroller[] = {
	{ Z_FBO, true },
	{ Z_VISIBLED, false },
	{ Z_FOREGROUND_TILE1, z.R.integer.ccontrolL },
	{ Z_FOREGROUND_TILE2, z.R.integer.ccontrolU },
	{ Z_FOREGROUND_TILE3, z.R.integer.ccontrolR },
	{ Z_FOREGROUND_TILE4, z.R.integer.ccontrolD },
	{ Z_FOREGROUND_COLOR | ZT_END, 0xA0FFFFFF }
};

static zStyle styles_z_acontroller[] = {
	{ Z_FBO, true },
	{ Z_VISIBLED, false },
	{ Z_FOREGROUND_TILE1, z.R.integer.acontrolY },
	{ Z_FOREGROUND_TILE2, z.R.integer.acontrolX },
	{ Z_FOREGROUND_TILE3, z.R.integer.acontrolA },
	{ Z_FOREGROUND_TILE4, z.R.integer.acontrolB },
	{ Z_FOREGROUND_COLOR | ZT_END, 0xA0FFFFFF }
};

static zStyle styles_z_msgform[] = {
	{ Z_FBO, true },
	{ Z_PADDING, 0x02020202 },
	{ Z_BACKGROUND_COLOR, 0xFFFFFFFF },
	{ Z_BACKGROUND_TILES | ZT_THEME, Z_THEME_TILES_FORM },
	{ Z_TEXT_SIZE, z.R.dimen.messageText },
	{ Z_TEXT_LINES, 100 },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_THEME_COLOR_TEXT_MESSAGE },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_MODE, ZS_FORM_OK | ZS_FORM_TERMINATE },
	{ Z_MASK_COLOR, 0xFF7F7F7F },
	{ Z_MASK_TILES | ZT_END, z.R.integer.rectRound }
};

static zStyle styles_z_form[] = {
	{ Z_FBO, true },
	{ Z_BACKGROUND_COLOR, 0xFFFFFFFF },
	{ Z_BACKGROUND_TILES | ZT_THEME, Z_THEME_TILES_FORM },
	{ Z_MODE, ZS_FORM_YES_NO_DEF },
	{ Z_MASK_COLOR, 0xFF7F7F7F },
	{ Z_MASK_TILES, z.R.integer.rectRound },
	{ Z_PADDING | ZT_END, 0x02020202 }
};

static zStyle styles_z_chart[] = {
	{ Z_FBO, true },
	{ Z_BACKGROUND_TILES, z.R.integer.selectDrop },
	{ Z_FOREGROUND_TILES, z.R.integer.rect },
	{ Z_FOREGROUND_COLOR, 0xFFFFFFFF },
	{ Z_DECORATE, ZS_SCROLLBAR },
	{ Z_SCROLLBAR_FADE, false },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_PADDING, 0x03030303 },
	{ Z_IPADDING | ZT_END, 0x02020202 }
};

static zStyle styles_z_chartdiagramm[] = {
	{ Z_STYLES, z.R.style.chart },
	{ Z_MODE | ZT_END, ZS_CHART_DIAGRAMM }
};

static zStyle styles_z_chartgraph[] = {
	{ Z_STYLES, z.R.style.chart },
	{ Z_MODE | ZT_END, ZS_CHART_GRAPH }
};

static zStyle styles_z_chartcircular[] = {
	{ Z_STYLES, z.R.style.chart },
	{ Z_MODE | ZT_END, ZS_CHART_CIRCULAR }
};

static zStyle styles_z_themedark[] = {
	{ Z_THEME_NAME, z.R.string.themeDark },
	{ Z_THEME_FONT, z.R.drawable.font_def },
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
	{ Z_THEME_TILE1_PROGRESS, z.R.integer.gradientSm },
	{ Z_THEME_TILE2_PROGRESS, z.R.integer.gradientSm },
	{ Z_THEME_TILE1_SLIDER, z.R.integer.sliderTrack1 },
	{ Z_THEME_TILE2_SLIDER, z.R.integer.sliderTrumb1 },
	{ Z_THEME_TILES_FORM, z.R.integer.formBackground },
	{ Z_THEME_TILES_WAIT, z.R.integer.progressWait },
	{ Z_THEME_MODE_SLIDER, ZS_SLIDER_ROTATE },
	{ Z_THEME_SIZE_TEXT_TEXT | ZT_END, z.R.dimen.textText }
};

static zStyle styles_z_themelight[] = {
	{ Z_THEME_NAME, z.R.string.themeLight },
	{ Z_THEME_FONT, z.R.drawable.font_def },
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
	{ Z_THEME_TILE1_PROGRESS, z.R.integer.gradientRadialSm },
	{ Z_THEME_TILE2_PROGRESS, z.R.integer.gradientRadialSm },
	{ Z_THEME_TILE1_SLIDER, z.R.integer.sliderTrack2 },
	{ Z_THEME_TILE2_SLIDER, z.R.integer.sliderTrumb2 },
	{ Z_THEME_TILES_FORM, z.R.integer.formBackground },
	{ Z_THEME_TILES_WAIT, z.R.integer.progressWait },
	{ Z_THEME_MODE_SLIDER, ZS_SLIDER_SCALE },
	{ Z_THEME_SIZE_TEXT_TEXT | ZT_END, z.R.dimen.textText }
};

