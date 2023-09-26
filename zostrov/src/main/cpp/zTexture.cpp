//
// Created by User on 18.04.2023.
//

#include "zostrov/zCommon.h"
#include "zstandard/zJpg.h"
#include "zstandard/zPng.h"
#include "zstandard/zTga.h"

void zTexture::release() {
    if(idFBO) {
        glDeleteFramebuffers(1, &idFBO);
        idFBO = 0; manager->volumeVideoMemory(_size.w * _size.h * 4, false);
    }
    if(id) {
        glDeleteTextures(1, &id);
        id = 0;
    }
}

void zTexture::enableFilter(bool set, int clamp) const {
    glBindTexture(type, id);
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST + set);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST + set);
    glTexParameteri(type, GL_TEXTURE_WRAP_S, clamp);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, clamp);
}

bool zTexture::save(czs& path) const {
    if(idFBO) {
        auto width(_size.w), height(_size.h);
        std::unique_ptr<u8> ptr(new u8[width * height * 4]);
        // установить fbo
        glBindFramebuffer(GL_FRAMEBUFFER, idFBO);
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, ptr.get());
        z_tgaSaveFile(path, ptr.get(), width, height, 4);
        // освободить fbo
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    return idFBO != 0;
}

void zTexture::makeTexture(u8* ptr, u32 size) {
    static u8 png_sig[8] = { 137,80,78,71,13,10,26,10 };
    if(!ptr) { release(); return; }
    int fmt(GL_RGBA), ww(0), hh(0), comp(0);
    u8* alpha; u8* image;
    // 1. параметры GL текстуры
    enableFilter(true);
    // 2. определение формата текстуры
    zJpg* _jpg{nullptr}; zPng* _png(nullptr); zTga* _tga(nullptr);
    if(ptr[0] == 0xFF && ptr[1] == 0xD8) {
        _jpg = new zJpg(ptr, size);
        if(_jpg->isValid()) {
            ww = _jpg->getWidth(); hh = _jpg->getHeight(); image = _jpg->getImage();
            fmt = _jpg->getComponent() == 3 ? GL_RGB : GL_RGBA;
        } else {
            DLOG("jpg is wrong!");
        }
    } else if(memcmp(ptr, png_sig, 8) == 0) {
        _png = new zPng(ptr, (int)size, &ww, &hh, &comp, 4);
        fmt = comp == 3 ? GL_RGB : GL_RGBA;
    } else if(*(u16*)ptr == 0) {
        _tga = new zTga(ptr, size);
        ww = (int)_tga->getWidth(); hh = (int)_tga->getHeight();  image = _tga->getImage();
        fmt = _tga->getComponent() == 3 ? GL_RGB : GL_RGBA;
    } else if(*(u32*)ptr == '2LTT') {
        // из активов
        auto head((HEAD_TTL*)ptr); ptr += sizeof(HEAD_TTL);
        ww = head->width; hh = head->height;
        ascent = head->ascent; descent = head->descent;
        // 3. alpha
        int sizeAlpha(ww * hh);
        alpha = z_rle_decompress(ptr, head->sizeAlpha, sizeAlpha);
        ptr += head->sizeAlpha;
        // 4. tiles
        tiles.clear();
        for(int i = 0 ; i < head->count; i++) {
            TILE_TTL tl((u16*)ptr);
            tiles += tl; ptr += 32;
        }
        // 5. rgb
        if(head->sizeRgb) {
            // если есть цветовые данные
            zJpg jpg(ptr, head->sizeRgb);
            if(!jpg.isValid()) {
                ILOG("Unable to create texture %s", name.str());
                delete[] alpha; tiles.clear();
                return;
            }
            // совместить ALPHA и распакованный JPEG
            auto img(new u32[sizeAlpha]); auto tmp(jpg.getImage());
            for(int i = 0; i < sizeAlpha; i++) {
                auto c((*(u32*)(tmp + i * 3)) & 0x00ffffff);
                img[i] = c | (alpha[i] << 24);
            }
            image = (u8*)img;
        } else {
            // если нет цветовых данных
            auto img(new u8[sizeAlpha * 2]); image = img;
            for(int i = 0; i < sizeAlpha; i++) {
                *img++ = 0xff; *img++ = alpha[i];
            }
            fmt = GL_LUMINANCE_ALPHA;
        }
    } else {
        ILOG("Unknown format texture!");
        release();
        return;
    }
    glTexImage2D(type, 0, fmt, ww, hh, 0, fmt, GL_UNSIGNED_BYTE, image);
    if(_jpg || _png || _tga) {
        delete _jpg; delete _png; delete _tga;
    } else { delete[] image; delete[] alpha; }
    glBindTexture(type, 0);
    _size.set(ww, hh); _rsize.set(1.0f / (float)ww, 1.0f / (float)hh);
}

void zTexture::makeEmpty(int internalFormat, int format, int width, int height, int _type) {
    glGenTextures(1, &id); type = _type;
    // сделаем текстуру активной
    enableFilter(true);
    // создаем "пустую" текстуру
    glTexImage2D(_type, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(_type, 0);
}

u32 zTexture::makeTexture(int width, int height, int _type) {
    // 1. создаем текстуру заданных размеров
    custom = true;
    makeEmpty(GL_RGBA, GL_RGBA, width, height, _type);
    // 2. создаем параметры
    tiles[0].rect.w = width;
    tiles[0].rect.h = height;
    _size.set(width, height);
    _rsize.set(1.0f / (float) _size.w, 1.0f / (float) _size.h);
    return id;
}

u32 zTexture::makeOES(int width, int height) {
    return 0;
}

u32 zTexture::makeFBO(int width, int height) {
    release();
    if(width > 1 && height > 1) {
        GLenum fboStatus;
        if(!makeTexture(width, height, GL_TEXTURE_2D)) return 0;
        // 3. создаем FBO
        glGenFramebuffers(1, &idFBO);
        if(!idFBO) {
            ILOG("glGenFramebuffers error!!! %ix%i", width, height);
            release();
            return 0;
        }
        // 4. делаем созданный FBO текущим
        glBindFramebuffer(GL_FRAMEBUFFER, idFBO);
        // 5. присоединяем созданную текстуру к текущему FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);
        // 6. проверим текущий FBO на корректность
        if((fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
            ILOG("glCheckFramebufferStatus error 0x%X %ix%i", fboStatus, width, height);
            release();
            return 0;
        }
        // 7. делаем активным дефолтный FBO
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    return idFBO;
}

void zTexture::setFBO(bool set, bool clear) {
    szi sz;
    if(set) {
        // устанавливаем активный FBO
        glBindFramebuffer(GL_FRAMEBUFFER, idFBO);
        sz = tiles[0].rect.size();
        // производим очистку буфера цвета
        if(clear) {
            glDisable(GL_SCISSOR_TEST);
            glClear(GL_COLOR_BUFFER_BIT);
            glEnable(GL_SCISSOR_TEST);
        }
    } else {
        // делаем активным дефолтный FBO
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        sz = zGL::instance()->getSizeScreen();
    }
    manager->setMainMatrix(sz.w, sz.h, set);
}
