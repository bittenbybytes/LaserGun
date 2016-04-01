#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QDir>
#include <QThread>

const char noDevString[] = "no device found";

void sleep(int ms)
{
	QThread::msleep(ms);
}

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	connect(ui->ButtonConnectCam, SIGNAL(clicked()), this, SLOT(connectToCam()));
	connect(ui->ButtonConnectTarget, SIGNAL(clicked()), this, SLOT(connectToTarget()));

	connect(ui->ButtonDetect, SIGNAL(clicked()), this, SLOT(detectTargets()));

	connect(ui->pushButtonTune, SIGNAL(clicked()), this, SLOT(tune()));

	QDir dir("/dev","tty*", QDir::Name, QDir::System);
	dir.setNameFilters( QStringList() << "ttyACM*" << "ttyUSB*");

	if (dir.exists())
	{
		QFileInfoList files = dir.entryInfoList();
		foreach (QFileInfo file, files)
		{
			ui->TargetsComboBox->addItem(file.filePath());
		}

		if (files.size() > 0)
			targetDevPath = files.at(0).filePath();
		else
		{
			targetDevPath = "";
			ui->TargetsComboBox->addItem(noDevString);
		}

		std::cout <<"default dev file: " << targetDevPath.toStdString() << std::endl;
	}

	connect(ui->TargetsComboBox, SIGNAL(activated(QString)), this, SLOT(updateTargetDevPath(QString)));

	camIndex = 0;
	connect(ui->CamIndexComboBox, SIGNAL(activated(int)), this, SLOT(updateCamIndex(int)));

	cam = NULL;

	ui->spinBoxHueLower->setRange(80,128);
	ui->spinBoxHueLower->setValue(detector.hueMin);
	connect(ui->spinBoxHueLower, SIGNAL(valueChanged(int)), this, SLOT(updateDetectorSettings()));

	ui->spinBoxHueUpper->setRange(128,160);
	ui->spinBoxHueUpper->setValue(detector.hueMax);
	connect(ui->spinBoxHueUpper, SIGNAL(valueChanged(int)), this, SLOT(updateDetectorSettings()));

	detector.markShots = true;
}

MainWindow::~MainWindow()
{
	delete ui;
}

void displayImage(cv::Mat frame, QGraphicsView* view)
{
	QGraphicsScene* scene = new QGraphicsScene;

	scene->setSceneRect(0, 0, view->width(), view->height());

	cv::Mat temp;
	cv::cvtColor(frame, temp, CV_BGR2RGB);
	QImage qImage((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
	qImage.bits();

	scene->clear();

	scene->addPixmap(QPixmap::fromImage(qImage));
	scene->update();

	view->setScene(scene);

	view->update();
}

void MainWindow::updateCamImage()
{
	cv::Mat frame;
	bool bNewFrame = cam->read(frame);

	if (bNewFrame)
	{

		int targetHit = detector.detectHit(frame);

		displayImage(frame, ui->LiveView);

		if(targetHit >= 0)
		{
			targetController.hit(targetHit);
		}
	}
}

void MainWindow::connectToCam()
{
	if(cam != NULL)
	{
		delete cam;
		cam = NULL;
	}

	cam = new cv::VideoCapture(camIndex);

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


void MainWindow::updateCamIndex(int index)
{
	camIndex = index;
}

void MainWindow::detectTargets()
{
	targetController.alldown();

	cv::Mat frame;

	if( cam->read(frame))
		detector.resetTargetSegments(frame.size());

	sleep(2000);

	for (int i =0; i<2; i++)
	{
		targetController.up(i);
		sleep(2000);

		for (int var = 0; var < 10; var++)
		{
			cam->read(frame);
		}

		if( cam->read(frame))
			detector.detectTargetSegment(frame, i);
		else
			std::cout << "Error reading frame!!" << std::endl;

		targetController.down(i);
		sleep(2000);
	}
}

void MainWindow::updateDetectorSettings()
{
	if(sender() == ui->spinBoxHueLower)
		detector.hueMin = ui->spinBoxHueLower->value();
	else if(sender() == ui->spinBoxHueUpper)
		detector.hueMax = ui->spinBoxHueUpper->value();
}

void MainWindow::tune()
{
	cv::Mat frame;

	if( cam->read(frame))
	{
		detector.resetTargetSegments(frame.size());
		detector.detectTargetSegment(frame, 0);
	}
}

