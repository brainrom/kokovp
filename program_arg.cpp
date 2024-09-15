#include <QDataStream>

#include "program_arg.h"

ProgramArgument::ProgramArgument(const QString &cmd, const QStringList &args)
    : cmd{cmd}, args{args}
{}

QDataStream &operator>>(QDataStream &istream, ProgramArgument &pa) {
    istream >> pa.cmd >> pa.args;
    return istream;
}

QDataStream &operator<<(QDataStream &ostream, const ProgramArgument &pa) {
    ostream << pa.cmd << pa.args;
    return ostream;
}
