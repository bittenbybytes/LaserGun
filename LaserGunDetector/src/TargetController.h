/*
 * TargetController.h
 *
 *  Created on: 08.03.2016
 *      Author: hele
 */

#ifndef TARGETCONTROLLER_H_
#define TARGETCONTROLLER_H_

#include <iostream>
#include <fstream>

class TargetController
{
public:
	TargetController();
	void init();
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
