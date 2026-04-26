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
#include "playercontroller.h"
#include "extensions.h"
#include "playerwidget.h"
#include "helper.h"
#include <QClipboard>
#include <QMessageBox>
#include <QApplication>

#if USE_EXIV2
#include "exiv2/exiv2.hpp"
#endif

PlayerController::PlayerController(PlayerWidget *parent)
    : QObject{parent}
{
    p = parent;
    prop("volume")->set(50);
    prop("pause")->set(true);
    p->setProp("audio-file-auto-exts", Extensions.audio());
    connect(p, &PlayerWidget::fileLoaded, this, &PlayerController::handleFileLoad);
    connect(p, &PlayerWidget::endFile, this, &PlayerController::endFile);
    connect(p, &PlayerWidget::endFile, this, &PlayerController::handleFileEnd);
}

void PlayerController::setSaveScreenshotsMetadata(bool on)
{
    saveMetadata = on;
}

PropertyObserver *PlayerController::prop(QString name)
{
    return p->propertyObserver(name);
}

void PlayerController::setProp(const QString &name, const QVariant &value)
{
    p->setProp(name, value);
}

QVariant PlayerController::getProp(const QString &name) const
{
    return p->getProp(name);
}

void PlayerController::setOption(const QString &name, const QVariant &value)
{
    p->setOption(name, value);
}

void PlayerController::handleFileEnd()
{
    haveFile = false;
    if (!queuedFile.isEmpty())
    {
        QUrl f = queuedFile;
        QMap<QString, QVariant> opts =  queuedFileOptions;
        queuedFile = QUrl();
        queuedFileOptions = QMap<QString, QVariant>();
        open(f, opts);
    }
}

void PlayerController::open(const QUrl &file, const QMap<QString, QVariant> &fileOptions)
{
    if (haveFile)
    {
        queuedFile = file;
        queuedFileOptions = fileOptions;
        return stop();
    }
    p_tracks.clear();

    // Here we need to scan siblings folder for possible external subtitles and audio
    //, then set it to sub-file-paths and audio-file-paths OPTIONs (not properties)
    if (file.isLocalFile())
    {
        QDir mediaDir = QFileInfo(file.toLocalFile()).absoluteDir();

        p->setOption("sub-auto", p_extSubMode);
        if (p_extSubMaxDepth>=0 && p_extSubMode!="no")
        {
            QStringList subsFolders;
            Helper::searchWithMaxDepth(subsFolders, Extensions.subtitles().forDirFilter(), mediaDir, p_extSubMaxDepth, false);
            p->setOption("sub-file-paths", subsFolders);
        }
        else
        {
            p->setOption("sub-file-paths", {});
        }

        p->setOption("audio-file-auto", p_extAudioMode);
        if (p_extAudioMaxDepth>=0 && p_extAudioMode!="no")
        {
            QStringList audioFolders;
            Helper::searchWithMaxDepth(audioFolders, Extensions.audio().forDirFilter(), mediaDir, p_extAudioMaxDepth, false);
            p->setOption("audio-file-paths", audioFolders);
        }
        else
        {
            p->setOption("audio-file-paths", {});
        }
        for (auto [key, value] : fileOptions.asKeyValueRange()) {
            p->setOption(key,value);
        }
    }
    p->command(QStringList{"loadfile", file.path()});
}

void PlayerController::stop()
{
    const static QVariantList cmd = QVariantList({"stop"});
    p->command(cmd);
    p_tracks.clear();
    emit tracksUpdated();
}

void PlayerController::togglePlayback()
{
    p->setProp("pause", isPlaying());
}

void PlayerController::seekAbsolute(double s)
{
    p->command(QVariantList({"seek", s, "absolute"}));
}

void PlayerController::seekRelative(double s)
{
    p->command(QVariantList({"seek", s, "relative"}));
}

void PlayerController::setScreenshotOpts(const QString &dir, const QString &scrTemplate, const QString &format)
{
    p->setProp("screenshot-dir", dir);
    p->setProp("screenshot-template", scrTemplate);
    p->setProp("screenshot-format", format);
}

void PlayerController::screenshot(const QString &outPath, bool includeSubs)
{
    const char *scrFlags = includeSubs ? "subtitles" : "video";
    // It's important to get it here, until video advance
    QString subsText = p->getProp("sub-text").toString();

    QVariant resultingPath;
    QString  resultingPathString;
    if (outPath.length()>0)
        resultingPath = p->command(QVariantList({"screenshot-to-file", outPath, scrFlags}));
    else
        resultingPath = p->command(QVariantList({"screenshot", scrFlags}));
    resultingPathString = resultingPath.toMap().value("filename").toString();

    if (resultingPathString.isEmpty() || !QFile::exists(resultingPathString))
    {
        QMessageBox::warning(p,
                             tr("Screenshot"),
                             tr("Screenshot wasn't saved. Something is wrong"));
        return;
    }

#if USE_EXIV2
    if (!saveMetadata)
        return;

    try {
        bool isOk = false;
        auto image = Exiv2::ImageFactory::open(resultingPathString.toUtf8().constData());
        if (!image) {
            qWarning() << QObject::tr("Failed to open image");
            return;
        }

        image->readMetadata();

        Exiv2::XmpData &xmp = image->xmpData();

        xmp["Xmp.dc.description"] = subsText.toStdString();
        xmp["Xmp.xmp.CreatorTool"] = QString("%1 %2").arg(QApplication::applicationName(), QApplication::applicationVersion()).toStdString();
        xmp["Xmp.dc.source"] = QFileInfo(currentFile()).fileName().toStdString();
        //xmp["Xmp.xmpMM.InstanceID"] = // TODO: Store file hash

        Exiv2::XmpProperties::registerNs(
            "https://kokokoshka.com/ns/kokovp/1.3/",
            "kokovp");

        double timestamp = getProp("time-pos").toDouble(&isOk);
        if (isOk)
            xmp["Xmp.kokovp.SourceTimestamp"] = QString::number(timestamp, 'f', 3).toStdString();

        image->setXmpData(xmp);
        image->writeMetadata();
    }
    catch (const Exiv2::Error &e) {
        qWarning() << QObject::tr("Exiv2 error:") << e.what();
    }
#endif
}

void PlayerController::subSeek(int skip, bool secondary)
{
    p->command(QVariantList({"sub-seek", skip, secondary ? "secondary" : "primary"}));
}

void PlayerController::subStep(int skip, bool secondary)
{
    p->command(QVariantList({"sub-step", skip, secondary ? "secondary" : "primary"}));
}

void PlayerController::frameStep(int step) {
    const static QVariantList cmd_forward = QVariantList({"frame-step"});
    const static QVariantList cmd_back = QVariantList({"frame-back-step"});

    switch (step)
    {
    case 1:
        p->command(cmd_forward);
        return;
    case -1:
        p->command(cmd_back);
        return;
    };
}

bool PlayerController::isPlaying()
{
    return !getProp("pause").toBool();
}

void PlayerController::handleFileLoad()
{
    lastFile = currentFile();
    haveFile = true;
    bool ok;
    int tracksCount = getProp("track-list/count").toInt(&ok);
    assert(ok);
    for (int i = 0; i<tracksCount; i++)
    {
        QString trackAddr = QString("track-list/%1/").arg(i);
        Track t;
        t.id = getProp(trackAddr + "id").toInt();
        t.title = getProp(trackAddr + "title").toString();
        t.lang = getProp(trackAddr + "lang").toString();
        t.isExternal = getProp(trackAddr + "external").toBool();
        QString type = getProp(trackAddr + "type").toString();
        if (type=="video")
            t.type = Track::TRACK_TYPE_VIDEO;
        else if (type=="audio")
            t.type = Track::TRACK_TYPE_AUDIO;
        else if (type=="sub")
            t.type = Track::TRACK_TYPE_SUB;

        if (t.isExternal)
        {
            t.filename = p->getProp(trackAddr + "external-filename").toString();
            QFileInfo fI(t.filename);
            // mpv after 57210df returns complete sufix as fallback title instead of basename.
            // Clean it early so rewrite rule can rely on empty title as a condition to rewrite
            if (fI.fileName()==t.title || fI.suffix()==t.title)
                t.title = QString();
        }

        p_tracks.append(t);
    }
    //p->command(QVariantList({"vf", "clr", ""})); //SVP

    emit tracksUpdated();
    emit fileMetaUpdated(p->getProp("media-title").toString(), prop("duration")->get().toDouble());
}
