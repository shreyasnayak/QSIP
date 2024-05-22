/** @file qsipcall.h
 *
 * @copyright (C) 2024
 * @date 2024.05.21
 * @version 1.0.0
 * @author Shreyas Nayak <shreyasnayak21@gmail.com>
 *
 */

#ifndef QSIPCALL_H
#define QSIPCALL_H
#include <QUuid>
#include <QHash>
#include <pjsua2.hpp>
#include <iostream>
#include <functional>
#include <regex>

using namespace pj;

class QSipCall : public Call
{
public:
    enum ECallType
    {
        INCOMING_CALL,
        OUTGOING_CALL,
        TRANSFER_CALL
    };

    struct SCallStats
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

    struct SCallSetting
    {
        float txVolume;
        float rxVolume;
        ECallType callType;
        QUuid callUuid;
        Call *transferThis;
    };

private:
    ECallType m_callType;
    QUuid m_callId;
    bool m_isCallAlive;
    bool m_callHoldIndicator;
    bool m_ignoreMediaState;
    bool m_ctStatus;
    float m_txvol;
    float m_rxvol;
    Call* m_tranferThis;
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

    bool getCallHoldIndicator() const;
    void setCallHoldIndicator(bool callHoldIndicator);
    void setCustomHeader(const QHash<QString,QString> &pairs);
    void appendCustomHeader(const QString key,const QString value);
    QHash<QString,QString> getCustomHeader() const;
    void ignoreMediaState(bool status);
    bool isCallAlive();
    AudioMedia* getCallMedia();
    SCallStats getCallStats();
    ECallType getCallType() const;
    unsigned int getCallState() const;
    unsigned int getCallMediaState() const;

    void onCallTransferRequest(std::function<void (QString number,QString proxy,QUuid txCallId)> functor);
    void onCallStateChange(std::function<void (QUuid callId,pjsip_inv_state state,QString remark,unsigned int sipCode)> functor);
    void onCallMediaStateChange(std::function<void (QUuid callId,int state)> functor);
    void onLog(std::function<void (int level,QString writtenBy,QString message)> functor);
private:
    void onCallState(OnCallStateParam &prm);
    void onCallMediaState(OnCallMediaStateParam &prm);
    void onCallTransferRequest(OnCallTransferRequestParam &prm);
};


#endif // QSIPCALL_H
