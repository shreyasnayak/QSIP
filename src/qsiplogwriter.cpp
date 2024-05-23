#include "qsiplogwriter.h"

QSipLogWriter::QSipLogWriter()
{

}

QSipLogWriter::~QSipLogWriter()
{

}

void QSipLogWriter::onPjSipLog(std::function<void (int, QString, QString)> functor)
{
    m_fnLogMessage = std::move(functor);
}

void QSipLogWriter::write(const LogEntry &entry)
{
    if(m_fnLogMessage)
    {
         m_fnLogMessage(0,entry.threadName.c_str(),entry.msg.c_str());
    }
}
