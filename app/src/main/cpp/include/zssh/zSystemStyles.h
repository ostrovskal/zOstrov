
#pragma once

static zStyle styles_default[] = { { Z_PADDING | ZT_END, 0 } };

static zStyle styles_z_default_layout[] = {
	{ Z_BEHAVIOR, ZS_CLICKABLE },
	{ Z_BACKGROUND, 0xff404040 },
	{ Z_DIVIDER, 0xffffffff },
	{ Z_DIVIDER_SIZE, 33817600 },
	{ Z_DIVIDER_TYPE, ZS_DIVIDER_BEGIN | ZS_DIVIDER_MIDDLE | ZS_DIVIDER_FINISH },
	{ Z_DIVIDER_TILES, z.R.integer.rect },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_PADDING | ZT_END, 0x05050505 }
};

static zStyle styles_z_scrollbar[] = {
	{ Z_BACKGROUND, 0xffffffff },
	{ Z_BACKGROUND_TILES, z.R.integer.gradientRadial },
	{ Z_SCROLLBAR_SIZE, 4 },
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
	{ Z_TAP, ZS_HOVERED },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_CENTER }
};

static zStyle styles_z_tabs[] = {
	{ Z_PADDING | ZT_END, 0x02020202 }
};

static zStyle styles_z_tabcontent[] = {
	{ Z_PADDING | ZT_END, 0x01010101 }
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
	{ Z_BACKGROUND | ZT_THEME, Z_COLOR_MENU },
	{ Z_BACKGROUND_TILES, z.R.integer.rectRound },
	{ Z_TEXT_FONT, z.R.drawable.font1 },
	{ Z_TEXT_SIZE, z.R.dimen.textText },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_MENU_TEXT },
	{ Z_BEHAVIOR, 0 },
	{ Z_DURATION, 30 },
	{ Z_PADDING, 0x05050505 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_MARGINS | ZT_END, 0x00000003 }
};

static zStyle styles_z_keybbuttons[] = {
	{ Z_FOREGROUND, 0x603f3f3f },
	{ Z_BEHAVIOR, ZS_LCLICKABLE | ZS_FOCUSABLE },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TAP | ZT_END, ZS_HOVERED }
};

static zStyle styles_z_bar[] = {
	{ Z_BACKGROUND | ZT_THEME, Z_COLOR_MENU },
	{ Z_GRAVITY, ZS_GRAVITY_START | ZS_GRAVITY_TOP },
	{ Z_DURATION, 20 },
	{ Z_PADDING | ZT_END, 0x02020202 }
};

static zStyle styles_z_barbutton[] = {
	{ Z_SIZE, 0x20002000 },
	{ Z_FOREGROUND | ZT_THEME, Z_BITMAP_COMMON },
	{ Z_FOREGROUND_COLOR, 0xffffffff },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_PADDING | ZT_END, 0x02020202 }
};

static zStyle styles_z_baroverflow[] = {
	{ Z_SIZE, 0x20002000 },
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_COLOR, 0xffffffff },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_PADDING | ZT_END, 0x02020202 }
};

static zStyle styles_z_menuimage[] = {
	{ Z_FOREGROUND | ZT_THEME, Z_BITMAP_COMMON },
	{ Z_FOREGROUND_COLOR, 0xffffffff },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_BEHAVIOR, 0 },
	{ Z_PADDING | ZT_END, 0x02020202 }
};

static zStyle styles_z_barpopup[] = {
	{ Z_DIVIDER | ZT_THEME, Z_COLOR_DIVIDER },
	{ Z_DIVIDER_SIZE, z.R.dimen.divider },
	{ Z_DIVIDER_TYPE, ZS_DIVIDER_MIDDLE },
	{ Z_BACKGROUND | ZT_THEME, Z_COLOR_MENU },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_SCROLLBAR_FADE, true },
	{ Z_DECORATE, ZS_VSCROLLBAR },
	{ Z_SELECTOR | ZT_THEME, Z_COLOR_SELECTOR },
	{ Z_PADDING | ZT_END, 0x04020402 }
};

static zStyle styles_z_formcaption[] = {
	{ Z_SIZE, 0x32001400 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_FONT, z.R.drawable.font1 },
	{ Z_PADDING, 0x01010101 },
	{ Z_TEXT_STYLE, ZS_TEXT_ITALIC },
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_TILES, z.R.integer.header },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER | ZS_SCALE_HEIGHT },
	{ Z_FOREGROUND_SCALE, 32767 },
	{ Z_TEXT_SIZE, z.R.dimen.headerText },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME | ZT_END, Z_COLOR_HEADER_TEXT }
};

static zStyle styles_z_formfooter[] = {
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_ICON, z.R.drawable.zssh },
	{ Z_FOREGROUND_TILES, z.R.integer.button2 },
	{ Z_ICON_SCALE, 43253 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER | ZS_SCALE_HEIGHT },
	{ Z_TAP, ZS_PRESSED },
	{ Z_PADDING, 0x02020202 },
	{ Z_FOREGROUND_SCALE, 62258 },
	{ Z_ICON_COLOR, 0xefffffff },
	{ Z_ICON_GRAVITY | ZT_END, ZS_GRAVITY_CENTER | ZS_SCALE_HEIGHT }
};

static zStyle styles_z_spin_item[] = {
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_TEXT_TEXT },
	{ Z_TEXT_SIZE, z.R.dimen.menuText },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TAP, ZS_HOVERED },
	{ Z_FOREGROUND_TILES | ZT_END, z.R.integer.selectItem }
};

static zStyle styles_z_list_item[] = {
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_TEXT_TEXT },
	{ Z_TEXT_SIZE, z.R.dimen.menuText },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_TILES | ZT_END, z.R.integer.selectItem }
};

static zStyle styles_z_menu_item[] = {
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_MENU_TEXT },
	{ Z_TEXT_SIZE, z.R.dimen.menuText },
	{ Z_GRAVITY, ZS_GRAVITY_START | ZS_GRAVITY_VCENTER },
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_TILES, 0x00002B2A },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_END },
	{ Z_TEXT_DISTANCE, 8 },
	{ Z_PADDING | ZT_END, 0x02020202 }
};

static zStyle styles_z_spin_capt[] = {
	{ Z_TEXT_NOWRAP, true },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_TILES, z.R.integer.selectCapt },
	{ Z_TEXT_SIZE, z.R.dimen.textText },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_HIGHLIGHT_TEXT },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_CENTER }
};

static zStyle styles_z_button[] = {
	{ Z_ICON | ZT_THEME, Z_BITMAP_ICONS },
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_ICON_COLOR, 0xffffffff },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_DISTANCE, 3 },
	{ Z_FOREGROUND_SCALE, 49151 },
	{ Z_TEXT_FONT, z.R.drawable.font1 },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_TEXT_TEXT },
	{ Z_TEXT_SIZE, z.R.dimen.textText },
	{ Z_PADDING, 0x03030303 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_END | ZS_GRAVITY_VCENTER },
	{ Z_TAP, ZS_HOVERED },
	{ Z_FOREGROUND_TILES, z.R.integer.gradientRadial },
	{ Z_ICON_SCALE | ZT_END, 32767 }
};

static zStyle styles_z_tools[] = {
	{ Z_SIZE, 0x32003200 },
	{ Z_ICON | ZT_THEME, Z_BITMAP_ICONS },
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_ICON_COLOR, 0xffffffff },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_DISTANCE, 0 },
	{ Z_FOREGROUND_SCALE, 49151 },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_TEXT_TEXT },
	{ Z_TEXT_SIZE, z.R.dimen.textText },
	{ Z_PADDING, 0x03030303 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TAP, ZS_HOVERED },
	{ Z_FOREGROUND_TILES, z.R.integer.tools },
	{ Z_ICON_SCALE | ZT_END, 32767 }
};

static zStyle styles_z_checkbox[] = {
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_TILES, 0x00002B2A },
	{ Z_FOREGROUND_SCALE, 32767 },
	{ Z_BEHAVIOR, ZS_CLICKABLE },
	{ Z_TEXT_DISTANCE, 5 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_VCENTER | ZS_GRAVITY_START },
	{ Z_PADDING, 0x03030303 },
	{ Z_TEXT_FONT, z.R.drawable.font1 },
	{ Z_GRAVITY, ZS_GRAVITY_VCENTER | ZS_GRAVITY_START },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_TEXT_TEXT },
	{ Z_TEXT_SIZE | ZT_END, z.R.dimen.textText }
};

static zStyle styles_z_radiobutton[] = {
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_TILES, 0x00002928 },
	{ Z_TEXT_FONT, z.R.drawable.font1 },
	{ Z_FOREGROUND_SCALE, 32767 },
	{ Z_TEXT_DISTANCE, 5 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_END },
	{ Z_GRAVITY, ZS_GRAVITY_VCENTER | ZS_GRAVITY_END },
	{ Z_PADDING, 0x03030303 },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_TEXT_TEXT },
	{ Z_TEXT_SIZE | ZT_END, z.R.dimen.textText }
};

static zStyle styles_z_edittext[] = {
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_TILES, z.R.integer.edit },
	{ Z_FOREGROUND_SCALE, 49151 },
	{ Z_TEXT_DISTANCE, 0 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_PADDING, 0x05050505 },
	{ Z_MODE, ZS_EDIT_TEXT },
	{ Z_TEXT_NOWRAP, true },
	{ Z_GRAVITY, ZS_GRAVITY_START | ZS_GRAVITY_VCENTER },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_TEXT_TEXT },
	{ Z_TEXT_SIZE | ZT_END, z.R.dimen.textText }
};

static zStyle styles_z_ribbon[] = {
	{ Z_SELECTOR | ZT_THEME, Z_COLOR_SELECTOR },
	{ Z_SCROLLBAR_FADE, false },
	{ Z_DURATION, 30 },
	{ Z_DECORATE, ZS_GLOW | ZS_SCROLLBAR },
	{ Z_BACKGROUND, z.R.drawable.zssh },
	{ Z_BACKGROUND_TILES, z.R.integer.selectDrop },
	{ Z_PADDING, 0x05050505 },
	{ Z_DIVIDER | ZT_THEME, Z_COLOR_DIVIDER },
	{ Z_DIVIDER_SIZE, z.R.dimen.divider },
	{ Z_DIVIDER_TYPE, ZS_DIVIDER_MIDDLE },
	{ Z_DIVIDER_PADDING | ZT_END, 0x02020202 }
};

static zStyle styles_z_table[] = {
	{ Z_SELECTOR | ZT_THEME, Z_COLOR_SELECTOR },
	{ Z_SCROLLBAR_FADE, false },
	{ Z_DURATION, 30 },
	{ Z_DECORATE, ZS_GLOW | ZS_SCROLLBAR },
	{ Z_BACKGROUND, z.R.drawable.zssh },
	{ Z_BACKGROUND_TILES, z.R.integer.selectDrop },
	{ Z_PADDING, 0x05050505 },
	{ Z_SPACING_LINE, 0 },
	{ Z_SPACING_CELL, 0 },
	{ Z_CELL_SIZE, 90 },
	{ Z_MODE, ZS_TABLE_UNIFORM },
	{ Z_LINES, 0 },
	{ Z_DIVIDER | ZT_THEME, Z_COLOR_DIVIDER },
	{ Z_DIVIDER_SIZE, z.R.dimen.divider },
	{ Z_DIVIDER_TYPE, ZS_DIVIDER_MIDDLE },
	{ Z_DIVIDER_PADDING, 0x03030303 },
	{ Z_SCROLLBAR_TILES | ZT_END, z.R.integer.gradientRadial }
};

static zStyle styles_z_select[] = {
	{ Z_DIVIDER | ZT_THEME, Z_COLOR_DIVIDER },
	{ Z_DIVIDER_SIZE, z.R.dimen.divider },
	{ Z_DIVIDER_TYPE, ZS_DIVIDER_MIDDLE },
	{ Z_DIVIDER_PADDING, 0x02020202 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_BACKGROUND, z.R.drawable.zssh },
	{ Z_BACKGROUND_TILES, z.R.integer.selectDrop },
	{ Z_SCROLLBAR_FADE, true },
	{ Z_DECORATE, ZS_VSCROLLBAR | ZS_GLOW },
	{ Z_PADDING, 0x03030303 },
	{ Z_SELECTOR | ZT_THEME | ZT_END, Z_COLOR_SELECTOR }
};

static zStyle styles_z_msgform[] = {
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_COLOR, 0xffffffff },
	{ Z_FOREGROUND_TILES | ZT_THEME, Z_FORM_TILE },
	{ Z_TEXT_SIZE, z.R.dimen.messageText },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_MESSAGE_TEXT },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_MODE, ZS_FORM_OK },
	{ Z_PADDING | ZT_END, 0x03030303 }
};

static zStyle styles_z_slider[] = {
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_TILES | ZT_THEME, Z_SLIDER_TILES },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_FONT, z.R.drawable.font1 },
	{ Z_FOREGROUND_SCALE, 65535 },
	{ Z_TEXT_SIZE, z.R.dimen.hintText },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_SLIDER_TEXT },
	{ Z_DURATION, 40 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_PADDING, 0x03030303 },
	{ Z_MODE | ZT_THEME, Z_SLIDER_MODE },
	{ Z_SPEED_TRACK, 32767 },
	{ Z_DISPLAY | ZT_END, ZS_VISIBLED | ZS_TIPS }
};

static zStyle styles_z_linearprogress[] = {
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_TILES | ZT_THEME, Z_PROGRESS_TILES },
	{ Z_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_PROGRESS },
	{ Z_COLOR_PROGRESS_PRIMARY | ZT_THEME, Z_COLOR_PROGRESS_PRIMARY },
	{ Z_PADDING, 0x03030303 },
	{ Z_TEXT_FONT, z.R.drawable.font1 },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_PROGRESS_TEXT },
	{ Z_MODE, ZS_SLIDER_NONE },
	{ Z_DISPLAY, ZS_VISIBLED | ZS_TIPS },
	{ Z_TEXT_SIZE, z.R.dimen.hintText },
	{ Z_FOREGROUND_SCALE | ZT_END, 45874 }
};

static zStyle styles_z_circularprogress[] = {
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_COLOR, 0xffffffff },
	{ Z_TEXT_FONT, z.R.drawable.font1 },
	{ Z_FOREGROUND_TILES | ZT_THEME, Z_PROGRESS_CIRCULAR },
	{ Z_MODE, ZS_SLIDER_ROTATE },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER | ZS_SCALE_HEIGHT },
	{ Z_FOREGROUND_SCALE, 131070 },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_PROGRESS_TEXT },
	{ Z_DISPLAY, ZS_VISIBLED | ZS_TIPS },
	{ Z_TEXT_SIZE | ZT_END, z.R.dimen.hintText }
};

static zStyle styles_z_switchbutton[] = {
	{ Z_TEXT_SIZE, z.R.dimen.textText },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_TEXT_TEXT },
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_TILES, 0x00002423 },
	{ Z_PADDING, 0x05050505 },
	{ Z_TEXT_FONT, z.R.drawable.font1 },
	{ Z_GRAVITY, ZS_GRAVITY_START | ZS_GRAVITY_VCENTER },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_END | ZS_GRAVITY_VCENTER },
	{ Z_DURATION, 20 },
	{ Z_FOREGROUND_SCALE | ZT_END, 65535 }
};

static zStyle styles_z_llinear[] = {
	{ Z_DIVIDER_PADDING, 0x03030303 },
	{ Z_DIVIDER | ZT_THEME, Z_COLOR_DIVIDER },
	{ Z_DIVIDER_TYPE, ZS_DIVIDER_BEGIN | ZS_DIVIDER_MIDDLE | ZS_DIVIDER_FINISH },
	{ Z_DIVIDER_SIZE, z.R.dimen.divider },
	{ Z_PADDING, 0x05050505 },
	{ Z_GRAVITY | ZT_END, ZS_GRAVITY_HCENTER }
};

static zStyle styles_z_tabwidget[] = {
	{ Z_FOREGROUND | ZT_THEME, Z_BITMAP_ICONS },
	{ Z_ICON_SCALE, 49151 },
	{ Z_ICON_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_SIZE, z.R.dimen.textText },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_TAB_TEXT },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_PADDING | ZT_END, 0x02020202 }
};

static zStyle styles_z_tabhost[] = {
	{ Z_BACKGROUND | ZT_THEME, Z_COLOR_TAB },
	{ Z_BACKGROUND_TILES, z.R.integer.gradientDiag },
	{ Z_SELECTOR | ZT_THEME, Z_COLOR_SELECTOR },
	{ Z_SELECTOR_TILES, z.R.integer.gradientDiag },
	{ Z_DIVIDER | ZT_THEME, Z_COLOR_DIVIDER },
	{ Z_DIVIDER_SIZE, z.R.dimen.divider },
	{ Z_DIVIDER_TYPE | ZT_END, ZS_DIVIDER_MIDDLE }
};

static zStyle styles_z_ccontroller[] = {
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_GRAVITY, ZS_GRAVITY_START | ZS_GRAVITY_BOTTOM },
	{ Z_FOREGROUND_TILES, 0x04030102 },
	{ Z_FOREGROUND_COLOR | ZT_END, 0xffffffff }
};

static zStyle styles_z_acontroller[] = {
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_GRAVITY, ZS_GRAVITY_END | ZS_GRAVITY_BOTTOM },
	{ Z_FOREGROUND_TILES, 0x09070806 },
	{ Z_FOREGROUND_COLOR | ZT_END, 0xffffffff }
};

static zStyle styles_z_form[] = {
	{ Z_BACKGROUND, z.R.drawable.zssh },
	{ Z_BACKGROUND_COLOR, 0xffffffff },
	{ Z_BACKGROUND_TILES | ZT_THEME, Z_FORM_TILE },
	{ Z_MODE | ZT_END, ZS_FORM_YES_NO_DEF }
};

static zStyle styles_z_chart[] = {
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

static zStyle styles_z_zscrolllayout1[] = {
	{ Z_BACKGROUND, 0xff804000 },
	{ Z_DECORATE, ZS_VSCROLLBAR | ZS_GLOW },
	{ Z_PADDING | ZT_END, 0x05050505 }
};

static zStyle styles_z_edit1[] = {
	{ Z_STYLES, z.R.style.edittext },
	{ Z_MODE | ZT_END, ZS_EDIT_TEXT }
};

static zStyle styles_z_zcelllayout1[] = {
	{ Z_GRAVITY, ZS_GRAVITY_START },
	{ Z_PADDING | ZT_END, 0x01010101 }
};

static zStyle styles_z_list1[] = {
	{ Z_BACKGROUND | ZT_THEME, Z_BITMAP_COMMON },
	{ Z_PADDING, 0x0A0A0A0A },
	{ Z_BACKGROUND_TILES, z.R.integer.selectDrop },
	{ Z_SELECTOR_COLOR | ZT_END, 0xff00ffff }
};

static zStyle styles_z_zviewedit1[] = {
	{ Z_MODE, ZS_EDIT_TEXT },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_TEXT_TEXT },
	{ Z_TEXT_SIZE, 30 },
	{ Z_PADDING, 0x0A0A0A0A },
	{ Z_GRAVITY, ZS_GRAVITY_VCENTER },
	{ Z_FOREGROUND | ZT_THEME, Z_BITMAP_COMMON },
	{ Z_FOREGROUND_TILES, z.R.integer.edit },
	{ Z_TEXT_FONT | ZT_THEME, Z_FONT1 },
	{ Z_TEXT_NOWRAP | ZT_END, true }
};

static zStyle styles_z_button1[] = {
	{ Z_FOREGROUND_TILES, z.R.integer.gradientRadial },
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_FONT | ZT_THEME, Z_FONT1 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_SCALE, 65535 },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_TEXT_TEXT },
	{ Z_TEXT_SIZE, 40 },
	{ Z_PADDING, 0x05050505 },
	{ Z_TAP | ZT_END, ZS_HOVERED }
};

static zStyle styles_z_switch1[] = {
	{ Z_TEXT_SIZE, 40 },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_TEXT_TEXT },
	{ Z_FOREGROUND | ZT_THEME, Z_BITMAP_COMMON },
	{ Z_FOREGROUND_TILES, 0x00002324 },
	{ Z_PADDING, 0x05050505 },
	{ Z_GRAVITY, ZS_GRAVITY_START | ZS_GRAVITY_VCENTER },
	{ Z_FOREGROUND_GRAVITY, ZS_GRAVITY_END },
	{ Z_DURATION, 10 },
	{ Z_TEXT_FONT | ZT_THEME, Z_FONT2 },
	{ Z_FOREGROUND_SCALE | ZT_END, 114686 }
};

static zStyle styles_z_seek[] = {
	{ Z_FOREGROUND | ZT_THEME, Z_BITMAP_COMMON },
	{ Z_TEXT_SIZE, 40 },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_SLIDER_TEXT },
	{ Z_FOREGROUND_TILES, 0x00002E00 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_FOREGROUND_SCALE, 65535 },
	{ Z_MODE | ZT_THEME, Z_SLIDER_MODE },
	{ Z_BACKGROUND_TILES, z.R.integer.header },
	{ Z_BACKGROUND | ZT_THEME, Z_BITMAP_COMMON },
	{ Z_DURATION, 40 },
	{ Z_TEXT_FONT | ZT_THEME, Z_FONT2 },
	{ Z_PADDING | ZT_END, 0x01010101 }
};

static zStyle styles_z_progress1[] = {
	{ Z_FOREGROUND, z.R.drawable.zssh },
	{ Z_FOREGROUND_TILES, 0x0014140C },
	{ Z_FOREGROUND_COLOR, 0xffffffff },
	{ Z_COLOR_PROGRESS_PRIMARY | ZT_THEME, Z_COLOR_PROGRESS_PRIMARY },
	{ Z_TEXT_FONT | ZT_THEME, Z_FONT2 },
	{ Z_TEXT_FOREGROUND_COLOR | ZT_THEME, Z_COLOR_PROGRESS_TEXT },
	{ Z_TEXT_SIZE, 20 },
	{ Z_GRAVITY, ZS_GRAVITY_CENTER },
	{ Z_TEXT_SHADOW_OFFS, 0x0000 },
	{ Z_PADDING, 0x01020304 },
	{ Z_MODE, ZS_SLIDER_ROTATE },
	{ Z_DISPLAY, ZS_VISIBLED | ZS_TIPS },
	{ Z_FOREGROUND_SCALE | ZT_END, 163837 }
};

