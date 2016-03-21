#include "ShotDetector.h"

ShotDetector::ShotDetector()
	: brightnessThreshold(220), markShots(true)
{
}

int ShotDetector::detectHit(cv::Mat& frame)
{
	int targetHit = -1;

	// convert to grayscale
	cv::Mat gs_rgb(frame.size(), CV_8UC1);
	cv::cvtColor(frame, gs_rgb, CV_RGB2GRAY);

	// find maximum intensity value
	double min, max;
	cv::Point min_loc, max_loc;
	cv::minMaxLoc(gs_rgb, &min, &max, &min_loc, &max_loc);

	if (max > this->brightnessThreshold)
	{
		// sufficient intensity
		std::cout << "Shot at location: " << max_loc.x << " " << max_loc.y << std::endl;

		if(this->markShots)
		{
			cv::circle(frame, max_loc, 3, cv::Scalar(0, 0, 0, 255), 2);
		}

		// look for possible hit in segment map
		//targetHit = int(detectHit(targetSegments, max_loc)) - 1;
		if(targetHit >= 0)
		{
			std::cout << "Hit at Target: " << targetHit << std::endl;
		}
	}

	return targetHit;
}
