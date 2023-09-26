//
// Created by User on 18.04.2023.
//

#pragma once

class zTextPaint;
class zTexture final {
public:
#pragma pack(push, 1)
    struct TILE_TTL {
        TILE_TTL() { }
        TILE_TTL(u16* ptr) : rect(ptr), patch9(*(int*)(ptr + 4)), name((char*)(ptr + 6)) { }
        bool operator == (cstr _name) const { return _name == name; }
        int getLeft() const { return (signed char)l; }
        int getRight() const { return (signed char)r; }
        rti rect{};
        union {
            int patch9{0};
            struct { char l, r; };
        };
        zString8 name{};
    };
    struct HEAD_TTL {
        // сигнатура
        u32 sign;
        // габариты изображения
        u16 width, height;
        // кол-во тайлов
        u16 count;
        // размер упакованного rgb(jpeg)
        u32 sizeRgb;
        // размер упакованной alpha
        int sizeAlpha;
        // базовая линия (для шрифта)
        u16 ascent, descent;
    };
#pragma pack(pop)
    zTexture() { tiles += TILE_TTL(); }
    // конструктор
    zTexture(cstr _name, u8* ptr, u32 size) : name(_name) { glGenTextures(1, &id); makeTexture(ptr, size); }
    // деструктор
    ~zTexture() { release(); tiles.clear(); }
    // создать текстуру
    void makeTexture(u8* ptr, u32 size);
    u32 makeTexture(int width, int height, int _type);
    // сохранить(tga)
    bool save(czs& path) const;
    // вернуть количество тайлов
    int getCountTiles() const { return tiles.size(); }
    // установить/сбросить целевую текстуру
    void setFBO(bool set, bool clear);
    // включение/отключение фильтрации
    void enableFilter(bool set, int clamp = GL_CLAMP_TO_EDGE) const;
    // создание текстуры для рендеринга
    u32 makeFBO(int width, int height);
    u32 makeOES(int width, int height);
    // проверка того, что тайл является патчем
    bool isTilePatch9(u32 tile) const { auto ptr(getTile(tile)); return ptr->patch9; }
    // признак OES текстуры
    bool isCustom() const { return custom; }
    // создание "пустой" текстуры с параметрами
    void makeEmpty(int internalFormat, int format, int width, int height, int type);
    // диагностика
    zString info() { return z_fmt("ids: %i(%i) - name: %s(%i) size: %ix%i", id, idFBO, name.str(), ref, _size.w, _size.h); }
    // вернуть тайл
    TILE_TTL* getTile(u32 tile) const { return (tile < tiles.size() ? &tiles[tile] : nullptr); }
    int getTile(czs& _name) const { return tiles.indexOf(_name); }
    // вернуть обратную величину габаритов
    cszf& getReverseSize() const { return _rsize; }
    // вернуть габариты
    cszi& getSize() const { return _size; }
    // вернуть параметры глифа
    TILE_TTL* paramGlyph(int ch) const { return getTile(ch - 32); }
    // проверка на размер текстуры
    bool isEqual(int width, int height) const { return (tiles[0].rect.w == width && tiles[0].rect.h == height); }
    // реализация
    void release();
    // оператор сравнения по имени
    bool operator == (cstr n) const { return name == n; }
    // имя
    zString8 name{};
    // количество ссылок
    i32 ref{0};
    // базовая линия шрифта
    i32 ascent{0}, descent{0};
    // идентификатор текстуры
    u32 id{0};
    // идентификатор FBO/OES
    u32 idFBO{0};
    // тип текстуры
    i32 type{GL_TEXTURE_2D};
protected:
    // своя текстура
    bool custom{false};
    // обратная величина габаритов текстуры
    szf _rsize{};
    // габариты текстуры
    szi _size{};
    // тайлы
    zArray<TILE_TTL> tiles{};
};
