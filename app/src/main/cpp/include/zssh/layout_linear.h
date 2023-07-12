// Автоматическая генерация кода для макета
	auto grp((zScrollLayout*)root->attach(new zScrollLayout(styles_z_zscrolllayout1, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	auto grp01((zLinearLayout*)grp->attach(new zLinearLayout(styles_z_llinear, 0, true), 
			VIEW_MATCH, VIEW_WRAP)); 
	grp01->attach(new zViewProgress(styles_z_linearprogress, z.R.id.progress2, z.R.string.progress, szi(0, 170), 150, false), 
			ZS_GRAVITY_CENTER, 0, VIEW_MATCH, VIEW_WRAP);
	grp01->attach(new zViewText(styles_z_tv1, z.R.id.tv1, z.R.string.serg), 
			VIEW_MATCH, VIEW_WRAP);
	grp01->attach(new zViewCheck(styles_z_checkbox, z.R.id.spans, z.R.string.changeSpans), 
			VIEW_MATCH, VIEW_WRAP);
	auto grp02((zEditLayout*)grp01->attach(new zEditLayout(styles_default, 0), 
			VIEW_MATCH, VIEW_WRAP)); 
	grp02->attach(new zViewEdit(styles_z_edit1, z.R.id.edit1, z.R.string.prompt), 
			VIEW_MATCH, VIEW_WRAP);
	grp01->attach(new zViewSelect(styles_z_select, z.R.id.select), 
			VIEW_MATCH, VIEW_WRAP);
	grp01->attach(new zViewGrid(styles_z_grid1, z.R.id.grid1, false), 
			VIEW_WRAP, VIEW_WRAP);
	grp01->attach(new zViewEdit(styles_z_edit2, z.R.id.edit2, z.R.string.enter_family), 
			VIEW_MATCH, VIEW_WRAP);
	auto grp12((zLinearLayout*)grp01->attach(new zLinearLayout(styles_z_zlinearlayout2, 0, false), 
			VIEW_MATCH, VIEW_MATCH)); 
	grp12->attach(new zViewRadio(styles_z_radiodark, z.R.id.radioDark, z.R.string.radio_dark, 1), 
			0, 1, VIEW_WRAP, VIEW_MATCH);
	grp12->attach(new zViewCheck(styles_z_chkbox, z.R.id.chkbox, z.R.string.checkbox_text), 
			0, 2, VIEW_WRAP, VIEW_MATCH);
	grp12->attach(new zViewRadio(styles_z_radiolight, z.R.id.radioLight, z.R.string.radio_light, 1), 
			0, 1, VIEW_WRAP, VIEW_MATCH);
	grp01->attach(new zViewRibbon(styles_z_list1, z.R.id.list1, false), 
			VIEW_WRAP, 120_dp);
	grp01->attach(new zViewButton(styles_z_button1, z.R.id.button1, z.R.string.press_me, z.R.integer.iconCancel), 
			VIEW_MATCH, VIEW_WRAP);
	grp01->attach(new zViewSwitch(styles_z_switchbutton, z.R.id.switch1, z.R.string.mode_already), 
			VIEW_MATCH, VIEW_WRAP);
	grp01->attach(new zViewButton(styles_z_button2, z.R.id.button2, z.R.string.press_me, z.R.integer.iconCancel), 
			VIEW_MATCH, VIEW_WRAP);
	grp01->attach(new zViewSwitch(styles_z_switch2, z.R.id.switch2, z.R.string.switcher), 
			VIEW_MATCH, VIEW_WRAP);
	grp01->attach(new zViewSwitch(styles_z_switch3, z.R.id.switch3, z.R.string.activate), 
			VIEW_MATCH, VIEW_WRAP);
	grp01->attach(new zViewButton(styles_z_button3, z.R.id.button3, z.R.string.press_me, z.R.integer.iconCancel), 
			VIEW_MATCH, VIEW_WRAP);
	grp01->attach(new zViewSlider(styles_z_seek, z.R.id.seek, z.R.string.slider_text, szi(5, 40), 3, false), 
			VIEW_MATCH, VIEW_WRAP);
	grp01->attach(new zViewSlider(styles_z_seek1, z.R.id.seek1, 0, szi(50, 170), 130, false), 
			VIEW_MATCH, VIEW_WRAP);
	grp01->attach(new zViewButton(styles_z_button11, z.R.id.button11, z.R.string.text_on_button, z.R.integer.iconCancel), 
			VIEW_MATCH, VIEW_WRAP);
	grp01->attach(new zViewSlider(styles_z_seek2, z.R.id.seek2, 0, szi(0, 70), 30, false), 
			VIEW_MATCH, VIEW_WRAP);
	grp01->attach(new zViewButton(styles_z_button3, z.R.id.button4, z.R.string.press_me, z.R.integer.iconCancel), 
			VIEW_MATCH, VIEW_WRAP);
	grp01->attach(new zViewProgress(styles_z_linearprogress, z.R.id.progress1, z.R.string.sergey, szi(0, 70), 10, false), 
			ZS_GRAVITY_CENTER, 0, VIEW_MATCH, VIEW_WRAP);
