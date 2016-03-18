#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->ButtonConnectCam, SIGNAL(clicked()), this, SLOT(connectToCam()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

 void MainWindow::updateCamImage()
 {
     cv::Mat frame;
     bool bNewFrame = cam->read(frame);

     if (bNewFrame)
     {
        QGraphicsScene* scene = new QGraphicsScene;

        scene->setSceneRect(0, 0, ui->LiveView->width(), ui->LiveView->height());

        cv::Mat temp;
        cv::cvtColor(frame, temp, CV_BGR2RGB);
        QImage qImage((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
        qImage.bits();

        scene->clear();

        scene->addPixmap(QPixmap::fromImage(qImage));
        scene->update();

        ui->LiveView->setScene(scene);
        //ui->LiveView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        ui->LiveView->update();
     }
 }

void MainWindow::connectToCam()
{
    cam = new cv::VideoCapture(-1);

    if (!cam->isOpened())
    {
        std::cout << "Error:  no webcam available!!!" << std::endl;
    }
    else
    {
        QTimer* timer = new QTimer(this);

        connect(timer, SIGNAL(timeout()), this, SLOT(updateCamImage()));
        timer->start(30);
    }
}
