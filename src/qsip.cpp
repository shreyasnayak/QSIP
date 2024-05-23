#include "qsip.h"

#ifndef QSIP_CLASS_NAME
#define QSIP_CLASS_NAME "QSIP"
#endif

QSIP::QSIP(QObject *parent) : QObject{parent}
{
    m_logLevel = QSIP::ELogLevel::INFO;
    m_transport = QSIP::ETransport::UDP;
    m_isStackInitialized = false;
    m_logSipMessage = false;
    m_verifyTlsCertificate = false;
    m_endPointPort = 5060;
}

QString QSIP::callStateToString(const ECallState &value)
{

}

void QSIP::setLogLevel(const ELogLevel &logLevel)
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
        emit logMessage(QSIP::ELogLevel::ERROR,QSIP_CLASS_NAME,QString("Please start sip stack before doing an operation, Error : %1").arg(logRemark));
        return true;
    }
    return false;
}

void QSIP::setPort(const unsigned int &port)
{
    m_endPointPort = port;
}

void QSIP::setTransport(const ETransport &transport)
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
        emit logMessage(QSIP::ELogLevel::ERROR,QSIP_CLASS_NAME,QString("Failed to scan capture device, Error : %1").arg(err.info().c_str()));
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
        foreach (pj::AudioDevInfo audioDevice, info)
        {
            list << QString(audioDevice.name.c_str());
        }
    }
    catch (Error &err)
    {
        emit logMessage(QSIP::ELogLevel::ERROR,QSIP_CLASS_NAME,QString("Failed to scan playback device, Error: %1").arg(err.info().c_str()));
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
        emit logMessage(QSIP::ELogLevel::ERROR,QSIP_CLASS_NAME,QString("Failed to set Capture Device, Error: %1").arg(err.info().c_str()));
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
        emit logMessage(QSIP::ELogLevel::ERROR,QSIP_CLASS_NAME,QString("Failed to set PlayBack Device, Error: %1").arg(err.info().c_str()));
        return false;
    }
}

void QSIP::setCaptureDeviceVol(const unsigned int &volume)
{
    if(sipStackInitCheck("Set capture device volume failed")) return;
    m_cptVol = static_cast<float>(volume) / 100;
    foreach (QSipAccount *account, m_allAccount)
    {
        account->setCaptureDeviceVol(m_cptVol);
    }
}

void QSIP::setPlaybackDeviceVol(const unsigned int &volume)
{
    if(sipStackInitCheck("Set playback device volume failed")) return;
    m_plyVol = static_cast<float>(volume) / 100;
    foreach (QSipAccount *account, m_allAccount)
    {
        account->setPlaybackDeviceVol(m_plyVol);
    }
}

void QSIP::setStunServer(const QString &stunServer)
{
    m_endPointStunServer = stunServer;
}

void QSIP::setEndPointNATMethod(const ENatTraversal &method)
{
    m_endPointNat = method;
}

void QSIP::setEndPointTurnServer(const QString &server)
{
    m_endPointStunServer = server;
}

void QSIP::setEndPointTurnUsername(const QString &uname)
{
    m_endPointTurnUser = uname;
}

void QSIP::setEndPointTurnPassword(const QString &password)
{
    m_endPointTurnPassword = password;
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
        emit logMessage(QSIP::ELogLevel::ERROR,QSIP_CLASS_NAME,QString("Failed to read Codec list,Error : %1").arg(err.reason.c_str()));
    }
    return availableCodecs;
}

void QSIP::setCodecList(const QStringList &codecs)
{
    if(sipStackInitCheck("Set Codecs list failed")) return ;
    if(codecs.isEmpty())
    {
        emit logMessage(QSIP::ELogLevel::ERROR,QSIP_CLASS_NAME,"Codec List canot be empty");
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
            emit logMessage(QSIP::ELogLevel::ERROR,QSIP_CLASS_NAME,QString("Failed to set codec : %1, Priority : %2, Error : %3").arg(codec,size,err.reason.c_str()));
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
                emit logMessage(QSIP::ELogLevel::ERROR,QSIP_CLASS_NAME,QString("Failed to clear codec, Error : %1 ").arg(err.reason.c_str()));
            }
        }
    }
    emit logMessage(QSIP::ELogLevel::INFO,QSIP_CLASS_NAME,msg);
}

bool QSIP::isTransferSipHeaderSet()
{
    return m_trasferHeaderOnCallTrasfer;
}

void QSIP::transferSipHeader(const bool &enable)
{
    m_trasferHeaderOnCallTrasfer = enable;
}

void QSIP::registerForMyPresence(const QUuid &accId)
{
    if(m_allAccount.contains(accId) && m_allAccount[accId])
    {
        m_allAccount[accId]->registerForMyPresence();
    }
}

bool QSIP::isStackInitialized()
{
    return m_isStackInitialized;
}

void QSIP::initializeSIPStack()
{
    /* Create end point*/

    m_endPoint = new pj::Endpoint();
    try
    {
        m_endPoint->libCreate();
    }
    catch(Error& err)
    {

        emit logMessage(QSIP::ELogLevel::ERROR,QSIP_CLASS_NAME,QString("Failed to start SIP Stack,Error: %1").arg(err.info().c_str()));
        emit stackInitializationStatus(false,"Failed to start SIP Stack",err.status);
        delete m_endPoint;
        return;
    }


    /* Initialize endpoint */
    EpConfig config;

    /*SetUp Log*/
    config.logConfig.level = m_logLevel;
    config.logConfig.msgLogging = m_logSipMessage;

    if(!m_endPointStunServer.isEmpty())
    {
        config.uaConfig.stunServer.push_back(m_endPointStunServer.toStdString());
    }

    QSipLogWriter* qSipLogWriter = new QSipLogWriter();

    qSipLogWriter->onPjSipLog([this](int logLevel,QString writenBy,QString message)
    {
        message.chop(1);
        emit logMessage(static_cast<QSIP::ELogLevel>(logLevel),writenBy,message);
    });
    config.logConfig.writer = log;

    try
    {
        m_endPoint->libInit(config);
    }
    catch(Error& err)
    {
        emit logMessage(QSIP::ELogLevel::ERROR,QSIP_CLASS_NAME,"Endpoint configuration failed,Error: "+QString(err.info().c_str()));
        emit stackInitializationStatus(false,QString("Endpoint configuration failed"),err.status);
        delete m_endPoint;
        return;
    }

    TransportConfig tcfg;
    tcfg.port = m_endPointPort;
    tcfg.qosParams.dscp_val = 24;
    tcfg.qosParams.flags = PJ_QOS_PARAM_HAS_DSCP;


    /* If TLS Enabled set Certificate file */
    if( m_transport == pjsip_transport_type_e::PJSIP_TRANSPORT_TLS)
    {
        tcfg.tlsConfig.method=  PJSIP_TLSV1_2_METHOD;
        tcfg.tlsConfig.CaListFile = m_caListFile.toStdString();
        tcfg.tlsConfig.certFile = m_certificateFile.toStdString();
        tcfg.tlsConfig.privKeyFile = m_privateKeyFile.toStdString();
        tcfg.tlsConfig.verifyServer = m_verifyTlsCertificate;
    }
    try
    {
        m_endPoint->transportCreate(m_transport, tcfg);
    }
    catch (Error &err)
    {
        emit logMessage(QSIP::ELogLevel::ERROR,QSIP_CLASS_NAME,QString("Transport configuration Failed,Error: %1").arg(err.info().c_str()));
        QString message;
        if((err.status==130048) | (err.status ==120098) )
        {
            message = "Sip Port already in use";
        }
        else
        {
            message = "Transport configuration Failed";
        }
        emit stackInitializationStatus(false,message,err.status);
        delete m_endPoint;
        return;
    }

    try
    {
        m_endPoint->libStart();
    }
    catch (Error &err)
    {
        emit logMessage(QSIP::ELogLevel::ERROR,QSIP_CLASS_NAME,"Failed to start PJSIP Lib,Error: "+QString(err.info().c_str()));
        emit stackInitializationStatus(false,QString("Failed to start PJSIP Lib"),err.status);
        delete m_endPoint;
        return;
    }

    m_isStackInitialized = true;
    emit logMessage(QSIP::ELogLevel::INFO,QSIP_CLASS_NAME,"TxSIP is Running");
    emit stackInitializationStatus(true,QString("SIP Stack is Running"),0);
}

void QSIP::terminateSIPStack()
{
    if(!m_isStackInitialized)
    {
        emit logMessage(QSIP::ELogLevel::ERROR,QSIP_CLASS_NAME,"Failed to Stop SIP Stack,SIP stack is not running");
        emit stackShutdownStatus(false,QString("Failed to Stop SIP Stack,SIP stack is not running"),-1);
        return;
    }
    else
    {
        emit logMessage(QSIP::ELogLevel::DEBUG,QSIP_CLASS_NAME,"Hangup all calls");
        Endpoint::instance().hangupAllCalls();
        pj_thread_sleep(2000);
    }

    foreach (QSipAccount *acc, m_allAccount)
    {
        if(acc)
        {
            acc->shutdown();
            delete acc;
        }
    }

    m_allAccount.clear();
    try
    {
        if( m_endPoint)
        {
            m_endPoint->libDestroy();
            delete m_endPoint;
            emit stackShutdownStatus(true,QString("SIP successfully stopped"),0);
        }
        else
        {
            emit stackShutdownStatus(false,QString("SIP Stack is not running"),0);
        }
    }
    catch (Error &err)
    {
        emit logMessage(QSIP::ELogLevel::ERROR,QSIP_CLASS_NAME,QString("Failed to Stop SIP Stack, Error : %1 ").arg(err.info().c_str()));
        emit stackShutdownStatus(false,QString("Failed to stop SIP Stack"),0);
    }
    m_isStackInitialized = false;
    emit logMessage(QSIP::ELogLevel::INFO,QSIP_CLASS_NAME,"TxSIP is Stoped");
}

void QSIP::hangupAllCalls()
{

}

unsigned int QSIP::nuberOfActiveCall()
{

}

QUuid QSIP::initAccountRegister(const QString &sipUser, const QString &sipPass, const QString &sipDomain, const QString &displayName, const bool &useProxy, const QString &proxyAddr, const ENatTraversal &natMethod, const TurnConfig &turnConfig, const unsigned int &timeout)
{
    if(sipStackInitCheck("Account Registartion failed"))
    {
        return QUuid();
    }

    QUuid accId = QUuid::createUuid();
    emit logMessage(QSIP::ELogLevel::INFO,QSIP_CLASS_NAME,QString("Registering a account,Domine: %1 Proxy: %2 UUID : %3 ").arg(sipDomain,proxyAddr,minifyUuid(accId)));
    QSipAccount* sipAcc = new QSipAccount(sipDomain,sipPass,proxyAddr,sipUser,accId,_transport,_useSRTP,displayName,useProxy,static_cast<unsigned int>(timeout));
    sipAcc->setTranSipHeader(m_trasferHeaderOnCallTrasfer);

    /*Setting onIncomingCall CallBack*/
    sipAcc->setIncomingCall_CallBack([this](QUuid accId,QUuid callId, QString destinationUri,QHash<QString,QString> headerCap)
     {
        QStringList list = destinationUri.split("\"");
        emit logMessage(QSIP::ELogLevel::TRACE,QSIP_CLASS_NAME,QString("Incoming Call From %1").arg(destinationUri));

         QString name;
         QString number;
         if(list.size() > 1)
         {
             number=list[2].split(":")[1].split("@")[0];
             name=list[1];
         }
         else
         {
             number=list[0].split(":")[1].split("@")[0];
         }
         emit incomingCall(accId, callId,number,name,headerCap);
         Q_UNUSED(accId)
     });

    sipAcc->setIM_CallBack([this](QString destinationUri, QString msgText)
    {
       QStringList list=destinationUri.split("\"");
       QString name;
       QString number;
       if(list.size() > 1)
       {
           number=list[2].split(":")[1].split("@")[0];
           name=list[1];
       }
       else
       {
           number=list[0].split(":")[1].split("@")[0];
       }
       emit incomingPager(number,name,msgText);
    });

    //void

    /*Setting onAccountStateChange CallBack*/
    sipAcc->setAccountStateChange_CallBack([this](QUuid accId, bool status, QString statusString)
                                           {
                                               if(status)
                                               {
                                                   emit registrationStateChanged(accId,status,statusString);
                                               }
                                               else
                                               {
                                                   emit registrationStateChanged(accId,status,statusString);
                                               }
                                               emit logMessage(TxSIP::ELogLevel::LOG_DEBUG,classId,"Account state changed to :"+QString::number(status)+" UUID:"+minifyUuid(accId)+",Remark: "+statusString);
                                           });

    /*Setting onCallStateChange CallBack*/
    sipAcc->setCallStateChange_CallBack([this](QUuid callId,pjsip_inv_state state,QString remark,unsigned int sipCode)
                                        {
                                            switch (state)
                                            {
                                            case pjsip_inv_state::PJSIP_INV_STATE_EARLY:        emit callStateChanged(callId, TxSIP::ECallState::EARLY,remark,sipCode); break;
                                            case pjsip_inv_state::PJSIP_INV_STATE_CONNECTING:   emit callStateChanged(callId, TxSIP::ECallState::CONNECTING,remark,sipCode); break;
                                            case pjsip_inv_state::PJSIP_INV_STATE_CONFIRMED:    emit callStateChanged(callId, TxSIP::ECallState::CONNECTED,remark,sipCode); break;
                                            case pjsip_inv_state::PJSIP_INV_STATE_DISCONNECTED: emit callStateChanged(callId, TxSIP::ECallState::DISCONNECTED,remark,sipCode); break;
                                            default: return;
                                            }
                                            emit logMessage(TxSIP::ELogLevel::LOG_TRACE,classId,"Call state update,  CallID:"+minifyUuid(callId)+" CallState:"+ECallStateEnum.valueToKey(state-3)+" Remark : "+remark+" Code:"+QString::number(sipCode));
                                        });

    /*Setting LogMessage CallBack*/
    sipAcc->setLog_CallBack([this](int logLevel,QString writenBy,QString message)
                            {
                                emit logMessage(static_cast<TxSIP::ELogLevel>(logLevel),writenBy,message);
                            });

    /*Setting onMediaState Change CallBack*/
    sipAcc->setMediaStateChange_CallBack([this](QUuid callId,int state)
                                         {
                                             switch (state)
                                             {
                                             case 0 :emit mediaStateChanged(callId, TxSIP::EMediaState::NO_MEDIA, QString("Connecting"));break;
                                             case 1 :emit mediaStateChanged(callId, TxSIP::EMediaState::ACTIVE, QString("Talking"));break;
                                             case 2 :emit mediaStateChanged(callId, TxSIP::EMediaState::ON_HOLD, QString("Call On Hold"));break;
                                             case 3 :emit mediaStateChanged(callId, TxSIP::EMediaState::MEDIA_ERROR, QString("Media error"));break;
                                             default: break;
                                             }
                                             emit logMessage(TxSIP::ELogLevel::LOG_TRACE,classId,"Media state changed,CallID:"+minifyUuid(callId)+" MediaState:"+EMediaStateEnum.valueToKey(state));
                                         });


    sipAcc->setCallTransferRequest_CallBack([this](QString number,QString proxy,QUuid txCallId)
                                            {
                                                emit callTransferRequest(number,proxy,txCallId);
                                            });

    sipAcc->setMessageWaitingIndication_CallBack([this](QUuid accId,int count)
                                                 {
                                                     emit messageWaitingIndication(accId,count);
                                                 });

    sipAcc->setPresenceChange_CallBack([this](QString statusText,QString contact,int state)
                                       {
                                           if(_registerPresence) emit presenceChanged(statusText,contact,state);
                                       });

    sipAcc->setAudioFilePlayingStoped_CallBack([this](QUuid txCallId,QString audioFile,QString message)
                                               {
                                                   emit audioFilePlayingStoped(txCallId,audioFile,message);
                                                   emit logMessage(TxSIP::ELogLevel::LOG_TRACE,classId,"Playing audio file is stoped,CallID:"+minifyUuid(txCallId));
                                               });

    if(_registerPresence)
    {
        // Register for my presence
        sipAcc->registerForMyPresence();

        // Register for buddys presence
        if(!_myBuddys.isEmpty())
        {
            QStringList::const_iterator buddysListIterator;
            for (buddysListIterator = _myBuddys.constBegin(); buddysListIterator != _myBuddys.constEnd();++buddysListIterator)
            {
                QString number = (*buddysListIterator).toLocal8Bit().constData();
                sipAcc->registerForBuddysPresence(number);
            }
        }
    }

    _allAccount[accId]=sipAcc;
    return accId;
    Q_UNUSED(natMethod)

}

bool QSIP::initAccountUnregister(const QUuid &accId)
{

}

QUuid QSIP::initCall(const QUuid &accId, const QString &destinationUri, const SIP_Header &sipHeaders)
{

}

bool QSIP::accept(const QUuid &callId)
{

}

bool QSIP::hold(const QUuid &callId)
{

}

bool QSIP::unhold(const QUuid &callId)
{

}

bool QSIP::mute(const QUuid &callId)
{

}

bool QSIP::unmute(const QUuid &callId)
{

}

bool QSIP::hangup(const QUuid &callId)
{

}

bool QSIP::sendDTMF(const QUuid &callId, const QString &number)
{

}

bool QSIP::transferCall(const QUuid &callId, const QString &destinationUri, const SIP_Header &sipHeaders)
{

}

bool QSIP::transferCall(const QUuid &sourceCallId, const QUuid &destCallId)
{

}


