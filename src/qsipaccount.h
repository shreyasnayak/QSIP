#ifndef QSIPACCOUNT_H
#define QSIPACCOUNT_H
#include <QString>
#include <QUuid>

class QSipAccount : public Account
{
public:
    QSipAccount();
private:
    QString _proxy;
    QString _server;
    QString _userId;
    QString _domain;
    QString _displayName;
    QUuid _accId;
    bool _accStatus;
    float _txVol=1.0f;
    float _rxVol=1.0f;
    bool _tranSipHeader;
    bool _sendSessionId;

    QMap<QUuid,CInCall*> _CallMap;
    CInBuddy *_pPresence;

    /* Call Back functions pointers */
    std::function<void (QUuid accId,QUuid callId,QString destinationUri,QHash<QString,QString> headerMap)> _callBack_InCall;
    std::function<void (QUuid accId, bool status, QString statusString)> _callBack_AccState;
    std::function<void (QUuid callId,pjsip_inv_state state,QString remark,unsigned int sipCode)> _callStateChange_callBack;
    std::function<void (QUuid callId,int state)> _mediaStateChange_callBack;
    std::function<void (int level,QString writtenBy,QString message)> _log_callBack;
    std::function<void (QString destinationUri, QString msgText)> _im_callBack;
    std::function<void (QString number,QString proxy,QUuid txCallId)> _callTransferRequest_callBack;
    std::function<void (QUuid accId, int msgCount)> _mwi_callBack;
    std::function<void (QString statusText,QString contact,int state)> _presenceChange_callBack;
    std::function<void (QUuid callId,QString audioFile,QString message)> _audioFilePlayingStoped_callBack;
    QUuid createCall(QString number,CInCall::ECallType callType,QHash<QString,QString> extraHeaders,CInCall *transferThis=nullptr);
    void logMessage(int level,QString writtenBy,QString message);



    static QString minifyUuid(QUuid id);
public:
    explicit QSipAccount(QString domain,QString accPassword,QString proxy,QString agentId,QUuid AccountId,pjsip_transport_type_e transport,bool useSRTP,QString displayName=QString(),bool useProxy=false,unsigned int timeout=120);
    ~QSipAccount();

public:
    struct AudioState
    {
        bool isCallExist;
        bool state;
        bool isPlaying;
        QString errorMessage;
        quint32 pos;
        quint32 endpos;
        QString filePath;
    };

    CInCall::CallStats callStats(QUuid callId);
    QUuid accountId() const;
    bool isDefaultAccount() const;
    bool accountStatus() const;
    void setDefaultAccount();
    QString server() const;
    QString proxy() const;
    QString domain() const;

    bool callExist(const QUuid &callId);
    CInCall *callObject(const QUuid &callId);

    void setCaptureDeviceVol(float volume);
    void setPlaybackDeviceVol(float volume);
    void setTranSipHeader(bool tranSipHeader);

    void registerForMyPresence();
    void registerForBuddysPresence(const QString &buddyNumber);
    void sendSessionId(const bool &sendSessionId);


    /*Call API*/
    QUuid makeCall(QString number,QHash<QString,QString> extraHeaders); /*Call type canot be Incoming */
    void acceptCall(const QUuid& callId);
    void rejectCall(const QUuid& callId);
    void hangupCall(const QUuid& callId);
    void holdCall(const QUuid& callId);
    void unholdCall(const QUuid& callId);
    void mute(const QUuid& callId);
    void unmute(const QUuid& callId);
    void readCallHeaders(QUuid callId,QHash<QString, QString> &headers);
    void sendDTMF(const QUuid& callId,const QString& number);
    void transferCall(const QUuid& callId,QString destinationNumber, QHash<QString, QString> extraHeaders);
    void transferCall(const QUuid &sourceCallId, const QUuid &destCallId);

    QSipAccount::AudioState playAudioFile(const QUuid &callId, const QString &audioFile, const quint32 &pos);
    QSipAccount::AudioState pauseAudioFile(const QUuid &callId);


    /*Call Back fucntion*/
    void setIncomingCall_CallBack(std::function<void (QUuid accId, QUuid callId, QString destinationUri,QHash<QString,QString> headerMap)> fptr){ _callBack_InCall=std::move(fptr); }
    void setAccountStateChange_CallBack(std::function<void (QUuid accId, bool status, QString statusString)> fptr){ _callBack_AccState=std::move(fptr); }
    void setCallStateChange_CallBack(std::function<void (QUuid callId,pjsip_inv_state state,QString remark,unsigned int sipCode)> fptr){_callStateChange_callBack=std::move(fptr);}
    void setMediaStateChange_CallBack(std::function<void (QUuid callId,int state)> fptr){_mediaStateChange_callBack=fptr;}
    void setCallTransferRequest_CallBack(std::function<void (QString number,QString proxy,QUuid txCallId)> fptr){_callTransferRequest_callBack=std::move(fptr);}
    void setLog_CallBack(std::function<void (int level,QString writtenBy,QString message)> fptr){_log_callBack=fptr;}
    void setIM_CallBack(std::function<void (QString destinationUri, QString msgText)> fptr){_im_callBack=fptr;}
    void setMessageWaitingIndication_CallBack(std::function<void (QUuid accId, int msgCount)> fptr){_mwi_callBack=fptr;}
    void setPresenceChange_CallBack(std::function<void (QString statusText,QString contact,int state)> fptr){ _presenceChange_callBack=std::move(fptr); }
    void setAudioFilePlayingStoped_CallBack(std::function<void (QUuid callId,QString audioFile,QString message)> fptr){_audioFilePlayingStoped_callBack=fptr;}

private:
    virtual void onRegState(OnRegStateParam &info);
    virtual void onIncomingCall(OnIncomingCallParam &callInfo);
    virtual void onInstantMessage(OnInstantMessageParam &prm);
    virtual void onMwiInfo(OnMwiInfoParam &prm);
    //virtual void OnCallTransferRequestParam();


};

#endif // QSIPACCOUNT_H
