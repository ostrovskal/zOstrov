//
// Created by User on 06.05.2022.
//

#pragma once

#include "zstandard/zFile.h"

class zFileAsset : public zFile {
public:
    zFileAsset() : zFile() { }
    // конструктор по значению
    zFileAsset(czs& _path, bool read, bool zipped = true, bool append = false) : zFile(_path, read, zipped, append) { }
    // открыть
    virtual bool open(czs& _path, bool read, bool zipped = true, bool append = false) override {
        close();
        fd = AAssetManager_open(manager->getAsset(), _path, AASSET_MODE_UNKNOWN);
        if(fd) path = _path;
        return fd != nullptr;
    }
    // закрыть
    virtual void close() override {
        if(fd) {
            AAsset_close(fd);
            fd = nullptr;
        }
    }
    virtual void* readn(i32* psize = nullptr, i32 size = 0, i32 pos = -1, i32 mode = 0) const override {
        if(fd) {
            auto asize(length());
            if(size && asize >= size) asize = size;
            return read(psize, new u8[asize], asize, pos, mode);
        }
        return nullptr;
    }
    virtual void* read(i32* psize, void* ptr, i32 size = 0, i32 pos = -1, i32 mode = 0) const override {
        if(fd) {
            if(pos != -1) {
                if(AAsset_seek(fd, pos, mode) == -1)
                    return nullptr;
            }
            auto asize(length());
            if(size && asize >= size) asize = size;
            auto ret(AAsset_read(fd, ptr, asize));
            if(psize) *psize = ret;
        }
        return ptr;
    }
    virtual bool info(zFileInfo& zfi, int zindex) const override {
        if(fd) {
            zfi.csize = zfi.usize = length();
            zfi.attr = S_IFREG; zfi.index = -1;
            zfi.path = path;
        }
        return fd != nullptr;
    }
    virtual int length() const override {
        return AAsset_getLength(fd);
    }
protected:
    AAsset* fd{nullptr};
};