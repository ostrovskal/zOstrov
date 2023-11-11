
#include "zostrov/zCommon.h"
#include "zostrov/zHandler.h"

void zHandler::send(zView* view, int what, u64 millis, int a1, int a2, cstr s) {
	zMutex mt;
	if(view) erase(view, what);
	for(auto& msg : looper) {
		if(!msg.millis) {
			msg.millis = (i64)(z_timeMillis() + millis);
			msg.view = view; msg.what = what; use++;
			msg.arg1 = a1; msg.arg2 = a2; msg.sarg = s;
			break;
		}
	}
}

void zHandler::info() {
    ILOG("use:%i", use);
	for(auto& msg : looper) {
		ILOG("%s(%x) what:%i %i", msg.view ? msg.view->typeName().str() : "null", msg.view ? msg.view->id : 0, msg.what, msg.millis != 0);
	}
}

HANDLER_MESSAGE* zHandler::obtain() {
    zMutex mt;
	if(use) {
		auto ms(z_timeMillis());
		for(auto& msg : looper) {
			if(!msg.millis) continue;
			if(ms >= msg.millis) { use--; return &msg; }
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
			msg.millis = 0; use--; if(!use) break;
		}
	}
}

HANDLER_MESSAGE* zHandler::get(zView* view, int what) {
    zMutex mt;
	if(use) {
		for(auto& msg : looper) {
			if(!msg.millis || msg.view != view) continue;
			if(what == msg.what) return &msg;
		}
	}
	return nullptr;
}

void zHandler::clear() {
    zMutex mt;
    for(auto& m : looper) m.millis = 0;
	use = 0;
}
