#ifndef PROGRAM_ARG_H
#define PROGRAM_ARG_H

#include <QString>
#include <QStringList>

class QDataStream;

enum class ProgramCmd
{
    NONE,
    OPEN,
    PLAYLAST,
};

struct ProgramArgument
{
    explicit ProgramArgument(const ProgramCmd &cmd = ProgramCmd::NONE, const QStringList &args = QStringList());

    ProgramCmd cmd;
    QStringList args;

    friend QDataStream &operator>>(QDataStream &istream, ProgramArgument &pa);
    friend QDataStream &operator<<(QDataStream &ostream, const ProgramArgument &pa);
};

#endif
