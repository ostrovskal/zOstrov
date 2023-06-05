//
// Created by User on 18.04.2023.
//

#pragma once

class zTextPaint;
class zTexture final {
public:
#pragma pack(push, 1)
    struct HEAD_TTL {
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
    zTexture() { tiles = new u16[6]; count = 1; memset(tiles, 0, 12); }
    // конструктор
    zTexture(cstr _name, u8* ptr, u32 size) : name(_name) { glGenTextures(1, &id); makeTexture(ptr, size); }
    // деструктор
    ~zTexture() { release(); SAFE_DELETE(tiles); count = 0; }
    // создать текстуру
    void makeTexture(u8* ptr, u32 size);
    // сохранить(tga)
    bool save(czs& path) const;
    // вернуть количество тайлов
    int getCountTiles() const { return count; }
    // установить/сбросить целевую текстуру
    void setFBO(bool set, bool clear);
    // включение/отключение фильтрации
    void enableFilter(bool set) const;
    // создание текстуры для рендеринга
    u32 makeFBO(int width, int height);
    // проверка того, что тайл является патчем
    bool isTilePatch9(u32 tile) const { auto ptr(getTile(tile)); return ptr[4] | ptr[5]; }
    // создание "пустой" текстуры с параметрами
    static u32 makeEmpty(int internalFormat, int format, int width, int height);
    // диагностика
    zString info() { return z_fmt("ids:%i(%i) - name:(%s(%i)) size:%ix%i", id, idFBO, name.str(), ref, tiles[2], tiles[3]); }
    // вернуть габариты тайла
    u16* getTile(u32 tile) const { return (tile < count ? &tiles[tile * 6] : nullptr); }
    // вернуть обратную величину габаритов
    cszf& getReverseSize() const { return _rsize; }
    // вернуть габариты
    cszi& getSize() const { return _size; }
    // вернуть параметры глифа
    u16* paramGlyph(int ch) const { ch -= 32; return ((tiles && ch < count) ? tiles + ch * 6 : nullptr); }
    // вернуть ширину глифа
    int widthGlyph(int ch, float factor) const;
    // проверка на размер текстуры
    bool isEqual(int width, int height) const { return (tiles[2] == width && tiles[3] == height); }
    // реализация
    void release();
    // оператор сравнения по имени
    bool operator == (cstr n) const { return name == n; }
    // имя
    zStringUTF8 name{};
    // количество ссылок
    i32 ref{0};
    // базовая линия шрифта
    i32 ascent{0}, descent{0};
    // идентификатор текстуры
    u32 id{0};
    // идентификатор FBO
    u32 idFBO{0};
protected:
    // обратная величина габаритов текстуры
    szf _rsize{};
    // габариты текстуры
    szi _size{};
    // количество тайлов
    u16 count{0};
    // тайлы
    u16* tiles{nullptr};
};
