// Автоматическая генерация кода для макета
	auto formSettings((zViewForm*)new zViewForm(styles_z_formsettings, z.R.id.formSettings, styles_z_formcaption, styles_z_formfooter, z.R.string.Nastroyki, true));
	formSettings->onInit(false);
	auto formSettings01((zTabLayout*)formSettings->attach(new zTabLayout(styles_z_tabhost, 0, styles_z_tabwidget, ZS_GRAVITY_TOP), 
			0, 0)); 
	auto formSettings02((zTabWidget*)formSettings01->attach(new zTabWidget(styles_default, 0, 0, z.R.integer.iconZxTools), 
			VIEW_WRAP, VIEW_WRAP)); 
	auto formSettings03((zLinearLayout*)formSettings02->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_MATCH)); 
	auto formSettings04((zLinearLayout*)formSettings03->attach(new zLinearLayout(styles_z_zlinearlayout3, 0, true), 
			0, 1, VIEW_WRAP, VIEW_MATCH)); 
	formSettings04->attach(new zViewText(styles_z_zviewtext1, 0, z.R.string.CPU), 
			VIEW_MATCH, VIEW_WRAP);
	formSettings04->attach(new zViewSlider(styles_z_slider, 0, 0, szi(0, 10), 3, false), 
			VIEW_MATCH, VIEW_WRAP);
	formSettings04->attach(new zViewText(styles_z_zviewtext1, 0, z.R.string.BORDER), 
			VIEW_MATCH, VIEW_WRAP);
	formSettings04->attach(new zViewSlider(styles_z_slider, 0, 0, szi(0, 10), 3, false), 
			VIEW_MATCH, VIEW_WRAP);
	auto formSettings14((zLinearLayout*)formSettings03->attach(new zLinearLayout(styles_z_zlinearlayout4, 0, true), 
			VIEW_WRAP, VIEW_MATCH)); 
	formSettings14->attach(new zViewText(styles_z_zviewtext1, 0, z.R.string.JOYSTICK), 
			VIEW_MATCH, VIEW_WRAP);
	formSettings14->attach(new zViewSlider(styles_z_slider, 0, 0, szi(0, 10), 3, false), 
			VIEW_MATCH, VIEW_WRAP);
	formSettings14->attach(new zViewText(styles_z_zviewtext1, 0, z.R.string.KEYBOARD), 
			VIEW_MATCH, VIEW_WRAP);
	formSettings14->attach(new zViewSlider(styles_z_slider, 0, 0, szi(0, 10), 3, false), 
			VIEW_MATCH, VIEW_WRAP);
	auto formSettings12((zTabWidget*)formSettings01->attach(new zTabWidget(styles_z_ztabwidget1, 0, 0, z.R.integer.iconZxSound), 
			VIEW_WRAP, VIEW_WRAP)); 
	auto formSettings13((zLinearLayout*)formSettings12->attach(new zLinearLayout(styles_default, 0, false), 
			VIEW_MATCH, VIEW_MATCH)); 
	formSettings13->attach(new zViewSlider(styles_z_slider, 0, 0, szi(0, 50), 10, true), 
			VIEW_WRAP, VIEW_MATCH);
	formSettings13->attach(new zViewSlider(styles_z_slider, 0, 0, szi(0, 150), 120, true), 
			VIEW_WRAP, VIEW_MATCH);
	auto formSettings22((zTabWidget*)formSettings01->attach(new zTabWidget(styles_default, 0, 0, z.R.integer.iconZxGamepad), 
			VIEW_WRAP, VIEW_WRAP)); 
	auto formSettings32((zTabWidget*)formSettings01->attach(new zTabWidget(styles_default, 0, 0, z.R.integer.iconZxDisplay), 
			VIEW_WRAP, VIEW_WRAP)); 
	auto formSettings42((zTabWidget*)formSettings01->attach(new zTabWidget(styles_default, 0, 0, z.R.integer.iconZxCasette), 
			VIEW_WRAP, VIEW_WRAP)); 
	auto formSettings52((zTabWidget*)formSettings01->attach(new zTabWidget(styles_default, 0, 0, z.R.integer.iconZxDisk), 
			VIEW_WRAP, VIEW_WRAP)); 
	auto formSettings62((zTabWidget*)formSettings01->attach(new zTabWidget(styles_default, 0, 0, z.R.integer.iconZxHelp), 
			VIEW_WRAP, VIEW_WRAP)); 
