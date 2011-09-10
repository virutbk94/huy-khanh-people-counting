
#pragma once


#include <stdio.h>
#include <tchar.h>
#include <cv.h>
#include <highgui.h>
#include <math.h>
#include <iostream>
#include <string.h>
#include <vector>
#include "GaussFilterColor.h"

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


int HeadDetect::getHairColor(CvRect rect)
{

	Gauss* g = new Gauss();
	g->LoadData("GaussModel.txt");
	//IplImage* tpl = cvLoadImage("2.jpg",CV_LOAD_IMAGE_COLOR);
	int			img_width, img_height;;
	int			tpl_width, tpl_height;
	int			res_width, res_height;

	if( image == 0 ) {
		fprintf( stderr, "Cannot load file %s!\n");
		//return 1; 
	}



	//res = cvCreateImage( cvSize(image->width, image->height), IPL_DEPTH_32F, 1 );

	//cvMatchTemplate( image, tpl, res, CV_TM_SQDIFF_NORMED );
	IplImage* temp = cvLoadImage("1.jpg",1);
	//res = g.Detect(image);
	res = g->Classify(image, -6.5);	

	cvShowImage("res sau khi classify",res);
	int number = getContour(rect);

	return number;
};


int HeadDetect::getContour(CvRect rect)
{
	CvMemStorage* storage;
	storage = cvCreateMemStorage(0);
	if((rect.height > res->height)&&(rect.width > res->width))
	{
		return 0;
	}
	cvSetImageROI(res, rect);
	//cvShowImage("res ban dau`",res);
	IplImage* cropRes = cvCreateImage(cvGetSize(res),res->depth,res->nChannels);

	cvCopy(res,cropRes,NULL);
	//cvShowImage("cropRes", cropRes);
	IplImage* res2 = cvCreateImage( cvGetSize(res), 8, 1 );



	cvThreshold(cropRes,cropRes,0.5,1,CV_THRESH_BINARY_INV);
	//cvShowImage("res sau",res);
	cvConvertScale(cropRes,res2);


	CvContourScanner scanner = cvStartFindContours(res2,storage,sizeof(CvContour), CV_RETR_EXTERNAL);
	contours = cvFindNextContour(scanner);

	int numb = cvFindContours(res2,storage,&contours,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

	contours = cvFindNextContour(scanner);
	IplImage* cc = cvCreateImage(cvGetSize(res),res->depth,res->nChannels);
	contours = cvEndFindContours(&scanner);
	cvDrawContours(cc, contours, CV_RGB(255,0,0), CV_RGB(0,255,0),1,1,CV_AA, cvPoint(0,0) );
	cvShowImage("drawImage",cc);
	cvResetImageROI(res);
	cvReleaseMemStorage(&storage);

	return numb;
}

