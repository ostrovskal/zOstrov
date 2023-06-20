
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
//            zJniHelper::instance()->hideNavigationPanel();
            break;
        case APP_CMD_WINDOW_REDRAW_NEEDED:
            redrawNativeWindow();
            break;
        case APP_CMD_WINDOW_RESIZED:
            updateNativeWindow(android->config, theme->styles ? theme->styles : styles_z_themelight, theme->user_resources, theme->user_styles);
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
//    zJniHelper::init(android->activity, nullptr);
}

void sshApp::run() {
    while(true) {
        i32 events, sens;
        android_poll_source* source;
        while((sens = ALooper_pollAll((int)isActive() - 1, nullptr, &events, (void**)&source)) >= 0) {
            if(source) source->process(android, source);
            if(android->destroyRequested) return;
            manager->sensor.processSensor(sens);
        }
        if(isActive()) drawViews();
    }
}

#include "zssh/zViewRibbons.h"

void sshApp::setContent() {
    debug = true;
    #include "zssh/layout_linear.h"
    #include "zssh/layout_tabbed.h"
    #include "zssh/layout_form.h"
    attachForm(formSettings, 300_dp, 300_dp);
    attachForm(formBrowser, 300_dp, 300_dp);
    zArray<zStringUTF8> objects(theme->findArray(z.R.array.spinArray));
    for(int i = 0 ; i < 1000; i += 7) {
        objects += zStringUTF8(z_ntos(&i, RADIX_HEX, false));
    }
    attachForm(formBrowser, 280_dp, 300_dp);
    auto sel(idView<zViewSelect>(z.R.id.choiceSource));
    auto lst(idView<zViewRibbon>(z.R.id.catalog));
    lst->setAdapter(new zAdapterList(objects, new zFabricListItem(styles_z_list_item)));
    sel->setAdapter(new zAdapterList(objects, new zFabricSelectItem(styles_z_spin_capt), new zFabricSelectItem(styles_z_spin_item)));
    auto txt(root->idView<zViewText>(z.R.id.tv1));
    if(txt) {
        txt->setLines(3);
        txt->setText("Завершение работы фоновых приложений после"
                     " блокировки экрана <c value=\"ff0ff00\">помогает</c>  экономить заряд аккумулятора. "
                     "С другой стороны, в этом случае вы не будете получать "
                     "новые сообщения (электронные, текстовые, из соцсетей и т.д.).", true);
/*
        txt->setTextFromHtml(txt->getText(), [](cstr tag, bool end, zHtml* html) {
            return false;
        });
*/
        txt->setSpan(new zTextSpanParagraph(), 0, 0);
        txt->setSpan(new zTextSpanBackgroundColor(0xff00ff00), 0, 10);
        txt->setSpan(new zTextSpanForegrounColor(0xff00ffff), 10, 20);
        txt->setSpan(new zTextSpanBackgroundColor(0xff9fff9f), 60, 220);
/*
        txt->setSpan(new zTextSpanHeightSize(30_dp), 20, 30);
        txt->setSpan(new zTextSpanRelativeHeightSize(1.2f), 30, 40);
        txt->setSpan(new zTextSpanStyle(ZS_TYPEFACE_ITALIC), 40, 45);
        txt->setSpan(new zTextSpanStyle(ZS_TYPEFACE_BOLD_ITALIC), 45, 50);
        txt->setSpan(new zTextSpanStrikeline(), 50, 55);
        txt->setSpan(new zTextSpanUnderline(), 55, 60);
        txt->setSpan(new zTextSpanStyle(ZS_TYPEFACE_BOLD), 60, 70);
        txt->setSpan(new zTextSpanImage("zssh", z.R.integer.rectRound, 0.3f, 0x0), 80, 80);
        txt->setSpan(new zTextSpanUrl("block"), 80, 85);
        txt->setSpan(new zTextSpanImage("zssh", z.R.integer.rectRound, 0.3f, 0x0), 82, 82);
        txt->setSpan(new zTextSpanSubscript(), 90, 92);
        txt->setSpan(new zTextSpanSuperscript(), 97, 99);
        txt->setSpan(new zTextSpanStyle(ZS_TYPEFACE_ITALIC), 100, 130);
        txt->setSpan(new zTextSpanForegrounColor(z.R.color.red), 90, 110);
        txt->setSpan(new zTextSpanImage("zssh", z.R.integer.gradientRadial, 0.3f, 0x0), 120, 120);
*/
    }
    getActionBar()->setOnClickMenuGroup([](zView* v, zMenuGroup* g) {
        DLOG("click group %i %i", g->getId(), g->count());
    })->setOnClickMenuItem([formBrowser, formSettings](zView* v, zMenuItem* i) {
        if(i->getId() == z.R.integer.MENU_BROWSER) {
            formBrowser->updateVisible(true);
        }
        if(i->getId() == z.R.integer.MENU_SETTINGS) {
            formSettings->updateVisible(true);
        }
        if(i->getId() == z.R.integer.MENU_KEYBOARD) {
            auto ic(i->getImage());
            i->setImage(ic == z.R.integer.iconZxKeyb ? z.R.integer.iconZxGamepad : (ic == z.R.integer.iconZxGamepad
                                                                                ? z.R.integer.iconZxDisplay
                                                                                : z.R.integer.iconZxKeyb));
        } else {
            //i->setEnabled(false);
            //i->setVisibled(i->getId() & 1);
            i->setChecked(true);
        }
       DLOG("click item %i %s", v->id, i->getText().str());
    });
    idView(z.R.id.radioLight)->setOnClick([this](zView*, int) {
        setTheme(styles_z_themelight, nullptr, nullptr);
    });
    idView(z.R.id.radioDark)->setOnClick([this](zView*, int) {
        setTheme(styles_z_themedark, nullptr, nullptr);
    });
    idView(z.R.id.chkbox)->setOnClick([this](zView* v, int) {
        getActionBar()->show(v->isChecked());
    });
    auto sel1(idView<zViewSelect>(z.R.id.select));
    grp01->detach(sel1); getActionBar()->setContent(sel1);
    sel1->setAdapter(new zAdapterList(objects, new zFabricSelectItem(styles_z_spin_capt),
                                     new zFabricSelectItem(styles_z_spin_item)))->
                                        setOnChangeSelected([](zView*, int s) {
                                            DLOG("select sel %i", s);
                                        });
    auto grd(idView<zViewGrid>(z.R.id.grid1));
    auto adapter(new zAdapterList(objects, new zFabricListItem(styles_z_list_item)));
    grd->setAdapter(adapter)->setOnChangeSelected([](zView*, int sel) {
//        DLOG("grid sel %i", sel);
    });
    auto lst1(idView<zViewRibbon>(z.R.id.list1));
    auto adapter1(new zAdapterList(objects, new zFabricListItem(styles_z_list_item)));
    lst1->setAdapter(adapter1);
}
