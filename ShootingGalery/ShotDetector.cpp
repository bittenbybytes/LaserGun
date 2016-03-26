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
		//std::cout << "Shot at location: " << max_loc.x << " " << max_loc.y << std::endl;

		if(this->markShots)
		{
			cv::circle(frame, max_loc, 3, cv::Scalar(0, 0, 0, 255), 2);
		}

		// look for possible hit in segment map
		if(targetSegments.size() == frame.size())
			targetHit = int(targetSegments.at<unsigned char>(max_loc)) - 1;

		if(targetHit >= 0)
		{
			std::cout << "Hit at Target: " << targetHit << std::endl;
		}
	}

	return targetHit;
}

int ShotDetector::detectTargetSegment(cv::Mat camImg, int index)
{
	// convert to hsv
	cv::Mat camImgHSV(camImg.size(), CV_8UC3);
	cv::cvtColor(camImg, camImgHSV, CV_BGR2HSV_FULL);

	cv::Mat threshold;

	unsigned char rotation = 128; // 255 = red
	cv::add(camImgHSV, cv::Scalar(rotation, 0, 0), camImgHSV);
	cv::inRange(camImgHSV, cv::Scalar(100, 100, 80), cv::Scalar(160, 255, 255), threshold);
	int dilation_type = cv::MORPH_RECT;

	int dilation_size = 2;

	cv::Mat element = cv::getStructuringElement( dilation_type,
									   cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),
									   cv::Point( dilation_size, dilation_size ) );
	/// Apply the erosion operation
	cv::erode(threshold, threshold, element);

	/// Apply the dilation operation
	cv::dilate(threshold, threshold, element);

	threshold /= 255;
	threshold *= (index+1);

	if(targetSegments.dims == 0)
		threshold.copyTo(targetSegments);
	else
		cv::add(targetSegments, threshold, targetSegments);

	cv::namedWindow("Threshold");
	cv::imshow("Threshold", targetSegments *64);
}

void ShotDetector::resetTargetSegments(cv::Size frameSize)
{
	targetSegments.zeros(frameSize, CV_8U);
}
