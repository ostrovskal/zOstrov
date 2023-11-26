
#include "zostrov/zCommon.h"
#include "zostrov/zHandler.h"

zHandler::zHandler(int count) {
    while(count-- > 0) unuses += new HANDLER_MESSAGE();
}

void zHandler::send(zView* view, int what, u64 millis, int a1, int a2, cstr s) {
	erase(view, what);
    zMutex m(&mt);
    auto is(unuses.isNotEmpty());
    auto msg(is ? unuses[0] : new HANDLER_MESSAGE());
    msg->millis = (i64)(z_timeMillis() + millis);
    msg->view = view; msg->what = what;
    msg->arg1 = a1; msg->arg2 = a2; msg->sarg = s;
    looper += msg; if(is) unuses.erase(0, 1, false);
}

void zHandler::info() {
    zMutex m(&mt);
    ILOG("useLooper:%i useUnused:%i", looper.size(), unuses.size());
	for(auto msg : looper) {
		ILOG("%s(%x) - %i", msg->view->typeName().str(), msg->view->id, msg->what);
	}
}

HANDLER_MESSAGE* zHandler::obtain() {
    zMutex m(&mt);
    auto ms(z_timeMillis());
    for(int i = 0 ; i < looper.size(); i++) {
        auto msg(looper[i]);
        if(ms < msg->millis) continue;
        looper.erase(i, 1, false);
        unuses += msg; return msg;
    }
	return nullptr;
}

void zHandler::erase(zView* view, int what) {
    zMutex m(&mt);
    for(int i = 0 ; i < looper.size(); i++) {
        auto msg(looper[i]);
        if(msg->view != view) continue;
        // если what == 0 - тогда удаляем все
		if(what && what != msg->what) continue;
        looper.erase(i--, 1, false);
        unuses += msg;
	}
}

HANDLER_MESSAGE* zHandler::get(zView* view, int what) {
    zMutex m(&mt);
    for(auto msg : looper) {
        if(msg->view != view) continue;
        if(what == msg->what) return msg;
    }
	return nullptr;
}

void zHandler::clear() {
    zMutex m(&mt);
    for(auto msg : looper) unuses += msg;
    looper.free();
}
