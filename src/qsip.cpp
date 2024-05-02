#include "qsip.h"

QSIP::QSIP(QObject *parent) : QObject{parent}
{
    _logLevel = QSIP::E_LOG_LEVEL::INFO;
    _transport = QSIP::E_TRANSPORT::UDP;
    _isStackInitialized = false;
    _logSipMessage = false;
    _verifyTlsCertificate = false;
    _endpointPort = 5060;
}

void QSIP::setLogLevel(const E_LOG_LEVEL &logLevel)
{
    _logLevel = logLevel;
}

void QSIP::logSipMessage(const bool &status)
{
    _logSipMessage = status;
}

bool QSIP::sipStackInitCheck(const QString &logRemark)
{
    if(!_isStackInitialized)
    {
        emit logMessage(QSIP::E_LOG_LEVEL::ERROR,classId,"Please start sip stack before doing an operation, Error : "+logRemark);
        return true;
    }
    return false;
}

void QSIP::setPort(const unsigned int &port)
{
    _endpointPort = port;
}

void QSIP::setTransport(const E_TRANSPORT &transport)
{
    _transport = transport;
}

void QSIP::setVerifyTlsCertificate(const bool &state)
{
    _verifyTlsCertificate = state;
}

void QSIP::setCaListFile(const QFile &caListFile)
{
    _caListFile.setFileName(caListFile.fileName());
}

void QSIP::setCertificateFile(const QFile &certificateFile)
{
    _certificateFile.setFileName(certificateFile.fileName());
}

void QSIP::setPrivateKeyFile(const QFile &privateKeyFile)
{
    _privateKeyFile.setFileName(privateKeyFile.fileName());
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
        emit logMessage(QSIP::E_LOG_LEVEL::ERROR,classId,"Failed to scan capture device, Error : "+QString(err.info().c_str()));
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
        emit logMessage(QSIP::E_LOG_LEVEL::ERROR,classId,"Failed to scan playback device, Error: "+QString(err.info().c_str()));
    }
    return  list;
}

// bool QSIP::isStackInitialized()
// {
//     return _isStackInitialized;
// }


