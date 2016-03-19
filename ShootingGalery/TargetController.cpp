/*
 * TargetControlle.cpp
 *
 *  Created on: 08.03.2016
 *      Author: hele
 */

#include "TargetController.h"

#include <opencv2/opencv.hpp>

#ifndef WIN32

#endif

static const int nTargets = 2;
static const char hitCmd[nTargets] = {'h','H'};
static const char downCmd[nTargets] = {'d','D'};
static const char upCmd[nTargets] = {'u','U'};

#define DEBUGOUT std::cout

const int delay = 3000;

void TargetController::init(const std::string devPath /* = DEFAULT_DEV_PATH */)
{
	if (device.is_open())
		device.close();

	device.open(devPath.c_str());
}

void TargetController::down(int num)
{
	if(check(num))
	{
		device << downCmd[num] << std::endl;
		cv::waitKey(delay);
	}

}

void TargetController::up(int num)
{
	if(check(num))
	{
		device << upCmd[num] << std::endl;
		cv::waitKey(delay);
	}

}

void TargetController::alldown()
{
	for (int num = 0; num < nTargets; num++)
	{
		down(num);
	}
}

void TargetController::allup()
{
	for (int num = 0; num < nTargets; num++)
	{
		up(num);
	}
}

TargetController::TargetController(const std::string devPath /* = "" */)
{
	if(devPath != "")
		device.open(devPath.c_str());
}

TargetController::~TargetController()
{
	if (device.is_open())
		device.close();
}

bool TargetController::check(int num)
{
	if(num < 0 && num >= nTargets)
	{
		DEBUGOUT << "Invalid target number" << std::endl;
		return false;
	}

	if(!device.is_open())
	{
		DEBUGOUT << "Invalid communication device" << std::endl;
		return false;
	}

	DEBUGOUT << "writing to target num " << num << std::endl;

	return true;
}

void TargetController::hit(int num)
{
	if(check(num))
		device << hitCmd[num] << std::endl;
}
