#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMultimedia>
#include <QtMultimediaWidgets>
#include <QAudioOutput>
#include <QtCore>
#include <QtWidgets>
#include <QtGui>
#include <QMediaPlayer>
#include <QVideoSink>
#include <QVideoFrame>
#include <QLabel>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 duration);
    void updateDuration(qint64 duration);

    void seekVideo(int position);

    void StoreFrames(QString file);

    void takeFrame(const QVideoFrame &frame);

    void on_pbPause_clicked();

    void on_pbMute_clicked();

    void on_pbFullScreen_clicked();

    void on_slider_sound_valueChanged(int value);

    void on_actionOpen_triggered();

    void movePreviewWidget(int x, int seconds);

    void setPlaybackRate(float speed);

    void on_pbSpeedDown_clicked();

    void on_pbSpeedUp_clicked();

private:
    Ui::MainWindow *ui;
    QWidget* previewContainer = nullptr;
    QLabel* frameLabel;
    QLabel* timeLabel;

    float playbackRate = 1.0;

    QMediaPlayer* ghostPlayer = nullptr;
    QVideoSink* ghostSink = nullptr;
    QMap<int, QImage> frames;
    int counter = 0;

    QMediaPlayer* player;
    QAudioOutput* audioController;
    QVideoWidget* video;
    qint64 mDuration;
    bool isPaused = false;
    bool isMuted = false;
    bool isVideoSwitching = false; //flag for opening new video
};
#endif // MAINWINDOW_H
