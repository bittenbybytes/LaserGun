#ifndef SHOTDETECTOR_H
#define SHOTDETECTOR_H

#include <opencv2/opencv.hpp>

class ShotDetector
{
public:
	ShotDetector();
	int detectHit(cv::Mat& frame);
	int detectTargetSegment(cv::Mat camImg, int index);
	void resetTargetSegments(cv::Size frameSize);

private:
	unsigned char brightnessThreshold; /**< Minimum brightness of image max to be detected as shot */
	bool markShots; /**< Mark hits in Live image */
	cv::Mat targetSegments;
};

#endif // SHOTDETECTOR_H
