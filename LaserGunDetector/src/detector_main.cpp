#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>     // std::string, std::to_string
using namespace cv;


int main() {
	// open any available webcam
	VideoCapture cam(-1);

	if(!cam.isOpened())
		return -1; // no webcam available

	char key = 0;

	vector<cv::Point> hits;
	vector<cv::Point> shots;

	Point upperLeft(246, 143);
	Point lowerRight(298, 170);

	do{
		// request new frame
		Mat frame;
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

				if(max_loc.x >= upperLeft.x && max_loc.x <= lowerRight.x &&
					max_loc.y >= upperLeft.y && max_loc.y <= lowerRight.y)
				{
					// shot landed within target area

					// record hit position
					hits.push_back(max_loc);
				}
			}

			// print number of shots and hits
			cv::Point& lastHit = shots.back();
			std::stringstream strstr;
			strstr << shots.size() << " Shot(s) fired  ";
			strstr << hits.size() << " Hit(s)  ";

			if (shots.size() > 0)
				strstr << "Last Hit " << lastHit.x << " " << lastHit.y;

			putText(frame, strstr.str(),Point(50,50+16),2,0.5,Scalar(255,255,255,255));

			rectangle(frame, upperLeft, lowerRight, Scalar(128,128,255,255));

			// paint shot marks
			for (size_t i =0; i < shots.size(); i++)
				circle(frame, shots.at(i),3,Scalar(0,0,0,255),2);

			// paint hit marks
			for (size_t i =0; i < hits.size(); i++)
				circle(frame, hits.at(i),3,Scalar(0,0,255,255),2);

			imshow("WebCamStream (press q to quit)",frame);

			if (backofCount > 0)
				backofCount--;
		}

		// wait for key input for 30ms
		key = waitKey(30);

	}while(key != 'q');

	return 0;
}
