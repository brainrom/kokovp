#ifndef PROGRAM_ARG_H
#define PROGRAM_ARG_H

#include <QString>
#include <QStringList>

class QDataStream;

struct ProgramArgument {
    ProgramArgument(const QString &cmd = QString(), const QStringList &args = QStringList());

    QString cmd; // TODO: Make cmd enum
    QStringList args;

    friend QDataStream &operator>>(QDataStream &istream, ProgramArgument &pa);
	friend QDataStream &operator<<(QDataStream &ostream, const ProgramArgument &pa);
};

#endif
