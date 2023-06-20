// Автоматическая генерация кода для макета
	auto formBrowser((zViewForm*)new zViewForm(styles_z_formopen, z.R.id.formOpen, styles_z_formcaption, styles_z_formfooter, z.R.string.Upravlenie_faylami, true));
formBrowser->onInit(false);
	auto formBrowser01((zLinearLayout*)formBrowser->attach(new zLinearLayout(styles_z_zlinearlayout1, 0, false), 
			VIEW_MATCH, VIEW_MATCH)); 
	auto formBrowser02((zLinearLayout*)formBrowser01->attach(new zLinearLayout(styles_default, 0, true), 
			VIEW_MATCH, VIEW_MATCH)); 
	formBrowser02->attach(new zViewSelect(styles_z_select, z.R.id.choiceSource), 
			VIEW_MATCH, VIEW_WRAP);
	formBrowser02->attach(new zViewRibbon(styles_z_ribbon, z.R.id.catalog, true), 
			VIEW_MATCH, VIEW_MATCH);
	auto formBrowser12((zLinearLayout*)formBrowser01->attach(new zLinearLayout(styles_default, 0, true), 
			VIEW_WRAP, VIEW_MATCH)); 
	auto formBrowser03((zLinearLayout*)formBrowser12->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_WRAP, VIEW_WRAP)); 
	formBrowser03->attach(new zViewButton(styles_z_zviewbutton1, 0, 0, z.R.integer.iconZxDEL), 
			VIEW_WRAP, VIEW_WRAP);
	formBrowser03->attach(new zViewButton(styles_z_zviewbutton1, 0, 0, z.R.integer.iconZxEject), 
			VIEW_WRAP, VIEW_WRAP);
	auto formBrowser13((zLinearLayout*)formBrowser12->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	formBrowser13->attach(new zViewButton(styles_z_zviewbutton1, 0, 0, z.R.integer.iconZxOpen), 
			VIEW_WRAP, VIEW_WRAP);
	formBrowser13->attach(new zViewButton(styles_z_zviewbutton1, 0, 0, z.R.integer.iconZxSave), 
			VIEW_WRAP, VIEW_WRAP);
	auto formBrowser23((zLinearLayout*)formBrowser12->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_WRAP)); 
	formBrowser23->attach(new zViewButton(styles_z_zviewbutton1, 0, 0, z.R.integer.iconZxTrDos), 
			VIEW_WRAP, VIEW_WRAP);
	formBrowser23->attach(new zViewButton(styles_z_tools, 0, 0, z.R.integer.iconCancel), 
			VIEW_WRAP, VIEW_WRAP);
	formBrowser12->attach(new zViewCheck(styles_z_checkbox, z.R.id.chkReadOnly, z.R.string.Read_only), 
			VIEW_WRAP, VIEW_WRAP);
