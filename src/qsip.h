/** @file qsip.h
 *
 * @copyright (C) 2024
 * @date 2024.05.21
 * @version 1.0.0
 * @author Shreyas Nayak <shreyasnayak21@gmail.com>
 *
 */


#ifndef QSIP_H
#define QSIP_H

#include <QObject>
#include <QFile>
#include <pjsua2.hpp>
#include "qsiplogwriter.h"
#include "qsipaccount.h"

using namespace pj;

class QSIP : public QObject
{
    Q_OBJECT
public:
    enum ELogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };

    enum ETransport
    {
        UDP,
        TCP,
        TLS
    };

    enum EPayload
    {
        RTP,
        SRTP
    };

    enum ENatTraversal
    {
        NONE,
        STUN,
        TURN,
        ICE
    };

    enum ECallState
    {
        EARLY,
        CONNECTING,
        CONNECTED,
        DISCONNECTED
    };


    enum ECallMediaState
    {
        NO_MEDIA,
        ACTIVE,
        ON_HOLD,
        MEDIA_ERROR
    };

    struct
    {
        QString turn_server;
        QString user_name;
        QString user_password;
    } TurnConfig;

    typedef QHash<QString,QString> SIP_Header;
    typedef QList<QUuid> SIP_Account_List;

    Q_ENUM(ELogLevel)
    Q_ENUM(ETransport)
    Q_ENUM(EPayload)
    Q_ENUM(ECallState)
    Q_ENUM(ECallMediaState)

private:
    explicit QSIP(QObject *parent = nullptr);
    ~QSIP();
    static QString minifyUuid(const QUuid &uuid);
    static QString callStateToString(const ECallState &value);

    pj::Endpoint *m_endPoint;
    QHash<QUuid,QSipAccount*> m_allAccount;

    bool m_isStackInitialized;
    bool m_logSipMessage;
    bool m_verifyTlsCertificate;
    bool m_trasferHeaderOnCallTrasfer;
    QFile m_caListFile;
    QFile m_certificateFile;
    QFile m_privateKeyFile;
    float m_plyVol;
    float m_cptVol;
    unsigned int m_endPointPort;
    ELogLevel m_logLevel;
    ETransport m_transport;
    ENatTraversal m_endPointNat;
    QString m_endPointStunServer;
    QString m_endPointTurnServer;
    QString m_endPointTurnUser;
    QString m_endPointTurnPassword;

public:
    static QSIP* GetInstance(QObject* parent = nullptr);

    /* Log settings */
    void setLogLevel(const QSIP::ELogLevel &logLevel);
    void logSipMessage(const bool &status);

    /* Endpoint transport setting */
    inline bool sipStackInitCheck(const QString &logRemark);
    void setPort(const unsigned int &port = 5060);
    void setTransport(const QSIP::ETransport &transport = QSIP::ETransport::UDP);
    void setVerifyTlsCertificate(const bool &state);
    void setCaListFile(const QFile &caListFile);
    void setCertificateFile(const QFile &certificateFile);
    void setPrivateKeyFile(const QFile &privateKeyFile);

    /* Audio Device */
    QStringList getCaptureDeviceList();
    QStringList getPlaybackDeviceList();
    bool setCaptureDevice(const unsigned int &index);
    bool setPlaybackDevice(const unsigned int &index);
    void setCaptureDeviceVol(const unsigned int &volume);
    void setPlaybackDeviceVol(const unsigned int &volume);

    /*Default STUN & Nat & Turn Server Setup*/
    void setStunServer(const QString &stunServer);
    void setEndPointNATMethod(const ENatTraversal &method);
    void setEndPointTurnServer(const QString &server);
    void setEndPointTurnUsername(const QString &uname);
    void setEndPointTurnPassword(const QString &password);

    /* Codec */
    QStringList getCodecsList();
    void setCodecList(const QStringList &codecs);

    /* Trasfer sip header */
    bool isTransferSipHeaderSet();
    void transferSipHeader(const bool &enable);

    /* Account settings */
    bool isAccountExist(const QUuid &accId);
    void setPayLoad(const QSIP::EPayload &payload);

    void registerForMyPresence(const QUuid &accId);
    void registerForBuddyPresence(const QUuid &accId, const QString &buddyContact);
    void registerForBuddyPresence(const QUuid &accId, const QStringList &buddysList);
    void publishMyPresence(const QUuid &accId, const bool state);
    bool sendInstantMessage(const QString &destinationUri,const QString &msg,const QUuid &accId);

    /* SIP stack status */
    bool isStackInitialized();
    void initializeSIPStack();
    void terminateSIPStack();
    void hangupAllCalls();
    unsigned int nuberOfActiveCall();

    QSIP::SIP_Account_List sipAccounts();

    /* startRegister takes the account parameters and returns a UUID which is the account ID. This account ID will be used to identify the account going forward.
     * Note that this function just sends the SIP REGISTER message. The state of the registration will be emitted through the signal `registrationStateChanged` */
    QUuid initAccountRegister(const QString &sipUser, const QString &sipPass, const QString &sipDomain, const QString &displayName, const bool &useProxy,const QString &proxyAddr, const QSIP::ENatTraversal &natMethod = QSIP::ENatTraversal::NONE, const QSIP::TurnConfig &turnConfig, const unsigned int &timeout = 120);

    /* startUnregister takes the account UUID as the argument and unregisters that account.
     * Note that this function only starts the unregistration process. The state of unregistration will be emitted through the signal `registrationStateChanged` */
    bool initAccountUnregister(const QUuid& accId);

    /* startCall takes ID of the account to use to make the call, the SIP URI and the SIP headers to set if any. The function returns a UUID which is the call ID.
     * This UUID will be used to identify this call going forward.
     * Note that this function just sends the SIP INVITE. The state of the call will be emitted through the signal `callStateChanged` */
    QUuid initCall(const QUuid& accId, const QString &destinationUri, const QSIP::SIP_Header &sipHeaders);

    /* When a incoming call arrives as is notified through the signal `incomingCall`, the call can be accepted by calling this function. */
    bool accept(const QUuid& callId);

    /* For any incoming or outgoing call, the call can be put on hold by calling this function. */
    bool hold(const QUuid& callId);

    /* For any incoming or outgoing call in hold, the call can be resumed by calling this function. */
    bool unhold(const QUuid& callId);

    /* Call this function to stop sending audio to a call. */
    bool mute(const QUuid& callId);

    /* Call this function to resume sending audio to a call. */
    bool unmute(const QUuid& callId);

    /* Reject incoming call or drop out going call */
    bool hangup(const QUuid& callId);

    bool sendDTMF(const QUuid& callId,const QString &number);

    /*This function will help you to do blind transfer*/
    bool transferCall(const QUuid& callId, const QString &destinationUri, const QSIP::SIP_Header &sipHeaders);

    /*This function will help you to do attended transfer*/
    bool transferCall(const QUuid& sourceCallId, const QUuid& destCallId);
signals:
     void logMessage(const QSIP::ELogLevel &level,const QString &written_by,const QString &message);
     void stackInitializationStatus(const bool &status,const QString &remark, const unsigned int &code);
     void stackShutdownStatus(const bool &status,const QString &remark, const unsigned int &code);
     void accountRegistrationStateChanged(const QUuid &accId,const bool &status,const QString &statusString);
     void incomingCall(const QUuid &accId,const QUuid &callId,const QString &callerNumber,const QString &callerName,const QSIP::SIP_Header &headerMap);
     void callStateChanged(const QUuid &accId,const QUuid &callId,const QSIP::ECallState &callState,const QString &remarks,const unsigned int &sipCode);
     void mediaStateChanged(const QUuid &accId,const QUuid &callId,const QSIP::ECallMediaState &mediaState,const QString &remarks);
     void incomingPager(const QUuid &accId, const QString  &callerNumber,const QString  &callerName,const QString  &msgText);
     void messageWaitingIndication(const QUuid  &accId,const unsigned int  &msgCount);
     void callTransferRequest(const QString  &number,const QString  &proxy,const QUuid  &callId);
     void callTransferStatus(const QUuid  &callId,const bool  &status,const QString  &remark);
     void presenceChanged(const QString &statusText,const QString &contact,const int &state);
};

#endif // QSIP_H
