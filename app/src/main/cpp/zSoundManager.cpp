//
// Created by User on 06.05.2022.
//

#include "zssh/zCommon.h"
#include "zssh/zSoundManager.h"

static SLresult result;

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
    SLInterfaceID ids[3]  = { type == TYPE_MEM ? SL_IID_ANDROIDSIMPLEBUFFERQUEUE : SL_IID_SEEK, SL_IID_VOLUME };
    SLboolean req[3]      = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
    auto engine(mgr->getEngine());
    shutdown();
    if((result = (*engine)->CreateAudioPlayer(engine, &object, &src, &snk, 2, ids, req)) == SL_RESULT_SUCCESS) {
        (*object)->Realize(object, SL_BOOLEAN_FALSE);
        (*object)->GetInterface(object, SL_IID_VOLUME, &volume);
        (*object)->GetInterface(object, SL_IID_PLAY, &player);
        if(ids[0] == SL_IID_SEEK) {
            (*object)->GetInterface(object, ids[0], &seek);
        } else {
            (*object)->GetInterface(object, ids[0], &queue);
            (*queue)->RegisterCallback(queue, playerCallback, this);
            (*player)->SetCallbackEventsMask(player, SL_PLAYEVENT_HEADMOVING);
            bufSize = (_bufSize ? _bufSize : PLAYER_FRAMES);
        }
        (*player)->SetPlayState(player, play ? SL_PLAYSTATE_PLAYING : SL_PLAYSTATE_STOPPED);
    }
    return result == SL_RESULT_SUCCESS;
}

bool zSoundPlayerVibra::create(const zPlayerParams& p, bool play) {
    auto engine(mgr->getEngine()); auto caps(mgr->getCaps());
    shutdown(); result = SL_RESULT_IO_ERROR;
    if(caps) {
        u32 idxVibra(0), idVibra(SL_DEFAULTDEVICEID_VIBRA);
        (*caps)->QueryVibraCapabilities(caps, &idxVibra, &idxVibra, nullptr);
        if((result = (*engine)->CreateVibraDevice(engine, &object, idVibra, 0, nullptr, nullptr)) == SL_RESULT_SUCCESS) {
            (*object)->Realize(object, SL_BOOLEAN_FALSE);
            (*object)->GetInterface(object, SL_IID_VIBRA, &vibrator);
            DLOG("vibra :%x", vibrator);
        }
    }
    return result == SL_RESULT_SUCCESS;
}

bool zSoundPlayerMem::create(const zPlayerParams& p, bool play) {
    SLDataLocator_AndroidSimpleBufferQueue loc = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2 };
    SLDataFormat_PCM fmt         = { SL_DATAFORMAT_PCM, p.chan, p.rate, p.bits, p.bits,
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

u32 zSoundPlayer::getStatus() const {
    u32 status(0);
    if(player) (*player)->GetPlayState(player, &status);
    return status;
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
        object = nullptr; player = nullptr;
        queue  = nullptr; seek   = nullptr;
        volume = nullptr; recorder = nullptr;
    }
    nextBuffer = nullptr;
    nextSize = totalSize = nextCount = 0;
}

zSoundManager::zSoundManager() {
    // создаем движек
    if(slCreateEngine(&object, 0, nullptr, 0, nullptr, nullptr) == SL_RESULT_SUCCESS) {
        (*object)->Realize(object, SL_BOOLEAN_FALSE);
        (*object)->GetInterface(object, SL_IID_ENGINECAPABILITIES, &capEngine);
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
