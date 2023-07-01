//
// Created by User on 06.05.2022.
//

#pragma once

#include "zstandard/zFile.h"

class zFileAsset : public zFile {
public:
    zFileAsset() : zFile() { }
    virtual bool open(cstr _path, bool read, bool zipped = true, bool append = false) override {
        fd = AAssetManager_open(manager->getAsset(), _path, AASSET_MODE_UNKNOWN);
        return fd != nullptr;
    }
    virtual void close() override {
        if(fd) {
            AAsset_close(fd);
            fd = nullptr;
        }
    }
    virtual void* read(i32* psize, void* ptr, i32 size = 0, i32 pos = -1, i32 mode = 0) const override {
        if(fd) {
            auto asize(length());
            if(pos != -1) AAsset_seek(fd, pos, mode);
            if(size != -1 && asize >= size) asize = size;
            if(!ptr) ptr = new u8[asize];
            auto ret(AAsset_read(fd, ptr, asize));
            if(psize) *psize = ret;
            return ptr;
        }
        return nullptr;
    }
    virtual int length() const override {
        return AAsset_getLength(fd);
    }
protected:
    AAsset* fd{nullptr};
};