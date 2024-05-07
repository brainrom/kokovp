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
