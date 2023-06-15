
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
            manager->sensor.processSensor(sens);
        }
        if(isActive()) drawViews();
    }
}

#include "zssh/zViewRibbons.h"

void sshApp::setContent() {
    //debug = true;
    #include "zssh/layout_linear.h"
    idView(z.R.id.radioLight)->setOnClick([this](zView*, int) {
        setTheme(themeLight, nullptr, nullptr);
    });
    idView(z.R.id.radioDark)->setOnClick([this](zView*, int) {
        setTheme(themeDark, nullptr, nullptr);
    });
    zArray<zStringUTF8> objects(theme->findArray(z.R.array.spinArray));
    for(int i = 0 ; i < 100; i++) {
        objects += zStringUTF8(z_ntos(&i, RADIX_DEC, false));
    }
    idView<zViewSelect>(z.R.id.select)->
            setAdapter(new zAdapterList(objects, new zFabricSelectItem(styles_z_spin_capt),
                                        new zFabricSelectItem(styles_z_spin_item)))->
                                        setOnChangeSelected([](zView*, int s) {
                                            DLOG("select sel %i", s);
                                        });
    auto grd(idView<zViewGrid>(z.R.id.grid1));
    auto adapter(new zAdapterList(objects, new zFabricListItem(styles_z_list_item)));
    grd->setAdapter(adapter)->setOnChangeSelected([](zView*, int sel) {
//        DLOG("grid sel %i", sel);
    })->setOnClick([](zView*, int) {
    });
    auto lst(idView<zViewRibbon>(z.R.id.list1));
    auto adapter1(new zAdapterList(objects, new zFabricListItem(styles_z_list_item)));
    lst->setAdapter(adapter1);
}
