#ifndef SHOTDETECTOR_H
#define SHOTDETECTOR_H

#include <opencv2/opencv.hpp>

class ShotDetector
{
public:
	ShotDetector();
	int detectHit(cv::Mat& frame);
	void detectTargetSegment(cv::Mat camImg, int index);
	void resetTargetSegments(cv::Size frameSize);

	bool markShots; /**< Mark hits in Live image */

	unsigned char hueMin; /**< Minimum hue value for target segment detection. */
	unsigned char hueMax; /**< Maximum hue value for target segment detection. */
	unsigned char satMin; /**< Minimum saturation value for target segment detection. */
	unsigned char valMin; /**< Minimum brightness value for target segment detection. */

private:
	unsigned char brightnessThreshold; /**< Minimum brightness of image max to be detected as shot */

	cv::Mat targetSegments;
};

#endif // SHOTDETECTOR_H
