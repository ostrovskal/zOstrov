
// Автоматически генерируемый файл ресурсов.
// Не редактируйте его.
// Автор: Шаталов С.В.
// Sun Jun  4 08:56:34 2023

#pragma once

struct _z {
	struct _R {
		struct _drawable {
			enum { 
				font1	= 0x00100000, 	font2	= 0x00100001, 	font3	= 0x00100002, 
				font4	= 0x00100003, 	znull	= 0x00100004, 	zssh	= 0x00100005, 
			};
		}; 
		_drawable drawable;
		struct _color {
			enum { 
				black	= 0xFF000000, 	silver	= 0xFFC0C0C0, 	maroon	= 0xFF000080, 
				red	= 0xFF0000FF, 	navy	= 0xFF800000, 	blue	= 0xFFFF0000, 
				purple	= 0xFF800080, 	fuchsia	= 0xFFFF00FF, 	green	= 0xFF008000, 
				lime	= 0xFF00FF00, 	olive	= 0xFF008080, 	yellow	= 0xFF00F0F0, 
				teal	= 0xFF808000, 	aqua	= 0xFFFFFF00, 	gray	= 0xFF808080, 
				white	= 0xFFFFFFFF, 	themeDark	= 0xFFFD0E4A, 	themeLight	= 0xFF0376B8, 
				selectorDark	= 0xFFA000A0, 	selectorLight	= 0xFFA0A000, 	dividerDark	= 0xFF755C5C, 
				dividerLight	= 0xFF4F4F4F, 	highlightTextDark	= 0xFFBC5A1D, 
				highlightTextLight	= 0xFF5670F1, 	messageTextDark	= 0xFFD2FA64, 
				messageTextLight	= 0xFFD2FA64, 	headerTextDark	= 0xFFCFBA41, 
				headerTextLight	= 0xFF45F1A9, 	linkTextDark	= 0xFFB8FA01, 
				linkTextLight	= 0xFFA0A0A0, 	textTextDark	= 0xFF9599F7, 	textTextLight	= 0xFFEA5191, 
				tabDark	= 0xFFA0A000, 	tabLight	= 0xFF00A0A0, 	tabTextDark	= 0xFFB0B000, 
				tabTextLight	= 0xFF007070, 	progressDark	= 0x80909090, 	progressLight	= 0x80505050, 
				progressPrimaryDark	= 0xFFFF7000, 	progressPrimaryLight	= 0xFF00FF00, 
				progressSecondaryDark	= 0xFFA03000, 	progressSecondaryLight	= 0xFF00A000, 
				progressTextDark	= 0xFFB0B0B0, 	progressTextLight	= 0xFFFFFFFF, 
				sliderTextDark	= 0xFFE0E0E0, 	sliderTextLight	= 0xFFE0E0E0, 
				menuDark	= 0xFFAF4001, 	menuLight	= 0xFF00406F, 	menuTextDark	= 0xFFFFFFFF, 
				menuTextLight	= 0xFFFFFFFF, 	hintTextDark	= 0xFF909090, 	hintTextLight	= 0xFF909090, 
			};
		}; 
		_color color;
		struct _dimen {
			enum { 
				toastMinWidth	= 75, 	toastMinHeight	= 30, 	videoMinWidth	= 120, 
				videoMinHeight	= 100, 	imageMinWidth	= 24, 	imageMinHeight	= 24, 
				actionMinWidth	= 30, 	actionMinHeight	= 30, 	textMinWidth	= 30, 
				textMinHeight	= 10, 	butMinWidth	= 50, 	butMinHeight	= 20, 
				toolsMinWidth	= 50, 	toolsMinHeight	= 50, 	checkMinWidth	= 50, 
				checkMinHeight	= 20, 	radioMinWidth	= 50, 	radioMinHeight	= 20, 
				headerMinWidth	= 50, 	headerMinHeight	= 20, 	controllerMinWidth	= 96, 
				controllerMinHeight	= 96, 	switchMinWidth	= 50, 	switchMinHeight	= 20, 
				progressMinWidth	= 70, 	progressMinHeight	= 20, 	progressMaxWidth	= 150, 
				progressMaxHeight	= 30, 	sliderMinWidth	= 70, 	sliderMinHeight	= 25, 
				sliderMaxWidth	= 150, 	sliderMaxHeight	= 35, 	selectMinWidth	= 60, 
				selectMinHeight	= 40, 	chartMinWidth	= 12, 	chartMinHeight	= 5, 
				editMinWidth	= 110, 	editMinHeight	= 30, 	divider	= 2, 	menuText	= 18, 
				hintText	= 14, 	textText	= 16, 	messageText	= 22, 	headerText	= 17, 
			};
		}; 
		_dimen dimen;
		struct _integer {
			enum { 
				ccontrol	= 0, 	ccontrolL	= 1, 	ccontrolU	= 2, 	ccontrolR	= 3, 
				ccontrolD	= 4, 	acontrol	= 5, 	acontrolY	= 6, 	acontrolX	= 7, 
				acontrolA	= 8, 	acontrolB	= 9, 	horzGlow	= 10, 	vertGlow	= 11, 
				progressWait	= 12, 	iconOk	= 13, 	iconCancel	= 14, 	iconEdit	= 15, 
				iconPopup	= 16, 	iconYes	= 17, 	iconNo	= 18, 	rect	= 19, 
				rectRound	= 20, 	gradient	= 21, 	gradientDiag	= 22, 	gradientRadial	= 23, 
				iconMenu	= 24, 	iconBullet	= 25, 	edit	= 26, 	toolsTriLimit	= 27, 
				toolsTri	= 28, 	tools	= 29, 	header	= 30, 	button1	= 31, 
				button2	= 32, 	button3	= 33, 	button4	= 34, 	switchTrack	= 35, 
				switchTrumb	= 36, 	selectDrop	= 37, 	selectItem	= 38, 	selectCapt	= 39, 
				radioOff	= 40, 	radioOn	= 41, 	checkOff	= 42, 	checkOn	= 43, 
				sliderTrumb1	= 44, 	sliderTrumb2	= 45, 	sliderTrack1	= 46, 
				sliderTrack2	= 47, 	formLight	= 48, 	formDark	= 49, 	iconDef	= 50, 
				keybButSmall	= 51, 	keybButBig	= 52, 			};
		}; 
		_integer integer;
		struct _id {
			enum { 
				ccontroller	= 0x00101000, 	acontroller	= 0x00101001, 	menuApp	= 0x00101002, 
				menuOverflow	= 0x00101003, 	ok	= 0x00101004, 	yes	= 0x00101005, 
				no	= 0x00101006, 	def	= 0x00101007, 	toast	= 0x00101008, 
				actionBar	= 0x00101009, 	wndPopup	= 0x0010100A, 	root	= 0x0010100B, 
				common	= 0x0010100C, 	keyboard	= 0x0010100D, 	edit1	= 0x0010100E, 
				button1	= 0x0010100F, 	switch1	= 0x00101010, 	seek	= 0x00101011, 
				progress1	= 0x00101012, 	edit2	= 0x00101013, 	form	= 0x00101014, 
			};
		}; 
		_id id;
		struct _string {
			enum { 
				acontrollerMap	= 0x00100006, 	ccontrollerMap	= 0x00100007, 
				themeDark	= 0x00100008, 	themeLight	= 0x00100009, 	textSpan	= 0x0010000A, 
				first_form	= 0x0010000B, 	enter_family	= 0x0010000C, 	podskazka	= 0x0010000D, 
				Perekljuchatelw	= 0x0010000E, 	slayder	= 0x0010000F, 	Sergey	= 0x00100010, 
			};
		}; 
		_string string;
		struct _array {
			enum { 
				spinArray	= 0x00100011, 			};
		}; 
		_array array;
		struct _style {
			enum {
				keyboardBase	= 0x00100000, 	keyboardAlt	= 0x00100001, 	scrollbar	= 0x00100002, 
				glow	= 0x00100003, 	caret	= 0x00100004, 	editButClear	= 0x00100005, 
				tabs	= 0x00100006, 	tabContent	= 0x00100007, 	formFooterLyt	= 0x00100008, 
				formContentLyt	= 0x00100009, 	toast	= 0x0010000A, 	bar	= 0x0010000B, 
				barButton	= 0x0010000C, 	barOverflow	= 0x0010000D, 	menuImage	= 0x0010000E, 
				barPopup	= 0x0010000F, 	formCaption	= 0x00100010, 	formFooter	= 0x00100011, 
				spin_item	= 0x00100012, 	list_item	= 0x00100013, 	menu_item	= 0x00100014, 
				spin_capt	= 0x00100015, 	button	= 0x00100016, 	tools	= 0x00100017, 
				checkbox	= 0x00100018, 	radiobutton	= 0x00100019, 	edittext	= 0x0010001A, 
				ribbon	= 0x0010001B, 	table	= 0x0010001C, 	select	= 0x0010001D, 
				msgForm	= 0x0010001E, 	slider	= 0x0010001F, 	linearProgress	= 0x00100020, 
				circularProgress	= 0x00100021, 	switchButton	= 0x00100022, 
				llinear	= 0x00100023, 	tabWidget	= 0x00100024, 	tabHost	= 0x00100025, 
				ccontroller	= 0x00100026, 	acontroller	= 0x00100027, 	form	= 0x00100028, 
				chart	= 0x00100029, 	chartDiagramm	= 0x0010002A, 	chartGraph	= 0x0010002B, 
				zlinearlayout1	= 0x0010002C, 	zlinearlayout2	= 0x0010002D, 
				edit1	= 0x0010002E, 	button1	= 0x0010002F, 	switch1	= 0x00100030, 
				seek	= 0x00100031, 	edit2	= 0x00100032, 	zcelllayout1	= 0x00100033, 
				zviewedit1	= 0x00100034, 	zviewbutton1	= 0x00100035, 	zviewswitch1	= 0x00100036, 
				zviewslider1	= 0x00100037, 	progress1	= 0x00100038, 	};
		};
		_style style;
	};
	_R R;
};

static _z z;

#include "zSystemStyles.h"

/* styles resources */
static zStyles z_styles[] = {
		{ _z::_R::_style::keyboardBase, styles_z_keyboardbase },
		{ _z::_R::_style::keyboardAlt, styles_z_keyboardalt },
		{ _z::_R::_style::scrollbar, styles_z_scrollbar },
		{ _z::_R::_style::glow, styles_z_glow },
		{ _z::_R::_style::caret, styles_z_caret },
		{ _z::_R::_style::editButClear, styles_z_editbutclear },
		{ _z::_R::_style::tabs, styles_z_tabs },
		{ _z::_R::_style::tabContent, styles_z_tabcontent },
		{ _z::_R::_style::formFooterLyt, styles_z_formfooterlyt },
		{ _z::_R::_style::formContentLyt, styles_z_formcontentlyt },
		{ _z::_R::_style::toast, styles_z_toast },
		{ _z::_R::_style::bar, styles_z_bar },
		{ _z::_R::_style::barButton, styles_z_barbutton },
		{ _z::_R::_style::barOverflow, styles_z_baroverflow },
		{ _z::_R::_style::menuImage, styles_z_menuimage },
		{ _z::_R::_style::barPopup, styles_z_barpopup },
		{ _z::_R::_style::formCaption, styles_z_formcaption },
		{ _z::_R::_style::formFooter, styles_z_formfooter },
		{ _z::_R::_style::spin_item, styles_z_spin_item },
		{ _z::_R::_style::list_item, styles_z_list_item },
		{ _z::_R::_style::menu_item, styles_z_menu_item },
		{ _z::_R::_style::spin_capt, styles_z_spin_capt },
		{ _z::_R::_style::button, styles_z_button },
		{ _z::_R::_style::tools, styles_z_tools },
		{ _z::_R::_style::checkbox, styles_z_checkbox },
		{ _z::_R::_style::radiobutton, styles_z_radiobutton },
		{ _z::_R::_style::edittext, styles_z_edittext },
		{ _z::_R::_style::ribbon, styles_z_ribbon },
		{ _z::_R::_style::table, styles_z_table },
		{ _z::_R::_style::select, styles_z_select },
		{ _z::_R::_style::msgForm, styles_z_msgform },
		{ _z::_R::_style::slider, styles_z_slider },
		{ _z::_R::_style::linearProgress, styles_z_linearprogress },
		{ _z::_R::_style::circularProgress, styles_z_circularprogress },
		{ _z::_R::_style::switchButton, styles_z_switchbutton },
		{ _z::_R::_style::llinear, styles_z_llinear },
		{ _z::_R::_style::tabWidget, styles_z_tabwidget },
		{ _z::_R::_style::tabHost, styles_z_tabhost },
		{ _z::_R::_style::ccontroller, styles_z_ccontroller },
		{ _z::_R::_style::acontroller, styles_z_acontroller },
		{ _z::_R::_style::form, styles_z_form },
		{ _z::_R::_style::chart, styles_z_chart },
		{ _z::_R::_style::chartDiagramm, styles_z_chartdiagramm },
		{ _z::_R::_style::chartGraph, styles_z_chartgraph },
		{ _z::_R::_style::zlinearlayout1, styles_z_zlinearlayout1 },
		{ _z::_R::_style::zlinearlayout2, styles_z_zlinearlayout2 },
		{ _z::_R::_style::edit1, styles_z_edit1 },
		{ _z::_R::_style::button1, styles_z_button1 },
		{ _z::_R::_style::switch1, styles_z_switch1 },
		{ _z::_R::_style::seek, styles_z_seek },
		{ _z::_R::_style::edit2, styles_z_edit2 },
		{ _z::_R::_style::zcelllayout1, styles_z_zcelllayout1 },
		{ _z::_R::_style::zviewedit1, styles_z_edit1 },
		{ _z::_R::_style::zviewbutton1, styles_z_zviewbutton1 },
		{ _z::_R::_style::zviewswitch1, styles_z_switch1 },
		{ _z::_R::_style::zviewslider1, styles_z_zviewslider1 },
		{ _z::_R::_style::progress1 | ZT_END, styles_z_progress1 }
};

/* string resources [values] */
static zResource z_resources[] = {
		{ 0, "" },
		/* drawable */
		{ _z::_R::_drawable::font1, "font1" },
		{ _z::_R::_drawable::font2, "font2" },
		{ _z::_R::_drawable::font3, "font3" },
		{ _z::_R::_drawable::font4, "font4" },
		{ _z::_R::_drawable::znull, "znull" },
		{ _z::_R::_drawable::zssh, "zssh" },
		/* string */
		{ _z::_R::_string::acontrollerMap, "acontrollerMap.xml" },
		{ _z::_R::_string::ccontrollerMap, "ccontrollerMap.xml" },
		{ _z::_R::_string::themeDark, "_dark" },
		{ _z::_R::_string::themeLight, "_light" },
		{ _z::_R::_string::textSpan, "Finish work background application after blocked screen helps saved battery." },
		{ _z::_R::_string::first_form, "First form!!!" },
		{ _z::_R::_string::enter_family, "Entering family" },
		{ _z::_R::_string::podskazka, "подсказка" },
		{ _z::_R::_string::Perekljuchatelw, "Переключатель" },
		{ _z::_R::_string::slayder, "слайдер" },
		{ _z::_R::_string::Sergey, "Sergey" },
		/* array */
		{ _z::_R::_array::spinArray | ZT_END, "Ivan|Irina|Sergey Shatalov|Maxim|Vladislav|06.06.1979|Shatalov Sergey|Olga|Viktor Alexeevish|Cat|Maria|Nina Shatalova|Alex|Alexandr" }
};

/* string resources [values_ru] */
static zResource z_resources_ru[] = {
		{ 0, "ru" },
		/* string */
		{ _z::_R::_string::textSpan, "Завершение работы фоновых приложений после блокировки экрана помогает экономить заряд. С другой стороны, в этом случае вы не будете получать новые сообщения (электронные, текстовые, из соцсетей и т.д.)." },
		{ _z::_R::_string::first_form, "Первая форма!!!" },
		{ _z::_R::_string::enter_family, "Введите фамилию и отчество" },
		/* array */
		{ _z::_R::_array::spinArray | ZT_END, "Иван|Ирина|Сергей Викторович|Максим|Владислав|06.06.1979|Шаталов Сергей|Ольга|Виктор Алексеевич|Катя|Маша|Нина Шаталова|Алекс|Саша" }
};


static zResource* z_resources_ptr_arrays[] = {z_resources, z_resources_ru, nullptr };

