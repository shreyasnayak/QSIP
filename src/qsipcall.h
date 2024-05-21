#ifndef QSIPCALL_H
#define QSIPCALL_H
#include <QUuid>
#include <QHash>
#include <pjsua2.hpp>
#include <iostream>
#include <functional>

using namespace pj;

class QSipCall : public Call
{
public:
    enum E_CALL_TYPE
    {
        INCOMING,
        OUTGOING,
        TRANSFER
    };

    struct CallStats
    {
        /** @brief This will is this valid data or not */
        bool valid=false;
        /** @brief upload band width */
        QString txbps;

        /** @brief download band width */
        QString rxbps;

        /** @brief number of packet sent */
        unsigned int txPkt;

        /** @brief number of packet received */
        unsigned int rxPkt;

        /** @brief Payload sent in bytes */
        unsigned int txPayload;

        /** @brief Payload received in bytes  */
        unsigned int rxPayload;

        /** @brief Transmission loss rate in percentage */
        double txLossRate;

        /** @brief Reception loss rate in percentage  */
        double rxLossRate;

        /** @brief Transmission Jitter in microsecond */
        int txJitterMean;

        /** @brief Reception Jitter in microsecond */
        int rxJitterMean;

        /** @brief Round trip delay in microsecond  */
        int rttMean;

        QString codecInUse;
    };

    struct CallSetting
    {
        float TxVolume;
        float RxVolume;
        QSipCall::E_CALL_TYPE CallType;
        QUuid CallUuid;
        Call *TransferThis;
    };

private:
    E_CALL_TYPE m_callType;
    QUuid m_callId;
    bool m_isCallAlive;
    bool m_isOnHold;
    bool m_ignoreMediaState;
    bool m_ctStatus;
    float m_txvol;
    float m_rxvol;
    Call *m_tranferThis;
    AudioMedia *m_callMedia;
    QHash<QString,QString> m_customHeader;
    unsigned int m_callState;
    unsigned int m_callMediaState;

    std::function<void (QUuid callId, pjsip_inv_state state, QString remark, unsigned int sipCode)> m_fnCallStateChange;
    std::function<void (QUuid callId,int state)> m_fnCallMediaStateChange;
    std::function<void (int level,QString writtenBy,QString message)> m_fnLogMessage;
    std::function<void (QString number,QString proxy,QUuid txCallId)> m_fnCallTransferRequest;


public:
    QSipCall(Account &account, CallSetting setting, int callId = PJSUA_INVALID_ID);
    ~QSipCall();

    bool isCallAlive();
    bool isOnHold() const;
    void setOnHold(bool isOnHold);
    void ignoreMediaState(bool status);

    AudioMedia *callMedia();
    AudioMedia *callMediaNow();
    QSipCall::CallStats callStats();
    E_CALL_TYPE callType() const;
    unsigned int callState() const;
    unsigned int callMediaState() const;

    void onCallTransferRequest(std::function<void (QString number,QString proxy,QUuid txCallId)> functor);
    void onCallStateChange(std::function<void (QUuid callId,pjsip_inv_state state,QString remark,unsigned int sipCode)> functor);
    void onCallMediaStateChange(std::function<void (QUuid callId,int state)> functor);
    void onLog(std::function<void (int level,QString writtenBy,QString message)> functor);

    QHash<QString, QString> getCustomHeader() const;
    void setCustomHeader(const QHash<QString, QString> &value);
    void appendCustomHeader(const QString key,const QString value);

private:
    void onCallState(OnCallStateParam &prm);
    void onCallMediaState(OnCallMediaStateParam &prm);
    void onCallTransferRequest(OnCallTransferRequestParam &prm);
};


#endif // QSIPCALL_H
