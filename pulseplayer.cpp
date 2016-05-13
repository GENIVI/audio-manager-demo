/* SPDXLicenseID: MPL-2.0
*
* Copyright (C) 2014, GENIVI Alliance
*
* This file is part of AudioManager Monitor
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License (MPL), v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* For further information see http://www.genivi.org/.
*
* List of changes:
*/

#include "pulseplayer.h"

//PulsePlayer::PulsePlayer(QString filePath, QString sinkName, QString sourceName, QObject *parent) :
//    QObject(parent)
PulsePlayer::PulsePlayer(QObject *parent) :
    QObject(parent), mPlayer(0), mWorker(0)
{
}

void PulsePlayer::play()
{
    QMap<QString, QString> prop;
    prop.insert("media.role", mRoleName);

    if(!mPlayer) {
        mPlayer = new InternalPlayer(mFileName, mSinkName, mSourceName, prop, this);
        mWorker = new QThread;
        mPlayer->moveToThread(mWorker);
        mWorker->start();
        connect(this,SIGNAL(doPlay(QString)),mPlayer,SLOT(play(QString)));
        connect(this,SIGNAL(doStop(QString)),mPlayer,SLOT(stop(QString)));
        connect(mPlayer,SIGNAL(playStateChanged()),this,SIGNAL(playStateChanged()));
    }

    mPlayer->mIsAlive = true;
    while(pa_context_get_state(mPlayer->getContext()) != PA_CONTEXT_READY) {
        //qDebug() << " Waiting for ready state";
    }

    emit doPlay(mPlayer->mSourceName);
}

void PulsePlayer::stop()
{
    qDebug() << "STOP : " << mPlayer->mSourceName;
    mPlayer->mIsAlive = false;
    emit doStop(mPlayer->mSourceName);
}

InternalPlayer::InternalPlayer(QString fileName, QString sinkName, QString sourceName, QMap<QString, QString> proplist, QObject *parent)
    : QObject(parent)
{

    mIsPlaying = false;
    mIsInitSuccess = false;
    mBufferIndex = 0;
    mBufferLength = 0;
    mBuffer = 0;
    mStream = 0;
    mAttrBuff = (pa_buffer_attr*)malloc(sizeof(pa_buffer_attr));
    mAttrBuff->maxlength = (uint32_t) -1;
    mAttrBuff->prebuf = (uint32_t) -1;
    mAttrBuff->tlength = (uint32_t) -1;
    mAttrBuff->minreq = 8192;
    mDevice = NULL;
    mFileName = fileName;

    mPropMap = proplist;
    mPropList = pa_proplist_new();
    QMap<QString, QString>::iterator i = mPropMap.begin();

    for(;i != mPropMap.end(); i++) {
        QString key = i.key();
        QString value = i.value();
        pa_proplist_sets(mPropList, key.toUtf8().constData(), value.toUtf8().constData());
        qDebug() << "Prop List Set " << key.toUtf8().constData() << " / " << value.toUtf8().constData();
    }

    mFile = new QFile(QString(":/")+fileName);
    qDebug() << "FILE NAME : " << fileName << " , Exists = " << mFile->exists();
    bool opened = mFile->open(QIODevice::ReadOnly);

    mSinkName = sinkName;
    mSourceName = sourceName;
    if (!opened) {
        qDebug() << "Cannot open wave file";
        return;
    } else {
        QByteArray content = mFile->readAll();
        QDataStream header(&content, QIODevice::ReadOnly);
        header.setByteOrder(QDataStream::LittleEndian);
        char tempBuffer[1024];
        short channel;
        int sampleRate;

        header.readRawData(tempBuffer,22);
        header >> channel;
        header >> sampleRate;

        content.clear();
        qDebug() << "CHANNEL : " << channel;
        qDebug() << "sampleRate : " << sampleRate;

        mPASampleSpec.channels = channel;
        mPASampleSpec.format = PA_SAMPLE_S16LE;
        mPASampleSpec.rate = sampleRate;
        mFile->close();
    }

    setupPulseAudio();
}

bool InternalPlayer::setupPulseAudio()
{
    mMainloop = pa_threaded_mainloop_new();
    if (pa_threaded_mainloop_start(mMainloop)) {
        qDebug() << "Unable to start pulseaudio threaded mainloop";
        pa_threaded_mainloop_free(mMainloop);
        mMainloop= 0;
        return false;
    }

    mApi = pa_threaded_mainloop_get_api(mMainloop);
    mContext = pa_context_new(mApi, "AM Monitor Player");
    pa_threaded_mainloop_lock(mMainloop);
    pa_context_set_state_callback(mContext, &InternalPlayer::contextStateCallback,this);

    if (pa_context_connect(mContext, NULL, (pa_context_flags_t)0, NULL)) {
        qDebug() << " Cannot connect to context" << pa_strerror(pa_context_errno(mContext));
        pa_context_unref(mContext);
        pa_threaded_mainloop_free(mMainloop);
        return false;
    }

    pa_threaded_mainloop_unlock(mMainloop);

    return true;
}

void InternalPlayer::setupStream()
{
    while(pa_context_get_state(mContext) == PA_OPERATION_RUNNING)
        pa_threaded_mainloop_wait(mMainloop);

    // Connect or Reconnect the stream
    if(mStream) {
        qDebug() << "Reconnect the stream";
        pa_stream_disconnect(mStream);
        pa_stream_unref(mStream);
        mStream = 0;
    }

    mStream = pa_stream_new_with_proplist(mContext, mFile->fileName().toUtf8().constData(), &mPASampleSpec,NULL, mPropList);
    qDebug() << "Stream created";

    pa_stream_connect_playback(mStream, mDevice, mAttrBuff, (pa_stream_flags_t)0, NULL, NULL);
    pa_stream_set_state_callback(mStream, &InternalPlayer::streamStateCallback, this);
}

void InternalPlayer::waitForOperation(pa_operation *)
{
    while(pa_context_get_state(mContext) == PA_OPERATION_RUNNING)
        pa_threaded_mainloop_wait(mMainloop);

    pa_context_unref(mContext);
}

void InternalPlayer::streamStateCallback(pa_stream *stream, void *userdata)
{
    InternalPlayer *player = reinterpret_cast<InternalPlayer*>(userdata);
    qDebug() << "stream State Callback " << QString::number(pa_stream_get_state(stream));
    switch(pa_stream_get_state(stream)) {
        case PA_STREAM_UNCONNECTED:
        case PA_STREAM_TERMINATED:
        case PA_STREAM_CREATING:
            break;
        case PA_STREAM_READY:
            pa_stream_set_write_callback(stream, &InternalPlayer::streamWriteCallback, userdata);
            qDebug() << "Start to play";
            break;
        case PA_STREAM_FAILED:
            break;
    }

    pa_threaded_mainloop_signal(player->getMainloop(),0);
}

void InternalPlayer::streamDrainCallback(pa_stream *, int success, void *userdata)
{
    InternalPlayer *player = reinterpret_cast<InternalPlayer*>(userdata);
    //pa_operation *o = NULL;
    pa_context *context = player->getContext();
    qDebug() << "stream Drain Callback";

    if (!success) {
        qDebug("Failed to drain stream : %s", pa_strerror(pa_context_errno(context)));
        return;
    }

    /*
    pa_stream_disconnect(stream);
    pa_stream_unref(stream);
    stream = NULL;
    player->mIsAlive = false;

    if (!(o = pa_context_drain(context,&InternalPlayer::contextDrainCallback,userdata))) {
        qDebug("Failed to context drain");
        pa_context_disconnect(context);
    } else {
        pa_operation_unref(o);
    }

    */
    player->disconnectStream();
    pa_threaded_mainloop_signal(player->getMainloop(),0);
}

void InternalPlayer::disconnectStream()
{
    pa_operation *o = NULL;
    pa_stream_disconnect(mStream);
    pa_stream_unref(mStream);
    mStream = 0;
    mIsAlive = false;

    if (!(o = pa_context_drain(mContext,&InternalPlayer::contextDrainCallback, this))) {
        qDebug("Failed to context drain");
        pa_context_disconnect(mContext);
    } else {
        pa_operation_unref(o);
    }
}

void InternalPlayer::streamWriteCallback(pa_stream *stream, size_t length, void *userdata)
{
    InternalPlayer *player = reinterpret_cast<InternalPlayer*>(userdata);
    player->playInternal(stream, length, userdata);
    pa_threaded_mainloop_signal(player->getMainloop(),0);
}
void InternalPlayer::contextDrainCallback(pa_context *, void *userdata)
{
    InternalPlayer *player = reinterpret_cast<InternalPlayer*>(userdata);

    qDebug() << " Context Drain Callback";
    pa_threaded_mainloop_signal(player->getMainloop(),0);
}

void InternalPlayer::playInternal(pa_stream *stream, size_t length, void *userdata)
{
    InternalPlayer *player = reinterpret_cast<InternalPlayer*>(userdata);
    pa_operation *o = NULL;


    mBufferLength = length;

    //qDebug(" playInternal : %d, %d", mBufferLength, player->isPlaying());

    while (mBufferLength >0 && player->isPlaying()) {

        mBuffer = mFile->read(mBufferLength);
        int l = mBuffer.size();

        if (l <= 0) {
            mIsPlaying = false;
            emit playStateChanged();
            pa_stream_set_write_callback(stream, NULL, NULL);
            if (!(o = pa_stream_drain(stream, &InternalPlayer::streamDrainCallback, this))) {
                qDebug("pa_stream_drain failed : %s", pa_strerror(pa_context_errno(mContext)));
            }
            if(o)
                pa_operation_unref(o);
            break;
        }

        if (pa_stream_write(stream, (uint8_t*)mBuffer.constData(), l, NULL, 0, PA_SEEK_RELATIVE) < 0) {
            qDebug("pa_stream_write() failed : %s", pa_strerror(pa_context_errno(mContext)));
            break;
        }

        mBufferLength -= l;
        mBufferIndex += l;
        if (!mBufferLength) {
            mBufferLength = mBufferIndex = 0;
        }
    }
}

void InternalPlayer::contextStateCallback(pa_context *context, void *userdata)
{
    InternalPlayer *player = reinterpret_cast<InternalPlayer*>(userdata);

    pa_context_state_t state = pa_context_get_state(context);
    qDebug() << "Context Received " << QString::number(state);
    switch(state) {
        case PA_CONTEXT_CONNECTING:
        case PA_CONTEXT_AUTHORIZING:
        case PA_CONTEXT_SETTING_NAME:
            break;
        case PA_CONTEXT_READY:
            // Ready to play, Start setup the stream
            break;
        case PA_CONTEXT_TERMINATED:
        case PA_CONTEXT_FAILED:
        case PA_CONTEXT_UNCONNECTED:
            break;
    }

    pa_threaded_mainloop_signal(player->getMainloop(),0);
}

bool InternalPlayer::openFile()
{
    if (!mFile->exists()) {
        qDebug() << "File Not exists";
        return false;
    }

    if (!mFile->isOpen()) {
        mFile->open(QIODevice::ReadOnly);
    }
    mFile->seek(22);
    return true;
}

void InternalPlayer::play(QString sourceName)
{

    mIsPlaying = true;
    qDebug() << "Try to play";
    if (!openFile()) {
        qDebug() << "Error occured";
        goto finish;
    }

    if (sourceName != mSourceName) {
        qDebug() << "Different source name " << sourceName << " / " << mSourceName;
        return;
    }

    setupStream();
    emit playStateChanged();
    return;
finish:
    mIsPlaying = false;
    mIsAlive = false;
    emit playStateChanged();
}

void InternalPlayer::stop(QString sourceName)
{
    qDebug() << "SOURCE NAME : " << sourceName << " / " << mSourceName;
    if (sourceName == mSourceName) {
        mIsPlaying = false;
        disconnectStream();
        emit playStateChanged();
    }
}
