
#include "zostrov/zCommon.h"
#include "zostrov/zHandler.h"

void zHandler::send(zView* view, int what, u64 millis, int a1, int a2, cstr s) {
	if(view) erase(view, what);
	for(auto& msg : looper) {
		if(!msg.millis) {
			msg.view = view; msg.what = what;
			msg.millis = (i64)(z_timeMillis() + millis);
			msg.arg1 = a1; msg.arg2 = a2; msg.sarg = s;
			use++; break;
		}
	}
}

void zHandler::info() {
	for(int i = 0 ; i < 10; i++) {
		auto& msg(looper[i]);
		if(msg.millis) DLOG("%i - view: %llx %s", i, msg.view, msg.view ? msg.view->typeName().str() : "null");
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
			if(!msg.millis || msg.view != view) continue;
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
