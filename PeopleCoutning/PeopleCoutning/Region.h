
#pragma once
#include "cv.h"

typedef struct StrRegion {
	CvPoint leftPoint, rightPoint;
	CvPoint *arrPoint,*leftArrPoint, *rightArrPoint;
	IplImage* bgrImg;
	int indPoint;
	bool bEnableSelectRegion;

	StrRegion(IplImage* background)
	{
		int deltaSidewalk = 20;
		bgrImg = background;
		leftPoint = cvPoint(0 + deltaSidewalk, bgrImg->height - 1);
		rightPoint = cvPoint(bgrImg->width - deltaSidewalk, bgrImg->height - 1);
		arrPoint = new CvPoint[4];
		leftArrPoint = new CvPoint[4];
		rightArrPoint = new CvPoint[4];
		indPoint = 0;
		bEnableSelectRegion = true;
	}

} STR_REGION;