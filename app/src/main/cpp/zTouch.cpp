
#include "zssh/zCommon.h"

int zTouch::getEventAction(AInputEvent* event, int* idx) {
    auto action(AMotionEvent_getAction(event));
    if(idx) *idx = ((action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
    return (action & AMOTION_EVENT_ACTION_MASK);
}

bool zTouch::intersect(AInputEvent* event, crti& r) {
    int idx;
    auto action(getEventAction(event, &idx));
    if(action == AMOTION_EVENT_ACTION_POINTER_DOWN || action == AMOTION_EVENT_ACTION_DOWN) {
        int x((int) AMotionEvent_getX(event, idx)), y((int) AMotionEvent_getY(event, idx));
        return r.contains(x, y);
    }
    return false;
}

bool zTouch::event(AInputEvent* event, crti& r) {
    bool result(false); float x, y; int idx;
    // врем¤
    auto time(AMotionEvent_getEventTime(event));
    // событие
    auto action(getEventAction(event, &idx));
    // идентификатор касания
    auto _id(AMotionEvent_getPointerId(event, idx));
    act = action; coord = r; ctm = time;
    switch(action) {
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
        case AMOTION_EVENT_ACTION_DOWN:
            x = AMotionEvent_getX(event, idx);
            y = AMotionEvent_getY(event, idx);
            if(id == -1 && r.contains((int)x, (int)y)) {
                id = _id;
                // убрать признак отпускани¤
                flags &= ~TOUCH_RELEASE;
                // установить признак захвата
                flags |= TOUCH_CAPTURE;
                cpt.x = bpt.x = x;
                cpt.y = bpt.y = y;
                btm = time; result = true;
            }
            break;
        case AMOTION_EVENT_ACTION_POINTER_UP:
        case AMOTION_EVENT_ACTION_UP:
            // если было касание с определенным идентификатором
            if(id == _id && flags & TOUCH_CAPTURE) {
                flags &= ~TOUCH_INIT_DOUBLE_PRESSED;
                // установить признак отпускани¤ касани¤
                flags |= TOUCH_RELEASE;
                id = -1; result = true;
            }
            break;
        case AMOTION_EVENT_ACTION_CANCEL:
            DLOG("AMOTION_EVENT_ACTION_CANCEL %i", id);
            result = true; flags = TOUCH_RELEASE;
            break;
        default:
            break;
    }
    // было отпускание?
    if(flags & TOUCH_RELEASE) {
        tm = ctm;
        // убрать признак захвата
        flags &= ~TOUCH_CAPTURE;
        flags |= TOUCH_INIT_DOUBLE_PRESSED;
    }
    if(flags & TOUCH_INIT_DOUBLE_PRESSED) {
        if(flags & TOUCH_CAPTURE) {
            tm = ((ctm - tm) / 1000000);
            flags |= (tm < 300) * TOUCH_DOUBLE_CLICKED;
        }
    }
    if(action == AMOTION_EVENT_ACTION_MOVE) {
        auto count(AMotionEvent_getPointerCount(event));
        for(int i = 0; i < count; i++) {
            if(id == AMotionEvent_getPointerId(event, i)) {
                if(flags & TOUCH_CAPTURE) {
                    cpt.x = AMotionEvent_getX(event, i);
                    cpt.y = AMotionEvent_getY(event, i);
                    ctm = time; result = true;
                    break;
                }
            }
        }
    }
    return result;
}

void zTouch::reset() {
    btm = ctm = 0UL; id = -1; oldAngle = 10000.0f;
    bpt.empty(); cpt.empty();
    flags &= TOUCH_INIT_DOUBLE_PRESSED; act = AMOTION_EVENT_ACTION_CANCEL;
}

// клик в области [rc]
bool zTouch::click() const {
    if(isReleased()) {
        return coord.contains((int)bpt.x, (int)bpt.y) && coord.contains((int)cpt.x, (int)cpt.y);
    }
    return false;
}

void zTouch::rotate(cszi& cell, cptf& center, const std::function<void(float, float, bool)>& fn) {
    auto released(isReleased());
    if(delta(cell) || released) {
        auto a(rotate(center, cpt));
        if(abs(oldAngle - 10000.0f) < Z_EPSILON) oldAngle = a;
        fn(a, a - oldAngle, !released);
        oldAngle = released ? 10000.0f : a;
        resetPosition();
    }
}

void zTouch::drag(cszi& cell, const std::function<void(cszi&, bool)>& fn) {
    auto released(isReleased()), _delta(!released && delta(cell));
    // если величина сдвига больше ¤чейки или отпустили
    if(_delta || released) {
        //sz.w *= cell.w; sz.h *= cell.h;
        // при ложном срабатывании
        if(delayed(50)) sz.empty();
        fn(sz, released);
        resetPosition();
        resetTime();
    }
}

void zTouch::scale(zTouch* other, cszi& cell, const std::function<void(float, bool)>& fn) {
    auto released1(isReleased()), released2(other->isReleased());
    auto dsz1(!released1 && delta(cell));
    auto dsz2(!released2 && delta(cell));
    if((dsz1 || dsz2) || released1 || released2) {
        auto l(length(cell, other->cpt));
        if(len < 0.0f) len = l;
        fn(l / len, !released1 && !released2);
        resetPosition();
        other->resetPosition();
        if(released1) len = -1.0f;
    }
}

int zTouch::direction(cszi& cell, cptf& center, bool is4) {
    /*
        if(!isReleased()) {
            auto dir(DIR0);
            auto dx(ptCurrent.x - center.x), dy(ptCurrent.y - center.y);
            auto adx(abs(dx)), ady(abs(dy));
            if(adx > cell.w || ady > cell.h) {
                if(is4) {
                    dir = if(adx >= ady) if(dx < cell.w) DIRL else DIRR else if(dy < cell.h) DIRU else DIRD
                } else {
                    auto angle((int)rotate(center, ptCurrent));
                    for(r in 0..9) {
                        if(angle >= aranges[r] && angle < aranges[r + 1]) {
                            dir = dirs8[r]
                            break
                        }
                    }
                }
            }
            fn(dir);
        } else flags = 0;
    */
    return -1;
}
