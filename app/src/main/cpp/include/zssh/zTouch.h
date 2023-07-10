
#pragma once

// Флаги касания. Фиксация нажатия
#define TOUCH_CAPTURE               0x00000001UL
// Флаги касания. Двойной клик
#define TOUCH_DOUBLE_CLICKED        0x00000002UL
// Флаги касания. Отпуск нажатия
#define TOUCH_RELEASE               0x00000004UL
// Флаги касания. Инит двойного клика
#define TOUCH_INIT_DOUBLE_PRESSED   0x00000008UL

class zTouch {
public:
    zTouch() {}
    // инициализация события
    bool event(AInputEvent* event, crti& r);
    // Признак завершения касания
    bool isReleased() const { return (flags & TOUCH_RELEASE) != 0; }
    // Признак захвата касания
    bool isCaptured() const { return (flags & TOUCH_CAPTURE) != 0; }
    // Признак двойного клика
    bool isDblClicked() const { return (flags & TOUCH_DOUBLE_CLICKED) != 0; }
    // проверка на драг
    bool isDragging(bool vert) {
        if(!vert) return abs((int)cpt.x - bpt.x) > 5;
        return abs((int)(cpt.y - bpt.y)) > 5;
    }
    // Сброс в начальную позицию
    void resetPosition() { bpt = cpt; }
    // Сброс в начальное время
    void resetTime() { btm = ctm; }
    // Вычисление угла между центральной [center] и заданной [p] точками
    float rotate(ptf center, cptf& p) {
        auto a(atan2f((center.y - p.y), (center.x - p.x)) / 3.1415926535897932385f * 180.0f);
        return (a < 0.0f ? a + 360.0f : a);
    }
    // Вернуть координату точки [p] с учетом чувствительности [cell] и записать ее в [ret]
    ptf point(szi cell, ptf p) {
        return ptf(p.x / cell.w, p.y / cell.h);
    }
    // Вернуть признак времени задержки, с учетом максимального времени [limit] */
    bool delayed(u64 limit) const { return ((ctm - btm) / 1000000) >= limit; }
    // Вернуть признак того, что нажатие было только что */
    bool just() const { return (ctm - btm) < 5; }
    // Найти индекс области в который попадает точка [p] из массива областей [rcElems]
    i32 contains(const zArray<rti>& rects, cptf& p) const {
        for(int idx = 0 ; idx < rects.size(); idx++) {
            if(rects[idx].contains(p.x, p.y)) 
                return idx;
        }
        return -1;
    }
    static bool intersect(AInputEvent* event, crti& r);
    static int getEventAction(AInputEvent* event, int* idx = nullptr);
    // сброс
    void reset();
    // вернуть признак длинного нажатия
    bool isLongClick() const { return delayed(400); }
    // Двойной клик в области [rc]
    bool dblClick() { return isDblClicked(); }
    // Клик в области [rc]
    bool click() const;
    // Ротация вокруг центральной точки [center] и чувствительностью [cell]
    void rotate(cszi& cell, cptf& center, const std::function<void(float, bool)>& fn);
    // Перетаскивание для касания с чувствительностью [cell]
    void drag(cszi& cell, const std::function<void(cszi& offs, bool)>& fn);
    // Масштабирование для касаний с чувствительностью [cell]
    void scale(zTouch* other, cszi& cell, const std::function<void(float, bool)>& fn);
    // Определение направления
    int direction(cszi& cell, cptf& center, bool is4);
    // Величина смещения относительно начальной точки с учетом размера ячейки [cell]
    bool delta(cszi& cell) {
        sz.set((int)roundf((cpt.x - bpt.x) / (float)cell.w), (int)(roundf((cpt.y - bpt.y) / (float)cell.h)));
        return (abs(sz.w) > 0 || abs(sz.h) > 0);
    }
    // Длина "линии" между начальной [p] и текущей точкой с учетом размера ячейки [cell]
    float length(cszi& cell, cptf& p) {
        auto x((cpt.x - p.x) / cell.w); x *= x;
        auto y((cpt.y - p.y) / cell.h); y *= y;
        return sqrtf(x + y);
    }
    // Флаги операций
    u32 flags{0};
    // Тип касания
    u32 act{AMOTION_EVENT_ACTION_CANCEL};
    // Начальная координата касания
    ptf bpt{};
    // Текущая координата касания
    ptf cpt{};
    // Идентификатор касания
    i32 id{-1};
    // Начальное время касания
    i64 btm{0};
    // Текущее время касания
    i64 ctm{0};
    // координаты и габариты
    rti coord{};
    // Временное значение длины
    float len{0.0f};
    // Временное значение времени
    u64 tm{0};
    // временный размер
    szi sz{};
protected:
};
