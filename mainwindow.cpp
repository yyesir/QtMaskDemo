#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myvideosurface.h"
#include "FaceDetectionAlgo.h"
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label->installEventFilter(this); //这行不能省

    Pix = QPixmap(ui->label->width(), ui->label->height());

    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)   //用过滤器eventFilter（）拦截QLabel中的QEvent::Paint事件
{
    if(watched ==ui->label && event->type() == QEvent::Paint)
        labelPaint();

    return QWidget::eventFilter(watched,event);
}

/**
 * @brief MainWindow::createMask
 * @param painter
 * @param width     客户区宽度
 * @param height    客户区高度
 * @param maskRate  透明度系数
 * @param angleRate 填充系数
 * @param faceFound 是否检测到人脸
 */
void MainWindow::createMask(QPainter* painter, int width, int height, float maskRate, float angleRate, bool faceFound)
{
    /*=================== 填充四周 =======================*/
    int rows = width;
    int cols = height;
    int paddingReference = (rows > cols) ? cols : rows;
    int paddingWidth = paddingReference * maskRate;   //四周半透明遮罩宽
    int paddingHeight = paddingWidth;  //四周半透明遮罩高
    int blankRow = (rows - paddingWidth) * 0.5; //中心空白区域的起始行
    int blankCol = (cols - paddingHeight) * 0.5;//中心空白区域的起始列

    const int PADDING_R = 0;
    const int PADDING_G = 0;
    const int PADDING_B = 0;
    const int PADDING_A = 120;

    //半透明填充左、右、上、下四个区域
    painter->fillRect(0, 0, rows, blankCol
        , QColor(PADDING_B, PADDING_G, PADDING_R, PADDING_A));

    painter->fillRect(0, blankCol + paddingWidth, rows, cols
        , QColor(PADDING_B, PADDING_G, PADDING_R, PADDING_A));

    painter->fillRect(0, blankCol, blankRow, paddingWidth
        , QColor(PADDING_B, PADDING_G, PADDING_R, PADDING_A));

    painter->fillRect(blankRow + paddingHeight, blankCol, rows, paddingWidth
        , QColor(PADDING_B, PADDING_G, PADDING_R, PADDING_A));

    /*=================== 填充边角 =======================*/
    int angleWidth = paddingReference * angleRate;   //直角宽 11
    int angleHeight = paddingReference * angleRate * 5;//直角高 11 * 5
    const int ANGLE_R = faceFound ? 0 : 255;
    const int ANGLE_G = faceFound ? 255 : 0;
    const int ANGLE_B = 0;
    const int ANGLE_A = 255;

    //左上角
    painter->fillRect(blankRow, blankCol, angleWidth, angleHeight
        , QColor(ANGLE_R, ANGLE_G, ANGLE_B, ANGLE_A));
    painter->fillRect(blankRow, blankCol, angleHeight, angleWidth
        , QColor(ANGLE_R, ANGLE_G, ANGLE_B, ANGLE_A));

    //右上角
    painter->fillRect(blankRow + paddingWidth - angleWidth, blankCol, angleWidth, angleHeight
        , QColor(ANGLE_R, ANGLE_G, ANGLE_B, ANGLE_A));
    painter->fillRect(blankRow + paddingWidth - angleHeight, blankCol, angleHeight, angleWidth
        , QColor(ANGLE_R, ANGLE_G, ANGLE_B, ANGLE_A));

    //左下角
    painter->fillRect(blankRow, blankCol + paddingHeight - angleHeight, angleWidth, angleHeight
        , QColor(ANGLE_R, ANGLE_G, ANGLE_B, ANGLE_A));
    painter->fillRect(blankRow, blankCol + paddingHeight - angleWidth, angleHeight, angleWidth
        , QColor(ANGLE_R, ANGLE_G, ANGLE_B, ANGLE_A));

    //右下角
    painter->fillRect(blankRow + paddingWidth - angleWidth, blankCol + paddingHeight - angleHeight, angleWidth, angleHeight
        , QColor(ANGLE_R, ANGLE_G, ANGLE_B, ANGLE_A));
    painter->fillRect(blankRow + paddingWidth - angleHeight, blankCol + paddingHeight - angleWidth, angleHeight, angleWidth
        , QColor(ANGLE_R, ANGLE_G, ANGLE_B, ANGLE_A));
}

QRect MainWindow::getFaceBoxRect(cv::Rect faceRect, int drawingWidth, int imageWidth)
{
    float ratio = (float)drawingWidth / imageWidth;

    QRect rect;
    rect.setLeft((int)(faceRect.x * ratio));
    rect.setTop((int)(faceRect.y * ratio));
    rect.setRight((int)(faceRect.x + faceRect.width * ratio));
    rect.setBottom((int)(faceRect.y + faceRect.height * ratio));

    return rect;
}

void MainWindow::labelPaint()     //绘图
{
    QPainter painter(ui->label);
    QBrush brush(QColor(0,0,0,0));
    painter.setBrush(brush);
    if(!videoImg.isNull())
    {
        Pix = QPixmap::fromImage(videoImg);

        painter.drawPixmap(0, 0, ui->label->width(), ui->label->height(),Pix);
        bool noFaceFound = false;
        cv::Rect faceRect;
        if (!detectFace(videoImg, faceRect))
            noFaceFound = false;
        else
            noFaceFound = true;

        createMask(&painter, ui->label->width(), ui->label->height(), 0.67, 0.023, noFaceFound);
        //画人脸框
        if(noFaceFound)
        {
            QPen pen;
            pen.setColor(Qt::green);
            pen.setWidth(3);
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
            qDebug() << "face x: " << faceRect.x << ", y: " << faceRect.y << ", width: " << faceRect.width << ", height: " << faceRect.height;
            painter.drawRect(getFaceBoxRect(faceRect, ui->label->width(), videoImg.width()));
        }
    }
}

bool MainWindow::detectFace(QImage &image, cv::Rect& faceRect)
{
    cv::Mat srcImage = QImage2cvMat(image, true);
    vector<cv::Rect> rects;
    FaceDetectionAlgo::detect(srcImage, rects);
    if (rects.empty()) {
        return false;
    } else {
        faceRect = rects[0];
        return true;
    }
}

void MainWindow::paintEvent(QPaintEvent* e)
{
    QPainter _painter(this);
    QPen pen;
    pen.setColor(Qt::green);
    pen.setWidth(2);
    pen.setStyle(Qt::SolidLine);
    _painter.setPen(pen);

    _painter.drawLine(0,0,width(),height());
}

void MainWindow::init()
{
    cameras = QCameraInfo::availableCameras();
    viscamera = new QCamera(cameras[1]);                     //此处测试使用的双目摄像头 index 1为可见光
    m_imageCapture = new QCameraImageCapture(viscamera);     //截图部件

    m_imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToFile);          //设置捕获的图片的存储的位置
    viscamera->setCaptureMode(QCamera::CaptureStillImage);                              //设置摄像头出于何种工作模式--抓取静态图片模式

    //自己用QPainter将每一帧视频画出来
    myvideosurface *surface_ = new myvideosurface(this);
    viscamera->setViewfinder(surface_);                                                 //预览窗口重定向在surface_中
    //处理myvideosurface中每一帧视频
    connect(surface_, SIGNAL(frameAvailable(QVideoFrame)), this, SLOT(rcvFrame(QVideoFrame)), Qt::DirectConnection);

    viscamera->start();
}

void MainWindow::rcvFrame(QVideoFrame m_currentFrame)
{
    m_currentFrame.map(QAbstractVideoBuffer::ReadOnly);
    //将视频帧转化成QImage,devicePixelRatio设备像素比，bytesPerLine一行的像素字节（1280*4=5120）
    videoImg =  QImage(m_currentFrame.bits(),
                   m_currentFrame.width(),
                   m_currentFrame.height(),
                   QVideoFrame::imageFormatFromPixelFormat(m_currentFrame.pixelFormat())).copy();       //这里要做一个copy,因为char* pdata在emit后释放了
    videoImg =  videoImg.scaled(640, 480, Qt::KeepAspectRatio, Qt::SmoothTransformation);   //图像缩放
    videoImg = videoImg.mirrored(true, true);                       //水平翻转，原始图片是反的
    m_currentFrame.unmap();                                         //释放map拷贝的内存
    ui->label->update();                                            //更新了，就会触发paintEvent画图
}

