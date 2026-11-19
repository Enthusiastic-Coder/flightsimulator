#pragma once

#include "Interfaces.h"

#include <QElapsedTimer>
#include <QOpenGLWindow>
#include <QSet>
#include <QTimer>

class QtGameLoop : public QOpenGLWindow, public IScreenMouseInfo
{
public:
    QtGameLoop();

    bool run(const QString& title);
    void resetTimer();
    void setFramePerSecond(float fps);
    void setRunning(bool running);
    bool isRunning() const;
    float frameTime() const;
    void postQuit();
    bool isKeyDown(Qt::Key key) const;

    void GetMousePos(int& x, int& y) const override;
    void GetScreenDims(int& w, int& h) const override;
    void GetScreenPos(int& x, int& y) const override;

protected:
    virtual bool onInitialise() = 0;
    virtual void onUnInitialise() = 0;
    virtual void onUpdate() = 0;
    virtual void onRender() = 0;
    virtual void onSize(int width, int height);
    virtual void onKeyDown(QKeyEvent* event) = 0;

    QPoint framebufferPoint(const QPointF& point) const;
    qint64 elapsedMilliseconds() const;

private:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    bool event(QEvent* event) override;
    void shutDown();

    QTimer _redrawTimer;
    QElapsedTimer _frameClock;
    QElapsedTimer _applicationClock;
    QSet<int> _pressedKeys;
    float _dt = 0.0f;
    bool _running = true;
    bool _initialised = false;
};
