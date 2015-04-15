#ifndef PULSEPLAYER_H
#define PULSEPLAYER_H

#include <QFile>
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QObject>
#include <QThread>
#include <QString>
#include <QDebug>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <pulse/thread-mainloop.h>
#include <pulse/context.h>
#include <pulse/pulseaudio.h>
#include <pulse/proplist.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pulseaudiocontroller.h>

#define PA_BUF_SIZE 2048
class InternalPlayer : public QObject
{
    Q_OBJECT
public:
    InternalPlayer(QString fileName, QString sinkName, QString sourceName, QMap<QString, QString> proplist, QObject *parent = 0);
    QString mFilePath;
    QString mSourceName;
    QString mSinkName;
    bool mIsAlive;

    pa_threaded_mainloop* getMainloop() {    return mMainloop;    }
    pa_context* getContext() {    return mContext;    }
    bool isPlaying() {    return mIsPlaying;    }
    char* getDevice() {    return mDevice;    }
    pa_buffer_attr* gerAttrBuff() {    return mAttrBuff;    }
    void setupStream();
    void disconnectStream();

public slots:
    void play(QString sourceName);
    void stop(QString sourceName);

signals:
    void playStateChanged();

private:
    QMap<QString,QString> mPropMap;
    QString mFileName;
    QFile *mFile;
    int mLastError;
    bool mIsInitSuccess;
    bool mIsPlaying;
    //void *mBuffer;
    QByteArray mBuffer;
    size_t mBufferIndex;
    size_t mBufferLength;
    pa_buffer_attr *mAttrBuff;


    // Member variables to control pulseaudio...
    pa_threaded_mainloop *mMainloop;
    pa_mainloop_api *mApi;
    pa_context *mContext;
    pa_stream *mStream;
    pa_proplist *mPropList;

    pa_sample_spec mPASampleSpec;
    char *mDevice;


    // PulseAudio Callback functions...
    static void contextStateCallback(pa_context *context, void *userdata);
    static void streamStateCallback(pa_stream *stream, void *userdata);
    static void streamWriteCallback(pa_stream *stream, size_t length, void *userdata);
    static void streamDrainCallback(pa_stream *stream, int success, void *userdata);
    static void contextDrainCallback(pa_context *context, void *userdata);

    bool setupPulseAudio();
    bool openFile();


    // PulseAudio Processing functions...
    void waitForOperation(pa_operation *operation);
    void playInternal(pa_stream *stream, size_t length, void *userdata);
};

class PulsePlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool playing READ isPlaying NOTIFY playStateChanged)
    Q_PROPERTY(QString source READ sourceName WRITE setSourceName)
    Q_PROPERTY(QString sink READ sinkName WRITE setSinkName)
    Q_PROPERTY(QString role READ roleName WRITE setRoleName)
    Q_PROPERTY(QString file READ fileName WRITE setFileName)
public:
    explicit PulsePlayer(QObject *parent = 0);

    Q_INVOKABLE bool isPlaying() { return mPlayer ? mPlayer->isPlaying() : false; }
    Q_INVOKABLE void play();
    Q_INVOKABLE void stop();
    const QString sourceName() { return mSourceName; }
    void setSourceName(const QString &name) { mSourceName = name; }
    const QString sinkName() { return mSinkName; }
    void setSinkName(const QString &name) { mSinkName = name; }
    const QString roleName() { return mRoleName; }
    void setRoleName(const QString &name) { mRoleName = name; }
    const QString fileName() { return mFileName; }
    void setFileName(const QString &name) { mFileName = name; }


signals:
    void doPlay(QString sourceName);
    void doStop(QString sourceName);
    void playStateChanged();

private:
    InternalPlayer *mPlayer;
    QThread *mWorker;
    QString mSourceName;
    QString mSinkName;
    QString mRoleName;
    QString mFileName;

};

#endif // PULSEPLAYER_H
