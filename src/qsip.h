#ifndef QSIP_H
#define QSIP_H

#include <QObject>
#include <QFile>
#include <pjsua2.hpp>

using namespace pj;

class QSIP : public QObject
{
    Q_OBJECT
public:
    enum E_LOG_LEVEL
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };

    enum E_TRANSPORT
    {
        UDP,
        TCP,
        TLS
    };

    enum E_PAYLOAD
    {
        RTP,
        SRTP
    };

    enum E_NAT_TRAVERSAL
    {
        NONE,
        STUN,
        TURN,
        ICE
    };

    enum E_CALL_STATE
    {
        EARLY,
        CONNECTING,
        CONNECTED,
        DISCONNECTED
    };


    enum E_CALL_MEDIA_STATE
    {
        NO_MEDIA,
        ACTIVE,
        ON_HOLD,
        MEDIA_ERROR
    };

    Q_ENUM(E_LOG_LEVEL)
    Q_ENUM(E_TRANSPORT)
    Q_ENUM(E_PAYLOAD)
    Q_ENUM(E_CALL_MEDIA_STATE)

private:
    explicit QSIP(QObject *parent = nullptr);
    ~QSIP();
    static QString minifyUuid(const QUuid &uuid);
    static QString callStateInString(const E_CALL_STATE &value);
    bool _isStackInitialized, _logSipMessage, _verifyTlsCertificate;
    QFile _caListFile,_certificateFile,_privateKeyFile;
    E_LOG_LEVEL _logLevel;
    unsigned int _endpointPort;
    E_TRANSPORT _transport;

public:
    typedef QHash<QString,QString> SIP_Header;
    typedef QList<QUuid> SIP_Account_List;

    /* Log settings */
    void setLogLevel(const QSIP::E_LOG_LEVEL &logLevel);
    void logSipMessage(const bool &status);

    /* Endpoint transport setting */
    inline bool sipStackInitCheck(const QString &logRemark);
    void setPort(const unsigned int &port = 5060);
    void setTransport(const QSIP::E_TRANSPORT &transport = QSIP::E_TRANSPORT::UDP);
    void setVerifyTlsCertificate(const bool &state);
    void setCaListFile(const QFile &caListFile);
    void setCertificateFile(const QFile &certificateFile);
    void setPrivateKeyFile(const QFile &privateKeyFile);

    /* Audio Device */
    QStringList getCaptureDeviceList();
    QStringList getPlaybackDeviceList();
    // bool setCaptureDevice(const unsigned int &index);
    // bool setPlaybackDevice(const unsigned int &index);
    // void setCaptureDeviceVol(const unsigned int &volume);
    // void setPlaybackDeviceVol(const unsigned int &volume);

    // /* Codec */
    // QStringList getCodecsList();
    // void setCodecList(const QStringList &codecs);

    // /* Default NAT & Turn server setup */
    // void setNATMethod(const QSIP::E_NAT_TRAVERSAL &method);
    // void setTurnServer(const QString &server);
    // void setTurnUsername(const QString &uname);
    // void setTurnPassword(const QString &password);

    // /* Account settings */
    // bool isAccountExist(const QUuid &accId);
    // void setPayLoad(const QSIP::E_PAYLOAD &payload);
    // void setStunServer(const QString &stunServer);


    // void registerForMyPresence();
    // void registerForBuddyPresence(const QString &buddyContact);
    // void registerForBuddyPresence(const QStringList &buddysList);
    // void enablePresencePublish();
    // void disablePresencePublish();


    // bool sendInstantMessage(const QString &destinationUri,const QString &msg,const QUuid &accId);


    // /* SIP stack status */
    // bool isStackInitialized();
    // void initializeSIPStack();
    // void terminateSIPStack();

    // QSIP::SIP_Account_List sipAccounts();

    // /* startRegister takes the account parameters and returns a UUID which is the account ID. This account ID will be used to identify the account going forward.
    //  * Note that this function just sends the SIP REGISTER message. The state of the registration will be emitted through the signal `registrationStateChanged` */
    // QUuid initAccountRegister(const QString &sipUser, const QString &sipPass, const QString &sipDomain, const QString &displayName, const bool &useProxy,const QString &proxyAddr, const QSIP::E_NAT_TRAVERSAL &natMethod = QSIP::E_NAT_TRAVERSAL::NONE, const unsigned int &timeout = 120);

    // /* startUnregister takes the account UUID as the argument and unregisters that account.
    //  * Note that this function only starts the unregistration process. The state of unregistration will be emitted through the signal `registrationStateChanged` */
    // bool initAccountUnregister(const QUuid& accId);

    // /* startCall takes ID of the account to use to make the call, the SIP URI and the SIP headers to set if any. The function returns a UUID which is the call ID.
    //  * This UUID will be used to identify this call going forward.
    //  * Note that this function just sends the SIP INVITE. The state of the call will be emitted through the signal `callStateChanged` */
    // QUuid initCall(const QUuid& accId, const QString &destinationUri, const QSIP::SIP_Header &sipHeaders);

    // /* When a incoming call arrives as is notified through the signal `incomingCall`, the call can be accepted by calling this function. */
    // bool accept(const QUuid& callId);

    // /* For any incoming or outgoing call, the call can be put on hold by calling this function. */
    // bool hold(const QUuid& callId);

    // /* For any incoming or outgoing call in hold, the call can be resumed by calling this function. */
    // bool unhold(const QUuid& callId);

    // /* Call this function to stop sending audio to a call. */
    // bool mute(const QUuid& callId);

    // /* Call this function to resume sending audio to a call. */
    // bool unmute(const QUuid& callId);

    // /* Reject incoming call or drop out going call */
    // bool hangup(const QUuid& callId);

    // bool sendDTMF(const QUuid& callId,const QString &number);

    // /*This function will help you to do blind transfer*/
    // bool transferCall(const QUuid& callId, const QString &destinationUri, const QSIP::SIP_Header &sipHeaders);

    // /*This function will help you to do attended transfer*/
    // bool transferCall(const QUuid& sourceCallId, const QUuid& destCallId);

    // void hangupAllCalls();

    // unsigned int nuberOfActiveCall();

signals:
     void logMessage(const QSIP::E_LOG_LEVEL &level,const QString &written_by,const QString &message);
     void stackInitializationStatus(const bool &status,const QString &remark, const unsigned int &code);
     void stackShutdownStatus(const bool &status,const QString &remark, const unsigned int &code);
     void accountRegistrationStateChanged(const QUuid &accId,const bool &status,const QString &statusString);
     void incomingCall(const QUuid &accId,const QUuid &callId,const QString &callerNumber,const QString &callerName,const QSIP::SIP_Header &headerMap);
     void callStateChanged(const QUuid &accId,const QUuid &callId,const QSIP::E_CALL_STATE &callState,const QString &remarks,const unsigned int &sipCode);
     void mediaStateChanged(const QUuid &accId,const QUuid &callId,const QSIP::E_CALL_MEDIA_STATE &mediaState,const QString &remarks);
     void incomingPager(const QUuid &accId, const QString  &callerNumber,const QString  &callerName,const QString  &msgText);
     void messageWaitingIndication(const QUuid  &accId,const unsigned int  &msgCount);
     void callTransferRequest(const QString  &number,const QString  &proxy,const QUuid  &callId);
     void callTransferStatus(const QUuid  &callId,const bool  &status,const QString  &remark);
     void presenceChanged(const QString &statusText,const QString &contact,const int &state);

};

#endif // QSIP_H


// #ifndef TXSIP_H
// #define TXSIP_H
// #include <QObject>
// #include <QStringList>
// #include <QHash>
// #include <QUuid>
// #include <iostream>
// #include <functional>
// #include <pjlib.h>
// #include <pjlib-util.h>
// #include <pjmedia.h>
// #include <pjmedia-codec.h>
// #include <pjsip.h>
// #include <pjsip_simple.h>
// #include <pjsip_ua.h>
// #include <pjsua-lib/pjsua.h>
// #include <cinaccount.h>
// #include <cinlogwriter.h>


// using namespace pj;

// class ConferenceRoom;

// class QSIP : public QObject
// {
//     Q_OBJECT
// public:
//     typedef QHash<QString, QString> SipHeader;



// private:
//     static TxSIP* _PTxSIP;
//     pj::Endpoint *_pEndPoint;
//     QHash<QUuid,CInAccount*> _allAccount;
//     bool _isInitialized=false;
//     unsigned int _logSipMsg=0;
//     unsigned int _logLevel=3;
//     float _cptVol=0.8f;
//     float _plyVol=0.8f;
//     bool _trnSipHeader,_sendSessionId;

//     /*Endpoint Transport setting*/
//     unsigned int _port = 5060;
//     pjsip_transport_type_e _transport = pjsip_transport_type_e::PJSIP_TRANSPORT_UDP;
//     bool _verifyTlsCertificate=false;
//     bool _useSRTP=false;
//     QString _caListFile,_certificateFile,_privateKeyFile;

//     /*Endpoint Payload setting*/
//     EPayLoad _payload = EPayLoad::RTP;

//     /*Endpoint Nat Traversal setting*/
//     QString _stunServer, _turnServer, _turnUsername, _turnPassword;
//     QStringList _custHeader,_myBuddys;

//     /* Account setting */
//     bool _registerPresence;



// public:
//     static QSIP* GetInstance(QObject* parent = nullptr);
//     friend class ConferenceRoom;








//     bool isTransferSipHeaderSet() const;
//     void transferSipHeader(bool trnSipHeader);

//     /* This will generate one unique id in sip header ( X-TX-SESSION-ID ) for outgoing call and incoming call (if not exist) */
//     void sendSessionId(const bool &sendSessionId);




// signals:
//     void logMessage(const TxSIP::ELogLevel &level,const QString &writtenBy,const QString &message);
//     void stackInitializationStatus(const bool &status,const QString &remark);
//     void stackShutdownStatus(bool status,QString remark);
//     void registrationStateChanged(const QUuid &accId,const bool &status,const QString &statusString);
//     void callStateChanged(const QUuid &callId,const TxSIP::ECallState &callState,const QString &remarks,const unsigned int &sipCode);
//     void mediaStateChanged(const QUuid &callId,const TxSIP::EMediaState &mediaState,const QString &remarks);
//     void incomingCall(const QUuid &accId,const QUuid &callId,const QString &callerNumber,const QString &callerName,const QHash<QString,QString> &headerMap);
//     void incomingPager(const QString  &callerNumber,const QString  &callerName,const QString  &msgText);
//     void messageWaitingIndication(const QUuid  &accId,const int  &msgCount);
//     void callTransferRequest(const QString  &number,const QString  &proxy,const QUuid  &callId);
//     void callTransferStatus(const QUuid  &callId,const bool  &status,const QString  &remark);
//     void audioFilePlayingStoped(const QUuid  &callId,const QString &audioFile,const QString &message);
//     void presenceChanged(const QString &statusText,const QString &contact,const int &state);

// private:
//     inline bool preCheckFailed(const QString &logRemark);
// };


// class ConferenceRoom : public QObject
// {
//     Q_OBJECT

//     QUuid _confID;
//     QList<QUuid> _allCalls;
//     QList<QUuid> _mergedCalls;
//     TxSIP *_pTxSip;

//     CInCall *getCall(const QUuid &callId);
//     bool mergeTheCallAudio(const QUuid& callId);
//     bool separateTheCallAudio(const QUuid& callId);

// signals:
//     void logMessage(const TxSIP::ELogLevel &level,const QString &writtenBy,const QString &message);

// public:
//     explicit ConferenceRoom(QList<QUuid> callList,TxSIP *sip,QObject *parent = nullptr,QUuid ConfID=QUuid::createUuid());
//     ~ConferenceRoom();
//     QUuid conferenceId() const;
//     bool removeCall(const QUuid& callId);
//     bool addCall(const QUuid& callId);
//     void muteMySelf(const bool& mute,TxSIP* txSIP);
//     void startCallOnHold();
//     void startCallOnUnhold();
//     QList<QUuid> conferenceCalls();
// public slots:
//     void callStateChanged(const QUuid &callId,const TxSIP::ECallState &callState,const QString &remarks,const unsigned int &sipcode);
//     void mediaStateChanged(const QUuid &callId,const TxSIP::EMediaState &mediaState,const QString &remarks);
// };

// Q_DECLARE_METATYPE(TxSIP::ELogLevel)
// Q_DECLARE_METATYPE(TxSIP::ECallState)
// Q_DECLARE_METATYPE(TxSIP::EMediaState)
// Q_DECLARE_INTERFACE(TxSIP, "com.ilinknet.TxSIP/0.1")
// #endif // TXSIP_H

