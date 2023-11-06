//
// Created by User on 06.05.2022.
//

#pragma once

#include "zstandard/zFile.h"

class zFileAsset : public zFile {
public:
    zFileAsset() : zFile() { }
    // конструктор по значению
    zFileAsset(czs& pth, bool read, bool zipped = true, bool append = false) : zFile() { intOpen(pth, read, zipped, append); }
    // открыть/создать файл
    virtual bool open(czs& pth, bool read, bool zipped = true, bool append = false) override;
    // закрыть
    virtual void close() override;
    // копировать/распаковать в папку
    virtual bool copy(czs& pth, i32 index) override;
    // прочитать в буфер
    virtual void* read(i32* psize, void* ptr, i32 size = 0, i32 pos = -1, i32 mode = 0) const override;
    // прочитать в автобуфер
    virtual void* readn(i32* psize = nullptr, i32 size = 0, i32 pos = -1, i32 mode = 0) const override;
    // записать буфер
    virtual bool write(void* ptr, int size, cstr name = nullptr) const override { return fd == nullptr && zFile::write(ptr, size, name); }
    // записать строку
    virtual bool writeString(cstr s, bool clr) const override { return fd == nullptr && zFile::writeString(s, clr); }
    // определить количество файлов
    virtual int	countFiles() const override { return (fd ? 1 : zFile::countFiles()); }
    // получить информацию
    virtual bool info(zFileInfo& zfi, int zindex = 0) const override;
    // установить позицию
    virtual bool seek(i32 pos, i32 mode) const override { return (fd ? AAsset_seek(fd, pos, mode) == pos : zFile::seek(pos, mode)); }
    // вернуть длину
    virtual int length() const override { return (fd ? AAsset_getLength(fd) : zFile::length()); }
    // вернуть позицию
    virtual int tell() const override { return (fd ? AAsset_seek(fd, 0, SEEK_CUR) : zFile::tell()); }
    // найти
    static zArray<zFileInfo> find(czs& pth, czs& _msk);
    // проверка на наличие файл/папку
    static bool isFile(czs& pth);
    static bool isDir(czs& pth);
    // создание папки
    static bool makeDir(czs& pth);
    // переименование/перемещение
    static bool	move(czs& _old, czs& _new);
    // удаление
    static bool remove(czs& pth);
protected:
    AAsset* fd{nullptr};
};
