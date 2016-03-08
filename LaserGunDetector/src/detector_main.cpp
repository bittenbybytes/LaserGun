#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>     // std::string, std::to_string
#include "detector_main.h"

#include "TargetController.h"

#ifdef WIN32
#include <windows.h> // Beep
#endif

struct RectanglePoints{
	cv::Point a;
	cv::Point b;
};

void detectTargets(cv::Mat camImg, cv::Mat& targetSegments);

int detectHit(cv::Mat& targetSegments, const cv::Point shotPos);

void onMouse(int event, int x, int y, int d, void* points);

bool isPointWithinArea(cv::Point point, RectanglePoints area);


int main() {
	// open any available webcam
#ifdef WIN32
	cv::VideoCapture cam(0);
#else
	cv::VideoCapture cam(-1);
#endif

	if (!cam.isOpened())
	{
		std::cout << "Error:  no webcam available!!!" << std::endl;
		return -1; // no webcam available
	}


	cam.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

	//cam.set(CV_CAP_PROP_EXPOSURE, 10);

	char key = 0;

	std::vector<cv::Point> hits;
	std::vector<cv::Point> shots;

	cv::Point upperLeft(246, 143);
	cv::Point lowerRight(298, 170);

	std::string winname = "WebCamStream (press q to quit, r to reset, draw target area with mouse)";
	cv::namedWindow(winname);
	RectanglePoints rectPoints = { {0,0}, {0,0} };
	cv::setMouseCallback(winname, onMouse, &rectPoints);

	cv::Mat targetSegments;

	TargetController target;
	target.alldown();

	target.allup();

	do{
		// request new frame
		cv::Mat frame;
		cv::Mat dispFrame;
		bool bNewFrame = cam.read(frame);

		if (bNewFrame) {
			// display frame if valid
			dispFrame = frame;
			static int backofCount = 0;

			// convert to grayscale
			cv::Mat gs_rgb(dispFrame.size(), CV_8UC1);
			cv::cvtColor(dispFrame, gs_rgb, CV_RGB2GRAY);

			// find maximum intensity value
			double min, max;
			cv::Point min_loc, max_loc;
			cv::minMaxLoc(gs_rgb, &min, &max, &min_loc, &max_loc);

			if (max > 220 && backofCount == 0)
			{
				// sufficient intensity and not detected recently

				// set back off time for next detection
				backofCount = 10;

				// record shot position
				shots.push_back(max_loc);

				int segmentHit = detectHit(targetSegments, max_loc);

				if(isPointWithinArea(max_loc, rectPoints) ||
						segmentHit > 0)
				{
					// shot landed within target area
					std::cout << "Target hit at: " << max_loc.x << " " << max_loc << std::endl;

#ifdef WIN32
					Beep(1000, 50);
#endif

					target.hit(segmentHit>0);

					// record hit position
					hits.push_back(max_loc);
				}
			}

			// print number of shots and hits
			std::stringstream strstr;
			strstr << shots.size() << " Shot(s) fired  ";
			strstr << hits.size() << " Hit(s)  ";

			if (shots.size() > 0)
			{
				cv::Point& lastHit = shots.back();
				strstr << "Last Hit " << lastHit.x << " " << lastHit.y;
			}

			cv::putText(dispFrame, strstr.str(), cv::Point(50, 50 + 16), 2, 0.5, cv::Scalar(255, 255, 255, 255));

			cv::rectangle(dispFrame, rectPoints.a, rectPoints.b, cv::Scalar(128, 128, 255, 255));

			// paint shot marks
			for (size_t i =0; i < shots.size(); i++)
				cv::circle(dispFrame, shots.at(i), 3, cv::Scalar(0, 0, 0, 255), 2);

			// paint hit marks
			for (size_t i =0; i < hits.size(); i++)
				cv::circle(dispFrame, hits.at(i), 3, cv::Scalar(0, 0, 255, 255), 2);


			cv::imshow(winname, dispFrame);

			if (backofCount > 0)
				backofCount--;
		}

		// wait for key input for 30ms
		key = cv::waitKey(30);

		switch(key)
		{
		case 'r':
			shots.clear();
			hits.clear();
			break;

		case 'd':
			detectTargets(frame, targetSegments);
			break;

		case 'D':
			target.alldown();
			break;
		case 'U':
			target.allup();
			break;
		}

	}while(key != 'q');

	return 0;
}


void onMouse(int event, int x, int y, int d, void* points)
{
	static bool drawing = false;
	RectanglePoints* rectPoints = (RectanglePoints*)(points);
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		rectPoints->a = cv::Point(x, y);
		rectPoints->b = cv::Point(x, y);
		drawing = true;
	}

	if (event == CV_EVENT_LBUTTONUP || (event == CV_EVENT_MOUSEMOVE && drawing))
	{
		rectPoints->b = cv::Point(x, y);
	}

	if (event == CV_EVENT_LBUTTONUP)
	{
		drawing = false;
		std::cout << "Target area redrawn:\n";
		std::cout << "\t x=" << rectPoints->a.x << "\t y=" << rectPoints->a.x << "\n";
		std::cout << "\t x=" << rectPoints->b.x << "\t y=" << rectPoints->b.y << "\n";
	}

	//cv::Point pt = cv::Point(x, y);
	//std::cout << "x=" << pt.x << "\t y=" << pt.y << "\n";
}


bool isPointWithinArea(cv::Point point, RectanglePoints area)
{
	return isWithinRange(point.x, area.a.x, area.b.x) && isWithinRange(point.y, area.a.y, area.b.y);
}


void detectTargets(cv::Mat camImg, cv::Mat& targetSegments)
{
#ifdef TH_GRAYSCALE

	// convert to grayscale
	cv::Mat camImgGrayScale(camImg.size(), CV_8UC1);
	cv::cvtColor(camImg, camImgGrayScale, CV_RGB2GRAY);

	cv::Mat threshold;
	cv::threshold(camImgGrayScale, threshold, 220,255, cv::THRESH_BINARY);

#else

	// convert to hsv
	cv::Mat camImgHSV;
	cv::cvtColor(camImg, camImgHSV, CV_RGB2HSV_FULL);

	cv::Mat threshold;

	int rotation = 128 - 255; // 255 = red
	//cv::add(camImgHSV, cv::Scalar(rotation, 0, 0), camImgHSV);
	//cv::inRange(camImgHSV, cv::Scalar(114, 135, 135), cv::Scalar(142, 255, 255), threshold);
	cv::inRange(camImgHSV, cv::Scalar(150, 115, 115), cv::Scalar(195, 255, 255), threshold);
#endif
	int dilation_type = cv::MORPH_RECT;

	int dilation_size = 2;

	cv::Mat element = cv::getStructuringElement( dilation_type,
									   cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),
									   cv::Point( dilation_size, dilation_size ) );
	/// Apply the dilation operation
	cv::erode(threshold, targetSegments, element);

	cv::namedWindow("Threshold");
	cv::imshow("Threshold", targetSegments);

}

int detectHit(cv::Mat& targetSegments, const cv::Point shotPos)
{
	if (targetSegments.size() == cv::Size(0,0))
		return -1;

	return (int)(targetSegments.at<unsigned char>(shotPos));
}
