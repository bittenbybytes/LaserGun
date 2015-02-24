#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>     // std::string, std::to_string
#include "detector_main.h"
#ifdef WIN32
#include <windows.h> // Beep
#endif

struct RectanglePoints{
	cv::Point a;
	cv::Point b;
};

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

	do{
		// request new frame
		cv::Mat frame;
		bool bNewFrame = cam.read(frame);

		if (bNewFrame) {
			// display frame if valid

			static int backofCount = 0;

			// convert to grayscale
			cv::Mat gs_rgb(frame.size(), CV_8UC1);
			cv::cvtColor(frame, gs_rgb, CV_RGB2GRAY);

			// find maximum intensity value
			double min, max;
			cv::Point min_loc, max_loc;
			cv::minMaxLoc(gs_rgb, &min, &max, &min_loc, &max_loc);

			if (max > 220 && backofCount == 0)
			{
				// sufficient intensity and not detected recently

				// set back off time for next detection
				backofCount = 3;

				// record shot position
				shots.push_back(max_loc);

				if(isPointWithinArea(max_loc, rectPoints))
				{
					// shot landed within target area
					std::cout << "Target hit at: " << max_loc.x << " " << max_loc << std::endl;

#ifdef WIN32
					Beep(1000, 50);
#endif

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

			cv::putText(frame, strstr.str(), cv::Point(50, 50 + 16), 2, 0.5, cv::Scalar(255, 255, 255, 255));

			cv::rectangle(frame, rectPoints.a, rectPoints.b, cv::Scalar(128, 128, 255, 255));

			// paint shot marks
			for (size_t i =0; i < shots.size(); i++)
				cv::circle(frame, shots.at(i), 3, cv::Scalar(0, 0, 0, 255), 2);

			// paint hit marks
			for (size_t i =0; i < hits.size(); i++)
				cv::circle(frame, hits.at(i), 3, cv::Scalar(0, 0, 255, 255), 2);


			cv::imshow(winname, frame);

			if (backofCount > 0)
				backofCount--;
		}

		// wait for key input for 30ms
		key = cv::waitKey(30);

		if (key == 'r')
		{
			shots.clear();
			hits.clear();
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