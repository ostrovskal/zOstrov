
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
//    debug = true;
    #include "zssh/layout_linear.h"
    getActionBar()->setOnClickMenuGroup([](zView* v, zMenuGroup* g) {
        DLOG("click group %i %i", g->getId(), g->count());
    })->setOnClickMenuItem([](zView* v, zMenuItem* i) {
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
    DLOG("clk %i %s", root->atView(0)->isClickabled(), root->atView(0)->typeName());
    root->atView(0)->setOnClick([this](zView*, int) {
        getActionBar()->show(!getActionBar()->isChecked());
    });
    idView(z.R.id.radioLight)->setOnClick([this](zView*, int) {
        setTheme(styles_z_themelight, nullptr, nullptr);
    });
    idView(z.R.id.radioDark)->setOnClick([this](zView*, int) {
        setTheme(styles_z_themedark, nullptr, nullptr);
    });
    idView(z.R.id.chkbox)->setOnClick([this](zView* v, int) {
//        getActionBar()->show(v->isChecked());
    });
    zArray<zStringUTF8> objects(theme->findArray(z.R.array.spinArray));
    for(int i = 0 ; i < 1000; i += 7) {
        objects += zStringUTF8(z_ntos(&i, RADIX_HEX, false));
    }
    auto sel(idView<zViewSelect>(z.R.id.select));
    grp10->detach(sel); getActionBar()->setContent(sel);
    sel->setAdapter(new zAdapterList(objects, new zFabricSelectItem(styles_z_spin_capt),
                                        new zFabricSelectItem(styles_z_spin_item)))->
                                        setOnChangeSelected([](zView*, int s) {
                                            DLOG("select sel %i", s);
                                        });
    auto grd(idView<zViewGrid>(z.R.id.grid1));
    auto adapter(new zAdapterList(objects, new zFabricListItem(styles_z_list_item)));
    grd->setAdapter(adapter)->setOnChangeSelected([](zView*, int sel) {
//        DLOG("grid sel %i", sel);
    });
    auto lst(idView<zViewRibbon>(z.R.id.list1));
    auto adapter1(new zAdapterList(objects, new zFabricListItem(styles_z_list_item)));
    lst->setAdapter(adapter1);
}
