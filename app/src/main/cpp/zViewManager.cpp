//
// Created by User on 18.04.2023.
//

#include "zssh/zCommon.h"
#include "zssh/zCommonLayout.h"
#include "zssh/zViewWidgets.h"

// идентификатор треда для анимации
//static pthread_t threadID(0);

static float filterMtxs[6][16] = {
        {   1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },                   // normal
        {   -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f  },               // inverted
        {   0.75f, 0.0f, 0.0f, 0.0f, 0.0f, 0.75f, 0.0f, 0.0f, 0.0f, 0.0f, 0.75f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f  },               // pressed
        {   1.55f, 0.0f, 0.0f, 0.0f, 0.0f, 1.55f, 0.0f, 0.0f, 0.0f, 0.0f, 1.55f, 0.0f, 0.0f, 0.0f, 0.0f, 1.55f },               // hovered
        {   1.45f, 0.0f, 0.0f, 0.0f, 0.0f, 1.45f, 0.0f, 0.0f, 0.0f, 0.0f, 1.45f, 0.0f, 0.0f, 0.0f, 0.0f, 1.45f },               // focused
        {   0.213f, 0.715f, 0.072f, 0.0f, 0.213f, 0.715f, 0.072f, 0.0f, 0.213f, 0.715f, 0.072f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f }, // disabled
};

static cstr vertShader = "attribute vec4 pos;\n"
                         "attribute vec2 tex;\n"
                         "varying vec2 v_Texture;\n"
                         "uniform mat4 wmtx;\n"
                         "uniform mat4 pmtx;\n"
                         "void main() {\n"
                         "    v_Texture = tex;\n"
                         "    gl_Position = pmtx * wmtx * pos;\n}";

static cstr fragShader = "precision mediump float;\n"
                         "uniform mat4 cflt;\n"
                         "uniform vec4 tcolor;\n"
                         "uniform sampler2D u_TextureUnit;\n"
                         "varying vec2 v_Texture;\n"
                         "void main() {\n"
                         "vec4 tmp = texture2D(u_TextureUnit, v_Texture);\n"
                         "    gl_FragColor = (tmp * tcolor) * cflt;\n}";

zViewManager* manager(nullptr);

/*
static void* threadFunc(void*) {
    static HANDLER_MESSAGE *msg(nullptr);
    while(manager && !manager->isQuit()) {
        if(!manager->isAnim()) continue;
        while((msg = manager->event.obtain())) {
            // отправить на обработку в представление
            if(msg->what == MSG_ANIM) {
                auto view(msg->view);
                if(view->onAnimation(view, msg->arg)) {
                    manager->event.send(view, MSG_ANIM, view->duration, msg->arg);
                }
            }
        }
        usleep(1000);
    }
    return nullptr;
}
*/

zViewManager::zViewManager(AAssetManager* _assets, int size_cache) {
    memset(shaderVars, 0, sizeof(shaderVars));
    if(!theme) theme = new zTheme();
    if(!cache) cache = new zImageCache(size_cache);
    manager   = this;
    assets    = _assets;
}

zViewManager::~zViewManager() {
    DLOG("~zViewManager");
    destroyResources();
    displayDestroy();
    SAFE_DELETE(bufViewStates);
    SAFE_DELETE(cache);
    sizeViewStates = 0;
    manager = nullptr;
}

void zViewManager::destroyResources() {
    event.clear();
    touch.clear();
    SAFE_DELETE(caret);
    SAFE_DELETE(common);
    cache->recycle(debugTexture);
    focus       = nullptr;
    debugTexture= nullptr;
    cache->info();
}

i32 zViewManager::processInputEvens(AInputEvent *_event) {
    if(isActive()) {
        switch(AInputEvent_getType(_event)) {
            case AINPUT_EVENT_TYPE_MOTION:
                common->touchEvent(_event);
//                touch.info();
                break;
            case AINPUT_EVENT_TYPE_KEY:
                if(AKeyEvent_getAction(_event) == AKEY_EVENT_ACTION_UP)
                    return keyEvent(AKeyEvent_getKeyCode(_event));
            default: break;
        }
    }
    return 0;
}

zViewForm* zViewManager::attachForm(zViewForm* form, cszi& rect) {
    root->detach(form);
    root->attach(form, ZS_GRAVITY_CENTER, 0, rect.w, rect.h);
    // отобразить форму
    form->visibility(true);
    form->setGravity(ZS_GRAVITY_CENTER);
    return form;
}

void zViewManager::showToast(cstr _text, zStyle* _styles) {
    new zViewToast(_text, _styles);
}

int zViewManager::keyEvent(int key) {
    auto v(common->sysView());
    return v ? v->keyEvent(key, true) : 0;
}

bool zViewManager::displayInit() {
    if(!zGL::instance()->init(window))
        return false;
    // шейдеры
    program  = new zShader(new zShader(vertShader, GL_VERTEX_SHADER), new zShader(fragShader, GL_FRAGMENT_SHADER));
    // параметры шейдеров
    program->linkVariables(shaderVars, "apos;atex;uu_TextureUnit;ucflt;utcolor;upmtx;uwmtx");
    glEnableVertexAttribArray(shaderVars[ZSH_APOS]);
    glEnableVertexAttribArray(shaderVars[ZSH_ATEX]);
    glUniform4fv(shaderVars[ZSH_UCOL], 1, zColor::white);
    glUniform1i(shaderVars[ZSH_UTEX], 0);
    // параметры по умолчанию
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_SCISSOR_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // запускаем тред анимации
/*
    pthread_attr_t lAttributes;
    if(pthread_attr_init(&lAttributes)) abort();
    if(pthread_create(&threadID, &lAttributes, threadFunc, this)) abort();
*/
    return true;
}

void zViewManager::displayDestroy() {
    zGL::instance()->invalidate();
    // уничтожить шейдер
    SAFE_DELETE(program);
    DLOG("displayDestroy!");
    status = Z_QUIT;
}

void zViewManager::refreshConfig(AConfiguration* config) {
    auto sz(zGL::instance()->getSizeScreen());
    landscape   = sz.w >= sz.h;
    scaleScreen = ((float)AConfiguration_getSmallestScreenWidthDp(config) / 160.0f);
    leftLayout  = AConfiguration_getLayoutDirection(config) != 0;
    AConfiguration_getLanguage(config, languages);
    DLOG("width:%i height:%i sw:%.3f land:%i lang:%s ldir:%i", sz.w, sz.h, scaleScreen, landscape, languages, leftLayout);
}

void zViewManager::focusNativeWindow(bool _focus) {
    DLOG("%s", _focus ? "APP_CMD_SET_FOCUS" : "APP_CMD_LOST_FOCUS");
    status &= ~Z_ACTIVE; status |= Z_ACTIVE * _focus;
//    sound.actionAll(_focus ? SL_PLAYSTATE_PAUSED : SL_PLAYSTATE_PLAYING, _focus ? SL_PLAYSTATE_PLAYING : SL_PLAYSTATE_PAUSED);
}

void zViewManager::redrawNativeWindow() {
    DLOG("APP_CMD_WINDOW_REDRAW_NEEDED");
}

void zViewManager::setMainMatrix(int ww, int hh, bool invert) {
    static zMatrix mtx;
    if(invert) {
        mtx.ortho(0, ww, hh, 0);
    } else {
        mtx.ortho(0, ww, 0, hh);
    }
    glUniformMatrix4fv(shaderVars[ZSH_PMTX], 1, false, mtx);
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
    countVertices = 0; countLn = 0; countObjs = 0;
#endif
    glScissor(0, 0, common->rview.w, common->rview.h);
    glClear(GL_COLOR_BUFFER_BIT);
    // 1. выполнить подсчет габаритов всех представлений
    common->measure(measureCommon);
    // 2. выполнить позиционирование всех представлений
    common->layout(common->rview);
    // 3. состояния представлений
    if(bufViewStates) stateAllViews(Z_LOAD, nullptr, nullptr);
    // 4. анимация
    if(status & Z_ANIM) {
        while((msg = manager->event.obtain())) {
            // отправить на обработку в представление
            if(msg->what == MSG_ANIM) {
                auto view(msg->view);
                if(view->onAnimation(view, msg->arg)) {
                    manager->event.send(view, MSG_ANIM, view->duration, msg->arg);
                }
            }
        }
    }
    // 5. отрисовка всех представлений
    zView::fbo = nullptr;
    common->draw();
    zGL::instance()->swap();
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
    }
#endif
    status |= Z_ANIM;
}

void zViewManager::saveStateView(zViewGroup *view, zView::STATE &state, int &index) {
    int count(view->countChildren());
    for(int i = 0 ; i < count; i++) {
        auto child(view->atView(i));
        auto group(dynamic_cast<zViewGroup*>(child));
        if(group) {
            saveStateView(group, state, index);
        } else if(child->id) {
            state.reset();
            state.id   = child->id;
            state.type = child->crcType();
            child->stateView(state, true, index);
            viewStates += state;
        }
    }
}

void zViewManager::stateAllViews(u32 action, u8** _ptr, u32* _size) {
    zView::STATE state; int index(0);
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
        saveStateView(common, state, index);
        // определяем размер байтового буфера
        u32 _sizeStates(8);
        for(auto& vs : viewStates) {
            // id, type, countData, string + 1
            _sizeStates += 8 + vs.string.size() + 1 + 4;
            // dataData
            _sizeStates += vs.data.size() * 4;
        }
        auto _bufStates(new u8[_sizeStates]);
        *_ptr = _bufStates; *_size = _sizeStates;
        // также запомнить статус клавиатуры
        dwordLE(&_bufStates, 0);//keyboard ? keyboard->getOwnerID() : 0);
        // count structures viewStates
        dwordLE(&_bufStates, viewStates.size());
        for(auto& vs : viewStates) {
            dwordLE(&_bufStates, vs.id); dwordLE(&_bufStates, vs.type);
            wordLE(&_bufStates, vs.data.size() * 4); wordLE(&_bufStates, vs.string.size() + 1);
            z_memcpy(&_bufStates, vs.string, vs.string.size() + 1);
            z_memcpy(&_bufStates, vs.data.get_data(), vs.data.size() * 4);
        }
        viewStates.free();
        DLOG("Сохранение состояния представлений %i!", _sizeStates);
    } else if(action == Z_LOAD && bufViewStates) {
        DLOG("Восстановление состояния представлений %i!", sizeViewStates);
        auto _bufStates(bufViewStates);
        auto keybStatus(dwordLE(&_bufStates)), countViews(dwordLE(&_bufStates));
        for(u32 i = 0 ; i < countViews; i++) {
            // id, crc, sizeData, lengthString
            auto id(dwordLE(&_bufStates)), type(dwordLE(&_bufStates));
            auto size(wordLE(&_bufStates)), length(wordLE(&_bufStates));
            auto view(idView(id));
            if(view && view->crcType() == type) {
                // нашли
                state.string = (cstr)_bufStates; state.data.resize((int)size);
                memcpy(state.data.get_data(), _bufStates + length, size);
                index = 0; view->stateView(state, false, index);
            }
            _bufStates += size + length;
        }
        SAFE_DELETE(bufViewStates); sizeViewStates = 0;
        // восстановить клавиатуру
        if(keybStatus && idView(keybStatus)) {
            showSoftKeyboard(keybStatus, true);
        }
    }
}

u8* zViewManager::assetFile(cstr src, i32* plen, u8* ptr, int len, int pos) {
    u8* ret(nullptr);
    if(assets) {
        auto asset(AAssetManager_open(assets, src, AASSET_MODE_UNKNOWN));
        if(asset) {
            auto alen(AAsset_getLength(asset));
            if(pos != -1) AAsset_seek(asset, pos, SEEK_SET);
            if(len != -1 && alen >= len) alen = len;
            if(!ptr) ptr = new u8[alen];
            AAsset_read(asset, ptr, alen);
            AAsset_close(asset);
            ret = ptr;
            if(plen) *plen = alen;
        }
    }
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
            // добавить панель действий
//        common->attach(actionBar = new zActionBar(styles_z_bar, styles_z_barbutton, styles_z_barpopup), VIEW_MATCH, VIEW_WRAP);
            // добавить клаву
            common->attach(keyboard = new zViewKeyboard("keyboardDefault.xml"), 0, sz.h, VIEW_MATCH, VIEW_WRAP);
            setContent();
            status &= ~Z_CHANGE_THEME;
        }
    }
    // обновить элементы
    common->changeTheme();
    // перегрузить все текстуры
    cache->update();
}

void zViewManager::setTheme(zStyle* _styles, zResource** _user, zStyles* _user_styles) {
    if(theme->setTheme(_styles, "-" + zStringUTF8(languages, 2), _user, _user_styles)) {
        ILOG("Установка темы <%s>!", theme->themeName.str());
        status |= Z_CHANGE_THEME;
        if(common) {
            common->changeTheme();
            status &= ~Z_CHANGE_THEME;
        }
    }
}

void zViewManager::prepareRender(zVertex2D* vertices, crti& scissor, const zMatrix& wmtx) {
    glVertexAttribPointer(shaderVars[ZSH_APOS], 2, GL_FLOAT, GL_FALSE, sizeof(zVertex2D), &vertices->x);
    glVertexAttribPointer(shaderVars[ZSH_ATEX], 2, GL_FLOAT, GL_FALSE, sizeof(zVertex2D), &vertices->u);
    //glDisable(GL_SCISSOR_TEST);
    glScissor(scissor.x, zView::fbo ? scissor.y : (screen.h - scissor.extent(true)), scissor.w, scissor.h);
    glUniformMatrix4fv(shaderVars[ZSH_WMTX], 1, false, wmtx);
}

void zViewManager::setColorFilter(zView* view, const zColor& _color) {
    u32 cf(ZCF_NORMAL);
    if(view) {
        if(focus == view && view->isFocusableInTouch()) {
            cf = ZCF_FOCUSED;
        } else {
            auto sts(view->status);
            auto s(sts & (ZS_DISABLED | ZS_TAP));
            if(s) cf = ((s & ZS_DISABLED) ? ZCF_DISABLED : ((sts & ZS_TAP_MASK) >> 8));
        }
    }
    glUniformMatrix4fv(shaderVars[ZSH_UFLT], 1, false, filterMtxs[cf]);
    glUniform4fv(shaderVars[ZSH_UCOL], 1, _color);
}

zTexture *zViewManager::loadResourceTexture(u32 _id, zTexture *_t) const {
    auto res(theme->findString(_id));
    return (res.isNotEmpty() ? cache->get(res, _t) : nullptr);
}

void zViewManager::changeFocus(zView* view) {
    if(focus == view) return;
    if(focus) {
        // обновить каретку
        if(caret) caret->update(nullptr, 0, 0, 0);
        focus->onChangeFocus(false);
    }
    auto isTouchable(false);
    if(view) {
        view->onChangeFocus(true);
        isTouchable = view->isFocusableInTouch();
    }
    focus = view;
    // вызвать/спрятать клавиатуру
    showSoftKeyboard(view ? view->id : 0, isTouchable);
}

void zViewManager::showSoftKeyboard(u32 idOwner, bool _show) {
    keyboard->show(idOwner, _show);
}

u8* zImageCache::load(cstr _name) {
    u8* ptr; zStringUTF8 name(_name);
    if(name.indexOf("/") == -1) {
        // загрузить изображение из активов и скопировать ее в текстуру
        ptr = manager->assetFile("textures/" + name + ".ttl", nullptr);
    } else {
        zFile f(name, true); ptr = (u8*)f.readn();
    }
    return ptr;
}

zTexture* zImageCache::get(cstr _name, zTexture* t) {
    recycle(t);
    // 1. проверить такая уже есть?
    auto idx(tex.indexOf(_name));
    if(idx != -1) {
        auto _tx(&tex[idx]);
        _tx->millis = z_timeMillis();
        use++; _tx->tex->ref++;
        return _tx->tex;
    }
    // 2. создать новую
    zTexture* tx; auto ptr(load(_name));
    if(ptr) {
        tx = new zTexture(_name, ptr); delete ptr;
    } else {
        ILOG("Ошибка при загрузке текстуры - %s", _name);
        return get("znull", nullptr);
    }
    tx->ref++; use++;
    auto _size(tx->getSize()); int szTmp(_size.w * _size.h * 4), szTex(szTmp);
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
    DLOG("place %i-%i use:%i %s", maxSize, curSize, use, _name);
    return tx;
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
    if(tx) {
        auto idx(tex.indexOf(tx->name));
        // уменьшаем ссылку
        if(--tx->ref < 0) {
            // уже удалили - ошибка в системе!!!(просто удаляем)
            ILOG("texture already removed: %s", tx->name.str());
            delete tx;
        }
        if(idx == -1) {
            // нет в кэше
            ILOG("texture not cache: %s", tx->name.str());
        } else {
            use--;
        }
    }
}

void zImageCache::clear() {
    for(auto& c : tex) delete c.tex;
    tex.clear(); curSize = 0;
}

void zImageCache::info() {
    DLOG("Кэш текстур: используемые - %i, всего - %i, память - %i, макс - %i", use, tex.size(), curSize, maxSize);
    if(use) {
        for(auto& t : tex) { DLOG("%s", t.tex->info().str()); }
    }
}

void zImageCache::update() {
    for(auto& t: tex) {
        auto ptr(load(t.tex->name));
        if(ptr) { t.tex->makeTexture(ptr); delete ptr; }
    }
}

zImageCache::~zImageCache() {
    clear();
}
