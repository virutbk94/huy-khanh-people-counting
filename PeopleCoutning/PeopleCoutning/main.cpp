// PeopleCount.cpp : Defines the entry point for the console application.
//
#pragma once
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include "BackgroundSubtract.h"
#include "KLT.h"
#include "GaussFilterColor.h"
#include "DelaunayTriangle.h"
#include "PeopleContainer.h"
#define FILE "Video 6.wmv"

#define NUMBER_FEATURE 500

int main()
{
	CvCapture *capture = cvCaptureFromFile(FILE);
	if (capture == NULL)
	{
		return 0;
	}

	 CvScalar delaunay_color  = CV_RGB( 0,0,0);
	 CvScalar voronoi_color = CV_RGB(0, 180, 0);
	IplImage *image;
	image = cvQueryFrame(capture);
	CvSize size;
	size = cvGetSize(image);

	Container container(image);
	
	/*double fps = cvGetCaptureProperty(
		capture,
		CV_CAP_PROP_FPS
		);*/
	/*CvVideoWriter *writer = cvCreateVideoWriter(
		"out.avi",
		-1,
		fps,
		cvGetSize(image)
		);
		*/
	int depth = 8;
	int channels = 1;
	
	CvFont font;
	cvInitFont(&font,CV_FONT_ITALIC, 0.5,0.5,0,1,8);

	IplImage *background = cvCreateImage(size, depth, channels);
	cvCvtColor(image, background, CV_BGR2GRAY);
	
	KLT kltTracker;

	kltTracker.initial(size, background);
	
	
	DelaunayTriangle delaunay;
	CvPoint2D32f point[NUMBER_FEATURE];
	CvSubdiv2D *subdiv;

	IplImage *foreground1 = cvCreateImage(size, depth, channels);
	IplImage *foreground2 = cvCreateImage(size, depth, channels);
	IplImage *imgTmp = cvCreateImage(size, image->depth, image->nChannels);
	
	int isInit = 0;
	//for (int i = 0; i < 150; ++i)
		//cvQueryFrame(capture);
	int i = 0;
//	char count[10];
	cvCvtColor(image, foreground1, CV_RGB2GRAY);
	Gauss *m_gauss = new Gauss();
	m_gauss->LoadData("GaussModel.txt");
	IplImage *subtract;
	//m_gauss->SetThreshold(-5);
	//BackgroundSubtract(foreground1, background, foreground1);
	while ((image = cvQueryFrame(capture))!=NULL)
	{
		
		if (image== NULL) {
			break;
		}	
		cvCopyImage(image, imgTmp);

		subtract = m_gauss->Classify(
			image, 
			-6.5);
		
		cvShowImage("abc", subtract);
		
		++i;
		//itoa(i,count,10);
		//cvPutText(image, count, cvPoint(100, 100),&font , cvScalar(255,255,255));
		
		//background subtract
		/*
		cvCvtColor(image, foreground2, CV_BGR2GRAY);
		BackgroundSubtract(foreground2, background, foreground2);
		*/
		//KLT+ Delaunay clustering
		
		//kltTracker.flowTracking(foreground1, foreground2, point);
		//cvCopyImage(foreground2,foreground1);

		//kltTracker.drawArrow(imgTmp,COLOR_GREEN, point);
		//delaunay.initialize(image);
		//
		//delaunay.insertPoint(point);
		//
		//delaunay.draw_subdiv(imgTmp, delaunay_color, voronoi_color);
		
		
		//cvShowImage("imgTmp",imgTmp);
		
		//cvShowImage("background", foreground2);
		//foreground2 = foreground1;
		
		//cvWriteFrame(writer, foreground);
		
		//histogram matching
		/*if (isInit == 0)
		{
			container.initHist(image);
			isInit = 1;
		}
		container.getHist(image);
		container.matchPeople(image);
		*/
		
		//contour
		
		container.initPoint();
		container.Process(subtract, image);
		
		//cvShowImage("Contour", container.destinyImage);
		//cvWriteFrame(writer, container.destinyImage);
		//cvZero(container.destinyImage);
		
		if (cvWaitKey(1) == 27 )
		{
			//system("PAUSE");
			//getchar();
			//system("CONTINUE");
		}
		
	}
//	 cvReleaseVideoWriter(&writer);
	cvReleaseImage( &image);
	cvReleaseCapture(&capture);
	cvDestroyAllWindows();
	
	return 0;
}

