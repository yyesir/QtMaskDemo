#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QComboBox>
#include <QFuture>
#include <QCameraInfo>

#include "convert.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void init();
    void createMask(QPainter* painter, int width, int height, float maskRate, float angleRate, bool faceFound);
    bool detectFace(QImage &image, cv::Rect& faceRect);
    QRect getFaceBoxRect(cv::Rect faceRect, int drawingWidth, int imageWidth);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void labelPaint();
    void paintEvent(QPaintEvent* e);

private slots:
    void rcvFrame(QVideoFrame);                             //接收图像帧数据

private:
    Ui::MainWindow *ui;
    QPixmap        Pix;
    QList<QCameraInfo> cameras;
    QCamera *viscamera;
    QCameraImageCapture *m_imageCapture;                    //截图部件
    QImage               videoImg;                          //从QAbstractVideoSurface传过来的图像帧
};

#endif // MAINWINDOW_H
