
#pragma once

#include <zstandard/zstandard.h>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include <android/window.h>
#include <android/input.h>
#include <android/native_activity.h>
#include <android/asset_manager.h>
#include <android/configuration.h>

#include <curl2/curl.h>

static int gravity_shift[] = { 31, 31, 0, 1 };

constexpr int MSG_ALL              = 0;
constexpr int MSG_ANIM             = 1;
constexpr int MSG_EDIT             = 2;
constexpr int MSG_EDIT_FINISH      = 3;

constexpr int VIEW_MATCH		   = -1;
constexpr int VIEW_WRAP			   = -2;

constexpr int MEASURE_MASK_SIZE	   = 0x3fffffff;
constexpr int MEASURE_MASK_MODE	   = 0xC0000000;
constexpr int MEASURE_EXACT  	   = 0x80000000;
constexpr int MEASURE_MOST  	   = 0x40000000;
constexpr int MEASURE_UNDEF  	   = 0x00000000;

constexpr int PIVOT_X               = 1;
constexpr int PIVOT_Y               = 2;
constexpr int PIVOT_ALL             = 3;

/* background */
#define DRW_BK                      0
#define DRW_FK                      1
#define DRW_TXT                     2
#define DRW_ICON                    3
#define DRW_SHADE                   2
/* selector */
#define DRW_SEL                     2
/* divider */
#define DRW_DIV                     3
/* fbo */
#define DRW_FBO                     4

enum { TOUCH_CONTINUE, TOUCH_FINISH, TOUCH_ACTION, TOUCH_STOP };

// гравитация
#define ZS_GRAVITY_START 	        0x00000001U	// гор. выравнивание элемента	- по левому краю
#define ZS_GRAVITY_END   	        0x00000002U	// гор. выравнивание элемента	- по правому краю
#define ZS_GRAVITY_HCENTER 	        0x00000003U	// гор. выравнивание элемента	- по центру
#define ZS_GRAVITY_TOP 		        0x00000004U	// верт. выравнивание элемента	- по верхней границе
#define ZS_GRAVITY_BOTTOM  	        0x00000008U	// верт. выравнивание элемента	- по нижней границе
#define ZS_GRAVITY_VCENTER	        0x0000000CU	// верт. выравнивание элемента	- по центру
#define ZS_GRAVITY_CENTER  	        0x0000000FU	// верт. выравнивание элемента	- по нижней границе
#define ZS_GRAVITY_HORZ             (ZS_GRAVITY_START | ZS_GRAVITY_END | ZS_GRAVITY_HCENTER)
#define ZS_GRAVITY_VERT             (ZS_GRAVITY_TOP | ZS_GRAVITY_BOTTOM | ZS_GRAVITY_VCENTER)
#define ZS_GRAVITY_MASK             (ZS_GRAVITY_VERT | ZS_GRAVITY_HORZ)

// стили
#define ZS_VISIBLED        	        0x80000000U	// невидимо
#define ZS_DISABLED      	        0x40000000U	// заблокировано
#define ZS_MEASURE	      	        0x20000000U	// полное обновление
#define ZS_MEASURE_SET              0x10000000U // признак того, что габариты были установлены
#define ZS_CLICKABLE		        0x08004000U	// признак возможности клика
#define ZS_LCLICKABLE		        0x08204000U	// признак возможности долгого клика
#define ZS_ELLIPSIS                 0x04000000U // признак ... при завершении текста(если он не поместился)
#define ZS_READ_ONLY 		        0x02000000U	// только для чтения
#define ZS_TIPS				        0x01000000U	// признак отображени¤ подсказки
#define ZS_MODAL   			        0x00800000U	// модальный
#define ZS_TAP	        	        0x00400000U	// прикоснулись
#define ZS_CHECKED			        0x00100000U // признак выбора
#define ZS_NOWRAP                   0x00080000U // признак однострочного текста
#define ZS_LAYOUT                   0x00040000U // признак вызова layout
#define ZS_FOCUSABLE_IN_TOUCHABLE   0x00020000U // признак получать фокус при касании
#define ZS_SYSTEM                   0x00010000U // способен принимать сис. события
#define ZS_FOCUSABLE                0x00008000U // признак возможности фокуса
#define ZS_TOUCHABLE                0x00004000U // признак возможности касания
#define ZS_FBO                      0x00002000U // отрисовка в FBO
#define ZS_DESTROY                  0x00001000U // признак удаления объекта
#define ZS_DIRTY_LAYER              0x00000800U // признак перерисовки слоя
//#define ZS_                         0x00000400U // undef
// тип тапа
#define ZS_INVERTED   		        0x00000100U // помечать
#define ZS_PRESSED   		        0x00000200U // нажимать
#define ZS_HOVERED 			        0x00000300U // подсвечивать
#define ZS_TAP_MASK                 (ZS_HOVERED | ZS_INVERTED | ZS_PRESSED)

// значения для атрибутов стилей
// отображение
// поведение
#define ZS_BEHAVIOR_MASK            (ZS_CLICKABLE | ZS_LCLICKABLE | ZS_FOCUSABLE_IN_TOUCHABLE | ZS_FOCUSABLE | ZS_TOUCHABLE)

// масштабирование фореграунда
#define ZS_SCALE_MIN                16
#define ZS_SCALE_MAX                32
#define ZS_SCALE_WIDTH              48
#define ZS_SCALE_HEIGHT             64
#define ZS_SCALE_MASK               (ZS_SCALE_MIN | ZS_SCALE_MAX | ZS_SCALE_WIDTH | ZS_SCALE_HEIGHT)
// фигура обрезки
#define ZS_SHAPE_NONE               0
#define ZS_SHAPE_OVAL               1
#define ZS_SHAPE_ROUND              2

// декорации
#define ZS_GLOW                     1
#define ZS_VSCROLLBAR               2
#define ZS_HSCROLLBAR               4
#define ZS_SCROLLBAR                (ZS_VSCROLLBAR | ZS_HSCROLLBAR)
#define ZS_DECORATE_MASK            (ZS_TIPS | ZS_ELLIPSIS | ZS_READ_ONLY | ZS_CHECKED | ZS_DISABLED)

#define ZS_STATES                   (ZS_DIRTY_LAYER | ZS_VISIBLED | ZS_DISABLED | ZS_CHECKED)

// режим для слайдера/прогресса/формы/чарта/таблицы/редактора
// слайдер
#define ZS_SLIDER_NONE                  0
#define ZS_SLIDER_SCALE                 1
#define ZS_SLIDER_ROTATE                2
#define ZS_MODE_SLIDER_MASK             (ZS_SLIDER_NONE | ZS_SLIDER_SCALE | ZS_SLIDER_ROTATE)
// прогресс
#define ZS_PROGRESS_LINEAR              0
#define ZS_PROGRESS_CIRCULAR            1
#define ZS_MODE_PROGRESS_MASK           (ZS_PROGRESS_LINEAR | ZS_PROGRESS_CIRCULAR)
// форма
#define ZS_FORM_NONE                    0
#define ZS_FORM_OK                      1
#define ZS_FORM_YES_NO                  2
#define ZS_FORM_YES_NO_DEF              3
#define ZS_FORM_CLOSE                   4
#define ZS_FORM_TERMINATE               8
#define ZS_MODE_FORM_MASK               (ZS_FORM_NONE | ZS_FORM_OK | ZS_FORM_YES_NO | ZS_FORM_YES_NO_DEF)
// чарт
#define ZS_CHART_DIAGRAMM               0
#define ZS_CHART_GRAPH                  1
#define ZS_CHART_CIRCULAR               2
#define ZS_CHART_STRETCH                4
#define ZS_CHART_SCROLL                 8
#define ZS_MODE_CHART_MASK              (ZS_CHART_DIAGRAMM | ZS_CHART_GRAPH | ZS_CHART_CIRCULAR)
// таблица
#define ZS_GRID_SPACING                 1 // пространство между ячейками
#define ZS_GRID_CELL                    2 // растягивать ячейки
#define ZS_GRID_UNIFORM                 3 // пространство между ячейками – расстояние
#define ZS_MODE_GRID_MASK               (ZS_GRID_SPACING | ZS_GRID_CELL | ZS_GRID_UNIFORM)
// редактор
#define ZS_EDIT_TEXT                    0
#define ZS_EDIT_NUMERIC                 1
#define ZS_EDIT_PHONE                   2
#define ZS_EDIT_MAIL                    3
#define ZS_EDIT_PASSWORD                4
#define ZS_EDIT_ZX_DATA                 5
#define ZS_MODE_EDIT_MASK               (ZS_EDIT_TEXT | ZS_EDIT_NUMERIC | ZS_EDIT_PHONE | ZS_EDIT_MAIL | ZS_EDIT_PASSWORD | ZS_EDIT_ZX_DATA)
// разделитель
#define ZS_DIVIDER_BEGIN                1
#define ZS_DIVIDER_MIDDLE               2
#define ZS_DIVIDER_END                  4
#define ZS_MODE_DIVIDER_MASK            (ZS_DIVIDER_BEGIN | ZS_DIVIDER_MIDDLE | ZS_DIVIDER_END)
// текст
#define ZS_TEXT_NORMAL                  0
#define ZS_TEXT_BOLD                    1
#define ZS_TEXT_ITALIC                  2
#define ZS_TEXT_BOLD_ITALIC             3
#define ZS_TEXT_STRIKE                  4
#define ZS_TEXT_UNDERLINE               8
#define ZS_MODE_TEXT_MASK               (ZS_TEXT_STRIKE | ZS_TEXT_UNDERLINE | ZS_TEXT_NORMAL | ZS_TEXT_BOLD | ZS_TEXT_ITALIC | ZS_TEXT_BOLD_ITALIC)
#define ZS_MODE_TEXT_FLAGS_MASK         (0)

enum MenuItem { menuEnd, menuPopupBegin, menuPopupEnd, menuItem };
enum MenuAction { actNever = 0, actAlways = 1, actIfRoom = 2 };
enum MenuGroup { menuText = 4, menuRadio = 8, menuCheck = 16, menuPopup = 32 };

struct zMenuItem {
    // тип - группа/элемент
    u32 type;
    // идентификатор элемента/идентификатор текста
    i32 id, title;
    // идентификатор изображения
    i32 image;
    // тип(текст/радио/флажок)|размещение(никогда/всегда/если есть место)
    u32 action;
};

class MENUITEM;
class POPUPMENU {
    friend class zActionBar;
public:
    POPUPMENU() { }
    POPUPMENU(int _id): id(_id) { }
    ~POPUPMENU() { reset(); }
    void reset() { children.clear(); }
    void add(MENUITEM* item) { children += item; }
    int size() const { return children.size(); }
    MENUITEM* getItem(int pos) const { return ((pos >= 0 && pos < size()) ? children[pos] : nullptr); }
    MENUITEM* idFind(int _id) const { return getItem(children.indexOf(_id)); }
    MENUITEM* atFind(int _idx) const { return getItem(_idx); }
    void setChecked(int _id, bool _set);
    bool isNotEmpty() const { return children.size() > 1; }
    int id{0};
protected:
    zArray<MENUITEM*> children{};
};

class MENUITEM {
    friend class zActionBar;
public:
    MENUITEM() { }
    MENUITEM(int _id, int _img, int _txt, u32 _flg, POPUPMENU* _pop);
    ~MENUITEM() { SAFE_DELETE(pop); }
    bool operator==(int _id) const { return id == _id; }
    void reset();
    bool isCheckBox() const { return flags & menuCheck; }
    bool isRadioButton() const { return flags & menuRadio; }
    bool isPopup() const { return flags & menuPopup; }
    bool isAlways() const { return (flags & 3) == actAlways; }
    bool isNever() const { return (flags & 3) == actNever; }
    bool isText() const { return flags & menuText; }
    bool isEnabled() const { return enable; }
    bool isChecked() const { return checked; }
    bool isVisibled() const { return visible; }
    void setText(cstr _text) { title = _text; }
    void setEnabled(bool _set) { enable = _set; }
    void setVisibled(bool _set) { visible = _set; }
    void setChecked(bool _set) { checked = _set; }
    const zString& getText() const { return title; }
    // идентификатор/изображение
    i32 id{0}, image{0};
protected:
    // указатель на попап меню
    POPUPMENU* pop{nullptr};
    // признак выбранного элемента
    bool checked{false}, enable{true}, visible{true};
    // текст
    i32 text{0};
    // флаги
    u32 flags{0};
    // заголовок
    zString title;
};

// основные компоненты
#include "zstandard/zGL.h"
#include "zstandard/zJniHelper.h"
#include "zstandard/zInterpolator.h"
#include "zTexture.h"
#include "zDrawable.h"
#include "zTheme.h"
#include "zTouch.h"
#include "zHandler.h"
#include "zView.h"
#include "zViewManager.h"
#include "zstandard/zStringUTF8.h"

