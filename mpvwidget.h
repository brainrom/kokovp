#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QOpenGLWidget>

struct mpv_handle;
struct mpv_render_context;
struct mpv_event;

class MpvWidget;

class PropertyObserver : public QObject
{
    Q_OBJECT
    friend class MpvWidget;
public:
    PropertyObserver(MpvWidget *mpv, QString name);
    QString name() { return p_name; }
    QVariant get();
    void set(QVariant value);
    void setDefVal(QVariant value);
    void setRelative(double relValue);
    void reset() { set(defVal); }
signals:
    void changed(QVariant value);
    void changedString(QString value);
    void changedBool(bool value);
    void changedInt(int value);
    void changedDouble(double value);
private:
    void captureProperty(QString propName, QVariant data);
    QString p_name;
    QVariant defVal;
    MpvWidget *p_mpv;
    int internalType;
};

class MpvWidget: public QOpenGLWidget
{
    Q_OBJECT
    friend class PropertyObserver;
public:
    MpvWidget(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
    ~MpvWidget();
    void command(const QVariant& params);
    void setProp(const QString& name, const QVariant& value);
    QVariant getProp(const QString& name) const;
    void setOption(const QString& name, const QVariant& value);
    PropertyObserver *propertyObserver(QString name);
signals:
    void endFile(bool wasStopped);
    void fileLoaded();
protected:
    virtual void paintOverlay(QPainter &painter) {};
protected slots:
    void processMPVevents();
    void maybeUpdate();
private:
    void initializeGL() override;

    static void catchMPVevents(void *ctx);
    static void on_update(void *ctx);
    static void *get_proc_address(void *ctx, const char *name);

    void handleMPVevents(mpv_event *event);

    mpv_handle *mpv;
    mpv_render_context *mpv_gl;
    QMap<QString, PropertyObserver*> observers;
    void paintEvent(QPaintEvent *event) override;
};



#endif // PLAYERWINDOW_H
