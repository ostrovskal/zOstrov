//
// Created by User on 06.05.2022.
//

#include "zssh/zCommon.h"
#include "zssh/zSoundManager.h"

static SLresult result;

/*
void setRec() {
        SLresult result;

        if (pthread_mutex_trylock(&audioEngineLock)) {
            return;
        }
        // in case already recording, stop recording and clear buffer queue
        result = (*recorderRecord)->SetRecordState(recorderRecord, SL_RECORDSTATE_STOPPED);
        assert(SL_RESULT_SUCCESS == result);
        (void)result;
        result = (*recorderBufferQueue)->Clear(recorderBufferQueue);
        assert(SL_RESULT_SUCCESS == result);
        (void)result;

        // the buffer is not valid for playback yet
        recorderSize = 0;

        // enqueue an empty buffer to be filled by the recorder
        // (for streaming recording, we would enqueue at least 2 empty buffers to start things off)
        result = (*recorderBufferQueue)->Enqueue(recorderBufferQueue, recorderBuffer,
                                                 RECORDER_FRAMES * sizeof(short));
        // the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
        // which for this code example would indicate a programming error
        assert(SL_RESULT_SUCCESS == result);
        (void)result;

        // start recording
        result = (*recorderRecord)->SetRecordState(recorderRecord, SL_RECORDSTATE_RECORDING);
        assert(SL_RESULT_SUCCESS == result);
        (void)result;
    }
}
 *
 */
static void recorderCallback(SLAndroidSimpleBufferQueueItf, void *context) {
    auto pl((zSoundPlayer*)context);
    if((result = (*pl->recorder)->SetRecordState(pl->recorder, SL_RECORDSTATE_STOPPED)) == SL_RESULT_SUCCESS) {
        pl->nextSize = RECORDER_FRAMES * sizeof(u16);
    }
}

static void playerCallback(SLAndroidSimpleBufferQueueItf, void* context) {
    auto pl((zSoundPlayer*)context);
    pl->totalSize -= pl->nextSize;
    if(--pl->nextCount > 0 && pl->totalSize) {
        pl->nextBuffer += pl->nextSize;
        pl->nextSize = pl->totalSize >= pl->bufSize ? pl->bufSize : pl->totalSize;
        (*pl->queue)->Enqueue(pl->queue, pl->nextBuffer, pl->nextSize);
    } else {
        pl->stop();
    }
}

bool zSoundPlayer::make(SLDataSource& src, SLDataSink& snk, bool play, int _bufSize) {
    SLInterfaceID ids[3]  = { SL_IID_SEEK, SL_IID_MUTESOLO, SL_IID_VOLUME };
    SLboolean req[3]      = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
    auto isrec(type == TYPE_REC);
    if(type == TYPE_MEM || isrec) ids[0] = SL_IID_ANDROIDSIMPLEBUFFERQUEUE;
    auto engine(mgr->getEngine());
    shutdown();
    if((result = (*engine)->CreateAudioPlayer(engine, &object, &src, &snk, 3, ids, req)) == SL_RESULT_SUCCESS) {
        (*object)->Realize(object, SL_BOOLEAN_FALSE);
        if(isrec) {
            (*object)->GetInterface(object, SL_IID_RECORD, &recorder);
        } else {
            (*object)->GetInterface(object, SL_IID_PLAY, &player);
        }
        (*object)->GetInterface(object, SL_IID_VOLUME, &volume);
        (*object)->GetInterface(object, SL_IID_MUTESOLO, &mute);
        if(ids[0] == SL_IID_SEEK) {
            (*object)->GetInterface(object, SL_IID_SEEK, &seek);
        } else {
            (*object)->GetInterface(object, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &queue);
            (*player)->SetCallbackEventsMask(player, SL_PLAYEVENT_HEADMOVING);
            (*queue)->RegisterCallback(queue, isrec ? recorderCallback : playerCallback, this);
            if(_bufSize != -1) {
                bufSize = (_bufSize ? _bufSize : (isrec ? RECORDER_FRAMES : PLAYER_FRAMES));
            }
        }
        if(!isrec) (*player)->SetPlayState(player, play ? SL_PLAYSTATE_PLAYING : SL_PLAYSTATE_STOPPED);
    }
    return result == SL_RESULT_SUCCESS;
}

bool zSoundPlayerVibra::create(const zPlayerParams& p, bool play) {
    return false;
}

bool zSoundPlayerRec::create(const zPlayerParams& p, bool play) {
    SLDataLocator_AndroidSimpleBufferQueue loc = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2 };
    SLDataLocator_IODevice dev  = { SL_DATALOCATOR_IODEVICE, SL_IODEVICE_AUDIOINPUT, SL_DEFAULTDEVICEID_AUDIOINPUT, nullptr };
    SLDataFormat_PCM fmt        = { SL_DATAFORMAT_PCM, p.chan, p.rate,
                                    SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
                                    SL_SPEAKER_FRONT_CENTER, SL_BYTEORDER_LITTLEENDIAN };
    SLDataSource src            = { &dev, nullptr };
    SLDataSink snk              = { &loc, &fmt };
    return make(src, snk, play, p.bufSize);
}

bool zSoundPlayerMem::create(const zPlayerParams& p, bool play) {
    SLDataLocator_AndroidSimpleBufferQueue loc = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2 };
    SLDataFormat_PCM fmt         = { SL_DATAFORMAT_PCM, p.chan, p.rate,
                                     SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
                                     p.chan == 1 ? SL_SPEAKER_FRONT_CENTER : (SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT),
                                     SL_BYTEORDER_LITTLEENDIAN };
    SLDataLocator_OutputMix mix  = { SL_DATALOCATOR_OUTPUTMIX, mgr->getMixer() };
    SLDataSource src             = { &loc, &fmt };
    SLDataSink snk               = { &mix, nullptr };
    return make(src, snk, play, p.bufSize);
}

void zSoundPlayerMem::setData(u8* data, int size) {
    if(data) {
        auto state(getStatus());
        if(state != SL_PLAYSTATE_STOPPED) return;
        totalSize = size; nextBuffer = data;
        nextCount = size / bufSize;
        nextSize = size >= bufSize ? bufSize : size;
        if((*queue)->Enqueue(queue, nextBuffer, nextSize) == SL_RESULT_BUFFER_INSUFFICIENT) {
            (*queue)->Clear(queue);
            (*queue)->Enqueue(queue, nextBuffer, nextSize);
        }
    }
}

bool zSoundPlayerUri::create(const zPlayerParams& p, bool play) {
    SLDataLocator_URI loc       = { SL_DATALOCATOR_URI, (u8*)p.uri };
    SLDataFormat_MIME fmt       = { SL_DATAFORMAT_MIME, nullptr, SL_CONTAINERTYPE_UNSPECIFIED };
    SLDataLocator_OutputMix mix = { SL_DATALOCATOR_OUTPUTMIX, mgr->getMixer() };
    SLDataSource src            = { &loc, &fmt };
    SLDataSink snk              = { &mix, nullptr };
    return make(src, snk, play, -1);
}

bool zSoundPlayerAsset::create(const zPlayerParams& p, bool play) {
    off_t start, length;
    auto asset(AAssetManager_open(manager->getAsset(), p.uri, AASSET_MODE_UNKNOWN));
    if(!asset) return false;
    auto fd(AAsset_openFileDescriptor(asset, &start, &length));
    SLDataLocator_AndroidFD loc  = { SL_DATALOCATOR_ANDROIDFD, fd, start, length };
    SLDataFormat_MIME fmt        = { SL_DATAFORMAT_MIME, nullptr, SL_CONTAINERTYPE_UNSPECIFIED };
    SLDataLocator_OutputMix mix  = { SL_DATALOCATOR_OUTPUTMIX, mgr->getMixer() };
    SLDataSource src             = { &loc, &fmt };
    SLDataSink snk               = { &mix, nullptr };
    return make(src, snk, play, -1);
}

bool zSoundPlayer::setSolo(int chan, bool set) const {
    return (mute && (*mute)->SetChannelSolo(mute, chan, set));
}

bool zSoundPlayer::setMute(int chan, bool set) const {
    return (mute && (*mute)->SetChannelMute(mute, chan, set));
}

u32 zSoundPlayer::getStatus() const {
    u32 status(0);
    if(player) (*player)->GetPlayState(player, &status);
    return status;
}

int zSoundPlayer::getChannels() const {
    u8 num(0);
    if(mute && (*mute)->GetNumChannels(mute, &num) != SL_RESULT_SUCCESS) {
        num = 0;
    }
    return num;
}

bool zSoundPlayer::setVolume(SLmillibel vol) const {
    return (volume && (*volume)->SetVolumeLevel(volume, vol));
}

int zSoundPlayer::getVolume() const {
    SLmillibel vol(0);
    if(volume && (*volume)->GetVolumeLevel(volume, &vol) != SL_RESULT_SUCCESS) {
        vol = 0;
    }
    return vol;
}

bool zSoundPlayer::enableStereoPos(bool set) const {
    return (volume && (*volume)->EnableStereoPosition(volume, set));
}

bool zSoundPlayer::setStereoPos(SLpermille pos) const {
    return (volume && (*volume)->SetStereoPosition(volume, pos));
}

bool zSoundPlayer::play(bool set) {
    auto ret(player && (*player)->SetPlayState(player, set ? SL_PLAYSTATE_PLAYING : SL_PLAYSTATE_PAUSED) == SL_RESULT_SUCCESS);
    if(ret && set) millis = z_timeMillis();
    return ret;
}

bool zSoundPlayer::stop() const {
    return (player && (*player)->SetPlayState(player, SL_PLAYSTATE_STOPPED) == SL_RESULT_SUCCESS);
}

bool zSoundPlayer::loop(bool set) const {
    return (seek && (*seek)->SetLoop(seek, set, 0, SL_TIME_UNKNOWN) == SL_RESULT_SUCCESS);
}

void zSoundPlayer::shutdown() {
    if(object) {
        (*object)->Destroy(object);
        object = nullptr; player = nullptr; recorder = nullptr;
        queue  = nullptr; seek   = nullptr;
        mute   = nullptr; volume = nullptr;
    }
    nextBuffer = nullptr;
    nextSize = totalSize = nextCount = 0;
}

zSoundManager::zSoundManager() {
    // создаем движек
    if((result = slCreateEngine(&object, 0, nullptr, 0, nullptr, nullptr)) == SL_RESULT_SUCCESS) {
        (*object)->Realize(object, SL_BOOLEAN_FALSE);
        (*object)->GetInterface(object, SL_IID_ENGINE, &engine);
        (*engine)->CreateOutputMix(engine, &mixer, 0, nullptr, nullptr);
        (*mixer)->Realize(mixer, SL_BOOLEAN_FALSE);
    }
}

void zSoundManager::shutdown() {
    clear();
    if(mixer) {
        (*mixer)->Destroy(mixer);
        mixer = nullptr;
    }
    if(object) {
        (*object)->Destroy(object);
        object = nullptr; engine = nullptr;
    }
}

zSoundPlayer* zSoundManager::createPlayer(int id, int type, const zPlayerParams& p) {
    zSoundPlayer* pl(nullptr);
    auto idx(players.indexOf(id));
    bool _not(idx == -1);
    if(!_not) {
        pl = players[idx];
        _not = type != pl->type;
        if(_not) {
            players.erase(idx, 1, true);
            pl = nullptr;
        }
    }
    if(!pl) {
        switch(type) {
            case TYPE_URI:  pl = new zSoundPlayerUri(this, id); break;
            case TYPE_MEM:  pl = new zSoundPlayerMem(this, id); break;
            case TYPE_ASSET:pl = new zSoundPlayerAsset(this, id); break;
            case TYPE_REC:  pl = new zSoundPlayerRec(this, id); break;
            case TYPE_VIBRA:pl = new zSoundPlayerVibra(this, id); break;
            default:        return nullptr;
        }
    }
    if(!pl->create(p, false)) { SAFE_DELETE(pl); }
    if(_not && pl) players += pl;
    return pl;
}

zSoundPlayer* zSoundManager:: getPlayer(int id) const {
    auto idx(players.indexOf(id));
    return (idx != -1 ? players[idx] : nullptr);
}

zSoundPlayer* zSoundManager::getFreePlayer(int type) const {
    zSoundPlayer* pl(nullptr);
    auto millis(z_timeMillis());
    for(auto p : players) {
        if(p->type != type) continue;
        if(p->isPlay()) {
            if(p->millis < millis) millis = p->millis, pl = p;
            continue;
        }
        pl = p; break;
    }
    if(pl) pl->stop();
    return pl;
}

void zSoundManager::actionAll(int actionOld, int actionNew) {
    for(auto pl : players) {
        if(pl->getStatus() != actionOld) continue;
        switch(actionNew) {
            case SL_PLAYSTATE_PLAYING:
            case SL_PLAYSTATE_PAUSED:  pl->play(actionNew == SL_PLAYSTATE_PLAYING); break;
            case SL_PLAYSTATE_STOPPED: pl->stop(); break;
            default: break;
        }
    }
}
