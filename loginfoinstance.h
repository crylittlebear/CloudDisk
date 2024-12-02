#ifndef LOGINFOINSTANCE_H
#define LOGINFOINSTANCE_H

#include <QObject>

class LogInfoInstance : public QObject
{
    Q_OBJECT
public:
    static LogInfoInstance* instance();

    void setUser(QString user) const;
    void setToken(QString token) const;
    void setIp(QString ip) const;
    void setPort(QString port) const;

    QString user() const;
    QString token() const;
    QString ip() const;
    QString port() const;

private:
    LogInfoInstance() = default;

private:
    QString user_;
    QString token_;
    QString ip_;
    QString port_;

    static LogInfoInstance* instance_;
};

#endif // LOGINFOINSTANCE_H
