#include "pulseaudiocontroller.h"
#include <QDebug>

class MainLoopLocker {
public:
    MainLoopLocker(pa_threaded_mainloop *mainloop) : m_mainloop(mainloop) {
        pa_threaded_mainloop_lock(mainloop);
    }
    ~MainLoopLocker() {
        pa_threaded_mainloop_unlock(m_mainloop);
    }
private:
    pa_threaded_mainloop *m_mainloop;
};

PulseAudioController::PulseAudioController(QObject *parent) :
    QObject(parent), mController(0)
{
    setupController();

}

void PulseAudioController::setupController()
{
    if (!mController) {
        mController = new InternalController();
        mWorker = new QThread;
        mController->moveToThread(mWorker);
        mWorker->start();

        connect(mController, SIGNAL(sinkInputChanged(QVariantMap)), this, SIGNAL(sinkInputChanged(QVariantMap)));
        connect(mController, SIGNAL(sinkInputRemoved(int)), this, SIGNAL(sinkInputRemoved(int)));

        connect(mController, SIGNAL(sinkInfoChanged(QVariantMap)), this, SIGNAL(sinkInfoChanged(QVariantMap)));
        connect(mController, SIGNAL(sinkInfoRemoved(int)), this, SIGNAL(sinkInfoRemoved(int)));
        connect(mController, SIGNAL(clientChanged(QVariantMap)), this, SIGNAL(clientChanged(QVariantMap)));
        connect(mController, SIGNAL(clientRemoved(int)), this, SIGNAL(clientRemoved(int)));

    }
}




InternalController::InternalController(PulseAudioController *parent) : QObject(parent)
{
    mMainloop = pa_threaded_mainloop_new();

    if (pa_threaded_mainloop_start(mMainloop)) {
        qDebug("Unable to start pulseaudio mainloop");
        pa_threaded_mainloop_free(mMainloop);
        mMainloop = 0;
        return;
    }

    mApi = pa_threaded_mainloop_get_api(mMainloop);

    QTimer::singleShot(0, this, SLOT(connectToContext()));
}

void InternalController::waitForOperation(pa_operation *operation)
{
    while(pa_operation_get_state(operation) == PA_OPERATION_RUNNING)
        pa_threaded_mainloop_wait(mMainloop);
    pa_operation_unref(operation);
}

void InternalController::connectToContext()
{
    MainLoopLocker looplock(mMainloop);
    mContext = pa_context_new(mApi,"AM Monitor PulseAudio Observer");

    if (!mContext) {
        qDebug("Cannot create new pulseaudio context");
        pa_threaded_mainloop_free(mMainloop);
        return;
    }

    pa_context_set_state_callback(mContext, &InternalController::contextStateCallback, this);
    pa_context_set_event_callback(mContext, NULL, NULL);

   if (pa_context_connect(mContext,NULL, (pa_context_flags_t)0, NULL) < 0) {
        qDebug("Cannot create a connection to the pulseaudio context");
        pa_context_unref(mContext);
        pa_threaded_mainloop_free(mMainloop);
        return;
    }
}

void InternalController::setupSubscription()
{
    qDebug("SETUP SUBSCRIPTION");
    pa_operation *o;
    pa_context_set_subscribe_callback(mContext,&InternalController::contextSubscriptionCallback, this);

    if(!(o = pa_context_subscribe(mContext,
                     (pa_subscription_mask_t)
                     (PA_SUBSCRIPTION_MASK_CLIENT
                      |PA_SUBSCRIPTION_MASK_SINK_INPUT
                      | PA_SUBSCRIPTION_MASK_SINK
                      /*|PA_SUBSCRIPTION_MASK_SOURCE_OUTPUT*/),
                     &InternalController::contextSuccessCallback,this)))
        qCritical() << "pa_context_subscribe() failed";
    pa_operation_unref(o);

}



void InternalController::contextStateCallback(pa_context *context, void *userdata)
{
    InternalController *controller = reinterpret_cast<InternalController*>(userdata);

    pa_context_state_t state = pa_context_get_state(context);
    switch(state) {
        case PA_CONTEXT_CONNECTING:
        case PA_CONTEXT_AUTHORIZING:
        case PA_CONTEXT_SETTING_NAME:
            break;
        case PA_CONTEXT_READY:
            controller->setupSubscription();
            break;
        case PA_CONTEXT_UNCONNECTED:
        case PA_CONTEXT_TERMINATED:
        case PA_CONTEXT_FAILED:
            break;
    }

    pa_threaded_mainloop_signal(controller->getMainloop(),0);
}

void InternalController::clientCallback(pa_context *context, const pa_client_info *info, int eol, void *userdata) {
    (void)eol;
    (void)context;
    InternalController *controller = reinterpret_cast<InternalController*>(userdata);
    if (!info)
        return;

    QMap<QString, QVariant> client;
    client["index"] = info->index;
    client["name"] = QString::fromLatin1(info->name);
    client["ownerModule"] = info->owner_module;
    client["driver"] = QString::fromLatin1(info->driver);

    emit controller->clientChanged(client);
}



void InternalController::sinkInputCallback(pa_context *context, const pa_sink_input_info *info, int eol, void *userdata) {
    (void)eol;
    (void)context;
    InternalController *controller = reinterpret_cast<InternalController*>(userdata);
    if (!info)
        return;

    QMap<QString, QVariant> sinkinput;
    sinkinput["index"] = info->index;
    sinkinput["name"] = QString::fromLatin1(info->name);
    sinkinput["volume"] = InternalController::getVolume(info->volume,0);
    sinkinput["clientIndex"] = info->client;
    sinkinput["sinkIndex"] = info->sink;
    sinkinput["corked"] = info->corked;
    sinkinput["appName"] = QString(pa_proplist_gets(info->proplist,"application.name"));
    sinkinput["role"] = QString(pa_proplist_gets(info->proplist,"media.role"));
    //sinkinput["propList"] = info->proplist;
    //sinkinput["sampleSpec"] = info->sample_spec;
    //sinkinput["channelMap"] = info->channel_map;

    emit controller->sinkInputChanged(sinkinput);
}

void InternalController::sinkInfoCallback(pa_context *context, const pa_sink_info *info, int eol, void *userdata)
{
    (void)eol;
    (void)context;
    InternalController *controller = reinterpret_cast<InternalController*>(userdata);

    if (!info)
        return;

    QMap<QString, QVariant> sinkInfo;
    sinkInfo["index"] = info->index;
    sinkInfo["name"] = info->name;
    sinkInfo["volume"] = InternalController::getVolume(info->volume, 0);
    sinkInfo["mute"] = info->mute;

    emit controller->sinkInfoChanged(sinkInfo);
}

int InternalController::getVolume(pa_cvolume volume, int channel)
{
    char ret[PA_VOLUME_SNPRINT_MAX];
    pa_volume_snprint(ret, sizeof(ret), volume.values[channel]);
    return atoi(ret);
}

void InternalController::contextSubscriptionCallback(pa_context *context, pa_subscription_event_type_t t, uint32_t index, void *userdata)
{
    InternalController *controller = reinterpret_cast<InternalController*>(userdata);
    switch( t&PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
        case PA_SUBSCRIPTION_EVENT_CLIENT:
            {
                qDebug() << "XXXXXXXevent: " << (t&PA_SUBSCRIPTION_EVENT_TYPE_MASK);
                if((t&PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
                    emit controller->clientRemoved(index);
                    qDebug() << "removed client" << index;
                } else {
                    qDebug() << "added client" << index;
                    pa_operation *o;
                    if (!(o=pa_context_get_client_info(context, index, &InternalController::clientCallback, controller)))
                        qCritical("pa_context_get_sink_input_info() failed");
                    pa_operation_unref(o);
                }
            break;
            }
        case PA_SUBSCRIPTION_EVENT_SINK_INPUT:
            {
                qDebug() << "event: " << (t&PA_SUBSCRIPTION_EVENT_TYPE_MASK);
                if((t&PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
                    emit controller->sinkInputRemoved(index);
                    qDebug() << "removed sink input" << index;
                } else {
                    qDebug() << "added sink input" << index;
                    pa_operation *o;
                    if (!(o=pa_context_get_sink_input_info(context, index, &InternalController::sinkInputCallback, controller)))
                        qCritical("pa_context_get_sink_input_info() failed");
                    pa_operation_unref(o);
                }
            }
            break;
        case PA_SUBSCRIPTION_EVENT_SINK:
            {
                if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
                    emit controller->sinkInfoRemoved(index);
                } else {
                    pa_operation *o;
                    if (!(o=pa_context_get_sink_info_by_index(context, index, &InternalController::sinkInfoCallback, controller)))
                        qCritical("pa_context_get_sink_info_by_index() failed");
                    pa_operation_unref(o);
                }
            }
            break;
        case PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT:
            break;
    }
    pa_threaded_mainloop_signal(controller->getMainloop(),0);
}

void InternalController::contextSuccessCallback(pa_context *context, int success, void *userdata)
{
    (void)success;
    InternalController *controller = reinterpret_cast<InternalController*>(userdata);
    pa_operation *o;
    if (!(o=pa_context_get_client_info_list(context, &InternalController::clientCallback, controller)))
        qCritical("pa_context_get_client_info_list() failed");
    pa_operation_unref(o);

    if (!(o=pa_context_get_sink_input_info_list(context, &InternalController::sinkInputCallback, controller)))
        qCritical("pa_context_get_sink_input_info_list() failed");
    pa_operation_unref(o);

    if (!(o=pa_context_get_sink_info_list(context, &InternalController::sinkInfoCallback, controller)))
        qCritical("pa_context_get_sink_info_list() failed");
    pa_operation_unref(o);

    pa_threaded_mainloop_signal(controller->getMainloop(),0);
}

InternalController::~InternalController()
{

}

