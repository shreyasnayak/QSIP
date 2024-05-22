/** @file qsipbuddy.h
 *
 * @copyright (C) 2024
 * @date 2024.05.21
 * @version 1.0.0
 * @author Shreyas Nayak <shreyasnayak21@gmail.com>
 *
 */

#ifndef QSIPBUDDY_H
#define QSIPBUDDY_H
#include <QString>
#include <functional>
#include <pjsua2.hpp>

using namespace pj;

class QSipBuddy : public Buddy
{
private:
    QString m_buddyContact;
    std::function<void (QString statusText,QString contact,int state)> m_fnPresenceChanged;

public:
    QSipBuddy();
    ~QSipBuddy();

    void setBuddyContact(const QString &contact);
    QString getContact() const;
    void onBuddyPresenceChanged(std::function<void (QString statusText,QString contact,int state)> functor);

private:
    virtual void onBuddyState();
};

#endif // QSIPBUDDY_H
