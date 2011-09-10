#pragma once
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include "Param.h"


#define NUMBER_FEATURE 500
#define Pi 3.141592653
using namespace std;
class KLT
{
	IplImage *prev_pyr, *curr_pyr, *tmpImg, *kltImg, *eigImg, *bgrImgTmp;

	//CvPoint2D32f prev_features[NUMBER_FEATURE];
	CvPoint2D32f curr_features[NUMBER_FEATURE];
	CvSize win_size;
	int level, numFeatures, threshRoad;
	char features_found[NUMBER_FEATURE];	
	float feature_errors[NUMBER_FEATURE];	
	CvPoint	pnt1, pnt2;
	CvTermCriteria criteria,termSubPix;
	
	CvSize rectSize;
public:
	KLT ()
	{

	}
	~KLT()
	{
		Param::releaseAllImage(kltImg, eigImg, tmpImg, prev_pyr, curr_pyr);
	}
	void initial(CvSize rectSize, IplImage * bgrImg )
	{
		win_size = cvSize(5,5);
		level = 5;
		threshRoad = 20;
		numFeatures = NUMBER_FEATURE;
		criteria = cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.3);
		termSubPix = cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10, 0.7);

		kltImg = cvCreateImage(rectSize, IPL_DEPTH_8U, 3);
		eigImg = cvCreateImage(rectSize, IPL_DEPTH_32F, 1);
		tmpImg = cvCreateImage(rectSize, IPL_DEPTH_32F, 1);
		prev_pyr = cvCreateImage(rectSize, IPL_DEPTH_8U, 1);
		curr_pyr = cvCreateImage(rectSize, IPL_DEPTH_8U, 1);

		bgrImgTmp = bgrImg;
	}
	void flowTracking(IplImage *gray1, IplImage *gray2, CvPoint2D32f prev_features[])
	{
		//cvCopy( img, kltImg);
		//cvCopy(img,kltImg);
		bool flagTrack;
		//cvCornerHarris(gray1,prev_features, 3,3,0.04);

		//cvGoodFeaturesToTrack(gray1, eigImg, tmpImg, prev_features, &numFeatures, 0.01, 20);
		cvGoodFeaturesToTrack(gray1, eigImg, tmpImg, prev_features, &numFeatures, 0.01, 5.0, 0,3,0,0.04);
		//cvFindCornerSubPix(gray1, prev_features, numFeatures, win_size, cvSize(-1,-1), termSubPix);
		//cvCalcOpticalFlowPyrLK(gray1, gray2, prev_pyr, curr_pyr, prev_features, curr_features, numFeatures, win_size, level, features_found, feature_errors, criteria, 0);
		cvSetZero(eigImg);
		cvSetZero(tmpImg);
		cvSetZero(prev_pyr);
		cvSetZero(curr_pyr);
		
	}
	
	void drawArrow(IplImage* image,CvScalar color,CvPoint2D32f prev_features[])
	{
		if (numFeatures > 0)
		{
			for (int i=0; i < numFeatures; i++)
			
			{
				
				pnt1.x = (int) prev_features[i].x;
				pnt1.y = (int) prev_features[i].y;
				pnt2.x = (int) prev_features[i].x+1;
				pnt2.y = (int) prev_features[i].y+1;
				
				// kiem tra tinh hop le cua dac trung
				
				if (((pnt1.x != pnt2.x) || (pnt1.y != pnt2.y)))
				{
					double displacementY = pnt1.y - pnt2.y;
					double displacementX = abs(pnt2.x - pnt1.x);
					/*if (displacementY >= 2)
					{
						
						drawArrow(kltImg, pnt1, pnt2, COLOR_GREEN);
					}
					else if (displacementY <= -2 || displacementX >= 4)
					{
					
						drawArrow(kltImg, pnt1, pnt2, COLOR_BLUE);
					}*/
					cvLine(image, pnt1, pnt2, color, 2 ,8 ,0);
				//	CvScalar bgrColor, pxlColor;
				//	bgrColor = getPixalValue(bgrImgTmp, pnt2.x, pnt2.y, 3);
				//	pxlColor = getPixalValue(kltImg, pnt2.x, pnt2.y, 3);

					/*if ((abs(bgrColor.val[0] - pxlColor.val[0]) > threshRoad) || \
						(abs(bgrColor.val[1] - pxlColor.val[1]) > threshRoad) || \
						(abs(bgrColor.val[2] - pxlColor.val[2]) > threshRoad))
					{
						flagTrack = true;

					}*/

				

				
			}
		}
		}
		
	}
	
	CvScalar getPixalValue(IplImage* img, int x, int y, int channels)
	{
		CvScalar rtValue;
		uchar* ptrSrc = (uchar*) (img->imageData + y * img->widthStep + x * channels);
		for (int i=0; i < channels; i++)
		{
			rtValue.val[i] = ptrSrc[i];
		}
		return rtValue;
	}
};