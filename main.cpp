#include "kokovp.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDir>
#include <QCommandLineParser>

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
            inst.sendMessage("open:" + parser.positionalArguments().join(','));
            inst.closeSocket();
            return 0;
        }

        if (!inst.hostServer())
            return -1;
    }

    KokoVP w;
    if (parser.positionalArguments().length()>0)
    {
        w.handleNewMessage("open:" + parser.positionalArguments().join(','));
        w.handleNewMessage("playlast");
    }
    QObject::connect(&inst, &SingleInstance::newMessage, &w, &KokoVP::handleNewMessage);
    w.showNormal();

    return a.exec();
}
