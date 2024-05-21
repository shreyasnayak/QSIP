#include "qsipcall.h"

QSipCall::QSipCall(Account &account, CallSetting setting, int callId) : Call(account,callId)
{
    m_callType = setting.CallType;
    m_callId = setting.TxCallId;
    m_callTransferRequest_callBack = nullptr;
    m_ctStatus = false;
    m_callState = 0;
    m_callMediaState = 0;
    m_isCallAlive = true;
    m_isOnHold = false;
    m_ignoreMediaState = false;
    m_callMedia = nullptr;

    m_tranferThis = setting.TransferThis;
    m_txvol = setting.TxVolume;
    m_rxvol = setting.RxVolume;
}

QSipCall::~QSipCall()
{

}

bool QSipCall::isCallAlive()
{
    return m_isCallAlive;
}

bool QSipCall::isOnHold() const
{
    return m_isOnHold;
}

void QSipCall::setOnHold(bool isOnHold)
{

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

