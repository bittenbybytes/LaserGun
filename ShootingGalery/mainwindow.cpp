#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->ButtonConnectCam, SIGNAL(clicked()), this, SLOT(connectToCam()));
	connect(ui->ButtonConnectTarget, SIGNAL(clicked()), this, SLOT(connectToTarget()));

	QDir dir("/dev","tty*", QDir::Name, QDir::System);
	dir.setNameFilters( QStringList() << "ttyACM*" << "ttyUSB*");

	if (dir.exists())
	{
		QFileInfoList files = dir.entryInfoList();
		foreach (QFileInfo file, files)
		{
				ui->TargetsComboBox->addItem(file.filePath());
		}

		targetDevPath = files.at(0).filePath();
		std::cout <<"default dev file: " << targetDevPath.toStdString() << std::endl;
	}

	connect(ui->TargetsComboBox, SIGNAL(activated(QString)), this, SLOT(updateTargetDevPath(QString)));
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

void MainWindow::updateTargetDevPath(QString path)
{
	targetDevPath = path;

	std::cout <<"updated dev file: " << targetDevPath.toStdString() << std::endl;
}

void MainWindow::connectToTarget()
{
	targetController.init(targetDevPath.toStdString());

	connect(ui->buttonDown, SIGNAL(clicked()), this, SLOT(targetButtonClicked()));
	connect(ui->buttonUp, SIGNAL(clicked()), this, SLOT(targetButtonClicked()));

	ui->buttonDown->setEnabled(true);
	ui->buttonUp->setEnabled(true);

}

void MainWindow::targetButtonClicked()
{
	if (sender() == ui->buttonDown)
		targetController.alldown();
	else if (sender() == ui->buttonUp)
		targetController.allup();
}
