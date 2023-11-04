//
// Created by User on 06.05.2022.
//

#pragma once

enum { PLAYER_FRAMES = 16384, RECORDER_FRAMES = 16000 * 5 };
// типы проигрывателей
enum { TYPE_UNK, TYPE_ASSET, TYPE_URI, TYPE_MEM, TYPE_VIBRA };

struct zPlayerParams {
    // для ASSET/URI - путь к ресурсу
    cstr uri;
    struct {
        // для MEM - частота/число каналов
        u32 rate{0};
        u32 chan{2};
        u32 bits{8};
        int bufSize{0};
    };
};

class zSoundManager;
class zSoundPlayer {
public:
    // конструктор
    zSoundPlayer(zSoundManager* _mgr, int _id, int _type) : mgr(_mgr), id(_id), type(_type) { millis = z_timeMillis(); }
    virtual ~zSoundPlayer() { shutdown(); }
    virtual bool create(const zPlayerParams& p, bool play) = 0;
    virtual void setData(u8* data, int size) { }
    void shutdown();
    bool operator == (int _id) const { return id == _id; }
    bool isPlay() const { return getStatus() == SL_PLAYSTATE_PLAYING; }
    // получить статус
    u32 getStatus() const;
    // установка громкости
    bool setVolume(SLmillibel vol) const;
    // получение громкости
    int getVolume() const;
    // включение/отключение стерео позиции
    bool enableStereoPos(bool set) const;
    // установка стерео позиции
    bool setStereoPos(SLpermille pos) const;
    // воспроизведение/пауза
    bool play(bool set);
    // остановка воспроизведения
    bool stop() const;
    // установка признака зацикленности(например, для музыки)
    bool loop(bool set) const;
    // формирование
    bool make(SLDataSource& src, SLDataSink& snk, bool play, int _bufSize);
    // главный объект плеера
    SLObjectItf object{nullptr};
    // интерфейс плеера
    SLPlayItf player{nullptr};
    // интферфейс рекордера
    SLRecordItf recorder{nullptr};
    // звук
    SLVolumeItf volume{nullptr};
    // поиск
    SLSeekItf seek{nullptr};
    // вибратор
    SLVibraItf vibrator{nullptr};
    // буфер
    SLAndroidSimpleBufferQueueItf queue{nullptr};
    // буфер
    u8* nextBuffer{nullptr};
    // размер
    i32 bufSize{0}, totalSize{0}, nextSize{0};
    // идентификатор/типы
    int id{0}, type{TYPE_UNK};
    // время запуска
    i64 millis{0};
    // менеджер
    zSoundManager* mgr{nullptr};
};

class zSoundPlayerMem : public zSoundPlayer {
public:
    zSoundPlayerMem(zSoundManager* mgr, int id) : zSoundPlayer(mgr, id, TYPE_MEM) { }
    virtual bool create(const zPlayerParams& p, bool play) override;
    virtual void setData(u8* data, int size) override;
    void dataMem();
};

class zSoundPlayerAsset : public zSoundPlayer {
public:
    zSoundPlayerAsset(zSoundManager* mgr, int id) : zSoundPlayer(mgr, id, TYPE_ASSET) { }
    virtual bool create(const zPlayerParams& p, bool play) override;
};

class zSoundPlayerUri : public zSoundPlayer {
public:
    zSoundPlayerUri(zSoundManager* mgr, int id) : zSoundPlayer(mgr, id, TYPE_URI) { }
    virtual bool create(const zPlayerParams& p, bool play) override;
};

class zSoundPlayerVibra : public zSoundPlayer {
public:
    zSoundPlayerVibra(zSoundManager* mgr, int id) : zSoundPlayer(mgr, id, TYPE_VIBRA) { }
    virtual bool create(const zPlayerParams& p, bool play) override;
};

class zSoundManager {
public:
    zSoundManager();
    virtual ~zSoundManager() { shutdown(); }
    // выгрузить
    void shutdown();
    // очистить
    void clear() { players.clear(); }
    // выполнить действие для всех активных
    void actionAll(int actionOld, int actionNew);
    // удалить плеер
    bool deletePlayer(int id);
    // создать новый плеер
    zSoundPlayer* createPlayer(int id, int type, const zPlayerParams& p);
    // вернуть плеер по идентификатору
    zSoundPlayer* getPlayer(int id) const;
    // вернуть свободный плеер по типу
    zSoundPlayer* getFreePlayer(int type) const;
    // вернуть движок
    SLEngineItf getEngine() const { return engine; }
    // вернуть миксер
    SLObjectItf getMixer() const { return mixer; }
    // вернуть капсы
    SLEngineCapabilitiesItf getCaps() const { return capEngine; }
protected:
    // движок
    SLObjectItf object{nullptr};
    SLEngineItf engine{nullptr};
    SLEngineCapabilitiesItf capEngine{nullptr};
    // миксер
    SLObjectItf mixer{nullptr};
    // проигрыватели
    zArray<zSoundPlayer*> players{};
};
