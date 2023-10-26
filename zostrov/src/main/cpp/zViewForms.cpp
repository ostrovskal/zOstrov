//
// Created by Sergo on 16.01.2021.
//

#include "zostrov/zCommon.h"
#include "zostrov/zViewForms.h"
#include "zostrov/zViewText.h"
#include "zostrov/zViewWidgets.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                               ФОРМА                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewForm::zViewForm(zStyle* _styles, i32 _id, zStyle* styles_header, zStyle* styles_button_footer, u32 _caption, bool _modal) :
					zLinearLayout(_styles, _id, true), caption(_caption) {
	updateStatus(ZS_SYSTEM | ZS_CLICKABLE, true); updateStatus(ZS_MODAL, _modal);
	updateStatus(ZS_VISIBLED, false);
	_styles_button_footer = styles_button_footer; _styles_header = styles_header;
}

zView* zViewForm::attach(zView *v, int width, int height, int where) {
	if(content) return setContent(v);
	return zViewGroup::attach(v, width, height, where);
}

void zViewForm::onDrawFBO() {
	// отрисовка затенения
	if(shade) shade->draw(nullptr);
	// отрисовка FBO
	drw[DRW_FBO]->draw(&rview);
}

int zViewForm::updateVisible(bool set) {
    if(set) {
		if(testFlags(ZS_MODAL)) {
			manager->changeFocus(nullptr);
			manager->getActionBar()->show(false);
		}
    }
	return zView::updateVisible(set);
}

void zViewForm::onInit(bool _theme) {
	zLinearLayout::onInit(_theme);
	if((isOpen = countChildren())) return;
	// затенение
	if(testFlags(ZS_MODAL)) {
		zParamDrawable fp{0x90000000, 0x0, 0, -1, 0, 0, 0, 1.0f};
		shade = new zDrawable(this, DRW_BK);
		shade->init(fp);
		auto screen(zGL::instance()->getSizeScreen());
		shade->measure(screen.w, screen.h, 0, false);
	}
	// заголовок
	if(_styles_header && caption) zViewGroup::attach(new zViewText(_styles_header, 0, caption), VIEW_MATCH, VIEW_WRAP);
	// тело
	content = (zFrameLayout*)attach(new zFrameLayout(styles_z_formcontentlyt, 0), VIEW_MATCH, VIEW_MATCH);
	// футер
	mode = styles->_int(Z_MODE, ZS_FORM_NONE);
	if(_styles_button_footer) {
		auto countButtons(mode & ZS_MODE_FORM_MASK);
		auto footer(new zLinearLayout(styles_z_formfooterlyt, 0, false));
		zViewGroup::attach(footer, VIEW_MATCH, VIEW_WRAP);
		// в футер поместить кнопки(OK|CANCEL|DEFAULT)
		static i32 idxParams[] = { 0, 3, 6, 12 };
		static i32 butParams[] = { 0, 0, 0,
								   z.R.id.ok, z.R.integer.iconOk, 0,
								   z.R.id.yes, z.R.integer.iconOk, 0, z.R.id.no, z.R.integer.iconCancel, 0,
								   z.R.id.yes, z.R.integer.iconOk, 1, z.R.id.def, z.R.integer.iconDef, 1, z.R.id.no,
								   z.R.integer.iconCancel, 1 };
		auto idx(idxParams[countButtons]);
		for(int i = 0; i < countButtons; i++) {
			auto but(new zViewButton(_styles_button_footer, butParams[idx], 0, butParams[idx + 1]));
			but->setOnClick([this](zView *v, int) { close(v->id); });
			footer->attach(but, 0, butParams[idx + 2], VIEW_WRAP, VIEW_WRAP);
			idx += 3;
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
		updateVisible(false);
		updateStatus(ZS_DESTROY, (mode & ZS_FORM_TERMINATE) != 0);
	}
}

zView* zViewForm::setContent(zView* view) {
    content->removeAllViews(false);
    content->attach(view, VIEW_MATCH, VIEW_MATCH);
    return view;
}

void zViewForm::stateView(STATE& state, bool save, int& index) {
	if(save) {
		state.data += (status & ZS_VISIBLED);
	} else {
		updateVisible(state.data[index++]);
	}
}

i32 zViewForm::touchEventDispatch(AInputEvent *event) {
	if(isVisibled()) {
		auto action(zTouch::getEventAction(event));
		if(action == AMOTION_EVENT_ACTION_POINTER_DOWN || action == AMOTION_EVENT_ACTION_DOWN) {
			if(testFlags(ZS_MODAL) && !zTouch::intersect(event, rview)) {
				close(z.R.id.no); return TOUCH_STOP;
			}
		}
		return zLinearLayout::touchEventDispatch(event);
	}
	return TOUCH_CONTINUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        ФОРМА СООБЩЕНИЙ                                                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewFormMessage::zViewFormMessage(zStyle* _styles, i32 _id, zStyle* _styles_caption, zStyle* _styles_footer, u32 _caption, const zString8& _message):
		zViewForm(_styles, _id, _styles_caption, _styles_footer, _caption, true) {
	setMessage(_message);
}

void zViewFormMessage::onInit(bool _theme) {
    zViewForm::onInit(_theme);
	// установить текст в контент
    ((zViewText*)attach(new zViewText(styles, 0, 0), VIEW_MATCH, VIEW_MATCH))->setText(message);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        ФОРМА ОЖИДАНИЯ                                                                  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewFormWait::zViewFormWait(int _id, int width, int height) :
				zViewForm(styles_default, _id, nullptr, nullptr, 0, false) {
	zViewForm::onInit(false);
	zViewForm::attach(new zViewProgress(styles_z_circularprogress, z.R.id.wait, 0, szi(0, 100), 0, false),
					  VIEW_MATCH, VIEW_MATCH);
	if(width && height) manager->attachForm(this, width, height, true);
}
