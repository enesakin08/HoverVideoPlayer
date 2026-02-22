#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QUrl>
#include <QTimer>
#include <QShortcut>
#include <QKeySequence>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this); // it opens the window

    previewContainer = new QWidget(nullptr);
    previewContainer->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    previewContainer->setAttribute(Qt::WA_ShowWithoutActivating);
    previewContainer->setAttribute(Qt::WA_TranslucentBackground);
    previewContainer->setStyleSheet("background-color: rgba(0, 0, 0, 200); border-radius: 5px;");
    previewContainer->setFixedSize(170, 130);

    frameLabel = new QLabel(previewContainer);
    frameLabel->setFixedSize(160,90);
    frameLabel->setScaledContents(true);
    frameLabel->setStyleSheet("border: 2px solid #666; border-radius: 4px;");
    frameLabel->move(5, 5);

    timeLabel = new QLabel(previewContainer);
    timeLabel->setFixedSize(40,20);
    timeLabel->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    timeLabel->setStyleSheet("QLabel { background-color: #EDEDED; color: black; border: 1px solid black; padding: 4px; border-radius: 4px; font-weight: bold; }");
    timeLabel->setAttribute(Qt::WA_ShowWithoutActivating);
    timeLabel->setAlignment(Qt::AlignCenter);
    timeLabel->move(170/2 - timeLabel->width()/2, 105);

    previewContainer->hide();

    player = new QMediaPlayer(this);
    audioController = new QAudioOutput(this);
    player->setAudioOutput(audioController);
    audioController->setVolume(0.3);
    player->setVideoOutput(ui->VideoPanel);

    ui->pbPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui->pbMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    ui->pbSpeedDown->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    ui->pbSpeedUp->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui->pbFullScreen->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
    ui->slider_sound->setMaximum(100);
    ui->slider_sound->setValue(30);

    QShortcut *shortcutMinForward = new QShortcut(QKeySequence("Ctrl+Right"), this);
    QShortcut *shortcutMinBackward = new QShortcut(QKeySequence("Ctrl+Left"), this);

    connect(shortcutMinForward, &QShortcut::activated, this, [this](){player->setPosition(player->position() + 60000);});
    connect(shortcutMinBackward, &QShortcut::activated, this, [this](){player->setPosition(player->position() - 60000);});
    connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);
    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onMediaStatusChanged);
    connect(ui->slider_videoBar, &QSlider::sliderMoved, this, &MainWindow::seekVideo);
    connect(ui->slider_videoBar, &QSlider::sliderReleased, this, [this](){player->setPosition(ui->slider_videoBar->value());});
    connect(ui->slider_videoBar, &HoverSlider::onHover, this, &MainWindow::movePreviewWidget);
    connect(ui->slider_videoBar, &HoverSlider::onLeave, this, [this](){
        previewContainer->hide();
        currentHoverTarget = -1;
    });

    this->setFocusPolicy(Qt::StrongFocus);

    ui->slider_videoBar->setRange(0, player->duration()/1000);
}

MainWindow::~MainWindow()
{
    delete timeLabel;
    delete ui;
}

void MainWindow::durationChanged(qint64 duration) // Slider boyunu değiştirmek
{
    mDuration = duration;
    ui->slider_videoBar->setMaximum(mDuration);
    updateDuration(player->position() / 1000);
}

void MainWindow::positionChanged(qint64 duration)
{
    if(!ui->slider_videoBar->isSliderDown())
        ui->slider_videoBar->setValue(duration);
    updateDuration(duration / 1000);
}

void MainWindow::updateDuration(qint64 duration)
{
    if(duration || mDuration){
        QTime currentTime(duration / 3600, (duration / 60) % 60, duration % 60);
        QTime totalTime(mDuration/1000 / 3600, (mDuration/1000 / 60) % 60, (mDuration/1000) % 60);
        QString format = "mm:ss";
        if(mDuration >= 3600000) format = "hh:mm:ss";

        ui->LabelCurrentTime->setText(currentTime.toString(format));
        ui->LabelTotalTime->setText(totalTime.toString(format));
    }
}

void MainWindow::seekVideo(int position)
{
    // Slider değerini Player'a gönderir
    player->setPosition(position);

    // Sürüklerken süreyi de güncelle ki kullanıcı nerede olduğunu görsün
    updateDuration(position / 1000);
}

void MainWindow::StoreFrames(QString file){
    if(ghostSink){
        disconnect(ghostSink, &QVideoSink::videoFrameChanged, this, &MainWindow::takeFrame);
        ghostSink->deleteLater();
    }
    if(ghostPlayer) {
        ghostPlayer->stop();
        ghostPlayer->deleteLater();
    }

    ghostSink = nullptr;
    ghostPlayer = nullptr;
    isPriorityFetching = false;
    counter = 0;
    frames.clear();

    ghostPlayer = new QMediaPlayer(this);
    ghostSink = new QVideoSink(this);

    ghostPlayer->setVideoOutput(ghostSink);
    ghostPlayer->setAudioOutput(nullptr);
    ghostPlayer->setSource(QUrl::fromLocalFile(file));

    connect(ghostSink, &QVideoSink::videoFrameChanged, this, &MainWindow::takeFrame);

    ghostPlayer->play();
    ghostPlayer->pause();
    ghostPlayer->setPosition(0);
}

void MainWindow::takeFrame(const QVideoFrame &frame){
    if (sender() != ghostSink) return;
    if(!frame.isValid() || isVideoSwitching) return;

    QImage fr = frame.toImage().scaled(160, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    if(isPriorityFetching){
        qint64 currentPos = ghostPlayer->position();
        qint64 key = (currentPos / 5000) * 5000;

        frames.insert(key, fr);

        if(currentHoverTarget == key || abs(currentHoverTarget - key) < 2000){
            frameLabel->setPixmap(QPixmap::fromImage(fr));
        }
        isPriorityFetching = false;

        counter = key + 5000;
        QTimer::singleShot(0, this, [this](){ if(ghostPlayer) ghostPlayer->setPosition(counter); });
        return;
    }

    frames.insert(counter, fr);

    if(currentHoverTarget == counter ) frameLabel->setPixmap(QPixmap::fromImage(fr));

    counter += 5000;

    if (counter >= ghostPlayer->duration() - 4998){
        counter = 0;
        qDebug() << "Başa dönüldü.";
    }

    qint64 startSearch = counter;
    bool isVideoFull = false;

    while (frames.contains(counter) /*&& counter < ghostPlayer->duration()*/){
        counter += 5000;
        if (counter >= ghostPlayer->duration()) { //sondaysan yine başa geliyon
            counter = 0;
        }
        if (counter == startSearch) { // taramaya başladığı yere geldi mi kontrol
            isVideoFull = true;
            break;
        }
    }
    if (!isVideoFull) {
        QTimer::singleShot(0, this, [this](){
            if(ghostPlayer && !isPriorityFetching)
                ghostPlayer->setPosition(counter);
        });
    }
    else{
        qDebug() << "Hepsi tamam.";

        disconnect(ghostSink, &QVideoSink::videoFrameChanged, this, &MainWindow::takeFrame);
        ghostPlayer->stop();
        ghostPlayer->deleteLater();
        ghostSink->deleteLater();

        ghostPlayer = nullptr;
        ghostSink = nullptr;
    }
}

void MainWindow::on_actionOpen_triggered(){
    QString FileName = QFileDialog::getOpenFileName(this, "Select video");
    if (FileName.isEmpty()) {
        return;
    }
    isVideoSwitching = true;
    previewContainer->hide();
    player->stop();
    frames.clear();
    counter = 0;
    StoreFrames(FileName);

    player->setSource(QUrl::fromLocalFile(FileName));
    player->play();
    ui->pbPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
}

void MainWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status){
    if(status == QMediaPlayer::LoadedMedia || status == QMediaPlayer::BufferedMedia){
        isVideoSwitching = false;
        // int hour = player->duration() / 3600;
        // int minutes = (player->duration() / 60) % 60;
        // int seconds = player->duration() % 60;
        // ui->LabelTotalTime->setText(QString::number(hour) + ":" + QString::number(minutes) + ":" + QString::number(seconds));
    }
}

void MainWindow::on_pbPause_clicked()
{
    isPaused = !isPaused;
    if(isPaused){
        ui->pbPause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        player->pause();
    }
    else{
        ui->pbPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        player->play();
    }
}

void MainWindow::on_pbMute_clicked()
{
    bool currentlyMuted = audioController->isMuted();
    audioController->setMuted(!currentlyMuted);
    if(!currentlyMuted){
        ui->pbMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    }
    else{
        ui->pbMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    }
}

void MainWindow::on_pbFullScreen_clicked()
{
    isFullScreen() ? showNormal() : showFullScreen();
}

void MainWindow::on_slider_sound_valueChanged(int value)
{
    audioController->setVolume((float)value / 100);
    ui->LabelSound->setText("%" + QString::number(value));
}

void MainWindow::movePreviewWidget(int x, int seconds){
    if(isVideoSwitching) {
        previewContainer->hide();
        return;
    }
    QString time;
    if(seconds < 3600)
        time = QTime(0, 0).addSecs(seconds).toString("mm:ss");
    else
        time = QTime(0, 0).addSecs(seconds).toString("hh:mm:ss");
    timeLabel->setText(time);
    timeLabel->show();
    timeLabel->raise();

    int targetMs = (seconds / 5) * 5000;
    currentHoverTarget = targetMs;

    if (frames.contains(targetMs)) {
        frameLabel->setPixmap(QPixmap::fromImage(frames[targetMs]));
    }
    else{
        QPixmap black(160, 90);
        black.fill(Qt::black);
        frameLabel->setPixmap(black);
        if (!isPriorityFetching && ghostPlayer) {
            isPriorityFetching = true;
            ghostPlayer->setPosition(targetMs);
        }
    }
    frameLabel->show();

    QPoint sliderPos = ui->slider_videoBar->mapToGlobal(QPoint(0,0));
    int newX = sliderPos.x() + x - (previewContainer->width() / 2);
    int newY = sliderPos.y() - previewContainer->height() - 10;

    int screenWidth = QApplication::primaryScreen()->geometry().width();

    if(newX < 5) newX = 5;
    if (newX + previewContainer->width() > screenWidth)
        newX = screenWidth - previewContainer->width() - 5;

    previewContainer->move(newX, newY);
    previewContainer->show();
    previewContainer->raise();
    //qDebug() << "Slider size: " << ui->slider_videoBar->width(); //TEST YAZISI
    //qDebug() << "movePreviewWidget Çalıştı! mouse X:" << x << " Saniye:" << seconds;
    //qDebug() << "pozisyon: " << previewContainer->x() << " " << previewContainer->y();
}

void MainWindow::setPlaybackRate(float speed){
    player->setPlaybackRate(speed);
    QString txt = "";
    ui->LabelRate->setText("x" + QString::number(speed));
}

void MainWindow::keyPressEvent(QKeyEvent* event){
    //qDebug() << "Basılan key: " << event->key();
    if(event->key() == Qt::Key_Space || event->key() == Qt::Key_K){
        on_pbPause_clicked();
    }
    else if(event->key() == Qt::Key_J){
        player->setPosition(player->position() - 10000);
    }
    else if(event->key() == Qt::Key_L){
        player->setPosition(player->position() + 10000);
    }
    else if(event->key() == Qt::Key_Right){
        player->setPosition(player->position() + 5000);
    }
    else if(event->key() == Qt::Key_Left){
        player->setPosition(player->position() - 5000);
    }
    else if(event->key() == Qt::Key_Up){
        ui->slider_sound->setValue(ui->slider_sound->value() + 5);
    }
    else if(event->key() == Qt::Key_Down){
        ui->slider_sound->setValue(ui->slider_sound->value() - 5);
    }
    else if(event->key() == Qt::Key_F){
        on_pbFullScreen_clicked();
    }
    else if(event->key() == Qt::Key_Escape){
        showNormal();
        qDebug() << "hız: " << player->playbackRate();
    }
    else if(event->key() == Qt::Key_Plus){
        if(player->playbackRate() < 8){
            playbackRate += 0.25;
            setPlaybackRate(playbackRate);
        }
        qDebug() << "hız arttı: " << player->playbackRate();
    }
    else if(event->key() == Qt::Key_Minus){
        if(player->playbackRate() > 0.30){
            playbackRate -= 0.25;
            setPlaybackRate(playbackRate);
        }
        qDebug() << "hız azaldı: " << player->playbackRate();
    }
    else if(event->key() == Qt::Key_R){
        playbackRate = 1.0;
        setPlaybackRate(playbackRate);
    }
    else if(event->key() == Qt::Key_M){
        on_pbMute_clicked();
    }
    else if(event->key() >= 48 && event->key() <= 57){
        int num = event->key() - 48;
        player->setPosition(player->duration() * num/10);
    }
}

void MainWindow::on_pbSpeedDown_clicked()
{
    if(player->playbackRate() > 0.30){
        playbackRate -= 0.25;
        setPlaybackRate(playbackRate);
    }
}

void MainWindow::on_pbSpeedUp_clicked()
{
    if(player->playbackRate() < 8){
        playbackRate += 0.25;
        setPlaybackRate(playbackRate);
    }
}

