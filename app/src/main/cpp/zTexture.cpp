//
// Created by User on 18.04.2023.
//

#include "zssh/zCommon.h"
#include "zstandard/zJpg.h"
#include "zstandard/zPng.h"
#include "zstandard/zTga.h"

void zTexture::release() {
    if(idFBO) {
        glDeleteFramebuffers(1, &idFBO);
        idFBO = 0;
    }
    if(id) {
        glDeleteTextures(1, &id);
        id = 0;
    }
}

void zTexture::enableFilter(bool set) const {
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST + set);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST + set);
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
    int type(GL_RGBA), ww(0), hh(0), comp(0);
    u8* alpha; u8* image;
    // 1. параметры GL текстуры
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 2. определение формата текстуры
    zJpg* _jpg{nullptr}; zPng* _png(nullptr); zTga* _tga(nullptr);
    if(ptr[0] == 0xFF && ptr[1] == 0xD8) {
        _jpg = new zJpg(ptr, size);
        if(_jpg->isValid()) {
            ww = _jpg->getWidth(); hh = _jpg->getHeight(); image = _jpg->getImage();
            type = _jpg->getComponent() == 3 ? GL_RGB : GL_RGBA;
        } else {
            DLOG("jpg is wrong!");
        }
    } else if(memcmp(ptr, png_sig, 8) == 0) {
        _png = new zPng(ptr, size, &ww, &hh, &comp, 4);
        type = comp == 3 ? GL_RGB : GL_RGBA;
    } else if(*(u16*)ptr == 0) {
        _tga = new zTga(ptr, size);
        ww = _tga->getWidth(); hh = _tga->getHeight();  image = _tga->getImage();
        type = _tga->getComponent() == 3 ? GL_RGB : GL_RGBA;
    } else {
        // из активов
        auto head((HEAD_TTL*)ptr); ptr += sizeof(HEAD_TTL);
        ww = head->width; hh = head->height;
        ascent = head->ascent; descent = head->descent;
        // 3. alpha
        int sizeAlpha(ww * hh);
        alpha = z_rle_decompress(ptr, head->sizeAlpha, sizeAlpha);
        ptr += head->sizeAlpha; count = head->count;
        // 4. tiles
        delete[] tiles; tiles = new u16[count * 6];
        memcpy(tiles, ptr, count * 12); ptr += count * 12;
        // 5. rgb
        if(head->sizeRgb) {
            // если есть цветовые данные
            zJpg jpg(ptr, head->sizeRgb);
            if(!jpg.isValid()) {
                ILOG("Unable to create texture %s", name.str());
                delete[] alpha; delete[] tiles;
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
                *img++ = alpha[i];
                *img++ = alpha[i] == 0 ? 0 : 0xff;
            }
            type = GL_LUMINANCE_ALPHA;
        }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, type, ww, hh, 0, type, GL_UNSIGNED_BYTE, image);
    if(_jpg || _png || _tga) {
        delete _jpg; delete _png; delete _tga;
    } else { delete[] image; delete[] alpha; }
    glBindTexture(GL_TEXTURE_2D, 0);
    _size.set(ww, hh); _rsize.set(1.0f / (float)ww, 1.0f / (float)hh);
}

int zTexture::widthGlyph(int ch, float factor) const {
    if(ch == 32 || ch == 256) return 10;
    auto ptr(paramGlyph(ch));
    return ptr ? (int)round((float)ptr[2] * factor) : 0;
}

u32 zTexture::makeEmpty(int internalFormat, int format, int width, int height) {
    u32 idTexture;
    glGenTextures(1, &idTexture);
    if(!idTexture) return 0;
    // сделаем текстуру активной
    glBindTexture(GL_TEXTURE_2D, idTexture);
    // установим параметры фильтрации текстуры - линейная фильтрация
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // создаем "пустую" текстуру
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
    return idTexture;
}

u32 zTexture::makeFBO(int width, int height) {
    release();
    if(width > 1 && height > 1) {
        GLenum fboStatus;
        // 1. создаем текстуру заданных размеров
        if(!(id = makeEmpty(GL_RGBA, GL_RGBA, width, height))) {
            ILOG("Не удалось создать текстуру для FBO!");
            return 0;
        }
        // 2. создаем параметры
        tiles[2] = (u16) width;
        tiles[3] = (u16) height;
        _size.set(width, height);
        _rsize.set(1.0f / (float) _size.w, 1.0f / (float) _size.h);
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
        sz.set(tiles + 2);
        // производим очистку буфера цвета
        if(clear) {
//            glInvalidateFramebuffer();
            glDisable(GL_SCISSOR_TEST);
//            glClearColor(1,1,1,1);
            glClear(GL_COLOR_BUFFER_BIT);
  //          glClearColor(0, 0, 0, 0);
            glEnable(GL_SCISSOR_TEST);
        }
    } else {
//        glInvalidateFramebuffer(GL_FRAMEBUFFER, 2, depth_and_stencil);
    //    const GLenum attachments[] = {GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT};
  //      glInvalidateFramebuffer(GL_FRAMEBUFFER, 2, attachments);
//        glDiscardFramebufferEXT (GL_FRAMEBUFFER, 2, attachments);
        // делаем активным дефолтный FBO
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        sz = zGL::instance()->getSizeScreen();
    }
    manager->setMainMatrix(sz.w, sz.h, set);
}
