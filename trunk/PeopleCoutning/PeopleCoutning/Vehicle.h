
#pragma once
#include "cv.h"

typedef struct StrVehicle
{
	CvRect boundingRect;
	CvPoint center;

	StrVehicle()
	{

	}
	StrVehicle(CvRect rect, CvPoint pntCenter)
	{
		boundingRect = rect;
		center = pntCenter;
	}
	~StrVehicle()
	{

	}

} VEHICLE;