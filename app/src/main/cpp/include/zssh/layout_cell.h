// Автоматическая генерация кода для макета
	auto grp((zCellLayout*)root->attach(new zCellLayout(styles_z_zcelllayout1, 0, szi(2, 8), 1_dp), 
			VIEW_MATCH, VIEW_MATCH)); 
	grp->attach(new zViewRibbon(styles_z_list1, z.R.id.list1, true), 
			2, 3);
	grp->attach(new zViewEdit(styles_z_edit1, z.R.id.edit1, z.R.string.podskazka), 
			0, 3, 2, 1);
	grp->attach(new zViewButton(styles_z_button1, z.R.id.button1, z.R.string.themeDark, z.R.integer.iconCancel), 
			0, 4, 2, 1);
	grp->attach(new zViewSwitch(styles_z_switch1, z.R.id.switch1, z.R.string.Perekljuchatelw), 
			0, 5, 2, 1);
	grp->attach(new zViewSlider(styles_z_seek, z.R.id.seek, 0, szi(0, 70), 0, false), 
			0, 6, 2, 1);
	grp->attach(new zViewProgress(styles_z_progress1, z.R.id.progress1, 0, szi(0, 70), 50, false), 
			0, 7, 2, 1);
