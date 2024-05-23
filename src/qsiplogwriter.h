#ifndef QSIPLOGWRITER_H
#define QSIPLOGWRITER_H

#include <QString>
#include <pjsua2.hpp>
#include <iostream>
#include <functional>

using namespace pj;

class QSipLogWriter : public LogWriter
{
private:
    std::function<void (int level,QString writtenBy,QString message)> m_fnLogMessage;
    void write(const LogEntry &entry);
public:
    QSipLogWriter();
    ~QSipLogWriter();
    void onPjSipLog(std::function<void (int level,QString writtenBy,QString message)> functor);
};

#endif // QSIPLOGWRITER_H
