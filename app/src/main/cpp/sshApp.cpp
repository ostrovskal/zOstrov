
#include "sshCommon.h"
#include "zssh/zViewManager.h"
#include "zssh/zViewWidgets.h"

sshApp* theApp(nullptr);

void sshApp::processEvents(i32 event) {
    switch(event) {
        case APP_CMD_INIT_WINDOW:
        case APP_CMD_TERM_WINDOW:
            appActivate(event == APP_CMD_INIT_WINDOW, android->window);
            break;
        case APP_CMD_SAVE_STATE:
            stateAllViews(Z_SAVE, (u8**)&android->savedState, (u32*)&android->savedStateSize);
            break;
        case APP_CMD_GAINED_FOCUS:
        case APP_CMD_LOST_FOCUS:
            focusNativeWindow(event == APP_CMD_GAINED_FOCUS);
            break;
        case APP_CMD_PAUSE:
            DLOG("APP_CMD_PAUSE");
            //android->activity->vm->DetachCurrentThread();
            break;
        case APP_CMD_RESUME:
            DLOG("APP_CMD_RESUME");
            stateAllViews(Z_RESUME, (u8**)&android->savedState, (u32*)&android->savedStateSize);
            zJniHelper::instance()->hideNavigationPanel();
            break;
        case APP_CMD_WINDOW_REDRAW_NEEDED:
            redrawNativeWindow();
            break;
        case APP_CMD_WINDOW_RESIZED:
            updateNativeWindow(android->config, theme->styles ? theme->styles : themeLight, theme->user_resources, theme->user_styles);
            break;
    }
}

static void callback_appEvent(android_app*, i32 cmd) {
    theApp->processEvents(cmd);
}

static i32 callback_inputEvent(android_app*, AInputEvent *event) {
    return theApp->processInputEvens(event);
}

void android_main(android_app* android) {
    ANativeActivity_setWindowFlags(android->activity, AWINDOW_FLAG_FULLSCREEN | AWINDOW_FLAG_KEEP_SCREEN_ON, 0x00800000);
    sshApp app(android);
    theApp->run();
}

sshApp::sshApp(android_app* _android) : zViewManager(_android->activity, 8) {
    theApp                = this;
    android               = _android;
    android->userData     = this;
    android->onAppCmd     = callback_appEvent;
    android->onInputEvent = callback_inputEvent;
    zJniHelper::init(android->activity, nullptr);
}

void sshApp::run() {
    while(true) {
        i32 events, sens;
        android_poll_source* source;
        while((sens = ALooper_pollAll((int)isActive() - 1, nullptr, &events, (void**)&source)) >= 0) {
            if(source) source->process(android, source);
            if(android->destroyRequested) return;
        }
        if(isActive()) drawViews();
    }
}

static zStyle styles_z_scrolllayout[] = {
        { Z_BEHAVIOR, ZS_CLICKABLE },
        //{ Z_BACKGROUND, 0xff000040 },
        { Z_DECORATE, ZS_SCROLLBAR | ZS_GLOW },
        { Z_GRAVITY, ZS_GRAVITY_START },
        { Z_PADDING | ZT_END, 0x08080808 }
};

void sshApp::setContent() {
    //debug = true;
  //  return;
    auto scl = new zScrollLayout(styles_z_scrolllayout, 0, false);
    auto ll = new zLinearLayout(styles_z_llinear, 0, false);
    auto chk = new zViewCheck(styles_z_checkbox, 1, z.R.string.enter_family);
    auto vw = new zViewButton(styles_z_button, 22, z.R.string.textSpan, z.R.integer.iconCancel);
    auto sl = new zViewSlider(styles_z_slider, 1, 0, szi(10, 32), 16, true);
    auto sl1 = new zViewSlider(styles_z_slider, 1, 0, szi(10, 32), 16, false);
    auto prg = new zViewProgress(styles_z_linearprogress, 1, 0, szi(0, 48), 16, true);
    auto edt1 = new zViewEdit(styles_z_edittext, 112, z.R.string.Sergey);
    auto edt2 = new zViewEdit(styles_z_edittext, 111, z.R.string.Sergey);
    auto edt3 = new zViewEdit(styles_z_edittext, 110, z.R.string.Sergey);
/*
    auto scl1 = new zScrollLayout(styles_z_scrolllayout, 0, false);
    auto ll1 = new zLinearLayout(styles_z_default_layout, 0, false);
    auto img = new zViewImage(styles_z_text, 0, z.R.integer.button3);
*/
    root->attach(scl, VIEW_MATCH, VIEW_MATCH);
    //scl1->attach(scl, VIEW_MATCH, VIEW_MATCH);
    //scl1->attach(ll1, VIEW_MATCH, VIEW_MATCH);
    scl->attach(ll, VIEW_WRAP, VIEW_WRAP);
    //ll1->attach(ll, VIEW_MATCH, VIEW_MATCH);
    ll->attach(prg, ZS_GRAVITY_CENTER, 0, VIEW_WRAP, VIEW_MATCH );
    ll->attach(edt3, 350, VIEW_WRAP)->updateStatus(ZS_NOWRAP, true);
    ll->attach(sl, ZS_GRAVITY_CENTER, 0, VIEW_WRAP, VIEW_MATCH );
    ll->attach(sl1, ZS_GRAVITY_CENTER, 0, 450, VIEW_WRAP );
    ll->attach(chk, VIEW_WRAP, VIEW_WRAP );
    //ll->attach(new zViewSwitch(styles_z_switchbutton, 2, z.R.string.enter_family), VIEW_MATCH, VIEW_WRAP );
    int vv = VIEW_WRAP;
    ll->attach(new zViewRadio(styles_z_radiobutton, 1, z.R.string.textSpan, 10), 350, VIEW_WRAP )->
                setForegroundGravity(ZS_GRAVITY_START | ZS_GRAVITY_VCENTER)->setGravity(ZS_GRAVITY_START | ZS_GRAVITY_VCENTER);
    ll->attach(new zViewSwitch(styles_z_switchbutton, 2, z.R.string.Sergey), VIEW_WRAP, VIEW_WRAP );
    ll->attach(new zViewRadio(styles_z_radiobutton, 1, z.R.string.first_form, 10), vv, VIEW_WRAP )->
            setForegroundGravity(ZS_GRAVITY_START | ZS_GRAVITY_VCENTER)->setGravity(ZS_GRAVITY_END | ZS_GRAVITY_VCENTER);
    ll->attach(new zViewRadio(styles_z_radiobutton, 1, z.R.string.first_form, 10), vv, VIEW_WRAP )->
            setForegroundGravity(ZS_GRAVITY_END | ZS_GRAVITY_VCENTER)->setGravity(ZS_GRAVITY_START | ZS_GRAVITY_VCENTER);
    ll->attach(new zViewRadio(styles_z_radiobutton, 1, z.R.string.first_form, 10), ZS_GRAVITY_TOP, 0, vv, VIEW_WRAP )->
            setForegroundGravity(ZS_GRAVITY_END | ZS_GRAVITY_VCENTER)->setGravity(ZS_GRAVITY_HCENTER | ZS_GRAVITY_VCENTER);
    ll->attach(vw, VIEW_WRAP, VIEW_WRAP)->setOnClick([this, ll, scl](zView* v, bool) {
        scl->setOrientation(!scl->isVertical());
        ll->setOrientation(!ll->isVertical());
        setTheme(theme->styles == themeLight ? themeDark : themeLight, nullptr, nullptr);
    });
    ll->attach(edt1, ZS_GRAVITY_END, 0, 300, VIEW_WRAP)->setGravity(ZS_GRAVITY_END | ZS_GRAVITY_VCENTER);
    ll->attach(edt2, 400, VIEW_WRAP)->setGravity(ZS_GRAVITY_HCENTER | ZS_GRAVITY_VCENTER);
    /*
    //root->attach(img, ZS_GRAVITY_START, 0, VIEW_WRAP, VIEW_WRAP );
    //root->attach(new zViewController(styles_z_acontroller, 0, z.R.integer.acontrol, z.R.string.acontrollerMap), ZS_GRAVITY_END | ZS_GRAVITY_BOTTOM, 0, 150_dp, 150_dp);
    //root->attach(new zViewController(styles_z_ccontroller, 0, z.R.integer.ccontrol, z.R.string.ccontrollerMap), ZS_GRAVITY_START | ZS_GRAVITY_BOTTOM, 0, 150_dp, 150_dp);
    //img->setRotation(0, 0, 20);
    manager->attachForm(new zViewForm(styles_z_form, z.R.id.form, styles_z_formcaption, styles_z_formfooter, z.R.string.first_form, true), rti(0, 0, 500, 500))->setRotation(0, 0, 10.0f);
    return;
    vw->setOnClick([this, chk](zView* v, bool l) {
        DLOG("click view %i", l);
        static bool is = true;
        chk->setText(is ? "Шаталов" : "Шаталов Сергей Викторович", false);
        is = !is;
        manager->showToast("Шаталов Сергей");
        //v->requestLayout();
    })->setOnTouch([](zView*, zTouch*) {
//        DLOG("touch");
        return 0;
    })->setOnAnimation([](zView* v, int) {
        float vv(0.0f);
        static float vv1(0);
        if(v->animator.isEmpty()) {
            v->animator.init(0.5f, true);
            //v->animator.add(1, zInterpolator::EASEINQUAD, 64);
            v->animator.add(2, zInterpolator::EASEINQUAD, 64);
            v->animator.add(0.5f, zInterpolator::EASEINQUAD, 64);
        }
        if(v->animator.update(vv)) {
//            auto s = f >= 8 ? 1.0f - (16.0f - f) * 0.08333f : 1.0f - f * 0.08333f;
//            ll->setAlpha(vv);
//            v->setAlpha(1.0 - vv);
            v->setRotation(0, 0, vv * 360.0f);
//            ll->setRotation(0, 0, vv * 360.0f);
//            ll->setScale(vv, vv);
//            DLOG("anim %f", vv - vv1);
            vv1 = vv;
        }
        return 1;
    });//->setGravity(ZS_GRAVITY_BOTTOM | ZS_GRAVITY_END);
    root->setOnClick([](zView*, bool l) {
        DLOG("click root %i", l);
    });
//    vw->post(MSG_ANIM, 50, 0);
     */
}
