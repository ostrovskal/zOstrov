//
// Created by User on 06.05.2022.
//

#include "zostrov/zCommon.h"
#include "zostrov/zViewWidgets.h"
#include "zostrov/zFileAsset.h"

// обратный вызов AndroidBufferQueueItf для подачи пакетов MPEG-2 TS в медиаплеер
/*
XAresult zViewVideo::callbackBuffer(XAAndroidBufferQueueItf caller, void *bufCtx, void *data, XAuint32 size, XAuint32,
                        const XAAndroidBufferItem*, XAuint32) {
    XAresult res;
    // был запрос на прерывание?
    if(discontinuity) {
        // Примечание: нельзя перематывать после EOS, который мы отправляем при достижении EOF.
        // (не отправляйте EOS, если вы планируете воспроизводить больше контента через тот же плеер)
        if(!reachedEof) {
            // clear the buffer queue
            // очистка буфера
            if((res = (*playerBuffer)->Clear(playerBuffer)) != XA_RESULT_SUCCESS)
                return res;
            // перематываем источник данных, чтобы гарантированно оказаться в нужной точке
            rewind();
            // Выдача начальных буферов, с индикатором прерывания на первом буфере
            initBuffers(true);
        }
        // подтвердить запрос на прерывание
        discontinuity = false;
        return XA_RESULT_SUCCESS;
    }
    if(!data && bufCtx) {
        auto processedCommand(*(int*)bufCtx);
        // наш буфер с сообщением EOS был израсходован
        if(kEosBufferCntxt == processedCommand) return XA_RESULT_SUCCESS;
    }
    // pBufferData is a pointer to a buffer that we previously Enqueued
    if((size <= 0) && ((size % MPEG2_TS_PACKET_SIZE) != 0)) return XA_RESULT_IO_ERROR;
    if(dataCache > (char*)data && (char*)data >= &dataCache[BUFFER_SIZE * NB_BUFFERS]) return XA_RESULT_IO_ERROR;
    if((((char*)data - dataCache) % BUFFER_SIZE)) return XA_RESULT_IO_ERROR;
    // не пытайтесь читать больше данных после того, как мы достигли EOF
    if(reachedEof) return XA_RESULT_SUCCESS;
    // обратите внимание, что мы вызываем fread из нескольких потоков, но никогда одновременно
    i32 bytesRead(0);
    if(file->read(&bytesRead, data, BUFFER_SIZE)) {
        if((bytesRead % MPEG2_TS_PACKET_SIZE) != 0) {
            DLOG("Отбрасывание последнего пакета, поскольку он не является целым");
        }
        auto packetsRead(bytesRead / MPEG2_TS_PACKET_SIZE);
        auto bufferSize(packetsRead * MPEG2_TS_PACKET_SIZE);
        if((res = (*caller)->Enqueue(caller, nullptr, data, bufferSize, nullptr, 0)) != XA_RESULT_SUCCESS) return res;
    } else {
        XAAndroidBufferItem msgEos[1];
        msgEos[0].itemKey = XA_ANDROID_ITEMKEY_EOS;
        msgEos[0].itemSize = 0;
        // Сообщение EOS не имеет параметров, поэтому общий размер сообщения равен размеру ключа
        // плюс размер, если itemSize, оба XAuint32
        if((res = (*caller)->Enqueue(caller, (void*)&kEosBufferCntxt, nullptr, 0, msgEos, sizeof(XAuint32) * 2)) != XA_RESULT_SUCCESS)
            return res;
        reachedEof = true;
    }
    return XA_RESULT_SUCCESS;
}

void zViewVideo::callbackStream(XAStreamInformationItf caller, XAuint32 id, XAuint32 idx, void*) {
    DLOG("StreamChangeCallback вызван для потока %u", idx);
    XAuint32 domain;
    if(id == XA_STREAMCBEVENT_PROPERTYCHANGE) {
        if((*caller)->QueryStreamType(caller, idx, &domain) != XA_RESULT_SUCCESS) return;
        if(domain == XA_DOMAINTYPE_VIDEO) {
            XAVideoStreamInformation videoInfo;
            if((*caller)->QueryStreamInformation(caller, idx, &videoInfo) != XA_RESULT_SUCCESS) return;
        } else {
            DLOG("Неожиданные данные %u\n", domain);
        }
    } else {
        DLOG("Неожиданное событие потока ID %u\n", id);
    }
}

XAresult AndroidBufferQueueCallback(XAAndroidBufferQueueItf caller, void *ctx, void *bufCtx, void *data,
                                           XAuint32 size, XAuint32 used, const XAAndroidBufferItem *items, XAuint32 itemsLength) {
    if(ctx) return ((zViewVideo*)ctx)->callbackBuffer(caller, bufCtx, data, size, used, items, itemsLength);
    return XA_RESULT_IO_ERROR;
}

// обратный вызов, вызываемый каждый раз, когда появляется новая или измененная информация о потоке
void StreamChangeCallback(XAStreamInformationItf caller, XAuint32 id, XAuint32 idx, void* data, void* ctx) {
    if(ctx) ((zViewVideo*)ctx)->callbackStream(caller, id, idx, data);
}

zViewVideo::zViewVideo(zStyle* _styles, i32 _id, cstr name, bool isAsset) : zView(_styles, _id) {
    minMaxSize.set(z.R.dimen.videoMinWidth, 0, z.R.dimen.videoMinHeight, 0);
    // создание движка
    xaCreateEngine(&engineObject, 0, nullptr, 0, nullptr, nullptr);
    if((*engineObject)->Realize(engineObject, XA_BOOLEAN_FALSE) == XA_RESULT_SUCCESS) {
        // получить интерфейс двигателя, который необходим для создания других объектов
        if((*engineObject)->GetInterface(engineObject, XA_IID_ENGINE, &engine) == XA_RESULT_SUCCESS) {
            // создать выходной микс
            if((*engine)->CreateOutputMix(engine, &outputMixObject, 0, nullptr, nullptr) == XA_RESULT_SUCCESS) {
                // реализовать выходной микс
                (*outputMixObject)->Realize(outputMixObject, XA_BOOLEAN_FALSE);
                // октрыть файл
                file = (isAsset ? new zFileAsset() : new zFile());
                file->open(name, true);
            }
        }
    }
}

bool zViewVideo::initBuffers(bool _discontinuity) {
    XAresult res; i32 size(0);
    // может быть преждевременным EOF или ошибкой ввода/вывода
    if(!file->read(&size, dataCache, BUFFER_SIZE * NB_BUFFERS)) return false;
    if((size % MPEG2_TS_PACKET_SIZE) != 0) {
        DLOG("Отбрасывание последнего пакета, поскольку он не является целым");
    }
    auto packetsRead(size / MPEG2_TS_PACKET_SIZE);
    DLOG("Первоначальная постановка в очередь %zu пакетов", packetsRead);
    // Перед началом воспроизведения, необходимо выгрузить содержимое кэша
    for(int i = 0; i < NB_BUFFERS && packetsRead > 0; i++) {
        // вычислить размер этого буфера
        auto packetsThisBuffer(packetsRead);
        if(packetsThisBuffer > PACKETS_PER_BUFFER) packetsThisBuffer = PACKETS_PER_BUFFER;
        auto bufferSize(packetsThisBuffer * MPEG2_TS_PACKET_SIZE);
        if(_discontinuity) {
            // сигнал прерывания
            XAAndroidBufferItem items[1];
            items[0].itemKey = XA_ANDROID_ITEMKEY_DISCONTINUITY;
            items[0].itemSize = 0;
            // Сообщение DISCONTINUITY не имеет параметров,
            // поэтому общий размер сообщения равен размеру ключа
            // плюс размер, если itemSize, оба XAuint32
            res = (*playerBuffer)->Enqueue(playerBuffer, nullptr, dataCache + i * BUFFER_SIZE, bufferSize, items, sizeof(XAuint32) * 2);
            _discontinuity = false;
        } else {
            res = (*playerBuffer)->Enqueue(playerBuffer, nullptr, dataCache + i * BUFFER_SIZE, bufferSize, nullptr, 0);
        }
        if(res != XA_RESULT_SUCCESS) return false;
        packetsRead -= packetsThisBuffer;
    }
    return true;
}

bool zViewVideo::createStream() {
    // конфигурирование источника данных
    XADataLocator_AndroidBufferQueue loc_abq = { XA_DATALOCATOR_ANDROIDBUFFERQUEUE, NB_BUFFERS };
    XADataFormat_MIME format_mime = { XA_DATAFORMAT_MIME, XA_ANDROID_MIME_MP2TS, XA_CONTAINERTYPE_MPEG_TS };
    XADataSource dataSrc = { &loc_abq, &format_mime };
    // настройка звука
    XADataLocator_OutputMix loc_outmix = { XA_DATALOCATOR_OUTPUTMIX, outputMixObject };
    XADataSink audioSnk = { &loc_outmix,  nullptr };
    // настраиваем видео изображение
    XADataLocator_NativeDisplay loc_nd = { XA_DATALOCATOR_NATIVEDISPLAY, (void*)surface, nullptr };
    XADataSink imageVideoSink = { &loc_nd, nullptr };
    // объявить интерфейсы для использования
    XAboolean     required[NB_MAXAL_INTERFACES] = { XA_BOOLEAN_TRUE, XA_BOOLEAN_TRUE, XA_BOOLEAN_TRUE };
    XAInterfaceID iidArray[NB_MAXAL_INTERFACES] = { XA_IID_PLAY, XA_IID_ANDROIDBUFFERQUEUESOURCE, XA_IID_STREAMINFORMATION };
    // создать медиаплеер
    if((*engine)->CreateMediaPlayer(engine, &playerObject, &dataSrc, nullptr, &audioSnk, &imageVideoSink, nullptr, nullptr,
                                             NB_MAXAL_INTERFACES, iidArray, required) != XA_RESULT_SUCCESS) return false;
    (*playerObject)->Realize(playerObject, XA_BOOLEAN_FALSE);
    // получить интерфейс проигрывателя
    (*playerObject)->GetInterface(playerObject, XA_IID_PLAY, &player);
    // получить интерфейс информации о потоке (для размера видео)
    (*playerObject)->GetInterface(playerObject, XA_IID_STREAMINFORMATION, &playerStream);
    // получить интерфейс громкости
    (*playerObject)->GetInterface(playerObject, XA_IID_VOLUME, &playerVolume);
    // получить интерфейс буферной очереди Android
    (*playerObject)->GetInterface(playerObject, XA_IID_ANDROIDBUFFERQUEUESOURCE, &playerBuffer);
    // указать, о каких событиях мы хотим получать уведомления
    (*playerBuffer)->SetCallbackEventsMask(playerBuffer, XA_ANDROIDBUFFERQUEUEEVENT_PROCESSED);
    // регистрируем обратный вызов, из которого OpenMAX AL может получить данные для воспроизведения
    (*playerBuffer)->RegisterCallback(playerBuffer, AndroidBufferQueueCallback, nullptr);
    // мы хотим получить уведомление о размере видео, как только оно будет найдено, поэтому регистрируем обратный вызов для этого
    (*playerStream)->RegisterStreamChangeCallback(playerStream, StreamChangeCallback, nullptr);
    // регистрировать начальные буферы
    if(!initBuffers(false)) return false;
    // подготовка проигрывателя
    (*player)->SetPlayState(player, XA_PLAYSTATE_PLAYING);
    // установка громкости
    (*playerVolume)->SetVolumeLevel(playerVolume, 100);
    return true;
}

void zViewVideo::shutdown() {
    // уничтожить объект потокового медиаплеера и аннулировать все связанные с ним интерфейсы
    if(playerObject) {
        (*playerObject)->Destroy(playerObject);
        playerObject = nullptr; player       = nullptr;
        playerBuffer = nullptr; playerStream = nullptr;
        playerVolume = nullptr;
    }
    // уничтожить объект выходной смеси и аннулировать все связанные с ним интерфейсы
    if(outputMixObject) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = nullptr;
    }
    // уничтожить объект движка и аннулировать все связанные с ним интерфейсы
    if(engineObject) {
        (*engineObject)->Destroy(engineObject);
        engineObject = nullptr;
        engine = nullptr;
    }
    // закрыть файл
    SAFE_DELETE(file);
}

void zViewVideo::play(bool _play) {
    if(player) (*player)->SetPlayState(player, _play ? XA_PLAYSTATE_PLAYING : XA_PLAYSTATE_PAUSED);
}

void zViewVideo::rewind() {
    if(player) {
        XAuint32  state;
        if((*player)->GetPlayState(player, &state) == XA_RESULT_SUCCESS) {
            if(state == XA_PLAYSTATE_PAUSED || state == XA_PLAYSTATE_STOPPED) {
                discontinuity = true;
            } else {
                play(false);
            }
            discontinuity = true;
        }
    }
}

void zViewVideo::onMeasure(cszm& spec) {
    auto sizeWidth(spec.w.size()), sizeHeight(spec.h.size());
    auto modeWidth(spec.w.mode()), modeHeight(spec.h.mode());
    int width, height;
    if(sizeWidth && modeWidth == MEASURE_EXACT) { width = sizeWidth;
    } else {
        width = minMaxSize.x;
        width += pad.extent(false);
        if(sizeWidth && modeWidth == MEASURE_MOST) width = z_min(width, sizeWidth);
    }
    if(sizeHeight && modeHeight == MEASURE_EXACT) {
        height = sizeHeight;
    } else {
        height = minMaxSize.w;
        height += pad.extent(true);
        if(sizeHeight && modeHeight == MEASURE_MOST) height = z_min(height, sizeHeight);
    }
    setMeasuredDimension(width, height);
}

void zViewVideo::onLayout(crti &position, bool changed) {
    zView::onLayout(position, changed);
    if(!surface && !context) {
        if(manager->createSurface(rclient.w, rclient.h, &surface)) {
            // создаем поток
            createStream();
        }
    }
}
*/