/*
 * TargetControlle.cpp
 *
 *  Created on: 08.03.2016
 *      Author: hele
 */

#include "TargetController.h"

#ifndef WIN32

#endif

static const int nTargets = 2;
static const char hitCmd[nTargets] = {'h','H'};
static const char downCmd[nTargets] = {'d','D'};
static const char upCmd[nTargets] = {'u','U'};

#define DEBUGOUT std::cout

void TargetController::init()
{
#ifdef WIN32
	device.open("COM0");
#else
	device.open("/dev/ttyUSB1");
#endif
}

void TargetController::down(int num)
{
	if(check(num))
		device << downCmd[num] << std::endl;
}

void TargetController::up(int num)
{
	if(check(num))
		device << upCmd[num] << std::endl;
}

void TargetController::alldown()
{
	for (int num = 1; num < nTargets; num++)
	{
		down(num);
	}
}

void TargetController::allup()
{
	for (int num = 1; num < nTargets; num++)
	{
		up(num);
	}
}

TargetController::TargetController()
{
	init();
}

bool TargetController::check(int num)
{
	if(num < 0 && num > nTargets)
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
