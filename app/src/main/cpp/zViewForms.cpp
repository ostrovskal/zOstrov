//
// Created by Sergo on 16.01.2021.
//

#include "zssh/zCommon.h"
#include "zssh/zViewForms.h"
#include "zssh/zViewText.h"
#include "zssh/zViewWidgets.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                               ФОРМА                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewForm::zViewForm(zStyle* _styles, i32 _id, zStyle* styles_header, zStyle* styles_button_footer, u32 _caption, bool _modal) :
					zLinearLayout(_styles, _id, true), caption(_caption) {
	updateStatus(ZS_SYSTEM, true); updateStatus(ZS_MODAL, _modal);
	_styles_button_footer = styles_button_footer; _styles_header = styles_header;
}

void zViewForm::onDrawFBO() {
	// отрисовка затенения
	if(testFlags(ZS_MODAL)) drw[DRW_FK]->draw(nullptr);
	// отрисовка FBO
	drw[DRW_FBO]->draw(&rview);
}

void zViewForm::onInit(bool _theme) {
	zLinearLayout::onInit(_theme);
	// затенение
	zParamDrawable fp(0x90000000, 0, -1, 0, 1.0f);
	setDrawable(testFlags(ZS_MODAL) ? &fp : nullptr, DRW_FK);
	auto screen(zGL::instance()->getSizeScreen());
	drw[DRW_FK]->measure(screen.w, screen.h, 0, false);
	// инициализация
	if(_styles_header && caption) {
		// заголовок
		attach(new zViewText(_styles_header, 0, caption), 0, 1, VIEW_MATCH, VIEW_WRAP);
	}
	// тело
	content = new zFrameLayout(styles_z_formcontentlyt, 0);
	attach(content, 0, 5, VIEW_MATCH, VIEW_WRAP);
	// футер
	mode = styles->_int(Z_MODE, ZS_FORM_NONE);
	if(_styles_button_footer) {
		auto countButtons(mode & ZS_MODE_FORM_MASK);
		auto footer(new zLinearLayout(styles_z_formfooterlyt, 0, false));
		attach(footer, 0, 1, VIEW_MATCH, VIEW_WRAP);
		// в футер поместить кнопки(OK|CANCEL|DEFAULT)
		static i32 idxParams[] = { 0, 2, 4, 8 };
		static i32 butParams[] = { 0, 0,
								   z.R.id.ok, z.R.integer.iconOk,
								   z.R.id.yes, z.R.integer.iconYes,
								   z.R.id.no, z.R.integer.iconNo,
								   z.R.id.yes, z.R.integer.iconYes,
								   z.R.id.def, z.R.integer.iconDef,
								   z.R.id.no, z.R.integer.iconNo };
		auto idx(idxParams[countButtons]);
		for(int i = 0; i < countButtons; i++) {
			auto but(new zViewButton(_styles_button_footer, butParams[idx], 0, butParams[idx + 1]));
			but->setOnClick([this](zView *v, int) { close(v->id); });
			footer->attach(but, 0, 1, VIEW_WRAP, VIEW_WRAP);
			idx += 2;
		}
	}
}

i32 zViewForm::keyEvent(int key, bool sysKey) {
	if(sysKey && key == AKEYCODE_BACK) close(z.R.id.no);
	return 1;
}

void zViewForm::close(int code) {
	auto ok(1);
	if(onClose) ok = onClose(this, code);
	if(ok) {
		updateStatus(ZS_VISIBLED, false);
		updateStatus(ZS_DESTROY, (mode & ZS_FORM_TERMINATE) != 0);
	}
}

zView* zViewForm::setContent(zView* view) {
    content->removeAllViews();
    content->attach(view, VIEW_MATCH, VIEW_MATCH);
    return view;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        ФОРМА СООБЩЕНИЙ                                                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewFormMessage::zViewFormMessage(zStyle* _styles, i32 _id, zStyle* _styles_caption, zStyle* _styles_footer, u32 _caption, u32 _message, bool _modal):
		zViewForm(_styles, _id, _styles_caption, _styles_footer, _caption, _modal), message(_message) {
}

void zViewFormMessage::onInit(bool _theme) {
    zViewForm::onInit(_theme);
	// установить текст в контент
	content->attach(new zViewText(styles, 0, message), VIEW_MATCH, VIEW_MATCH);
}
