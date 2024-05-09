#include "kokovp.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>
#include "QDir"
#include "singleinstance.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QLocale sysLocale = QLocale::system();

    QTranslator qtTranslator;
    if(qtTranslator.load(sysLocale, "qtbase_", QString(), QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
        a.installTranslator(&qtTranslator);

    QTranslator translator;
    if (translator.load(sysLocale, "KokoVP_", QString(), ":/i18n"))
        a.installTranslator(&translator);

    SingleInstance inst("KokoVP", &a);
    if (inst.connectServer())
    {
        inst.sendMessage("testmsg");
        inst.closeSocket();
        return 0;
    }

    if (!inst.hostServer())
        return -1;

    KokoVP w;
    QObject::connect(&inst, &SingleInstance::newMessage, &w, &KokoVP::handleNewMessage);
    w.showNormal();

    return a.exec();
}
