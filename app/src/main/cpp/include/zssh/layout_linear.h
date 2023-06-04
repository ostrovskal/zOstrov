// Автоматическая генерация кода для макета
	auto grp00((zScrollLayout*)root->attach(new zScrollLayout(styles_z_zscrolllayout1, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	auto grp10((zLinearLayout*)grp00->attach(new zLinearLayout(styles_z_llinear, 0, true), 
			VIEW_MATCH, VIEW_WRAP)); 
	grp10->attach(new zViewEdit(styles_z_zviewedit1, z.R.id.edit1, z.R.string.podskazka), 
			VIEW_MATCH, VIEW_WRAP);
	auto grp20((zLinearLayout*)grp10->attach(new zLinearLayout(styles_z_zlinearlayout1, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	grp20->attach(new zViewRadio(styles_z_radiobutton, 0, z.R.string.radio_1, 1), 
			VIEW_WRAP, VIEW_MATCH);
	grp20->attach(new zViewRadio(styles_z_zviewradio1, 0, z.R.string.radio_2, 1), 
			VIEW_WRAP, VIEW_MATCH);
	grp20->attach(new zViewRadio(styles_z_radiobutton, 0, z.R.string.radio_3, 1), 
			VIEW_WRAP, VIEW_MATCH);
	grp20->attach(new zViewCheck(styles_z_zviewcheck1, 0, z.R.string.Flagzok), 
			VIEW_WRAP, VIEW_MATCH);
	grp10->attach(new zViewButton(styles_z_zviewbutton1, z.R.id.button1, z.R.string.Nagzmi_na_menja__, z.R.integer.iconCancel), 
			VIEW_MATCH, VIEW_WRAP);
	grp10->attach(new zViewSwitch(styles_z_zviewswitch1, z.R.id.switch1, z.R.string.Regzim_gotovnosti), 
			VIEW_MATCH, VIEW_WRAP);
	grp10->attach(new zViewSwitch(styles_z_switch2, z.R.id.switch2, z.R.string.Perekljuchatelw), 
			VIEW_MATCH, VIEW_WRAP);
	grp10->attach(new zViewSwitch(styles_z_switch3, z.R.id.switch3, z.R.string.Aktivacija), 
			VIEW_MATCH, VIEW_WRAP);
	grp10->attach(new zViewSlider(styles_z_zviewslider1, z.R.id.seek, z.R.string.slayder, szi(5, 40), 3, false), 
			VIEW_MATCH, VIEW_WRAP);
	grp10->attach(new zViewSlider(styles_z_seek1, z.R.id.seek1, 0, szi(50, 170), 130, false), 
			VIEW_MATCH, VIEW_WRAP);
	grp10->attach(new zViewButton(styles_z_button11, z.R.id.button11, z.R.string.Tekst_na_knopke, z.R.integer.iconCancel), 
			VIEW_MATCH, VIEW_WRAP);
	grp10->attach(new zViewSlider(styles_z_seek2, z.R.id.seek2, 0, szi(0, 70), 30, false), 
			VIEW_MATCH, VIEW_WRAP);
	grp10->attach(new zViewProgress(styles_z_linearprogress, z.R.id.progress1, z.R.string.Sergey, szi(0, 170), 250, false), 
			ZS_GRAVITY_CENTER, 0, VIEW_MATCH, VIEW_WRAP);
	grp10->attach(new zViewEdit(styles_z_edit2, z.R.id.edit2, z.R.string.enter_family), 
			VIEW_MATCH, VIEW_WRAP);
	/* auto grp21((zViewForm*)root->attach(new zViewForm(styles_z_form, z.R.id.form, styles_z_formcaption, styles_z_formfooter, z.R.string.first_form, false), 
			500_dp, 400_dp)); 
*/
