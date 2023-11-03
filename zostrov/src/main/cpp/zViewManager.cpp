//
// Created by User on 18.04.2023.
//

#include "zostrov/zCommon.h"
#include "zostrov/zCommonLayout.h"
#include "zostrov/zViewWidgets.h"

static float filterMtxs[6][16] = {
        {   1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },                   // normal
        {   -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f  },               // inverted
        {   0.75f, 0.0f, 0.0f, 0.0f, 0.0f, 0.75f, 0.0f, 0.0f, 0.0f, 0.0f, 0.75f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f  },               // pressed
        {   1.55f, 0.0f, 0.0f, 0.0f, 0.0f, 1.55f, 0.0f, 0.0f, 0.0f, 0.0f, 1.55f, 0.0f, 0.0f, 0.0f, 0.0f, 1.55f },               // hovered
        {   1.45f, 0.0f, 0.0f, 0.0f, 0.0f, 1.45f, 0.0f, 0.0f, 0.0f, 0.0f, 1.45f, 0.0f, 0.0f, 0.0f, 0.0f, 1.45f },               // focused
        {   0.213f, 0.715f, 0.072f, 0.0f, 0.213f, 0.715f, 0.072f, 0.0f, 0.213f, 0.715f, 0.072f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }, // disabled
};

zViewManager::zViewManager(ANativeActivity* _activity, cstr* options, int size_cache) {
    if(!theme) theme = new zTheme();
    if(!cache) cache = new zImageCache(size_cache);
    manager                  = this;
    assets                   = _activity->assetManager;
    fileAsset                = new zFileAsset();
    settings                 = new zSettings(zString8(_activity->internalDataPath).substrBeforeLast("/"), options);
    basePaths[PATH_EXTERNAL] = _activity->externalDataPath; basePaths[PATH_EXTERNAL].slash();
    basePaths[PATH_INTERNAL] = _activity->internalDataPath; basePaths[PATH_INTERNAL].slash();
    basePaths[PATH_OBB]      = _activity->obbPath; basePaths[PATH_OBB].slash();
}

zViewManager::~zViewManager() {
    destroyResources();
    displayDestroy();
    SAFE_DELETE(bufViewStates);
    SAFE_DELETE(cache);
    SAFE_DELETE(settings);
    SAFE_DELETE(fileAsset);
    sizeViewStates = 0;
    manager  = nullptr;
    settings = nullptr;
}

void zViewManager::destroyResources() {
    event.clear();
    if(popup) popup->dismiss();
    SAFE_DELETE(caret);
    SAFE_DELETE(common);
    SAFE_DELETE(popup);
    SAFE_DELETE(dropdown);
    cache->recycle(debugTexture);
    for(auto& t : _touch) t.reset();
    focus       = nullptr;
    debugTexture= nullptr;
    zView::touch= nullptr;
    zView::fbo  = nullptr;
    zView::m.identity();
    cache->info();
}

zTouch* zViewManager::getSystemTouch(AInputEvent *_event) {
    int idx(0), id(0);
    zTouch::getEventAction(_event, &idx, &id);
    for(auto& t : _touch) {
        if(t.id == id) return &t;
    }
    return &_touch[idx];
}

i32 zViewManager::processInputEvens(AInputEvent *_event) {
    if(isActive()) {
        switch(AInputEvent_getType(_event)) {
            case AINPUT_EVENT_TYPE_MOTION:
                zView::touch = getSystemTouch(_event);
                zView::touch->event(_event, rti());
                common->touchEventDispatch(_event);
                if(zView::touch->isReleased()) zView::touch->reset();
                break;
            case AINPUT_EVENT_TYPE_KEY:
                if(AKeyEvent_getAction(_event) == AKEY_EVENT_ACTION_UP)
                    return keyEvent(AKeyEvent_getKeyCode(_event));
            default: break;
        }
    }
    return 0;
}

zViewForm* zViewManager::attachForm(zViewForm* form, int width, int height, bool vis) {
    root->detach(form);
    root->attach(form, ZS_GRAVITY_CENTER, 0, width, height);
    form->updateStatus(ZS_VISIBLED, vis);
    return form;
}

void zViewManager::showToast(cstr _text, zStyle* _styles) {
    new zViewToast(_text, _styles);
}

int zViewManager::keyEvent(int key) {
    auto v(common->sysView());
    return v && v->isEnabled()? v->keyEvent(key, true) : 0;
}

bool zViewManager::displayInit() {
    if(!zGL::instance()->init(window))
        return false;
    // шейдеры
    int sz;
    auto main(new zShader(new zShader(zString8((cstr)assetFile("shaders/main.vshader", &sz), sz), GL_VERTEX_SHADER),
                          new zShader(zString8((cstr)assetFile("shaders/main.fshader", &sz),  sz), GL_FRAGMENT_SHADER)));
    auto oes(new zShader(new zShader(zString8((cstr)assetFile("shaders/main.vshader", &sz), sz), GL_VERTEX_SHADER),
                         new zShader(zString8((cstr)assetFile("shaders/oes.fshader", &sz), sz), GL_FRAGMENT_SHADER)));
    auto text(new zShader(new zShader(zString8((cstr)assetFile("shaders/main.vshader", &sz), sz), GL_VERTEX_SHADER),
                          new zShader(zString8((cstr)assetFile("shaders/text.fshader", &sz), sz), GL_FRAGMENT_SHADER)));
    auto outl(new zShader(new zShader(zString8((cstr)assetFile("shaders/main.vshader", &sz), sz), GL_VERTEX_SHADER),
                          new zShader(zString8((cstr)assetFile("shaders/outline.fshader", &sz), sz), GL_FRAGMENT_SHADER)));
    shaders += new Z_SHADER("main", main, main->linkVariables("apos;atex;utex;upmtx;uwmtx;ucflt;utcolor"));
    shaders += new Z_SHADER( "oes",  oes,  oes->linkVariables("apos;atex;utex;upmtx;uwmtx;ucflt;utcolor"));
    shaders += new Z_SHADER("text", text, text->linkVariables("apos;atex;utex;upmtx;uwmtx;ucflt;utcolor"));
    shaders += new Z_SHADER("outl", outl, outl->linkVariables("apos;atex;utex;upmtx;uwmtx;ucflt;utcolor"));
    glEnableVertexAttribArray(shaders[0]->vars[ZSH_APOS]);
    glEnableVertexAttribArray(shaders[0]->vars[ZSH_ATEX]);
    glUniform4fv(shaders[0]->vars[ZSH_UCOL], 1, zColor::white);
    glUniform1i(shaders[0]->vars[ZSH_UTEX], 0);
    glUniform1i(shaders[0]->vars[ZSH_UTEX], 0);
    // параметры по умолчанию
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_DEPTH_TEST); glDisable(GL_STENCIL_TEST);
    glEnable(GL_BLEND); glEnable(GL_SCISSOR_TEST);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    return true;
}

void zViewManager::displayDestroy() {
    if(status != Z_QUIT) {
        zGL::instance()->invalidate();
        // уничтожить шейдер
        shaders.clear();
        DLOG("displayDestroy!");
        status = Z_QUIT;
    }
}

void zViewManager::refreshConfig(AConfiguration* config) {
    auto sz(zGL::instance()->getSizeScreen());
    landscape   = sz.w >= sz.h;
    scaleScreen = ((float)z_min(sz.w, sz.h)/*AConfiguration_getSmallestScreenWidthDp(config)*/ / 480.0f);
    leftLayout  = AConfiguration_getLayoutDirection(config) != 0;
    AConfiguration_getLanguage(config, languages);
    ILOG("width:%i height:%i sw:%.3f land:%i lang:%s ldir:%i",
         sz.w, sz.h, scaleScreen, landscape, languages, leftLayout);
}

void zViewManager::focusNativeWindow(bool _focus) {
    DLOG("%s", _focus ? "APP_CMD_SET_FOCUS" : "APP_CMD_LOST_FOCUS");
    status &= ~Z_ACTIVE; status |= Z_ACTIVE * _focus;
    sound.actionAll(_focus ? SL_PLAYSTATE_PAUSED : SL_PLAYSTATE_PLAYING, _focus ? SL_PLAYSTATE_PLAYING : SL_PLAYSTATE_PAUSED);
}

void zViewManager::redrawNativeWindow() {
    DLOG("APP_CMD_WINDOW_REDRAW_NEEDED");
}

void zViewManager::prepareRender(int ishader, zVertex2D* vertices, crti& scissor, const zMatrix& wmtx) {
    auto sh(shaders[ishader]);
    if(sh) {
        glUseProgram(sh->shader->getID());
        glVertexAttribPointer(sh->vars[ZSH_APOS], 3, GL_FLOAT, GL_FALSE, sizeof(zVertex2D), &vertices->x);
        glVertexAttribPointer(sh->vars[ZSH_ATEX], 2, GL_FLOAT, GL_FALSE, sizeof(zVertex2D), &vertices->u);
        //glDisable(GL_SCISSOR_TEST);
        glScissor(scissor.x, zView::fbo ? scissor.y : (screen.h - scissor.extent(true)), scissor.w, scissor.h);
        glUniformMatrix4fv(sh->vars[ZSH_WMTX], 1, false, wmtx);
    } else {
        ILOG("Unknown shader %i!", ishader);
    }
}

void zViewManager::setMainMatrix(int ww, int hh, bool invert) {
    static zMatrix mtx;
    if(invert) mtx.ortho(0, ww, hh, 0); else mtx.ortho(0, ww, 0, hh);
    glUniformMatrix4fv(shaders[0]->vars[ZSH_PMTX], 1, false, mtx);
    glViewport(0, 0, ww, hh);
}

void zViewManager::appActivate(bool activate, ANativeWindow *_window) {
    DLOG("%s", activate ? "APP_CMD_INIT_WINDOW" : "APP_CMD_TERM_WINDOW");
    window = _window;
    if(activate) {
        if(!isActive() && window) {
            ILOG("--------------------------------------------------------");
            ILOG("Executed application - %s", z_date("%c", 0));
            ILOG("--------------------------------------------------------\n");
            if(displayInit()) { status = Z_ACTIVE; return; }
        }
    }
    displayDestroy();
    status = Z_QUIT;
}

void zViewManager::drawViews() {
    static HANDLER_MESSAGE *msg(nullptr);
#ifndef NDEBUG
    static bool showTri(true);
    static int oldTri(0);
    static int oldLn(0);
    static int oldObjs(0);
    static int oldVideoMem(0);
    countVertices = 0; countLn = 0; countObjs = 0;
#endif
    screen = zGL::instance()->getSizeScreen();
    glScissor(0, 0, screen.w, screen.h);
    glClear(GL_COLOR_BUFFER_BIT);
    // 1. выполнить подсчет габаритов всех представлений
    common->measure(measureCommon);
    // 2. выполнить позиционирование всех представлений
    common->layout(screen);
    // 3. состояния представлений
    if(bufViewStates) stateAllViews(Z_LOAD, nullptr, nullptr);
    // 4. отрисовка всех представлений
    zView::fbo = nullptr;
    common->draw();
    zGL::instance()->swap();
    // 5. анимация
    if(status & Z_ANIM) {
        while((msg = manager->event.obtain())) {
            // отправить на обработку в представление
            msg->view->notifyEvent(msg);
        }
    }
#ifndef NDEBUG
    if(debug && showTri) {
        if(oldTri != countVertices) {
            DLOG("countVertices: %i", countVertices);
            oldTri = countVertices;
        }
        if(oldLn != countLn) {
            DLOG("countLn: %i", countLn);
            oldLn = countLn;
        }
        if(oldObjs != countObjs) {
            DLOG("countObjs: %i", countObjs);
            oldObjs = countObjs;
        }
        showTri = false;
        if(oldVideoMem != videoMemory) {
            DLOG("videoMem: %i", videoMemory);
            oldVideoMem = videoMemory;
        }
    }
#endif
    status |= Z_ANIM;
}

void zViewManager::drawCaret(zView* _view) {
    if(caret && caret->parent == _view) {
        caret->draw();
    }
}

void zViewManager::saveStateView(zViewGroup *view, int &index) {
    static zView::STATE state;
    if(view) {
        int count(view->countChildren());
        for(int i = 0 ; i < count; i++) {
            auto child(view->atView(i));
            if(child->id) {
                state.reset(child->id, child->crcType());
                child->stateView(state, true, index);
                if(state.data.isNotEmpty()) viewStates += state;
            }
            auto group(dynamic_cast<zViewGroup *>(child));
            if(group && group->stateChildren())
                saveStateView(group, index);
        }
    }
}

void zViewManager::stateAllViews(u32 action, u8** _ptr, u32* _size) {
    int index(0);
    if(action == Z_RESUME) {
        SAFE_DELETE(bufViewStates); sizeViewStates = 0;
        auto ptr(*_ptr);
        if(ptr) {
            sizeViewStates = *_size;
            bufViewStates = new u8[sizeViewStates];
            memcpy(bufViewStates, ptr, sizeViewStates);
        }
    } else if(action == Z_SAVE) {
        // сохраняем все структуры представлений в массив
        saveStateView(common, index);
        // определяем размер байтового буфера
        u32 _sizeStates(8);
        for(auto& vs : viewStates) {
            // id, type, sizeData, (len string + 1), chars string + 1
            _sizeStates += 4 + 4 + vs.string.size() + 1 + 2 + 2;
            // dataData * 4
            _sizeStates += vs.data.size() * 4;
        }
        auto _bufStates(new u8[_sizeStates]);
        *_ptr = _bufStates; *_size = _sizeStates;
        // также запомнить статус клавиатуры
        dwordLE(&_bufStates, keyboard ? keyboard->getOwnerID() : 0);
        // количество представлений
        dwordLE(&_bufStates, viewStates.size());
        for(auto& vs : viewStates) {
            dwordLE(&_bufStates, vs.id); dwordLE(&_bufStates, vs.type);
            wordLE(&_bufStates, vs.data.size() * 4); wordLE(&_bufStates, vs.string.size() + 1);
            z_memcpy(&_bufStates, vs.string, vs.string.size() + 1);
            z_memcpy(&_bufStates, vs.data.get_data(), vs.data.size() * 4);
        }
        ILOG("Сохранение состояния представлений (размер: %i(%i))!", _sizeStates, viewStates.size());
        viewStates.clear();
    } else if(action == Z_LOAD && bufViewStates) {
        auto _bufStates(bufViewStates); zView::STATE state;
        auto keybStatus(dwordLE(&_bufStates)), countViews(dwordLE(&_bufStates)), realViews(0U);
        for(u32 i = 0 ; i < countViews; i++) {
            // id, crc, sizeData, lengthString
            auto id(dwordLE(&_bufStates)), type(dwordLE(&_bufStates));
            auto size(wordLE(&_bufStates)), length(wordLE(&_bufStates));
            auto view(idView(id));
            if(view && view->crcType() == type) {
                // нашли
                state.string = (cstr)_bufStates; state.data.resize((int)size);
                memcpy(state.data.get_data(), _bufStates + length, size);
                index = 0; view->stateView(state, false, index); realViews++;
            }
            _bufStates += size + length;
        }
        ILOG("Восстановление состояния представлений (размер: %i(%i-%i))!", sizeViewStates, countViews, realViews);
        SAFE_DELETE(bufViewStates); sizeViewStates = 0;
        // восстановить клавиатуру
        if(keybStatus && idView(keybStatus)) showSoftKeyboard(keybStatus, true);
    }
}

u8* zViewManager::assetFile(cstr src, i32* plen, u8* ptr, int len, int pos) const {
    u8* ret(nullptr);
    mMutex.lock();
    if(fileAsset->open(src, true)) {
        ret = (ptr ? (u8*)fileAsset->read(plen, ptr, len, pos) : (u8*)fileAsset->readn(plen));
        fileAsset->close();
    }
    mMutex.unlock();
    return ret;
}

void zViewManager::updateNativeWindow(AConfiguration* config, zStyle* _styles, zResource** _user, zStyles* _user_styles) {
    DLOG("APP_CMD_WINDOW_RESIZED");
    if(window) {
        auto sz(zGL::instance()->getSizeScreen());
        ANativeWindow_setBuffersGeometry(window, sz.w, sz.h, ANativeWindow_getFormat(window));
        setMainMatrix(sz.w, sz.h, false);
        // при первом запуске/при изменении ориентации
        if(!common) {
            measureCommon.set(zMeasure(MEASURE_EXACT, sz.w), zMeasure(MEASURE_EXACT, sz.h));
            destroyResources();
            refreshConfig(config);
            // установить тему оформления
            setTheme(_styles, _user, _user_styles);
            // каретка
            caret = new zViewCaret();
            // "пустая" текстура
            debugTexture = cache->get("znull", nullptr);
            // корневой элемент(будет скрытым, чтобы избежать проверку на родителя)
            common = new zAbsoluteLayout(styles_default, z.R.id.common);
            common->rclient.set(0, 0, sz.w, sz.h);
            common->rview = common->rclient; common->rclip = common->rclient;
            common->parent = common; common->onInit(false);
            // добавить корневую группу
            common->attach(root = new zFrameLayout(styles_default, z.R.id.root), VIEW_MATCH, VIEW_MATCH);
            // привязка декораций
            dropdown = new zViewDropdown();
            popup = new zViewPopup();
            // добавить панель действий
            auto actStyles(theme->findStyles(theme->styles->_int(Z_THEME_ACTION_BAR, z.R.style.bar)));
            common->attach(actionBar = new zActionBar(actStyles, styles_z_barbutton, styles_z_barpopup), 0, INT_MAX, VIEW_MATCH, VIEW_WRAP);
            actionBar->setAdapter(new zAdapterList({}, new zFabricMenuItem(styles_z_menu_item)));
            // добавить клаву
            auto keybMap(theme->styles->_str(Z_THEME_LAYOUT_KEYBOARD, "keyboardDefault.xml"));
            common->attach(keyboard = new zViewKeyboard(keybMap), ZS_GRAVITY_HCENTER, zGL::instance()->getSizeScreen(true), VIEW_WRAP, VIEW_WRAP);
            // установка контента
            setContent();
            status &= ~Z_CHANGE_THEME;
        }
    }
    // обновить элементы
    common->changeTheme();
    // перегрузить все текстуры
    invalidateTexture();
}

void zViewManager::setTheme(zStyle* _styles, zResource** _user, zStyles* _user_styles) {
    if(theme->setTheme(_styles, "-" + zString8(languages, 2), _user, _user_styles)) {
        ILOG("Установка темы <%s>!", theme->themeName.str());
        status |= Z_CHANGE_THEME;
        if(common) {
            common->changeTheme();
            status &= ~Z_CHANGE_THEME;
        }
    }
}

void zViewManager::setColorFilter(zView* view, const zColor& _color) {
    u32 cf(ZCF_NORMAL);
    if(view) {
        auto sts(view->status);
        auto s(sts & (ZS_DISABLED | ZS_TAP));
        if(s) cf = ((s & ZS_DISABLED) ? ZCF_DISABLED : ((sts & ZS_TAP_MASK) >> 8));
    }
    glUniformMatrix4fv(shaders[0]->vars[ZSH_UFLT], 1, false, filterMtxs[cf]);
    glUniform4fv(shaders[0]->vars[ZSH_UCOL], 1, _color);
}

zTexture *zViewManager::loadResourceTexture(u32 _id, zTexture *_t) const {
    auto res(theme->findString(_id));
    return (res.isNotEmpty() ? cache->get(res, _t) : nullptr);
}

void zViewManager::changeFocus(zView* view) {
    if(focus == view) return;
    if(focus) {
        auto vw(focus); focus = nullptr;
        vw->onChangeFocus(false);
    }
    auto isTouchable(false);
    if(view) {
        if((isTouchable = view->isFocusableInTouch())) {
            focus = view; view->onChangeFocus(true);
        }
    }
    // вызвать/спрятать клавиатуру
    showSoftKeyboard(focus ? focus->id : 0, isTouchable);
}

zViewDropdown* zViewManager::getDropdown(zView* _owner, zStyle* _style, zBaseAdapter* _adapter) const {
    if(_style) dropdown->setStyles(_style);
    if(_adapter) dropdown->setAdapter(_adapter);
    dropdown->margin.empty(); dropdown->owner = _owner;
    return dropdown;
}

void zViewManager::eraseAllEventsView(zView* view) {
    event.erase(view, 0);
}

void zViewManager::showSoftKeyboard(u32 idOwner, bool _show) {
    keyboard->show(idOwner, _show);
}

u8* zImageCache::load(cstr _name, i32& size) {
    u8* ptr; zString8 name(_name);
    if(name.indexOf("/") == -1) {
        // загрузить изображение из активов и скопировать ее в текстуру
        ptr = manager->assetFile("textures/" + name + ".ttl", &size);
    } else {
        zFile f(name, true); size = f.length(); ptr = (u8*)f.readn();
    }
    return ptr;
}

zTexture* zImageCache::add(zTexture* tx, zTexture* t) {
    recycle(t);
    tx->ref++; use++;
    auto _size(tx->getSize()); int szTmp(_size.w * _size.h * 4), szTex(szTmp), idx;
    // 3. проверить - она помещается в кэш?
    if((curSize + szTmp) >= maxSize) {
        // 4. если не помещается - удалить из кэша неиспользуемые в порядке времени
        while((idx = findOlder()) != -1 && szTmp > 0) {
            auto _tx(tex[idx].sz); szTmp -= _tx; curSize -= _tx;
            tex.erase(idx, 1, true);
            use--;
        }
    } else {
        curSize += szTmp;
        szTmp = 0;
    }
    tex += CACHE(tx, szTex);
    if(szTmp > 0) {
        // если в кэше нет свободных текстур и наша не помещается
        maxSize += szTmp;
        curSize += szTmp;
    }
    DLOG("add cache: maxMem:%i-curMem:%i useAll: %i name: %s count: %i", maxSize, curSize, use, tx->name.str(), tex.size());
    return tx;
}

zTexture* zImageCache::get(cstr _name, zTexture* t) {
    recycle(t);
    // 1. проверить такая уже есть?
    auto idx(tex.indexOf(_name));
    if(idx != -1) {
        auto _tx(&tex[idx]);
        _tx->millis = z_timeMillis();
        use++; _tx->tex->ref++;
//        DLOG("ref cache: use: %i name: %s ref: %i", use, _name, _tx->tex->ref);
        return _tx->tex;
    }
    // 2. создать новую
    zTexture* tx; i32 size(0);
    auto ptr(load(_name, size));
    if(ptr && size) {
        tx = new zTexture(_name, ptr, size);
        delete ptr;
    } else {
        ILOG("Ошибка при загрузке текстуры - %s", _name);
        return get("znull", nullptr);
    }
    return add(tx, nullptr);
}

int zImageCache::findOlder() {
    i64 max(LONG_MAX); int idx(-1);
    for(int i = 0 ; i < tex.size(); i++) {
        auto tx(&tex[i]);
        if(tx->tex->ref > 0) continue;
        if(max > tx->millis) max = tx->millis, idx = i;
    }
    return idx;
}

void zImageCache::recycle(zTexture* tx) {
    if(tx && tx->name.isNotEmpty()) {
        auto idx(tex.indexOf(tx->name));
        if(idx == -1) {
            // нет в кэше
            ILOG("texture not cache: %s", tx->name.str());
        } else {
            // уменьшаем ссылку
            if(--tx->ref < 0) {
                // уже удалили - ошибка в системе!!!(просто удаляем)
                ILOG("texture already removed: %s!!!", tx->name.str());
                delete tx;
            }
            use--;
        }
    }
}

void zImageCache::clear() {
    for(auto& c : tex) delete c.tex;
    tex.clear(); curSize = 0;
}

void zImageCache::info() {
    if(use) {
        DLOG("Кэш текстур: текстур в памяти - %i, всего текстур - %i, память - %i, макс - %i", use, tex.size(), curSize, maxSize);
        for(auto& t : tex) { ILOG("%s", t.tex->info().str()); }
    }
}

void zImageCache::update() {
    int flt;
    for(auto& t: tex) {
        auto type(t.tex->type); glBindTexture(type, t.tex->id);
        glGetTexParameteriv(type, GL_TEXTURE_MIN_FILTER, &flt);
        if(t.tex->isCustom()) {
            auto sz(t.tex->getSize());
            t.tex->makeTexture(sz.w, sz.h, t.tex->type);
        } else {
            i32 size; auto ptr(load(t.tex->name, size));
            if(ptr && size) t.tex->makeTexture(ptr, size);
            delete ptr;
        }
        // восстановить фильтрацию
        t.tex->enableFilter(flt - GL_NEAREST);
    }
}
