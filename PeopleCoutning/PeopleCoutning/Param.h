
#pragma once
#include "cv.h"
#include "highgui.h"
#include <time.h>
#include <math.h>
#include <vector>
#include "Region.h"
#include "FrameSegment.h"



#define MAIN_WINDOW		"Main TMS Application"
#define LEFT_FLOW_KLT	"KLT LtFlow"
#define RIGHT_FLOW_KLT	"KLT RtFlow"
#define LEFT_FLOW_FGR	"FGR LtFlow"
#define RIGHT_FLOW_FGR	"FGR RtFlow"

#define COLOR_YELLOW	cvScalar(0,255,255)
#define COLOR_WHITE		cvScalar(255,255,255)
#define COLOR_GREEN		cvScalar(0,255,0)
#define COLOR_AQUA		cvScalar(255,255,0)
#define COLOR_BLUE		cvScalar(255,0,0)
#define COLOR_BLACK		cvScalar(0,0,0)

//////////////////////////////////////////////////////////////////////////
//
//

//template <typename T>

//
//////////////////////////////////////////////////////////////////////////
// dung nhung ham static do phai ho tro cho cac form dialog
//
class Param
{
public:
	

	static void releaseAllImage(IplImage* img1=NULL, IplImage* img2=NULL,IplImage* img3=NULL,IplImage* img4=NULL,IplImage* img5=NULL)
	{
		if (img1)
			cvReleaseImage(&img1);
		else if (img2)
			cvReleaseImage(&img2);
		else if (img3)
			cvReleaseImage(&img3);
		else if (img4)
			cvReleaseImage(&img4);
		else if (img5)
			cvReleaseImage(&img5);
	}

	static double calDistance2Point(CvPoint p1, CvPoint p2)
	{
		return sqrt(pow(p1.x - p2.x, 2.0) + pow(p1.y - p2.y, 2.0));
	}

	static bool isContainInRect(CvPoint pnt, CvRect rect)
	{
		bool flag = false;
		if ((pnt.x > rect.x && pnt.x < (rect.x + rect.width)) && \
			(pnt.y > rect.y && pnt.y < (rect.y + rect.height)))
		{
			flag = true;
		}
		return flag;
	}

	static CvPoint* convertVectorPointToArrayPoint(std::vector<CvPoint> points)
	{
		int numPoint = points.size();
		CvPoint* arrPoint = new CvPoint[numPoint];

		for (int i=0; i < numPoint; i++)
		{
			arrPoint[i] = points[i];
		}

		return arrPoint;
	}

	static void drawLinesFromArrPoints(IplImage* img, CvPoint* arrPoint, int nPoint, CvScalar color)
	{
		for (int i=0; i < nPoint; i++)
		{
			cvLine(img, arrPoint[i % nPoint], arrPoint[(i+1) % nPoint], color, 2, CV_AA);
		}
	}

	static CvRect getBoundingRect(std::vector<CvPoint> vecPoint)
	{
		CvRect rect;
		int nPoint = vecPoint.size();
		int xmax, ymax, xmin, ymin;
		xmin = xmax = vecPoint[0].x;
		ymin = ymax = vecPoint[0].y;

		for(int i=1; i < nPoint; i++)
		{
			if(xmin > vecPoint[i].x)
				xmin = vecPoint[i].x;

			if(xmax < vecPoint[i].x)
				xmax = vecPoint[i].x;

			if(ymin > vecPoint[i].y)
				ymin = vecPoint[i].y;

			if(ymax < vecPoint[i].y)
				ymax = vecPoint[i].y;		
		}

		rect.x = xmin;
		rect.y = ymin;
		rect.width  = xmax - xmin;
		rect.height = ymax - ymin;

		return rect;
	}

	static CvRect getBoundingRect(CvPoint* arrPoint, int nPoint)
	{
		CvRect rect;

		int xmax, ymax, xmin, ymin;
		xmin = xmax = arrPoint[0].x;
		ymin = ymax = arrPoint[0].y;

		for(int i=1; i < nPoint; i++)
		{
			if(xmin > arrPoint[i].x)
				xmin = arrPoint[i].x;

			if(xmax < arrPoint[i].x)
				xmax = arrPoint[i].x;

			if(ymin > arrPoint[i].y)
				ymin = arrPoint[i].y;

			if(ymax < arrPoint[i].y)
				ymax = arrPoint[i].y;		
		}

		rect.x = xmin;
		rect.y = ymin;
		rect.width  = xmax - xmin;
		rect.height = ymax - ymin;

		return rect;
	}
};

