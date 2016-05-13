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
* May 2016
*    Added header w/ copyright. 
*/


#include <QTimer>
#include "audiomanagerdbusinterface.h"

AudioManagerDBusInterface::AudioManagerDBusInterface(QObject* parent, QString serviceName, QString objectPath, QString interfaceName) :
    QObject(parent),
    mDBusConnection(QDBusConnection::sessionBus())
{
    initDBusInterface(serviceName, objectPath, interfaceName);
    log("AudioManagerDBusInterface created");
    connectInternalSignalToSlot();

    QTimer::singleShot(0, this, SLOT(callAllDataForInit()));
}


AudioManagerDBusInterface::AudioManagerDBusInterface(QObject* parent) :
    QObject(parent),
    mDBusConnection(QDBusConnection::sessionBus())
{
    initDBusInterface("org.genivi.audiomanager", "/org/genivi/audiomanager/commandinterface", "org.genivi.audiomanager.commandinterface");
    log("AudioManagerDBusInterface created");
    connectInternalSignalToSlot();

    QTimer::singleShot(0, this, SLOT(callAllDataForInit()));
}

void AudioManagerDBusInterface::callAllDataForInit()
{
    qDebug() << "callAllDataForInit";
    getListMainConnections();
    for (int i = 0; i < mMainConnectionTypeList.size(); i++)
        emit connectionAdded(mMainConnectionTypeList.at(i));

    getListMainSinkClasses();
    getListMainSinks();
    getListMainSourceClasses();
    getListMainSources();

    emit initAMMonitorCompleted();
}

void AudioManagerDBusInterface::connectInternalSignalToSlot()
{
    mDBusConnection.connect(mServiceName, mObjectPath, mInterfaceName, "MainConnectionStateChanged", this, SLOT(onMainConnectionStateChanged(ushort,short)));
    mDBusConnection.connect(mServiceName,mObjectPath,mInterfaceName,"VolumeChanged",this,SLOT(onVolumeChanged(ushort,short)));
    mDBusConnection.connect(mServiceName,mObjectPath,mInterfaceName,"NumberOfMainConnectionsChanged",this,SLOT(onNumberOfMainConnectionsChanged()));
}

void AudioManagerDBusInterface::onMainConnectionStateChanged(ushort mainConnectionID, short state)
{
    qDebug() << "GET MAIN CONNECTION STATE CHANGED";

    getListMainConnections();

    if (state == am::CS_CONNECTED) {
        QMap<QString, QVariant> connection;
        for (int i = 0; i < mMainConnectionTypeList.size(); i++) {
            QVariantMap element = mMainConnectionTypeList.at(i);
            if (element["id"] == mainConnectionID) {
                connection = element;
                connection["state"] = QVariant::fromValue(state);
            }
        }

        emit connectionAdded(connection);
    } else if (state == am::CS_DISCONNECTED) {
        QMap<QString, QVariant> connection;
        for (int i = 0; i < mMainConnectionTypeList.size(); i++) {
            QVariantMap element = mMainConnectionTypeList.at(i);
            if (element["id"] == mainConnectionID) {
                mMainConnectionTypeList.removeAt(i);
                break;
            }
        }
        emit connectionRemoved(mainConnectionID);
    }
}

void AudioManagerDBusInterface::onNumberOfMainConnectionsChanged()
{
    //getListMainConnections();
}

void AudioManagerDBusInterface::onVolumeChanged(ushort sinkID, short volume)
{
    qDebug() << "Volume Changed";

    emit volumeChanged(sinkID, volume);

//    getListMainSinks();
//    getListMainSources();
}

void AudioManagerDBusInterface::initDBusInterface(QString serviceName, QString objectPath, QString interfaceName)
{
    mServiceName = serviceName;
    mObjectPath = objectPath;
    mInterfaceName = interfaceName;

    if (!mDBusConnection.isConnected()) {
        mDBusConnection.connectToBus(QDBusConnection::SessionBus, serviceName);
        return;
    }

    qDBusRegisterMetaType<Availability>();
    qDBusRegisterMetaType<MainConnectionType>();
    qDBusRegisterMetaType<QList<MainConnectionType>>();
    qDBusRegisterMetaType<ClassProperty>();
    qDBusRegisterMetaType<QList<ClassProperty>>();
    qDBusRegisterMetaType<SinkClasses>();
    qDBusRegisterMetaType<QList<SinkClasses>>();
    qDBusRegisterMetaType<SourceClasses>();
    qDBusRegisterMetaType<QList<SourceClasses>>();
    qDBusRegisterMetaType<SinkType>();
    qDBusRegisterMetaType<QList<SinkType>>();
    qDBusRegisterMetaType<SourceType>();
    qDBusRegisterMetaType<QList<SourceType>>();
    qDBusRegisterMetaType<SoundProperty>();
    qDBusRegisterMetaType<QList<SoundProperty>>();
}

void AudioManagerDBusInterface::log(QString message)
{
    /*
    mTempLogFile = fopen("/tmp/tempQTLog","a");
    fprintf(mTempLogFile,"%s\n", message.toUtf8().constData());
    fclose(mTempLogFile);
    */
    qDebug() << message;
}

ushort AudioManagerDBusInterface::getSourceIDFromName(QString name)
{
    for (int i = 0; i < mMainSourceList.size(); i++) {
        QVariantMap item = mMainSourceList.at(i);
        if(item["name"] == name)
            return (ushort)item["id"].toInt();
    }
    return 0;
}

ushort AudioManagerDBusInterface::getSinkIDFromName(QString name)
{
    for (int i = 0; i < mMainSinkList.size(); i++) {
        QVariantMap item = mMainSinkList.at(i);
        if(item["name"] == name)
            return (ushort)item["id"].toInt();
    }
    return 0;
}

void AudioManagerDBusInterface::connectToSignal(QString signalName, QObject *receiver, const char* slot)
{
   mDBusConnection.connect(mServiceName,mObjectPath,mInterfaceName,signalName, receiver,slot);
}

void AudioManagerDBusInterface::disconnectToSignal(QString signalName, QObject* receiver, const char* slot)
{
    mDBusConnection.disconnect(mServiceName, mObjectPath, mInterfaceName, signalName, receiver, slot);
}

QDBusMessage AudioManagerDBusInterface::getDBusMessage(QString command)
{
    return QDBusMessage::createMethodCall(mServiceName, mObjectPath, mInterfaceName,command);
}

ushort AudioManagerDBusInterface::connect(QString sourceName, QString sinkName)
{
    QDBusMessage message = getDBusMessage("Connect");
    ushort sourceID = getSourceIDFromName(sourceName);
    ushort sinkID = getSinkIDFromName(sinkName);

    message << QVariant::fromValue(sourceID);
    message << QVariant::fromValue(sinkID);

    qDebug() << "=====";
    qDebug() << "source " << sourceName << sourceID;
    qDebug() << "sink " << sinkName << sinkID;
    qDebug() << "=====";

    QDBusMessage result = mDBusConnection.call(message);
    if(result.type() == QDBusMessage::ReplyMessage) {
        qDebug() << result.arguments();
        return (ushort)result.arguments().at(1).toInt();
    }
    qDebug() << result.errorMessage();
    return 0;
}

bool AudioManagerDBusInterface::setVolume(QString sinkName, short value)
{
    QDBusMessage message = getDBusMessage("SetVolume");
    ushort sinkID = getSinkIDFromName(sinkName);
    if (sinkID == 0)
        return false;
    message << QVariant::fromValue(sinkID);
    message << QVariant::fromValue(value);
    QDBusMessage result = mDBusConnection.call(message);

    if (result.type() != QDBusMessage::ReplyMessage) {
        qDebug() << "Error occured in setVolume()";
        return false;
    }

    mLastError = result.arguments().takeFirst().toInt();

    if (mLastError == am::am_Error_e::E_OK)
        return true;
    else
        return false;
}

bool AudioManagerDBusInterface::disconnect(ushort connectionID)
{
    QDBusMessage message = getDBusMessage("Disconnect");
    message << QVariant::fromValue(connectionID);
    QDBusMessage result = mDBusConnection.call(message);
    if(result.type() == QDBusMessage::ReplyMessage)
        return true;
    return false;
}

bool AudioManagerDBusInterface::getListMainConnections()
{
    QDBusMessage message = getDBusMessage("GetListMainConnections");
    QDBusMessage ret = mDBusConnection.call(message);
    if (ret.type() != QDBusMessage::ReplyMessage) {
        qDebug() << "DBus Error occured in getListMainConenctions";
        return false;
    }

    QList<QVariant> result = ret.arguments();
    mLastError = result.takeFirst().toInt();
    QDBusMessage retMessage;
    retMessage.setArguments(result);

    if (mLastError == am::am_Error_e::E_OK) {
        QDBusReply<QList<MainConnectionType>> reply(retMessage);

        QList<MainConnectionType> connectionList = reply.value();
        mMainConnectionTypeList.clear();

        for (int i = 0; i < connectionList.size(); i++) {
            MainConnectionType info = connectionList[i];
            QMap<QString, QVariant> connection;
            connection["id"] = QVariant::fromValue(info.mainConnectionID);
            connection["sourceId"] = QVariant::fromValue(info.sourceID);
            connection["sinkId"] = QVariant::fromValue(info.sinkID);
            connection["state"] = QVariant::fromValue(info.connectionState);

            mMainConnectionTypeList << connection;
        }
    } else {
        return false;
    }

    return true;
}

bool AudioManagerDBusInterface::getListMainSinks()
{

    QDBusMessage message = getDBusMessage("GetListMainSinks");
    QDBusMessage ret = mDBusConnection.call(message);

    if (ret.type() != QDBusMessage::ReplyMessage) {
        qDebug() << "DBus Error occured in getListMainConenctions";
        return false;
    }

    QList<QVariant> result = ret.arguments();
    mLastError = result.takeFirst().toInt();
    QDBusMessage retMessage;
    retMessage.setArguments(result);

    qDebug() << "REPLY MESSAGE SIZE : " + QString::number(ret.arguments().size());

    if (mLastError == am::am_Error_e::E_OK) {
        QDBusReply<QList<SinkType>> reply(retMessage);

        QList<SinkType> sinkList = reply.value();
        mMainSinkList.clear();

        for (int i = 0; i < sinkList.size(); i++) {
            SinkType info = sinkList[i];
            QMap<QString, QVariant> element;
            element["name"] = info.name;
            element["volume"] = info.volume;
            element["id"] = info.sinkID;
            element["muteState"] = info.muteState;
            emit sinkAdded(element);
            mMainSinkList << element;
        }
    } else {
        return false;
    }

    return true;
}

bool AudioManagerDBusInterface::getListMainSources()
{

    QDBusMessage message = getDBusMessage("GetListMainSources");
    QDBusMessage ret = mDBusConnection.call(message);

    if (ret.type() != QDBusMessage::ReplyMessage) {
        qDebug() << "DBus Error occured in getListMainConenctions";
        return false;
    }
    QList<QVariant> result = ret.arguments();
    mLastError = result.takeFirst().toInt();
    QDBusMessage retMessage;
    retMessage.setArguments(result);

   if (mLastError == am::am_Error_e::E_OK) {
        QDBusReply<QList<SourceType>> reply(retMessage);

        QList<SourceType> sourceList = reply.value();
        mMainSourceList.clear();

        for (int i = 0; i < sourceList.size(); i++) {
            SourceType info = sourceList[i];
            QMap<QString, QVariant> element;
            element["name"] = info.name;
            element["id"] = info.sourceID;
            mMainSourceList << element;
            emit sourceAdded(element);
        }
    } else {
       return false;
   }
   return true;
}

bool AudioManagerDBusInterface::getListMainSourceClasses()
{
    mMainSourceClassList.clear();

    QDBusMessage message = getDBusMessage("GetListSourceClasses");
    QDBusMessage ret = mDBusConnection.call(message);

    if (ret.type() != QDBusMessage::ReplyMessage) {
        qDebug() << "Error occured in getListMainSourceClasses";
        return false;
    }

    QList<QVariant> result = ret.arguments();
    mLastError = result.takeFirst().toInt();
    QDBusMessage retMessage;
    retMessage.setArguments(result);

    if (mLastError == am::am_Error_e::E_OK) {
        QDBusReply<QList<SourceClasses>> reply(retMessage);
        mMainSourceClassList = reply.value();
        log(QString("CLASS SOURCE RECEIVE SUCCESS : ") + QString::number(mMainSourceClassList.size()));

        for (int i = 0; i < mMainSourceClassList.size(); i++) {
            log(QString("SOURCE CLASSES : ") + mMainSourceClassList.at(i).name);
        }
    } else {
        return false;
    }

    return true;
}

bool AudioManagerDBusInterface::getListMainSinkClasses()
{
    mMainSinkClassList.clear();

    QDBusMessage message = getDBusMessage("GetListSinkClasses");
    QDBusMessage ret = mDBusConnection.call(message);
    if (ret.type() != QDBusMessage::ReplyMessage) {
        qDebug() << "Error occured in getListMainSinkClasses()";
        return false;
    }

    QList<QVariant> result = ret.arguments();
    mLastError = result.takeFirst().toInt();
    QDBusMessage retMessage;
    retMessage.setArguments(result);

    if (mLastError == am::am_Error_e::E_OK) {
        QDBusReply<QList<SinkClasses>> reply(retMessage);
        mMainSinkClassList = reply.value();

        qDebug() << QString("CLASS SINK RECEIVE SUCCESS : ") + QString::number(mMainSinkClassList.size());
        for (int i = 0; i < mMainSinkClassList.size(); i++) {

            qDebug() << QString("SINK CLASSES : ") << mMainSinkClassList.at(i).name;
        }
    } else {
        return false;
    }

    return true;
}

bool AudioManagerDBusInterface::getListSystemProperties()
{
    mSystemPropList.clear();

    QDBusMessage message = getDBusMessage("GetListSystemProperties");
    QDBusMessage ret = mDBusConnection.call(message);

    if (ret.type() != QDBusMessage::ReplyMessage) {
        qDebug() << "Error occured in getListSystemProperties()";
        return false;
    }

    QList<QVariant> result = ret.arguments();
    mLastError = result.takeFirst().toInt();
    QDBusMessage retMessage;
    retMessage.setArguments(result);

    if (mLastError == am::am_Error_e::E_OK) {
        QDBusReply<QList<SystemProperty>> reply(retMessage);

        mSystemPropList = reply.value();
        qDebug() << QString("GET SYSTEM PROPERTY LIST SUCCESS ") << mSystemPropList.size();

        for (int i = 0; i < mSystemPropList.size(); i++) {
            qDebug() << QString("System Prop :[") + QString::number(mSystemPropList.at(i).type) + " , " + QString::number(mSystemPropList.at(i).value);
        }

    } else {
        return false;
    }

    return true;
}



QList<SinkClasses> AudioManagerDBusInterface::getMainSinkClassesList()
{
    return mMainSinkClassList;
}

QList<SourceClasses> AudioManagerDBusInterface::getMainSourceClassesList()
{
    return mMainSourceClassList;
}

QString AudioManagerDBusInterface::getConnectionResultString(short connectionState)
{
    switch(connectionState) {
    case am::CS_CONNECTED:
        return "Connected";
    case am::CS_CONNECTING:
        return "Connecting";
    case am::CS_DISCONNECTED:
        return "Disconnected";
    case am::CS_DISCONNECTING:
        return "Disconnecting";
    case am::CS_MAX:
        return "Connection MAX";
    case am::CS_SUSPENDED:
        return "Suspended";
    default:
        return "Unknown";
    }

    return "Unknown"; // Maybe not reached
}

QDBusArgument &operator << (QDBusArgument &dest, const Availability &source)
{
    dest.beginStructure();
    dest << source.available
         << source.reason;
    dest.endStructure();
    return dest;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, Availability &theStruct)
{
    argument.beginStructure();
    argument >> theStruct.available
             >> theStruct.reason;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator << (QDBusArgument& dest, const MainConnectionType& source)
{
   dest.beginStructure();
   dest    << source.mainConnectionID
           << source.sourceID
           << source.sinkID
           << source.delay
           << source.connectionState;
   dest.endStructure();
   return dest;
}
const QDBusArgument &operator >> (const QDBusArgument& source, MainConnectionType& dest)
{
   source.beginStructure();
   source  >> dest.mainConnectionID
           >> dest.sourceID
           >> dest.sinkID
           >> dest.delay
           >> dest.connectionState;
   source.endStructure();
   return source;
}

QDBusArgument &operator << (QDBusArgument& dest, const SoundProperty& source)
{
    dest.beginStructure();
    dest	<< source.type
            << source.value;
    dest.endStructure();
    return dest;
}

const QDBusArgument &operator >> (const QDBusArgument& source, SoundProperty& dest)
{
    source.beginStructure();
    source >> dest.type
           >> dest.value;
    source.endStructure();
    return source;
}


QDBusArgument &operator << (QDBusArgument& dest, const SourceClasses& source)
{
    dest.beginStructure();
    dest << source.sourceClassID
         << source.name
         << source.listClassProperties;
    dest.endStructure();
    return dest;
}

const QDBusArgument &operator >> (const QDBusArgument& source, SourceClasses& dest)
{
    source.beginStructure();
    source >> dest.sourceClassID
            >> dest.name
            >> dest.listClassProperties;
    source.endStructure();
    return source;
}

QDBusArgument &operator << (QDBusArgument& dest, const SinkClasses& source)
{
    dest.beginStructure();
    dest << source.sinkClassID
         << source.name
         << source.listClassProperties;
    dest.endStructure();
    return dest;
}

const QDBusArgument &operator >> (const QDBusArgument& source, SinkClasses& dest)
{
    source.beginStructure();
    source >> dest.sinkClassID
            >> dest.name
            >> dest.listClassProperties;
    source.endStructure();
    return source;
}

QDBusArgument &operator << (QDBusArgument& dest, const ClassProperty& source)
{
    dest.beginStructure();
    dest << source.classProperty
         << source.value;
    dest.endStructure();
    return dest;
}

const QDBusArgument &operator >> (const QDBusArgument& source, ClassProperty& dest)
{
    source.beginStructure();
    source >> dest.classProperty
            >> dest.value;
    source.endStructure();
    return source;
}


QDBusArgument &operator << (QDBusArgument& dest, const SinkType& source)
{
    dest.beginStructure();
    dest << source.sinkID
         << source.name
         << source.availability
         << source.volume
         << source.muteState
         << source.sinkClassID;
    dest.endStructure();
    return dest;
}

const QDBusArgument &operator >> (const QDBusArgument& source, SinkType& dest)
{
    source.beginStructure();
    source >> dest.sinkID
            >> dest.name
            >> dest.availability
            >> dest.volume
            >> dest.muteState
            >> dest.sinkClassID;
    source.endStructure();
    return source;
}

QDBusArgument &operator << (QDBusArgument& dest, const SourceType& source)
{
    dest.beginStructure();
    dest << source.sourceID
         << source.name
         << source.availability
         << source.sourceClassID;
    dest.endStructure();

    return dest;
}

const QDBusArgument &operator >> (const QDBusArgument& source, SourceType& dest)
{
    source.beginStructure();
    source >> dest.sourceID
            >> dest.name
            >> dest.availability
            >> dest.sourceClassID;
    source.endStructure();

    return source;
}
