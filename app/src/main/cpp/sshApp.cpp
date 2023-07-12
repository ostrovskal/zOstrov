
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
//            android->activity->vm->DetachCurrentThread();
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
    ANativeActivity_setWindowFlags(android->activity, AWINDOW_FLAG_FULLSCREEN | AWINDOW_FLAG_KEEP_SCREEN_ON | AWINDOW_FLAG_LAYOUT_IN_SCREEN | AWINDOW_FLAG_LAYOUT_INSET_DECOR, 0);
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
        i32 events;
        android_poll_source* source;
        while((ALooper_pollAll((int)isActive() - 1, nullptr, &events, (void**)&source)) >= 0) {
            if(source) source->process(android, source);
            if(android->destroyRequested) return;
        }
        if(isActive()) drawViews();
    }
}

#include "zssh/zViewRibbons.h"
#include "zssh/zFileAsset.h"

void sshApp::setContent() {
    //debug = true;
    #include "zssh/layout_cell.h"
    #include "zssh/layout_tabbed.h"
    #include "zssh/layout_form.h"
    attachForm(formSettings, 300_dp, 300_dp);
    attachForm(formBrowser, 300_dp, 300_dp);
    formBrowser->setOnProcess([](zView*) {

    });
    zArray<zStringUTF8> objects(theme->findArray(z.R.array.spinArray));
    attachForm(formBrowser, 280_dp, 300_dp);
    auto sel(idView<zViewSelect>(z.R.id.choiceSource));
    auto lst(idView<zViewRibbon>(z.R.id.catalog));
    lst->setAdapter(new zAdapterList(objects, new zFabricListItem(styles_z_list_item)));
    sel->setAdapter(new zAdapterList(objects, new zFabricSelectItem(styles_z_spin_capt), new zFabricSelectItem(styles_z_spin_item)));
    auto chart1(idView<zViewChart>(z.R.id.chart1));
    auto chart2(idView<zViewChart>(z.R.id.chart2));
    chart1->setOnChangeSelected([](zView*, int it) {
       DLOG("chart sel %i", it);
    });
    i32 vals1[10] = { 100, 200, 20, 400, 190, 323, 55, 110, 350, 20 }; u32 cols1[1] = { z.R.color.lime };
    i32 vals2[] = { 50, 20, 120, 60, 390, 223, 155, 60, 250, 210, 50, 20, 120, 60, 390, 223, 155, 60, 250, 210,
                    50, 20, 120, 60, 390, 223, 155, 60, 250, 210, 50, 20, 120, 60, 390, 223, 155, 60, 250, 210
    };
    u32 cols2[1] = { z.R.color.olive };
    chart1->addData(vals1, 10, cols1, 1);
    chart1->addData(vals2, 40, cols2, 1);
    chart2->addData(vals1, 10, cols1, 1);
    chart2->addData(vals2, 40, cols2, 1);
    zFileAsset f; zPlayerParams pp;
    u8* ptr(nullptr); int size(0);
    if(f.open("sound/priz.wav", true)) {
        auto head((HEADER_WAV*)f.readn(nullptr, sizeof(HEADER_WAV)));
        ptr = (u8*)f.readn(&size, 0, sizeof(HEADER_WAV));
        pp.bits = head->bitsPerSample;
        pp.chan = head->NumOfChan;
        pp.rate = head->SampleRate * 1000;
        pp.bufSize = size;
        delete head;
        f.close();
    }
    auto onClick = [this, ptr, size, pp](zView* v, int) {
        auto id(v->id);
        zSoundPlayer* pl(nullptr);
        switch(id) {
            case z.R.id.bkg:
                pl = sound.createPlayer(id, TYPE_ASSET, {"sound/background.mp3"});
                break;
            case z.R.id.menu:
                pl = sound.createPlayer(id, TYPE_ASSET, {"sound/menu.mid"});
                break;
            case z.R.id.tm:
                pl = sound.createPlayer(id, TYPE_VIBRA, {});
                break;
            case z.R.id.exp:
                pl = sound.createPlayer(id, TYPE_MEM, pp);
                if(pl) pl->setData(ptr, size);
                else DLOG("player not create!");
                break;
            case z.R.id.bmb: break;
            case z.R.id.dth: break;
        }
        if(pl) pl->play(true);
    };
    idView<zViewButton>(z.R.id.bkg)->setOnClick(onClick);
    idView<zViewButton>(z.R.id.menu)->setOnClick(onClick);
    idView<zViewButton>(z.R.id.tm)->setOnClick(onClick);
    idView<zViewButton>(z.R.id.exp)->setOnClick(onClick);
    idView<zViewButton>(z.R.id.bmb)->setOnClick(onClick);
    idView<zViewButton>(z.R.id.dth)->setOnClick(onClick);
    /*
    auto grd(idView<zViewGrid>(z.R.id.grid1));
    auto adapter(new zAdapterList(objects, new zFabricListItem(styles_z_grid_item)));
    grd->setAdapter(adapter)->setOnChangeSelected([](zView*, int sel) {
//        DLOG("grid sel %i", sel);
    });
    auto edt(idView<zViewEdit>(z.R.id.edit1));
    auto edt2(idView<zViewEdit>(z.R.id.edit2));
    edt->setOnChangeText([adapter, edt, edt2](zView*, int m) {
        if(m == MSG_EDIT_FINISH) {
            auto txt(edt->getText());
            if(txt.isNotEmpty()) {
                adapter->add(txt);
                edt->setText("", true);
            }
            edt2->setText(txt, false);
        }
    });
    auto chSp(idView(z.R.id.spans));
    auto txt(root->idView<zViewText>(z.R.id.tv1));
    if(chSp) {
        chSp->setOnClick([txt](zView*, int) {
            txt->removeText(35, 1);
        });
    }
    if(txt) {
        txt->setLines(51);
        txt->setText(R"(<h1><bkg v="ff00ff00"><b>Заголовок 1</b></bkg></h1><h4><i>Заголовок 2</i></h4><p>Завершение <a href="serg"><b>работы фоновых приложений после</b> блокировки</a> экрана <c v="ff00ff00">помогает</c> экономить заряд аккумулятора.
<p>С другой стороны, в этом случае<img src="zssh" t="iconSelf" f="0.75"/>вы не будете получать<a href="self"><img src="zx_icons" t="iconApp_round" f="1.0"/></a>новые сообщения (электронные<sup>1</sup>, текстовые<sub>2</sub>, из соцсетей и т.д.)
<ul start="10" reversed="1"><li><a href="bullet">Сергей</a></li><li><bkg v="ffff00ff">Максим</bkg></li><li><c v="ff00ffff">Ольга</c></li></ul>)", true);
        txt->setHtmlText(txt->getText(), [](cstr tag, bool end, zHtml* html) { return false; });
        txt->setOnClickUrl([](zView*, czs& ref) {
           DLOG("click url %s", ref.str());
        });
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
    auto lst1(idView<zViewRibbon>(z.R.id.list1));
    auto adapter1(new zAdapterList(objects, new zFabricListItem(styles_z_list_item)));
    lst1->setAdapter(adapter1);
    edt2->setOnChangeText([adapter1, edt, edt2](zView*, int m) {
        if(m == MSG_EDIT_FINISH) {
            auto txt(edt2->getText());
            if(txt.isNotEmpty()) {
                adapter1->add(txt);
                edt2->setText("", true);
            }
            edt->setText(txt, false);
        }
    });
     */
}
