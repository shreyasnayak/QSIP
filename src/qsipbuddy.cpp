#include "qsipbuddy.h"

QSipBuddy::QSipBuddy() {}

void QSipBuddy::setBuddyContact(const QString &contact)
{
    m_buddyContact = contact;
}

QString QSipBuddy::getContact() const
{
    return m_buddyContact;
}

void QSipBuddy::onBuddyPresenceChanged(std::function<void (QString, QString, int)> functor)
{
    m_fnPresenceChanged = std::move(functor);
}

void QSipBuddy::onBuddyState()
{
    BuddyInfo bi = getInfo();
    QString dispState = QString(QByteArray(bi.presStatus.statusText.c_str(),int(bi.presStatus.statusText.length())));
    m_fnPresenceChanged(dispState,getContact(),state);
}
