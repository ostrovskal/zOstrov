//
// Created by User on 18.04.2023.
//

#pragma once

// чтобы не ругался на русские буквы
#pragma clang diagnostic ignored "-Winvalid-source-encoding"

struct zStyles;
struct zResource;
class zStyle;
class zTheme {
    friend class zStyle;
public:
    union data {
        cstr 	s;  // указатель на строку
        float 	f;  // вещественное
        u32   	u;  // целое
    };
    // конструктор
    zTheme();
    // установить новую тему(_styles - массив стилей темы, prefixes - префиксы языка ресурсов(например, ru-en), usr - массив массивов ресурсов, _usr_styles - доп.стили)
    bool setTheme(zStyle* _styles, czs& lang, zResource** _user_resources, zStyles* _user_styles);
    // найти атрибут в стилях
    const zStyle* findAttribute(u32 attr) const;
    // найти массив в ресурсах
    zArray<zStringUTF8> findArray(u32 _value) const { return zStringUTF8(getResource(_value)).split("|"); }
    // найти строку в ресурсах
    zStringUTF8 findString(u32 _value) const { return getResource(_value); }
    // значение определенного типа
    static data value;
    // имя темы
    zStringUTF8 themeName{};
    // цвет темы
    u32 themeColor{0xffffffff};
    // массив стилей
    zStyle* styles{nullptr};
    // пользовательские ресурсы
    zResource** user_resources{nullptr};
    // пользовательские стили
    zStyles* user_styles{nullptr};
protected:
    // заполнить значение в зависимости от типа
    static void fillValue(const zStyle* val);
    // найти ресурсы
    cstr getResource(u32 value) const;
    // найти стили
    zStyle* getStyles(u32 value) const;
    // массив префиксов
    zArray<zStringUTF8> prefixes;
};

class zStyle {
    friend class zTheme;
public:
    // аттрибут/значение
    u32 a, v;
    // вернуть целое
    u32 _int(u32 _attr, u32 def = 0) const;
    // вернуть вещественное
    float _float(u32 _attr, float def = 0.0f) const;
    // вернуть строку
    cstr _str(u32 _attr, cstr def = nullptr) const;
    // определить атрибут
    static const zStyle* resolveAttribute(const zStyle* _val);
    // перечислить все стили
    void enumerate(const std::function<void(u32)>& act) const;
protected:
    // найти и инициализировать атрибут
    const zStyle* findAttribute(u32 attr) const;
};

struct zStyles {
    u32 a{0};           // целое
    zStyle* s{nullptr}; // массив стилей
};

struct zResource {
    u32 a{0};           // атрибут
    cstr v{nullptr};    // строковое значение
};

// менеджер стилей(тема)
extern zTheme* theme;

#define ZT_THEME 	0x40000000U	// брать значение из темы
#define ZT_END		0x20000000U	// последний атрибут в стилях

#define ZTT_INT		0x00100000U	// атрибут является целым
#define ZTT_FLT		0x00200000U	// атрибут является вещественным
#define ZTT_DMN		0x00300000U	// атрибут является dp целым
#define ZTT_VEC		0x00400000U	// атрибут является dp ректом
#define ZTT_STR		0x01000000U // атрибут является строкой
#define ZTT_THM		0x02000000U	// атрибут используется при смене темы

#define ZTT_MASK	0x00700000U	// маска типа атрибута
#define ZTV_MASK	0x000fffffU	// маска значения атрибута
#define ZTA_MASK	0x03ffffffU	// маска атрибута

enum ViewStyles {
    // базовые атрибуты
    Z_SCALE_X					= (0 | ZTT_FLT), // масштаб X
    Z_SCALE_Y					= (1 | ZTT_FLT), // масштаб Y
    Z_ROTATE					= (2 | ZTT_FLT), // угол Z
    Z_ROTATE_X					= (3 | ZTT_FLT), // угол X
    Z_ROTATE_Y					= (4 | ZTT_FLT), // угол Y
    Z_TRANSLATE_X				= (5 | ZTT_FLT), // смещение по X
    Z_TRANSLATE_Y				= (6 | ZTT_FLT), // смещение по Y
    Z_ALPHA						= (7 | ZTT_FLT), // прозрачность
    Z_GRAVITY					= (8 | ZTT_INT), // гравитация
    Z_DURATION					= (9 | ZTT_INT), // скорость анимации
    Z_VISIBLED					= (10 | ZTT_INT), // отображение
    Z_BEHAVIOR					= (11 | ZTT_INT), // поведение
    Z_TAP					    = (12 | ZTT_INT), // тип тапа
    Z_DECORATE                  = (13 | ZTT_INT | ZTT_THM), // декорации
    Z_SIZE_TOUCH				= (14 | ZTT_VEC), // чувствительность касания
    Z_MODE						= (15 | ZTT_INT | ZTT_THM), // режим для слайдера/прогресса/формы/чарта/сетки/редактора
    Z_SIZE                      = (16 | ZTT_DMN), // минимальный/максимальный размер представления
    // отступы
    Z_MARGINS					= (17 | ZTT_VEC), // отступ от макета
    Z_PADDING					= (18 | ZTT_VEC), // внутренний отступ от бэкграунда
    // бэкграунд
    Z_BACKGROUND				= (19 | ZTT_INT | ZTT_THM),
    Z_BACKGROUND_COLOR			= (20 | ZTT_INT | ZTT_THM),
    Z_BACKGROUND_TILES			= (21 | ZTT_INT | ZTT_THM),
    Z_BACKGROUND_PADDING		= (22 | ZTT_VEC | ZTT_THM),
    // фореграунд
    Z_FOREGROUND				= (23 | ZTT_INT | ZTT_THM),
    Z_FOREGROUND_COLOR		    = (24 | ZTT_INT | ZTT_THM),
    Z_FOREGROUND_TILES			= (25 | ZTT_INT | ZTT_THM), // 4 тайла упакованных в целое
    Z_FOREGROUND_GRAVITY        = (26 | ZTT_INT),
    Z_FOREGROUND_SCALE          = (27 | ZTT_FLT | ZTT_THM),
    Z_FOREGROUND_PADDING		= (28 | ZTT_INT | ZTT_THM),
    // селектор
    Z_SELECTOR					= (29 | ZTT_INT | ZTT_THM),
    Z_SELECTOR_COLOR			= (30 | ZTT_INT | ZTT_THM),
    Z_SELECTOR_TILES			= (31 | ZTT_INT | ZTT_THM),
    Z_SELECTOR_PADDING			= (32 | ZTT_INT | ZTT_THM),
    // разделитель
    Z_DIVIDER					= (33 | ZTT_INT | ZTT_THM),
    Z_DIVIDER_COLOR				= (34 | ZTT_INT | ZTT_THM),
    Z_DIVIDER_TILES				= (35 | ZTT_INT | ZTT_THM),
    Z_DIVIDER_PADDING			= (36 | ZTT_INT | ZTT_THM),
    Z_DIVIDER_SIZE				= (37 | ZTT_VEC | ZTT_THM),
    Z_DIVIDER_TYPE				= (38 | ZTT_INT | ZTT_THM),
    // иконка
    Z_ICON    					= (39 | ZTT_INT | ZTT_THM),
    Z_ICON_COLOR				= (40 | ZTT_INT | ZTT_THM),
    Z_SPEED_TRACK				= (41 | ZTT_FLT),
    Z_ICON_SCALE				= (42 | ZTT_FLT | ZTT_THM),
    Z_ICON_GRAVITY				= (43 | ZTT_INT),
    // прокрутка
    Z_SCROLLBAR     			= (44 | ZTT_INT | ZTT_THM),
    Z_SCROLLBAR_TILES			= (45 | ZTT_INT | ZTT_THM),
    Z_SCROLLBAR_FADE			= (46 | ZTT_INT | ZTT_THM),
    Z_SCROLLBAR_SIZE			= (47 | ZTT_DMN | ZTT_THM),
    // текст
    Z_TEXT_FONT				    = (48 | ZTT_INT | ZTT_THM),
    Z_TEXT_SIZE					= (49 | ZTT_DMN),
    Z_TEXT_STYLE				= (50 | ZTT_INT),
    Z_TEXT_NOWRAP				= (51 | ZTT_INT),
    Z_TEXT_BACKGROUND			= (52 | ZTT_INT | ZTT_THM),
    Z_TEXT_BACKGROUND_COLOR		= (53 | ZTT_INT | ZTT_THM),
    Z_TEXT_BACKGROUND_TILES		= (54 | ZTT_INT | ZTT_THM),
    Z_TEXT_SHADOW_OFFS			= (55 | ZTT_VEC),
    Z_TEXT_DISTANCE				= (56 | ZTT_DMN),
    Z_TEXT_FOREGROUND_COLOR		= (57 | ZTT_INT | ZTT_THM),
    Z_TEXT_SHADOW_COLOR			= (58 | ZTT_INT | ZTT_THM),
    Z_TEXT_HIGHLIGHT_COLOR		= (59 | ZTT_INT | ZTT_THM),
    // таблица
    Z_LINES						= (60 | ZTT_INT),
    Z_SPACING_CELL				= (61 | ZTT_DMN),
    Z_SPACING_LINE				= (62 | ZTT_DMN),
    Z_CELL_SIZE					= (63 | ZTT_DMN),
    Z_CHART_FLAGS               = (64 | ZTT_INT),
    Z_IPADDING                  = (65 | ZTT_VEC),
    Z_FBO                       = (66 | ZTT_INT),
    Z_ICON_TILES                = (67 | ZTT_INT | ZTT_THM),
    Z_PRIMARY                   = (68 | ZTT_INT | ZTT_THM),
    Z_TEXT_LENGTH               = (69 | ZTT_INT),
    Z_SHAPE                     = (70 | ZTT_INT)
};

// значения для темы
enum ThemeStyles {
    Z_THEME_NAME					= (128 | ZTT_STR),
    Z_STYLES						= (129 | ZTT_INT),
    Z_BITMAP_COMMON				    = (130 | ZTT_INT),
    Z_BITMAP_ICONS				    = (131 | ZTT_INT),
    Z_COLOR_COMMON		            = (132 | ZTT_INT),
    Z_COLOR_HIGHLIGHT_TEXT   	    = (133 | ZTT_INT),
    Z_COLOR_TEXT_TEXT				= (134 | ZTT_INT),
    Z_COLOR_HEADER_TEXT			    = (135 | ZTT_INT),
    Z_COLOR_PROGRESS				= (136 | ZTT_INT),
    Z_COLOR_PROGRESS_TEXT			= (137 | ZTT_INT),
    Z_COLOR_PROGRESS_PRIMARY		= (138 | ZTT_INT),
    Z_COLOR_SLIDER_TEXT			    = (140 | ZTT_INT),
    Z_COLOR_TAB					    = (141 | ZTT_INT),
    Z_COLOR_TAB_TEXT				= (142 | ZTT_INT),
    Z_COLOR_MENU					= (143 | ZTT_INT),
    Z_COLOR_MENU_TEXT				= (144 | ZTT_INT),
    Z_COLOR_DIVIDER                 = (145 | ZTT_INT),
    Z_COLOR_SELECTOR                = (146 | ZTT_INT),
    Z_COLOR_HINT_TEXT               = (147 | ZTT_INT),
    Z_COLOR_MESSAGE_TEXT            = (148 | ZTT_INT),
    Z_COLOR_LINK_TEXT               = (149 | ZTT_INT),
    Z_PROGRESS_CIRCULAR 			= (150 | ZTT_INT),
    Z_PROGRESS_TILES				= (151 | ZTT_INT),
    Z_SLIDER_TILES			        = (152 | ZTT_INT),
    Z_FORM_TILE					    = (153 | ZTT_INT),
    Z_SLIDER_MODE                   = (154 | ZTT_INT),
    Z_FONT1                         = (155 | ZTT_INT),
    Z_FONT2                         = (156 | ZTT_INT),
    Z_FONT3                         = (157 | ZTT_INT)
};

#include "zR.h"
