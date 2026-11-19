#include "stdafx.h"
#include "QtGameLoop.h"
#include "QtKeyState.h"

#include <QCoreApplication>
#include <QCursor>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QScreen>
#include <QSurfaceFormat>

bool simulatorKeyDown(Qt::Key key)
{
    auto* window = dynamic_cast<QtGameLoop*>(QGuiApplication::focusWindow());
    return window && window->isKeyDown(key);
}

QtGameLoop::QtGameLoop()
{
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
    surfaceFormat.setDepthBufferSize(24);
    surfaceFormat.setStencilBufferSize(8);
    surfaceFormat.setSwapInterval(1);
    setFormat(surfaceFormat);

    _redrawTimer.setTimerType(Qt::PreciseTimer);
    connect(&_redrawTimer, &QTimer::timeout, this, [this] { update(); });
    setFramePerSecond(30.0f);
}

bool QtGameLoop::run(const QString& title)
{
    setTitle(title);
    _applicationClock.start();
#ifdef _DEBUG
    const QSize screenSize = screen()->availableGeometry().size();
    resize(screenSize / 2);
    show();
#else
    showFullScreen();
#endif
    const int result = QCoreApplication::exec();
    shutDown();
    return result == 0;
}

void QtGameLoop::initializeGL()
{
    _initialised = onInitialise();
    if (!_initialised)
    {
        postQuit();
        return;
    }
    resetTimer();
    if (_running)
        _redrawTimer.start();
}

void QtGameLoop::resizeGL(int, int)
{
    int w, h;
    GetScreenDims(w, h);
    if (_initialised && w > 0 && h > 0)
        onSize(w, h);
}

void QtGameLoop::paintGL()
{
    if (!_initialised || width() <= 0 || height() <= 0)
        return;

    const qint64 elapsed = _frameClock.nsecsElapsed();
    _frameClock.restart();
    _dt = _running ? static_cast<float>(elapsed / 1.0e9) : 0.0f;
    onUpdate();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    onRender();
}

void QtGameLoop::onSize(int width, int height)
{
    glViewport(0, 0, width, height);
}

void QtGameLoop::resetTimer()
{
    _frameClock.start();
}

void QtGameLoop::setFramePerSecond(float fps)
{
    _redrawTimer.setInterval(qRound(1000.0f / qMax(1.0f, fps)));
}

void QtGameLoop::setRunning(bool running)
{
    _running = running;
    resetTimer();
    if (_initialised && _running)
        _redrawTimer.start();
    else
        _redrawTimer.stop();
    update();
}

bool QtGameLoop::isRunning() const
{
    return _running;
}

float QtGameLoop::frameTime() const
{
    return _dt;
}

void QtGameLoop::postQuit()
{
    _redrawTimer.stop();
    QMetaObject::invokeMethod(this, [] { QCoreApplication::exit(1); }, Qt::QueuedConnection);
}

void QtGameLoop::keyPressEvent(QKeyEvent* event)
{
    _pressedKeys.insert(event->key());
    if (event->key() == Qt::Key_Escape)
    {
        close();
        return;
    }
    if (_initialised)
    {
        onKeyDown(event);
        if (!_running)
            update();
    }
}

void QtGameLoop::keyReleaseEvent(QKeyEvent* event)
{
    if (!event->isAutoRepeat())
        _pressedKeys.remove(event->key());
}

void QtGameLoop::focusOutEvent(QFocusEvent* event)
{
    _pressedKeys.clear();
    QOpenGLWindow::focusOutEvent(event);
}

bool QtGameLoop::event(QEvent* event)
{
    if (event->type() == QEvent::Close)
        shutDown();
    return QOpenGLWindow::event(event);
}

void QtGameLoop::shutDown()
{
    _redrawTimer.stop();
    if (_initialised)
    {
        makeCurrent();
        onUnInitialise();
        doneCurrent();
        _initialised = false;
    }
}

bool QtGameLoop::isKeyDown(Qt::Key key) const
{
    return _pressedKeys.contains(key);
}

QPoint QtGameLoop::framebufferPoint(const QPointF& point) const
{
    return (point * devicePixelRatio()).toPoint();
}

qint64 QtGameLoop::elapsedMilliseconds() const
{
    return _applicationClock.elapsed();
}

void QtGameLoop::GetMousePos(int& x, int& y) const
{
    const QPoint point = framebufferPoint(mapFromGlobal(QCursor::pos()));
    x = point.x();
    y = point.y();
}

void QtGameLoop::GetScreenDims(int& w, int& h) const
{
    w = qRound(width() * devicePixelRatio());
    h = qRound(height() * devicePixelRatio());
}

void QtGameLoop::GetScreenPos(int& x, int& y) const
{
    x = this->x();
    y = this->y();
}
