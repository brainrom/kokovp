#include "kokovp.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>
#include "QDir"

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

    KokoVP w;
    w.showNormal();

    return a.exec();
}
