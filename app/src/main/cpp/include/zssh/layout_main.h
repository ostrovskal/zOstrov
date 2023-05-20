// Автоматическая генерация кода для макета
	auto grp00((zScrollLayout*)root->attach(new zScrollLayout(styles_z_zscrolllayout1, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	auto grp10((zLinearLayout*)grp00->attach(new zLinearLayout(styles_z_llinear, z.R.id.lyt1, true), 
			VIEW_MATCH, VIEW_WRAP)); 
	grp10->attach(new zViewProgress(styles_z_circularprogress, z.R.id.progress1, z.R.string.Sergey, szi(0, 70), 50, true), 
			ZS_GRAVITY_CENTER, 3, 100_dp, 100_dp);
	auto grp20((zEditLayout*)grp10->attach(new zEditLayout(styles_default, z.R.id.editlyt1), 
			VIEW_WRAP, VIEW_WRAP)); 
	grp20->attach(new zViewEdit(styles_z_edit1, z.R.id.edit1, z.R.string.enter_family), 
			0, 2, VIEW_WRAP, VIEW_WRAP);
	grp10->attach(new zViewGrid(styles_z_table, z.R.id.grid1, false), 
			0, 2, 300_dp, 400_dp);
	/* root->attach(new zViewForm(styles_z_form, z.R.id.form, styles_z_formcaption, styles_z_formfooter, z.R.string.first_form, true), 
			500_dp, 400_dp);*/
