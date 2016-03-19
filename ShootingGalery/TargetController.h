/*
 * TargetController.h
 *
 *  Created on: 08.03.2016
 *      Author: hele
 */

#ifndef TARGETCONTROLLER_H_
#define TARGETCONTROLLER_H_

#include <iostream>
#include <string>
#include <fstream>

#ifdef WIN32
#define DEFAULT_DEV_PATH	(std::string("COM0"))
#else
#define DEFAULT_DEV_PATH	(std::string("/dev/ttyUSB0"))
#endif

class TargetController
{
public:
	TargetController(const std::string devPath = "");
	~TargetController();
	void init(const std::string devPath = DEFAULT_DEV_PATH);
	void hit(int num);

	void down(int num);

	void up(int num);

	void alldown();

	void allup();

private:

	std::fstream device;
	bool check(int num);

};

#endif /* TARGETCONTROLLE_H_ */
