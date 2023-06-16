
#include "zssh/zCommon.h"
#include "zssh/zHandler.h"

void zHandler::send(zView* view, int what, u64 millis, int arg) {
	erase(view, what);
	for(auto& msg : looper) {
		if(!msg.millis) {
			msg.view = view; msg.what = what;
			msg.millis = (i64)(z_timeMillis() + millis);
			msg.arg = arg; use++; break;
		}
	}
}

void zHandler::info() {
	for(int i = 0 ; i < 10; i++) {
		auto& msg(looper[i]);
		if(!msg.millis) continue;
		DLOG("%i - view:%llx %s", i, msg.view, msg.view ? msg.view->typeName() : "null");
	}
}

HANDLER_MESSAGE* zHandler::obtain() {
	if(use) {
		auto ms(z_timeMillis());
		for(auto& msg : looper) {
			if(!msg.millis) continue;
			if(ms < msg.millis) continue;
			msg.millis = 0; use--;
			return &msg;
		}
	}
	return nullptr;
}

void zHandler::erase(zView* view, int what) {
	if(use) {
		for(auto& msg : looper) {
			if(!msg.millis) continue;
			if(msg.view != view) continue;
			// если what == 0 - тогда удаляем все
			if(what && what != msg.what) continue;
			msg.millis = 0; if(!--use) break;
		}
	}
}

HANDLER_MESSAGE* zHandler::get(zView* view, int what) {
	if(use) {
		for(auto& msg : looper) {
			if(msg.millis == 0 || msg.view != view) continue;
			if(what != msg.what) continue;
			return &msg;
		}
	}
	return nullptr;
}

void zHandler::clear() {
    for(auto& m : looper) m.millis = 0;
	use = 0;
}

void zTouchHandler::erase(zView *view) {
	if(use) {
		for(auto& msg : looper) {
			if(!msg.valid || msg.view != view) continue;
			msg.valid = false; use--; break;
		}
	}
}

void zTouchHandler::clear() {
    for(auto& m : looper) {
        m.valid = false; m.touch.reset();
    }
	use = 0;
}

void zTouchHandler::info() {
	DLOG("count:%i", use);
	for(auto& msg : looper) {
		if(!msg.valid) continue;
		DLOG("%s(%i)", msg.view->typeName(), msg.view->id);
	}
}

HANDLER_TOUCH_MESSAGE *zTouchHandler::obtain() {
	for(auto& msg : looper) {
		if(msg.valid) continue;
		msg.valid = true; use++;
		msg.touch.reset();
		return &msg;
	}
	return nullptr;
}

HANDLER_TOUCH_MESSAGE *zTouchHandler::get(zView *view) {
	if(use) {
		for(auto& msg : looper) {
			if(msg.valid && msg.view == view) {
			    return &msg;
			}
		}
	}
	return nullptr;
}
