// Автоматическая генерация кода для макета
	auto grp((zLinearLayout*)root->attach(new zLinearLayout(styles_z_zlinearlayout1, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	auto grp01((zLinearLayout*)grp->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	grp01->attach(new zViewButton(styles_z_button, z.R.id.bkg, z.R.string.background, -1), 
			VIEW_MATCH, VIEW_WRAP);
	grp01->attach(new zViewButton(styles_z_button, z.R.id.menu, z.R.string.menu, -1), 
			VIEW_MATCH, VIEW_WRAP);
	auto grp11((zLinearLayout*)grp->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	grp11->attach(new zViewButton(styles_z_button, z.R.id.dth, z.R.string.droid_death, -1), 
			VIEW_MATCH, VIEW_WRAP);
	grp11->attach(new zViewButton(styles_z_button, z.R.id.bmb, z.R.string.drop_bomb, -1), 
			VIEW_MATCH, VIEW_WRAP);
	auto grp21((zLinearLayout*)grp->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	grp21->attach(new zViewButton(styles_z_button, z.R.id.exp, z.R.string.explosive, -1), 
			VIEW_MATCH, VIEW_WRAP);
	grp21->attach(new zViewButton(styles_z_button, z.R.id.tm, z.R.string.time_elapsed, -1), 
			VIEW_MATCH, VIEW_WRAP);
	grp->attach(new zViewSlider(styles_z_slider, 0, 0, szi(0, 10), 10, false), 
			VIEW_MATCH, VIEW_WRAP);
	grp->attach(new zViewChart(styles_z_chartdiagramm, z.R.id.chart2, ZS_GRAVITY_BOTTOM), 
			VIEW_MATCH, VIEW_WRAP);
	grp->attach(new zViewChart(styles_z_chartdiagramm, z.R.id.chart1, ZS_GRAVITY_END), 
			VIEW_MATCH, VIEW_MATCH);
