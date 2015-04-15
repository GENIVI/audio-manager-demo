#ifndef AUDIOMANAGERDBUSINTERFACE_H
#define AUDIOMANAGERDBUSINTERFACE_H

#include <iostream>

#include <QDebug>
#include <QString>
#include <QObject>
#include <QQuickView>
#include <QQuickItem>
#include <QMap>
#include <QMetaType>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusVariant>
#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusMetaType>
#include <QtDBus/QDBusReply>
#include <QFile>
#include <audiomanagertypes.h>


typedef struct
{
    short  available;
    short  reason;
} Availability;


typedef struct
{
    short classProperty;
    short value;
} ClassProperty;

typedef struct
{
    ushort sinkClassID;
    QString name;
    QList<ClassProperty> listClassProperties;
} SinkClasses;

typedef struct
{
    ushort sourceClassID;
    QString name;
    QList<ClassProperty> listClassProperties;
} SourceClasses;

typedef struct
{
    ushort      sinkID;
    QString       name;
    Availability  availability;
    short       volume;
    short       muteState;
    ushort      sinkClassID;
} SinkType;


typedef struct
{
    ushort      sourceID;
    QString       name;
    Availability  availability;
    ushort      sourceClassID;
} SourceType;

typedef struct
{
    ushort type;
    short value;
} SoundProperty;

typedef struct
{
    ushort type;
    short value;
} SystemProperty;

typedef struct
{
    ushort  mainConnectionID;
    ushort  sourceID;
    ushort  sinkID;
    short   delay;
    short   connectionState;
} MainConnectionType;

typedef struct
{
    ushort mainConenctionID;
    short connectionState;
    ushort sinkID;
    ushort sourceID;
    short delay;
    QList<ushort> listConnectionID;
} MainConnections;


Q_DECLARE_METATYPE(Availability)
Q_DECLARE_METATYPE(ClassProperty)
Q_DECLARE_METATYPE(SinkClasses)
Q_DECLARE_METATYPE(SourceClasses)
Q_DECLARE_METATYPE(SinkType)
Q_DECLARE_METATYPE(SourceType)
Q_DECLARE_METATYPE(SoundProperty)
Q_DECLARE_METATYPE(SystemProperty)
Q_DECLARE_METATYPE(MainConnectionType)

Q_DECLARE_METATYPE(QList<ClassProperty>)
Q_DECLARE_METATYPE(QList<SinkClasses>)
Q_DECLARE_METATYPE(QList<SourceClasses>)
Q_DECLARE_METATYPE(QList<SinkType>)
Q_DECLARE_METATYPE(QList<SourceType>)
Q_DECLARE_METATYPE(QList<SoundProperty>)
Q_DECLARE_METATYPE(QList<MainConnectionType>)

Q_DECLARE_METATYPE(QList<ushort>)


class AudioManagerDBusInterface : public QObject
{
Q_OBJECT

public:

    AudioManagerDBusInterface(QObject* parent, QString serviceName, QString objectPath, QString interfaceName);
    AudioManagerDBusInterface(QObject* parent=0);

    QDBusMessage getDBusMessage(QString command);

    Q_INVOKABLE ushort connect(QString sourceName, QString sinkName);
    Q_INVOKABLE bool disconnect(ushort connectionID);
    QList<SourceType> getMainSourceTypeList();
    QList<SinkClasses> getMainSinkClassesList();
    QList<SourceClasses> getMainSourceClassesList();

    Q_INVOKABLE bool getListMainConnections();

    Q_INVOKABLE bool getListMainSinks();

    Q_INVOKABLE bool getListMainSources();

    Q_INVOKABLE bool getListMainSinkClasses();

    Q_INVOKABLE bool getListMainSourceClasses();

    Q_INVOKABLE bool getListSystemProperties();

    QString getConnectionResultString(short connectionState);

    bool setVolume(QString sinkName, short value);

    void connectToSignal(QString signalName, QObject* receiver, const char* slot);
    void disconnectToSignal(QString signalName, QObject* receiver, const char* slot);
    ushort getSourceIDFromName(QString name);
    ushort getSinkIDFromName(QString name);
    void log(QString message);

signals:
     void sinkAdded(QVariantMap sink);
     void sinkRemoved(int index);
     void sourceAdded(QVariantMap source);
     void sourceRemoved(int index);
     void connectionAdded(QVariantMap connection);
     void connectionRemoved(int index);
     void volumeChanged(int sinkid, int volume);
     void initAMMonitorCompleted();

private slots:
    void onMainConnectionStateChanged(ushort mainConnectionID, short state);
    void onVolumeChanged(ushort sinkID, short volume);
    void onNumberOfMainConnectionsChanged();
    void callAllDataForInit();
    /*
    void onSinkAdded(SinkType sink);
    void onSinkRemoved(ushort sinkID);
    void onSourceAdded(SourceType source);
    void onSourceRemoved(ushort sourceID);
    void onNumberOfSinkClassesChanged();
    void onNumberOfSourceClassesChanged();
    */

private:
    // Parent UI Object Pointer
    QQuickView* mParent;

    // Source, Sink and Connection state list

    /*
    const QString mSourceNameList[5] = {"MediaPlayer","NaviPlayer", "TTSPlayer", "Skype", "ReverseBeep"};
    const QString mSinkNameList[2] = {ALSA_PRIMARY, ALSA_SECONDARY};
    const QString mConnectionStateList[6] = {"Unknown", "Connecting", "Connected", "Disconnecting", "DisConnected", "Suspend"};
    */

    const QString mDBusMessageStringList[21];
    QObject *mMainView;
    QMap<ushort,QString> mCommandList;
    QMap<QString,ushort> mSourceList;
    QMap<QString,ushort> mSinkList;

    // DBusConnection
    QDBusConnection mDBusConnection;
    QString mServiceName;
    QString mObjectPath;
    QString mInterfaceName;
    short mLastError;


    // List Of MainConnections
    QList<short> mMainConnectionList;
    QList<QVariantMap> mMainConnectionTypeList;

    // List for Sinks
    QList<SinkClasses> mMainSinkClassList;
    QList<QVariantMap> mMainSinkList;


    // List for Sources
    QList<SourceClasses> mMainSourceClassList;
    QList<QVariantMap> mMainSourceList;


    // SystemProperty
    QList<SystemProperty> mSystemPropList;

    FILE *mTempLogFile;


    // Declare Private functions

    void initDBusInterface(QString serviceName, QString objectPath, QString interfacename);
    void connectInternalSignalToSlot();
};

#endif // AUDIOMANAGERDBUSINTERFACE_H
