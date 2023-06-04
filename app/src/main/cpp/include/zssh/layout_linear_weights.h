// Автоматическая генерация кода для макета
	auto grp00((zLinearLayout*)root->attach(new zLinearLayout(styles_z_zlinearlayout1, 0, false), 
			VIEW_MATCH, VIEW_MATCH)); 
	auto grp10((zLinearLayout*)grp00->attach(new zLinearLayout(styles_z_zlinearlayout3, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	grp10->attach(new zViewEdit(styles_z_zviewedit1, z.R.id.edit1, z.R.string.podskazka), 
			0, 1, VIEW_MATCH, VIEW_WRAP);
	grp10->attach(new zViewButton(styles_z_zviewbutton2, z.R.id.button1, z.R.string.themeDark, z.R.integer.iconCancel), 
			0, 1, VIEW_MATCH, VIEW_WRAP);
	grp10->attach(new zViewSwitch(styles_z_switch1, z.R.id.switch1, z.R.string.Perekljuchatelw), 
			0, 1, VIEW_MATCH, VIEW_WRAP);
	grp10->attach(new zViewSwitch(styles_z_switch1, z.R.id.switch2, z.R.string.Perekljuchatelw), 
			0, 1, VIEW_MATCH, VIEW_WRAP);
	grp10->attach(new zViewSwitch(styles_z_switch1, z.R.id.switch3, z.R.string.Perekljuchatelw), 
			0, 1, VIEW_MATCH, VIEW_WRAP);
	grp10->attach(new zViewSlider(styles_z_zviewslider2, z.R.id.seek, z.R.string.slayder, szi(0, 70), 30, false), 
			0, 1, VIEW_MATCH, VIEW_WRAP);
	grp10->attach(new zViewSlider(styles_z_zviewslider3, z.R.id.seek1, z.R.string.slayder, szi(0, 70), 30, false), 
			0, 1, VIEW_MATCH, VIEW_WRAP);
	grp10->attach(new zViewSlider(styles_z_zviewslider4, z.R.id.seek2, z.R.string.slayder, szi(0, 70), 30, false), 
			0, 1, VIEW_MATCH, VIEW_WRAP);
	grp10->attach(new zViewProgress(styles_z_linearprogress, z.R.id.progress1, z.R.string.Sergey, szi(0, 170), 250, false), 
			ZS_GRAVITY_CENTER, 1, VIEW_MATCH, VIEW_WRAP);
	grp10->attach(new zViewEdit(styles_z_edit2, z.R.id.edit2, z.R.string.enter_family), 
			0, 1, VIEW_MATCH, VIEW_WRAP);
	/* auto grp20((zViewForm*)root->attach(new zViewForm(styles_z_form, z.R.id.form, styles_z_formcaption, styles_z_formfooter, z.R.string.first_form, false), 
			500_dp, 400_dp)); 
*/
