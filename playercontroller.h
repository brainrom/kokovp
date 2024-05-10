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
#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <QObject>
#include <QDir>
#include <QUrl>

class PlayerWidget;
class PropertyObserver;

class PlayerController : public QObject
{
    Q_OBJECT
public:
    struct Track
    {
        enum TrackType
        {
            TRACK_TYPE_VIDEO,
            TRACK_TYPE_AUDIO,
            TRACK_TYPE_SUB,
        };
        int id;
        TrackType type;
        QString title;
        QString lang;
        QString filename;
        bool isExternal;
    };

    explicit PlayerController(PlayerWidget *parent = nullptr);

    QString currentFile() { return getProp("path").toString(); }
    QString lastOpenFile() { return lastFile; }

    PropertyObserver *prop(QString name);
    void setProp(const QString& name, const QVariant& value);
    QVariant getProp(const QString& name) const;

    void setOption(const QString& name, const QVariant& value);

    void setExtSubOptions(QString mode, int depth) { p_extSubMode = mode; p_extSubMaxDepth = depth; }
    void setExtAudioOptions(QString mode, int depth) { p_extAudioMode = mode; p_extAudioMaxDepth = depth; }

    void open(const QUrl &file);
    void stop();

    void togglePlayback();

    void seekAbsolute(double s);
    void seekRelative(double s);

    void setScreenshotOpts(const QString &dir, const QString &scrTemplate, const QString &format = "jpg");
    void screenshot(const QString &outPath, bool includeSubs = false);

    bool isPlaying();

    void frameStep(int step);

    inline void subSeekPrimary(int step) { subSeek(step); }
    inline void subStepPrimary(int skip) { subStep(skip); }
    void subSeek(int step, bool secondary=false);
    void subStep(int skip, bool secondary=false);

    const QList<Track> &tracks() const { return p_tracks; }

signals:
    void tracksUpdated();
    void fileMetaUpdated(QString label, double duration);
    void endFile(bool wasStopped);
private:
    void handleFileEnd();
    void handleFileLoad();
    void searchWithMaxDepth(QStringList &outList, const QStringList &filter, QDir path, int maxDepth=2, int depth=0);

    PlayerWidget *p;

    int p_extSubMaxDepth=-1;
    QString p_extSubMode = "no";

    int p_extAudioMaxDepth=-1;
    QString p_extAudioMode = "no";

    QList<Track> p_tracks;

    QUrl queuedFile;
    QString lastFile;
    bool haveFile = false;
};

#endif // PLAYERCONTROLLER_H
