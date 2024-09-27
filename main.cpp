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
#include "kokovp.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDir>
#include <QCommandLineParser>

#include "program_arg.h"
#include "singleinstance.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("KokoVP");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::translate("KokoVP", "Video player"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("media", QCoreApplication::translate("KokoVP", "Media files to play"));
    parser.addPositionalArgument("urls", QCoreApplication::translate("KokoVP", "URLs to play"));

    // A boolean option with a single name (-p)
    QCommandLineOption newInstanceOption(QStringList() << "n" << "new-instance", QCoreApplication::translate("KokoVP", "Force open in new instance"));
    parser.addOption(newInstanceOption);
    parser.process(a);


    QLocale sysLocale = QLocale::system();
    QTranslator qtTranslator;
    if(qtTranslator.load(sysLocale, "qtbase_", QString(), QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
        a.installTranslator(&qtTranslator);

    QTranslator translator;
    if (translator.load(sysLocale, "KokoVP_", QString(), ":/i18n"))
        a.installTranslator(&translator);

    SingleInstance inst("KokoVP", &a);

    if (!parser.isSet(newInstanceOption))
    {
        if (inst.connectServer())
        {
            inst.sendMessage(ProgramArgument(ProgramCmd::OPEN, parser.positionalArguments()));
            inst.closeSocket();
            return 0;
        }

        if (!inst.hostServer())
            return -1;
    }

    KokoVP w;
    if (parser.positionalArguments().length()>0)
    {
        w.handleNewMessage(ProgramArgument(ProgramCmd::OPEN, parser.positionalArguments()));
        w.handleNewMessage(ProgramArgument(ProgramCmd::PLAYLAST));
    }
    QObject::connect(&inst, &SingleInstance::newMessage, &w, &KokoVP::handleNewMessage);
    w.showNormal();

    return a.exec();
}
