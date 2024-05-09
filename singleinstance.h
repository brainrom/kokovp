#ifndef SINGLEINSTANCE_H
#define SINGLEINSTANCE_H

#include <QObject>

class QLocalSocket;
class QLocalServer;

class SingleInstance : public QObject
{
    Q_OBJECT
public:
    explicit SingleInstance(QString appName, QObject *parent = nullptr);
    bool connectServer();
    void closeSocket();
    void sendMessage(QString msg);

    bool hostServer();
signals:
    void newMessage(QString msg);
private:
    void handleNewConnection();
    void readData();
    QString p_appName;
    QLocalSocket *p_socket;
    QLocalServer *p_server;
};

#endif // SINGLEINSTANCE_H
