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
#include "mpvwidget.h"

#include <QOpenGLContext>
#include <QMetaObject>
#include <QPainter>
#include <QGuiApplication>

#include <mpv/client.h>
#include <mpv/render_gl.h>

#include "mpv_qthelper.hpp"
#include <stdexcept>
#include "qdebug.h"

MpvWidget::MpvWidget(QWidget *parent, Qt::WindowFlags f)
    : QOpenGLWidget(parent, f)
{
    //setTextureFormat(GL_RGB10_A2);

    setlocale(LC_NUMERIC, "C");
    mpv = mpv_create();
    if (!mpv)
        throw std::runtime_error("could not create mpv context");

    mpv_set_option_string(mpv, "terminal", "yes");
    mpv_set_option_string(mpv, "msg-level", "all=v");
    if (mpv_initialize(mpv) < 0)
        throw std::runtime_error("could not initialize mpv context");

    // Request hw decoding, just for testing.
    mpv::qt::set_option_variant(mpv, "hwdec", "auto");

    // Neccessary to force mpv to do OpenGL rendering
    mpv_set_option_string(mpv, "vo", "libmpv");

    mpv_set_wakeup_callback(mpv, &MpvWidget::catchMPVevents, this);
}

MpvWidget::~MpvWidget()
{
    makeCurrent();
    if (mpv_gl)
        mpv_render_context_free(mpv_gl);
    mpv_terminate_destroy(mpv);
}

void MpvWidget::command(const QVariant& params)
{
    mpv::qt::command(mpv, params);
}

void MpvWidget::setProp(const QString& name, const QVariant& value)
{
    mpv::qt::set_property(mpv, name, value);
}

void MpvWidget::setOption(const QString &name, const QVariant &value)
{
    mpv::qt::set_option_variant(mpv, name, value);
}

QVariant MpvWidget::getProp(const QString &name) const
{
    return mpv::qt::get_property(mpv, name);
}

PropertyObserver *MpvWidget::propertyObserver(QString name)
{
    if (observers.contains(name))
        return observers.value(name);

    PropertyObserver *prop = new PropertyObserver(this, name);
    observers.insert(name, prop);
    return prop;
}

void MpvWidget::initializeGL()
{
    void *wlDisplay = nullptr;
    void *x11Display = nullptr;

#ifdef Q_OS_LINUX
    if (QNativeInterface::QWaylandApplication *wlApp = qApp->nativeInterface<QNativeInterface::QWaylandApplication>())
        wlDisplay = wlApp->display();

    if (QNativeInterface::QX11Application *x11App = qApp->nativeInterface<QNativeInterface::QX11Application>())
        x11Display = x11App->display();
#endif

    mpv_opengl_init_params gl_init_params[1] = {get_proc_address, nullptr};
    mpv_render_param params[]{
        {MPV_RENDER_PARAM_API_TYPE, const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)},
        {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
        {MPV_RENDER_PARAM_WL_DISPLAY, wlDisplay},
        {MPV_RENDER_PARAM_X11_DISPLAY, x11Display},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };

    if (mpv_render_context_create(&mpv_gl, mpv, params) < 0)
        throw std::runtime_error("failed to initialize mpv GL context");
    mpv_render_context_set_update_callback(mpv_gl, MpvWidget::on_update, reinterpret_cast<void *>(this));
}

void MpvWidget::catchMPVevents(void *ctx)
{
    QMetaObject::invokeMethod(static_cast<MpvWidget*>(ctx), &MpvWidget::processMPVevents, Qt::QueuedConnection);
}

void MpvWidget::processMPVevents()
{
    // Process all events, until the event queue is empty.
    while (mpv) {
        mpv_event *event = mpv_wait_event(mpv, 0);
        if (event->event_id == MPV_EVENT_NONE) {
            break;
        }
        handleMPVevents(event);
    }
}

void MpvWidget::handleMPVevents(mpv_event *event)
{
    switch (event->event_id) {
    case MPV_EVENT_PROPERTY_CHANGE:
    case MPV_EVENT_GET_PROPERTY_REPLY:
    {
        mpv_event_property *prop = (mpv_event_property *)event->data;
        QString propName = QString::fromUtf8(prop->name);
        QVariant data = mpv::qt::data_to_variant(prop->format, prop->data);
        if (observers.contains(propName))
            observers.value(propName)->captureProperty(propName, data);
        break;
    }
    case MPV_EVENT_FILE_LOADED:
        emit fileLoaded();
        break;
    case MPV_EVENT_END_FILE:
    {
        mpv_event_end_file *end_file = (mpv_event_end_file*)event->data;
        emit endFile(end_file->reason==MPV_END_FILE_REASON_STOP);
        break;
    }
    default:
        break;
        // Ignore uninteresting or unknown events.
    }
}

// Make Qt invoke mpv_render_context_render() to draw a new/updated video frame.
void MpvWidget::maybeUpdate()
{
    // If the Qt window is not visible, Qt's update() will just skip rendering.
    // This confuses mpv's render API, and may lead to small occasional
    // freezes due to video rendering timing out.
    // Handle this by manually redrawing.
    // Note: Qt doesn't seem to provide a way to query whether update() will
    //       be skipped, and the following code still fails when e.g. switching
    //       to a different workspace with a reparenting window manager.

    //TODO: isMinimized isn't enough in Wayland
    if (window()->isMinimized()) {
        makeCurrent();
        paintGL();
        context()->swapBuffers(context()->surface());
        doneCurrent();
    } else {
        update();
    }
}

void MpvWidget::on_update(void *ctx)
{
    QMetaObject::invokeMethod(reinterpret_cast<MpvWidget*>(ctx), &MpvWidget::maybeUpdate);
}

void* MpvWidget::get_proc_address(void *ctx, const char *name) {
    Q_UNUSED(ctx);
    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (!glctx)
        return nullptr;
    return reinterpret_cast<void *>(glctx->getProcAddress(name));
}

void MpvWidget::paintEvent(QPaintEvent *event)
{
    mpv_opengl_fbo mpfbo{
        .fbo = static_cast<int>(defaultFramebufferObject()),
        .w   = static_cast<int>(width() * devicePixelRatio()),
        .h   = static_cast<int>(height() * devicePixelRatio()),
        .internal_format = static_cast<int>(textureFormat()),
    };
    int flip_y{1};

    mpv_render_param params[] = {
        {MPV_RENDER_PARAM_OPENGL_FBO, &mpfbo},
        {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.beginNativePainting();
    //glClear(GL_COLOR_BUFFER_BIT);
    mpv_render_context_render(mpv_gl, params);
    painter.endNativePainting();
    paintOverlay(painter);
}

PropertyObserver::PropertyObserver(MpvWidget *mpv, QString name) : p_name{std::move(name)}, p_mpv(mpv)
{
    static const QMap<QString, mpv_format> formats = {
        { "volume", MPV_FORMAT_DOUBLE },
        { "duration", MPV_FORMAT_DOUBLE },
        { "time-pos", MPV_FORMAT_DOUBLE },
        { "audio-delay", MPV_FORMAT_DOUBLE },
        { "sub-delay", MPV_FORMAT_DOUBLE },
        { "sub-scale", MPV_FORMAT_DOUBLE },
        { "sub-pos", MPV_FORMAT_DOUBLE },
        { "pause", MPV_FORMAT_FLAG },
        { "mute", MPV_FORMAT_FLAG },
        { "chapter", MPV_FORMAT_INT64 },
        { "aid", MPV_FORMAT_INT64 },
        { "vid", MPV_FORMAT_INT64 },
        { "sid", MPV_FORMAT_INT64 }
    };

    mpv_format f = MPV_FORMAT_NONE;
    if (formats.contains(p_name))
        f = formats.value(p_name);

    internalType = f;
    defVal = QVariant(0);

    mpv_observe_property(mpv->mpv, 0, p_name.toUtf8().constData(), f);
}

QVariant PropertyObserver::get()
{
    return p_mpv->getProp(p_name);
}

void PropertyObserver::set(QVariant value)
{
    static const QVariant no = QString("no");
    static const QStringList tracksProps = { "aid", "vid", "sid", "secondary-sid" };

    if (tracksProps.contains(p_name)) // Workaround to use -1 int track num as "no" string
        p_mpv->setProp(p_name, value.toInt()>=0 ? value : no);
    else
        p_mpv->setProp(p_name, value);
}

void PropertyObserver::setDefVal(QVariant value)
{
    defVal = value;
}

void PropertyObserver::setRelative(double relValue)
{
    bool isOk;
    QVariant data = get();
    switch (internalType)
    {
        case MPV_FORMAT_INT64:
        {
            int ret = data.toInt(&isOk);
            if (isOk)
                set(ret+(int)relValue);
            return;
        }
        case MPV_FORMAT_DOUBLE:
        {
            double ret = data.toDouble(&isOk);
            if (isOk)
                set(ret+relValue);
            return;
        }
    }
}

void PropertyObserver::captureProperty(QString propName, QVariant data)
{
    assert(propName==p_name);

    bool isOk;
    switch (internalType)
    {
    case MPV_FORMAT_STRING:
        emit changed(data);
        emit changedString(data.toString());
        return;
    case MPV_FORMAT_FLAG:
        emit changed(data);
        emit changedBool(data.toBool());
        return;
    case MPV_FORMAT_INT64:
    {
        int ret = data.toInt(&isOk);
        ret = isOk ? ret : -1; // Same for returning tracks ids
        emit changed(ret);
        emit changedInt(ret);
        return;
    }
    case MPV_FORMAT_DOUBLE:
        emit changed(data);
        emit changedDouble(data.toDouble());
        return;
    default:
        emit changed(data);
    }
}
