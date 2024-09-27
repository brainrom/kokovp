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
        QString mediaUrl;
        bool isExternal;
    };

    explicit PlayerController(PlayerWidget *parent = nullptr);

    QString currentMediaUrl() { return getProp("path").toString(); }
    QString lastOpenMediaUrl() { return lastMediaUrl; }

    PropertyObserver *prop(QString name);
    void setProp(const QString& name, const QVariant& value);
    QVariant getProp(const QString& name) const;

    void setOption(const QString& name, const QVariant& value);

    void setExtSubOptions(QString mode, int depth) { p_extSubMode = mode; p_extSubMaxDepth = depth; }
    void setExtAudioOptions(QString mode, int depth) { p_extAudioMode = mode; p_extAudioMaxDepth = depth; }

    void open(const QUrl &url);
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
    void mediaMetaUpdated(QString label, double duration);
    void endMediaRessource(bool wasStopped);
private:
    void handleMediaEnd();
    void handleMediaLoad();

    PlayerWidget *p;

    int p_extSubMaxDepth=-1;
    QString p_extSubMode = "no";

    int p_extAudioMaxDepth=-1;
    QString p_extAudioMode = "no";

    QList<Track> p_tracks;

    QUrl queuedMediaUrl;
    QString lastMediaUrl;
    bool haveMediaUrl = false;
};

#endif // PLAYERCONTROLLER_H
