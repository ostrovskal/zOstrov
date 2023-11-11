
#include "zostrov/zCommon.h"

int zTouch::getEventAction(AInputEvent* event, int* _idx, int* _id) {
    auto action(AMotionEvent_getAction(event));
    auto idx((action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
    auto id(AMotionEvent_getPointerId(event, idx));
    if(_idx) *_idx = idx; if(_id) *_id = id;
    return (action & AMOTION_EVENT_ACTION_MASK);
}

bool zTouch::intersect(AInputEvent* event, crti& r) {
    int idx; auto action(getEventAction(event, &idx));
    if(action == AMOTION_EVENT_ACTION_POINTER_DOWN || action == AMOTION_EVENT_ACTION_DOWN) {
        int x((int)AMotionEvent_getX(event, idx)), y((int) AMotionEvent_getY(event, idx));
        return r.contains(x, y);
    }
    return false;
}

bool zTouch::event(AInputEvent* event, crti& r) {
    WORK_THREAD();
    bool result(false); float x, y; int idx, _id;
    // событие
    act = getEventAction(event, &idx, &_id);
    // врем¤
    ctm = AMotionEvent_getEventTime(event);
    x = AMotionEvent_getX(event, idx);
    y = AMotionEvent_getY(event, idx);
    switch(act) {
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
        case AMOTION_EVENT_ACTION_DOWN:
            own = nullptr;
            if(r.contains((int)x, (int)y)) {
                // убрать признак отпускания
                flags &= ~TOUCH_RELEASE; coord = r;
                // установить признак захвата
                id = _id; flags |= TOUCH_CAPTURE;
                dtm = btm = ctm; result = true;
                tm = ((ctm - tm) / 1000000);
                if(tm < 300) {
                    // и если старая позиция и новая близко друг к другу
                    //ILOG("x0:%f x1:%f y0:%f y1:%f %f %f", bpt.x, x, bpt.y, y, fabs(bpt.x - x), fabs(bpt.y - y));
                    if(fabs(bpt.x - x) <= 10.0f && fabs(bpt.y - y) <= 10.0f)
                        flags |= TOUCH_DOUBLE_CLICKED;
                }
                dblClick = (flags & TOUCH_DOUBLE_CLICKED) != 0;
                bpt.x = x; bpt.y = y;
                cpt.x = x; cpt.y = y;
            }
            break;
        case AMOTION_EVENT_ACTION_POINTER_UP:
        case AMOTION_EVENT_ACTION_UP:
            // если было касание с определенным идентификатором
            if(id == _id && (flags & TOUCH_CAPTURE)) {
                // установить признак отпускания касания
                flags |= TOUCH_RELEASE;
                id = -1; result = true;
                cpt.x = x; cpt.y = y;
                // убрать признак захвата
                tm = ctm; flags &= ~TOUCH_CAPTURE;
            }
            break;
        case AMOTION_EVENT_ACTION_CANCEL:
            result = true; flags = TOUCH_RELEASE;
            cpt.x = cpt.y = MAXFLOAT;
            break;
        case AMOTION_EVENT_ACTION_MOVE:
            if(flags & TOUCH_CAPTURE) {
                auto count(AMotionEvent_getPointerCount(event));
                for(int i = 0; i < count; i++) {
                    if(id == AMotionEvent_getPointerId(event, i)) {
                        cpt.x = AMotionEvent_getX(event, i);
                        cpt.y = AMotionEvent_getY(event, i);
                        result = true;
                        break;
                    }
                }
            }
            break;
    }
    return result;
}

void zTouch::reset() {
    btm = ctm = 0UL; id = -1; oldAngle = 10000.0f; own = nullptr;
    //bpt.set(MAXFLOAT, MAXFLOAT); cpt.set(MAXFLOAT, MAXFLOAT);
    act = AMOTION_EVENT_ACTION_CANCEL; flags = 0;
}

// клик в области [rc]
bool zTouch::click(crti& r) const {
    if(isReleased()) {
        return r.contains((int)bpt.x, (int)bpt.y) && r.contains((int)cpt.x, (int)cpt.y);
    }
    return false;
}

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
        // при ложном срабатывании
//        if((ctm - dtm) < 50) sz.empty();
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

void zTouch::direction(cszi& cell, cptf& center, bool is4, const std::function<void(int)>& fn) {
    auto dir(0);
    if(!isReleased()) {
        if(delta(cell)) {
            static int angl[] = { 337, 23, 68, 113, 158, 203, 248, 293, 338 };
            auto a((int)rotate(center, cpt) % 360);
            for(int i = 0; i < 8; i++) {
                if(a >= angl[i] && a < angl[i + 1]) { dir = i + 1; dir |= is4; break; }
            }
        }
    }
    fn(dir);
    resetPosition();
}
