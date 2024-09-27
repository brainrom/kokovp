/*  This is part of KokoVP

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef SINGLEINSTANCE_H
#define SINGLEINSTANCE_H

#include <QObject>

struct ProgramArgument;
class QLocalSocket;
class QLocalServer;

class SingleInstance : public QObject
{
    Q_OBJECT
public:
    explicit SingleInstance(QString appName, QObject *parent = nullptr);
    bool connectServer();
    void closeSocket();
    void sendMessage(const ProgramArgument &msg);

    bool hostServer();
signals:
    void newMessage(const ProgramArgument &msg);
    void handleNewMessage(const ProgramArgument &msg);
private:
    void handleNewConnection();
    void readData();
    QString p_appName;
    QLocalSocket *p_socket;
    QLocalServer *p_server;
};

#endif // SINGLEINSTANCE_H
