//
// Created by User on 05.11.2023.
//

#include "zostrov/zCommon.h"
#include "zostrov/zFileAsset.h"

static std::mutex mt{};

bool zFileAsset::open(czs& pth, bool read, bool zipped, bool append) {
    if(pth.prefix("/")) return zFile::open(pth, read, zipped, append);
    close(); bool ret(false);
    if((fd = AAssetManager_open(manager->getAsset(), pth, AASSET_MODE_UNKNOWN))) {
        if(zipped && pth.right(4).compare(".zip")) {
            // скопировать в папку кэша и открыть как архив
            auto p(settings->makePath(pth.substrAfterLast("/"), FOLDER_CACHE));
            ret = copy(p, 0) && zFile::open(p, read, zipped, append);
            fd = nullptr;
        } else { ret = true; path = pth; }
    }
    return ret;
}

void zFileAsset::close() {
    if(fd) { AAsset_close(fd); fd = nullptr; }
    zFile::close();
}

bool zFileAsset::copy(czs& pth, i32 index) {
    if(fd) {
        bool ret(false);
        // проверить на копирование во вклады
        if(pth.prefix("/")) {
            // читать/писать по кусочкам
            zFile f; int bsz(32 * 1024), sz(0);
            auto tmp(std::unique_ptr<u8>(new u8[bsz]));
            if(f.open(pth, false, false)) {
                while(true) {
                    read(&sz, tmp.get(), bsz);
                    if(sz) ret = f.write(tmp.get(), sz);
                    if(!ret || sz != bsz) break;
                }
            }
        }
        return ret;
    }
    return zFile::copy(pth, index);
}

void* zFileAsset::read(i32* psize, void* ptr, i32 size, i32 pos, i32 mode) const {
    if(fd) {
        zMutex m(&mt);
        seek(pos, mode); if(!size) size = length();
        if((pos = AAsset_read(fd, ptr, size)) != size) ptr = nullptr;
        if(psize) *psize = pos; return ptr;
    }
    return zFile::read(psize, ptr, size, pos, mode);
}

void* zFileAsset::readn(i32* psize, i32 size, i32 pos, i32 mode) const {
    if(fd) {
        if(!size) size = length(); auto ptr(new u8[size + 1]); ptr[size] = 0;
        if(!read(psize, ptr, size, pos, mode)) SAFE_A_DELETE(ptr);
        return ptr;
    }
    return zFile::readn(psize, size, pos, mode);
 }

bool zFileAsset::info(zFileInfo& zfi, int zindex) const {
    if(fd) {
        zfi.csize = zfi.usize = length();
        zfi.attr = S_IFREG; zfi.index = -1; zfi.setPath(path);
        zfi.atime= zfi.mtime = zfi.ctime = time(nullptr);
        return true;
    }
    return zFile::info(zfi, zindex);
}

zArray<zFile::zFileInfo> zFileAsset::find(czs& pth, czs& _msk) {
    if(pth.prefix("/")) return zFile::find(pth, _msk);
    static char fname[260];
    zArray<zFileInfo> fl; zFileInfo info{}; auto am(_msk.split("*"));
    if(auto dir = AAssetManager_openDir(manager->getAsset(), pth.substrBeforeLast("/", pth))) {
        while(auto ent = AAssetDir_getNextFileName(dir)) {
            // поиск по маске
            bool res(true); auto _s(fname); strcpy(_s, ent);
            for(auto i = 0; i < am.size(); i++) {
                auto m(am[i]); if(m.isEmpty()) continue;
                auto _m(_s ? strstr(_s, m.str()) : _s);
                if(i == 0 && _m == fname) { _s = _m + 1; continue; }
                if(i > 0 && _m) { _s = _m + 1; continue; }
                res = false; break;
            }
            if(res && zFileAsset(pth + ent, true, false).info(info))
                fl += info;
        }
        AAssetDir_close(dir);
    }
    return fl;
}

bool zFileAsset::isFile(czs& pth) {
    if(pth.prefix("/")) return zFile::isFile((pth));
    zMutex m(&mt);
    auto fd(AAssetManager_open(manager->getAsset(), pth, AASSET_MODE_UNKNOWN));
    if(fd) AAsset_close(fd);
    return fd != nullptr;
}

bool zFileAsset::isDir(czs& pth) {
    if(pth.prefix("/")) return zFile::isDir(pth);
    zMutex m(&mt);
    return pth.indexOf(".") == -1 && AAssetManager_openDir(manager->getAsset(), pth.str()) != nullptr;
}

bool zFileAsset::makeDir(czs& pth) {
    if(!pth.prefix("/")) return false;
    return zFile::makeDir(pth);
}

bool zFileAsset::move(czs& _old, czs& _new) {
    if(_old.prefix("/") && _new.prefix("/")) return zFile::move(_old, _new);
    return false;
}

bool zFileAsset::remove(czs& pth) {
    if(pth.prefix("/")) return zFile::remove(pth);
    return false;
}
