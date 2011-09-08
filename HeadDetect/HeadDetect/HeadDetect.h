
#pragma once


#include <stdio.h>
#include <tchar.h>
#include <cv.h>
#include <highgui.h>
#include <math.h>
#include <iostream>
#include <string.h>
#include <vector>

class HeadDetect
{
	public:
		CvRect rect;
		IplImage* image;
		IplImage* res;
		//CvContour* seqContour;

		
		CvSeq * contours;

		HeadDetect(CvRect Rect, IplImage* Image)
		{
			rect = Rect;
			image = Image;
		}
		HeadDetect(IplImage* Image)
		{
			image = Image;
		}

		HeadDetect()
		{

		}
		~HeadDetect()
		{

			

		}

		int getHairColor( CvRect rect);
		int getContour(CvRect rect);

		void convertImage();
};