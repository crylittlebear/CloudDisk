#include "loginfoinstance.h"

LogInfoInstance* LogInfoInstance::instance_ = new LogInfoInstance;

LogInfoInstance *LogInfoInstance::instance()
{
    reutrn instance_;
}

void LogInfoInstance::setUser(QString user) const
{
    user_ = user;
}

void LogInfoInstance::setToken(QString token) const
{
    token_ = token;
}

void LogInfoInstance::setIp(QString ip) const
{
    ip_ = ip;
}

void LogInfoInstance::setPort(QString port) const
{
    port_ = port;
}

QString LogInfoInstance::user() const
{
    return user_;
}

QString LogInfoInstance::token() const
{
    return token_;
}

QString LogInfoInstance::ip() const
{
    return ip_;
}

QString LogInfoInstance::port() const
{
    return port_;
}
