#include "qsipcall.h"

#ifndef QSIPCALL_CLASS_NAME
#define QSIPCALL_CLASS_NAME "QSipCall"
#endif

QSipCall::QSipCall(Account &account, CallSetting setting, int callId) : Call(account,callId)
{
    m_fnCallStateChange = nullptr;
    m_fnCallMediaStateChange = nullptr;
    m_fnLogMessage = nullptr;
    m_fnCallTransferRequest = nullptr;
    m_tranferThis = setting.transferThis;
    m_callType = setting.callType;
    m_callId = setting.callUuid;
    m_txvol = setting.txVolume;
    m_rxvol = setting.rxVolume;
    m_ctStatus = false;
    m_callState = 0;
    m_callMediaState = 0;
    m_isCallAlive = true;
    m_callHoldIndicator = false;
    m_ignoreMediaState = false;
}

QSipCall::~QSipCall()
{

}

bool QSipCall::isCallAlive()
{
    return m_isCallAlive;
}

AudioMedia *QSipCall::getCallMedia()
{
    if(m_ignoreMediaState | (!m_isCallAlive))
    {
        return nullptr;
    }

    CallInfo ci = getInfo();

    for (unsigned i = 0; i < ci.media.size(); i++)
    {
        if (ci.media[i].type==PJMEDIA_TYPE_AUDIO && getMedia(i))
        {
            AudioMedia *aud_med = static_cast<AudioMedia*>(getMedia(i));
            return aud_med;
        }
    }
    return nullptr;
}

QSipCall::SCallStats QSipCall::getCallStats()
{
    SCallStats info;
    info.valid = false;
    /* Getting BandWidth of the call connection*/
    try
    {
        StreamStat stats = getStreamStat(0);
        StreamInfo steramInfo = getStreamInfo(0);
        info.codecInUse = QString(QSipCall::getStreamInfo(0).codecName.c_str()) +"/"+ QString::number(QSipCall::getStreamInfo(0).codecClockRate);
        pj::RtcpStat stat = stats.rtcp;
        char dump[3000];
        pj::Call *callObj = static_cast<Call*>(this);
        if(pjsua_call_dump(callObj->getId(),PJ_TRUE,dump,sizeof(dump),"  ") == PJ_SUCCESS)
        {
            std::string str(dump);
            std::regex rx("@avg=(.*?)/");
            std::regex_iterator<std::string::iterator> rit(str.begin(), str.end(), rx);
            std::regex_iterator<std::string::iterator> rend;

            if(rit != rend)
            {
                info.rxbps = QString::fromStdString(rit->str());
                info.rxbps.chop(1);
                info.rxbps = info.rxbps.right(info.rxbps.length()-5);
                ++rit;
                if(rit != rend)
                {
                    info.txbps = QString::fromStdString(rit->str());
                    info.txbps.chop(1);
                    info.txbps = info.txbps.right(info.txbps.length()-5);
                }
            }
        }

        /* Packets sent and received */
        info.rxPkt=stat.rxStat.pkt;
        info.txPkt=stat.txStat.pkt;

        /* Payload sent and received */
        info.rxPayload=stat.rxStat.bytes;
        info.txPayload=stat.txStat.bytes;

        /* % of packet loss */
        info.rxLossRate=stat.rxStat.loss * 100.0/(stat.rxStat.pkt+stat.rxStat.loss);
        info.txLossRate=stat.txStat.loss * 100.0/(stat.txStat.pkt+stat.txStat.loss);

        /* Jitter in Microsecond */
        info.rxJitterMean=stat.rxStat.jitterUsec.mean;
        info.txJitterMean=stat.txStat.jitterUsec.mean;

        /*Round trip delay*/
        info.rttMean=stat.rttUsec.mean;

        info.valid = true;

    }
    catch (Error &err)
    {
        if(m_fnLogMessage) m_fnLogMessage(4,QSIPCALL_CLASS_NAME,QString("Failed to get Call stats,Error: %1 ").arg(err.reason.c_str()));
    }
    return info;
}

void QSipCall::ignoreMediaState(bool status)
{
    m_ignoreMediaState = status;
}

bool QSipCall::getCallHoldIndicator() const
{
    return m_callHoldIndicator;
}

void QSipCall::setCallHoldIndicator(bool callHoldIndicator)
{
    m_callHoldIndicator = callHoldIndicator
}

void QSipCall::setCustomHeader(const QHash<QString,QString> &pairs)
{
    m_customHeader = pairs;
}

void QSipCall::appendCustomHeader(const QString key, const QString value)
{
    m_customHeader.insert(key,value);
}

QSipCall::ECallType QSipCall::getCallType() const
{
    return m_callType;
}

unsigned int QSipCall::getCallState() const
{
    return m_callState;
}

unsigned int QSipCall::getCallMediaState() const
{
    return m_callMediaState;
}

QHash<QString,QString> QSipCall::getCustomHeader() const
{
    return m_customHeader;
}

void QSipCall::onCallStateChange(std::function<void (QUuid, pjsip_inv_state, QString, unsigned int)> functor)
{
    m_fnCallStateChange = std::move(functor);
}

void QSipCall::onCallTransferRequest(std::function<void (QString, QString, QUuid)> functor)
{
    m_fnCallTransferRequest = std::move(functor);
}

void QSipCall::onCallMediaStateChange(std::function<void (QUuid, int)> functor)
{
    m_fnCallMediaStateChange = std::move(functor);
}

void QSipCall::onLog(std::function<void (int, QString, QString)> functor)
{
    m_fnLogMessage = std::move(functor);
}

void QSipCall::onCallState(OnCallStateParam &prm)
{
    CallInfo ci = getInfo();
    m_callState = ci.state;

    if(m_fnCallStateChange != nullptr)
    {
        m_fnCallStateChange(m_callId,ci.state,ci.lastReason.c_str(),static_cast<unsigned int>(ci.lastStatusCode));
    }

    if(m_callType == ECallType::TRANSFER_CALL)
    {
        if(((ci.lastStatusCode==180) | (ci.lastStatusCode==183) | (ci.lastStatusCode==200)) && (m_ctStatus==false))
        {
            CallOpParam p;
            try
            {
                m_tranferThis->xferReplaces(*this,p);
                m_ctStatus = true;
                if(m_fnLogMessage)
                {
                    m_fnLogMessage(0,QSIPCALL_CLASS_NAME,QString("Blind transfer successful, Code: %1 ").arg(ci.lastStatusCode));
                }
            }
            catch (Error &err)
            {
                if(m_fnLogMessage)
                {
                    m_fnLogMessage(4,QSIPCALL_CLASS_NAME,QString("Blind Failed to transfer call, Error : %1").arg(err.reason.c_str()));
                }
            }
        }
    }

    if (ci.state == PJSIP_INV_STATE_DISCONNECTED)
    {
        m_isCallAlive=false;
        delete this;
    }

    Q_UNUSED(prm)
}

void QSipCall::onCallMediaState(OnCallMediaStateParam &prm)
{
    if(m_ignoreMediaState | (!m_isCallAlive))
    {
        return;
    }

    CallInfo ci = getInfo();

    for (unsigned i = 0; i < ci.media.size(); i++)
    {
        if ((ci.media[i].type == PJMEDIA_TYPE_AUDIO) && getMedia(i))
        {

            AudioMedia *callAudioMedia = static_cast<AudioMedia*>(getMedia(i));
            AudDevManager& callAudDevManager = Endpoint::instance().audDevManager();
            try
            {
                callAudioMedia->startTransmit(callAudDevManager.getPlaybackDevMedia());
                callAudDevManager.getCaptureDevMedia().startTransmit(*callAudioMedia);
                callAudioMedia->adjustRxLevel(m_rxvol);
                callAudioMedia->adjustTxLevel(m_txvol);
                m_callMediaState = 1;
                if(m_fnCallMediaStateChange)
                {
                    m_fnCallMediaStateChange(m_callId,1);
                }
            }
            catch (Error& err)
            {
                if(m_fnLogMessage)
                {
                    m_fnLogMessage(4,QSIPCALL_CLASS_NAME,QString("Failed to connect audio, 'onCallMediaState' Error: %1").arg(err.reason.c_str()));
                }

                m_callMediaState = 3;

                if(m_fnCallMediaStateChange)
                {
                    m_fnCallMediaStateChange(m_callId,3);
                }
            }
        }
    }
    Q_UNUSED(prm)
}

void QSipCall::onCallTransferRequest(OnCallTransferRequestParam &prm)
{
    /* This code will help to handle the 'Call Barging' */

    /*
        In The case of 'Call Barging' request will be something like this : <*9022001@172.16.0.78>;app=txsq;context=telenetix
        <NUMBER@Account>; ...
    */

    QString referDst = QString(QByteArray(prm.dstUri.c_str(),int(prm.dstUri.length()))); /* Better way of conversion 'std::string' to QString  */
    QStringList tempList = referDst.split('<')[1].split('@');
    QString destination = tempList[0];
    QString referProxy = tempList[1].split('>')[0];

    if(m_fnCallTransferRequest != nullptr)
    {
        m_fnCallTransferRequest(destination,referProxy,m_callId);
    }
}
