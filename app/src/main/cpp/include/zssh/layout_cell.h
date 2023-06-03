// Автоматическая генерация кода для макета
	auto grp00((zCellLayout*)root->attach(new zCellLayout(styles_z_zcelllayout1, 0, szi(0, 8), 1_dp), 
			VIEW_MATCH, VIEW_MATCH)); 
	grp00->attach(new zViewEdit(styles_z_zviewedit1, z.R.id.edit1, z.R.string.podskazka), 
			0, 3, 2, 1);
	grp00->attach(new zViewButton(styles_z_zviewbutton1, z.R.id.button1, z.R.string.themeDark, z.R.integer.iconCancel), 
			0, 4, 2, 1);
	grp00->attach(new zViewSwitch(styles_z_zviewswitch1, z.R.id.switch1, z.R.string.Perekljuchatelw), 
			0, 5, 2, 1);
	grp00->attach(new zViewSlider(styles_z_zviewslider1, z.R.id.seek, 0, szi(0, 70), 0, false), 
			0, 6, 2, 1);
	grp00->attach(new zViewProgress(styles_z_progress1, z.R.id.progress1, 0, szi(0, 70), 50, false), 
			0, 7, 2, 1);
