#include "singleinstance.h"

#include <QLocalSocket>
#include <QLocalServer>

SingleInstance::SingleInstance(QString appName, QObject *parent)
    : p_appName(appName), QObject{parent}
{}

bool SingleInstance::connectServer()
{
    p_socket = new QLocalSocket(this);
    p_socket->connectToServer(p_appName);
    bool ret = p_socket->waitForConnected(100); //TODO: subject to change
    if (!ret)
        qDebug() << "Unable to connect:" << p_socket->errorString();
    return ret;
}

void SingleInstance::closeSocket()
{
    p_socket->close();
}

bool SingleInstance::hostServer()
{
    p_server = new QLocalServer(this);
    p_server->removeServer(p_appName); // Asserting, that if no one is listening, then we can remove current server
    connect(p_server, &QLocalServer::newConnection, this, &SingleInstance::handleNewConnection);
    bool ret = p_server->listen(p_appName);
    if(!ret)
        qDebug() << "Unable to start server:" << p_server->errorString();
    return ret;
}

void SingleInstance::sendMessage(QString msg)
{
    p_socket->write(msg.toUtf8());
    p_socket->putChar('\n');
    p_socket->waitForBytesWritten(100);
}

void SingleInstance::handleNewConnection()
{
    while (p_server->hasPendingConnections())
    {
        QLocalSocket *sock = p_server->nextPendingConnection();
        connect(sock, &QLocalSocket::readyRead, this, &SingleInstance::readData);
        connect(sock, &QLocalSocket::disconnected, sock, &QLocalSocket::deleteLater);
    }
}

void SingleInstance::readData()
{
    QLocalSocket *sock = static_cast<QLocalSocket*>(sender());
    while (sock->canReadLine())
        emit newMessage(sock->readLine());
}
