#include "qsip.h"

#ifndef QSIP_CLASS_NAME
#define QSIP_CLASS_NAME "QSIP"
#endif

QSIP::QSIP(QObject *parent) : QObject{parent}
{
    m_logLevel = QSIP::E_LOG_LEVEL::INFO;
    m_transport = QSIP::E_TRANSPORT::UDP;
    m_isStackInitialized = false;
    m_logSipMessage = false;
    m_verifyTlsCertificate = false;
    m_endPointPort = 5060;
}

void QSIP::setLogLevel(const E_LOG_LEVEL &logLevel)
{
    m_logLevel = logLevel;
}

void QSIP::logSipMessage(const bool &status)
{
    m_logSipMessage = status;
}

bool QSIP::sipStackInitCheck(const QString &logRemark)
{
    if(!m_isStackInitialized)
    {
        emit logMessage(QSIP::E_LOG_LEVEL::ERROR,QSIP_CLASS_NAME,QString("Please start sip stack before doing an operation, Error : %1").arg(logRemark));
        return true;
    }
    return false;
}

void QSIP::setPort(const unsigned int &port)
{
    m_endPointPort = port;
}

void QSIP::setTransport(const E_TRANSPORT &transport)
{
    m_transport = transport;
}

void QSIP::setVerifyTlsCertificate(const bool &state)
{
    m_verifyTlsCertificate = state;
}

void QSIP::setCaListFile(const QFile &caListFile)
{
    m_caListFile.setFileName(caListFile.fileName());
}

void QSIP::setCertificateFile(const QFile &certificateFile)
{
    m_certificateFile.setFileName(certificateFile.fileName());
}

void QSIP::setPrivateKeyFile(const QFile &privateKeyFile)
{
    m_privateKeyFile.setFileName(privateKeyFile.fileName());
}

QStringList QSIP::getCaptureDeviceList()
{
    if(sipStackInitCheck("get capure device failed")) return QStringList();
    QStringList list;
    try
    {
        pj::AudioDevInfoVector2 info = Endpoint::instance().audDevManager().enumDev2();
        foreach (pj::AudioDevInfo audioDevice, info)
        {
            list << QString(audioDevice.name.c_str());
        }
    }
    catch (Error &err)
    {
        emit logMessage(QSIP::E_LOG_LEVEL::ERROR,QSIP_CLASS_NAME,QString("Failed to scan capture device, Error : %1").arg(err.info().c_str()));
    }
    return  list;
}

QStringList QSIP::getPlaybackDeviceList()
{
    if(sipStackInitCheck("get playback device failed")) return QStringList();
    QStringList list;
    try
    {
        pj::AudioDevInfoVector2 info=Endpoint::instance().audDevManager().enumDev2();
        foreach (pj::AudioDevInfo audioDevice, info) list<<QString(audioDevice.name.c_str());
    }
    catch (Error &err)
    {
        emit logMessage(QSIP::E_LOG_LEVEL::ERROR,QSIP_CLASS_NAME,QString("Failed to scan playback device, Error: %1").arg(err.info().c_str()));
    }
    return  list;
}

bool QSIP::setCaptureDevice(const unsigned int &index)
{
    if(sipStackInitCheck("set capture device failed")) return false;
    try
    {
        Endpoint::instance().audDevManager().setCaptureDev(index);
        return true;
    }
    catch (Error &err)
    {
        emit logMessage(QSIP::E_LOG_LEVEL::ERROR,QSIP_CLASS_NAME,QString("Failed to set Capture Device, Error: %1").arg(err.info().c_str()));
        return false;
    }
}

bool QSIP::setPlaybackDevice(const unsigned int &index)
{
    if(sipStackInitCheck("Set playback device failed")) return false;
    try
    {
        Endpoint::instance().audDevManager().setPlaybackDev(index);
        return true;
    }
    catch (Error &err)
    {
        emit logMessage(QSIP::E_LOG_LEVEL::ERROR,QSIP_CLASS_NAME,QString("Failed to set PlayBack Device, Error: %1").arg(err.info().c_str()));
        return false;
    }
}

void QSIP::setCaptureDeviceVol(const unsigned int &volume)
{
    if(sipStackInitCheck("Set capture device volume failed")) return;
    m_cptVol=static_cast<float>(volume)/100;
    foreach (CInAccount *acc, m_allAccount)
    {
        acc->setCaptureDeviceVol(m_cptVol);
    }
}

void QSIP::setPlaybackDeviceVol(const unsigned int &volume)
{
    if(sipStackInitCheck("Set playback device volume failed")) return;
    m_plyVol= static_cast<float>(volume)/100;
    foreach (CInAccount *acc, m_allAccount)
    {
        acc->setPlaybackDeviceVol(m_plyVol);
    }

}

QStringList QSIP::getCodecsList()
{
    if(sipStackInitCheck("Get codecs list failed")) return QStringList();
    QStringList availableCodecs;
    pj::CodecInfoVector2 info;
    try
    {
        info= Endpoint::instance().codecEnum2();
        foreach(pj::CodecInfo codec,info)
        {
            availableCodecs.append(QString(codec.codecId.c_str()));
        }
    }
    catch (Error &err)
    {
        emit logMessage(QSIP::E_LOG_LEVEL::ERROR,QSIP_CLASS_NAME,QString("Failed to read Codec list,Error : %1").arg(err.reason.c_str()));
    }
    return availableCodecs;
}

void QSIP::setCodecList(const QStringList &codecs)
{
    if(sipStackInitCheck("Set Codecs list failed")) return ;
    if(codecs.isEmpty())
    {
        emit logMessage(QSIP::E_LOG_LEVEL::ERROR,QSIP_CLASS_NAME,"Codec List canot be empty");
        return;
    }

    int size=codecs.size();
    QString msg="Codecs set order  : ";
    foreach(QString codec,codecs)
    {
        try
        {
            Endpoint::instance().codecSetPriority(codec.toStdString(),static_cast<pj_uint8_t>(size));
            msg+=codec + " (Priority : " + QString::number(static_cast<pj_uint8_t>(size)) + "), ";
        }
        catch (Error &err)
        {
            emit logMessage(QSIP::E_LOG_LEVEL::ERROR,QSIP_CLASS_NAME,QString("Failed to set codec : %1, Priority : %2, Error : %3").arg(codec,size,err.reason.c_str()));
        }
        size=size-1;
    }

    /* Disable codec */
    foreach(QString codec,getCodecsList())
    {
        if(!codecs.contains(codec))
        {
            try
            {
                Endpoint::instance().codecSetPriority(codec.toStdString(),0);
            }
            catch (Error &err)
            {
                emit logMessage(QSIP::E_LOG_LEVEL::ERROR,QSIP_CLASS_NAME,QString("Failed to clear codec, Error : %1 ").arg(err.reason.c_str()));
            }
        }
    }
    emit logMessage(QSIP::E_LOG_LEVEL::INFO,QSIP_CLASS_NAME,msg);
}

void QSIP::registerForMyPresence(const QUuid &accId)
{

    foreach (CInAccount *acc, m_allAccount)
    {
        acc->registerForMyPresence();
    }

}

bool QSIP::isStackInitialized()
{
    return m_isStackInitialized;
}

void QSIP::initializeSIPStack()
{

}

void QSIP::terminateSIPStack()
{

}

QUuid QSIP::initAccountRegister(const QString &sipUser, const QString &sipPass, const QString &sipDomain, const QString &displayName, const bool &useProxy, const QString &proxyAddr, const E_NAT_TRAVERSAL &natMethod, const TurnConfig &turnConfig, const unsigned int &timeout)
{

}

