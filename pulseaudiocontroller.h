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


#ifndef PULSEAUDIOCONTROLLER_H
#define PULSEAUDIOCONTROLLER_H

#include <QObject>
#include <QThread>
#include <QMap>
#include <QtQml>
#include <thread>
#include <pulse/mainloop.h>
#include <pulse/thread-mainloop.h>
#include <pulse/context.h>
#include <pulse/pulseaudio.h>
#include <pulse/proplist.h>


class InternalController;

class PulseLocker {
public:
    PulseLocker(pa_threaded_mainloop *mainloop)
        : mMainloop(mainloop) {
        pa_threaded_mainloop_lock(mainloop);
    }
    ~PulseLocker() {
        pa_threaded_mainloop_unlock(mMainloop);
    }
private:
    pa_threaded_mainloop *mMainloop;
};

class PulseAudioController : public QObject
{
    Q_OBJECT
public:
    explicit PulseAudioController(QObject *parent = 0);
    void setupController();


signals:
    void sinkInputChanged(QVariantMap sinkinput);
    void sinkInputRemoved(int index);
    void sinkInfoChanged(QVariantMap sinkinfo);
    void sinkInfoRemoved(int index);
    void clientChanged(QVariantMap client);
    void clientRemoved(int index);


private:
    InternalController *mController;
    QThread *mWorker;


};

class InternalController : public QObject
{
    Q_OBJECT
public:
    explicit InternalController(PulseAudioController *parent = 0);

    pa_threaded_mainloop* getMainloop() {    return mMainloop;    }

    ~InternalController();
signals:
    void sinkInputChanged(QVariantMap sinkinput);
    void sinkInputRemoved(int index);
    void sinkInfoChanged(QVariantMap sinkinfo);
    void sinkInfoRemoved(int index);
    void clientChanged(QVariantMap client);
    void clientRemoved(int index);

private slots:
    void connectToContext();

private:
    PulseAudioController* mParent;
    pa_threaded_mainloop *mMainloop;
    pa_mainloop_api* mApi;
    pa_context* mContext;

    void setupSubscription();

    static void contextStateCallback(pa_context *context, void *userdata);
    static void contextSubscriptionCallback(pa_context *context, pa_subscription_event_type_t t, uint32_t index, void *userdata);
    static void sinkInputCallback(pa_context *, const pa_sink_input_info *, int, void *);
    static void clientCallback(pa_context *, const pa_client_info *, int, void *);
    static void sinkInfoCallback(pa_context *context, const pa_sink_info *info,int eol, void *userdata);
    static void contextSuccessCallback(pa_context *context, int success, void *userdata);
    static void eventCallback(pa_context *context, const char *name, pa_proplist *pl, void *userdata);
    static int getVolume(pa_cvolume volume, int channel = 0);
    void waitForOperation(pa_operation *operation);
};

#endif // PULSEAUDIOCONTROLLER_H
