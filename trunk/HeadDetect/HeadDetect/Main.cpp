#pragma once


#include <stdio.h>
#include <tchar.h>
#include <cv.h>
#include <highgui.h>
#include <math.h>
#include <iostream>
#include <string.h>
#include <vector>
#include"Gauss.h"
#include"HeadDetect.h"

using namespace std;

#define FILENAME "Video 6.wmv"

void my_mouse_callback(int event,
					   int x,
					   int y,
					   int flag,
					   void* param);


CvRect rect;
CvPoint p1, p2;

bool bMoveDown = false;
bool isDrawing = false;
bool bDrawingFinished = false;

HeadDetect detect;

/*
{

	CvCapture *capture = cvCaptureFromFile(FILENAME);
	if (capture == NULL)
	{
		return 0;
	}

	IplImage *image;
	image = cvQueryFrame(capture);
	CvSize size;
	size = cvGetSize(image);

	int depth = 8;
	int channels = 1;

	int i = 0;

	Gauss *m_gauss = new Gauss();
	m_gauss->LoadData("GaussModel.txt");
	IplImage *subtract;
	while ((image = cvQueryFrame(capture))!=NULL)
	{

		if (image== NULL) {
			break;
		}	
		cvShowImage("image truoc khi classify",image);
		subtract = m_gauss->Classify(image, -6.5);
		
		cvShowImage("abc", subtract);
		
		++i;
		
		
	if(cvWaitKey(1) == 27 )
	{
	
	}
	}
	cvReleaseImage( &image);
	cvReleaseCapture(&capture);
	cvDestroyAllWindows();
	
	return 0;

	
}

*/
	////////////////////////////////////////////////////////////////////



int main()
{
	CvCapture* capture;
	capture = cvCreateFileCapture(FILENAME);

	if(capture == 0)
	{
		return 0;
	}

	cvNamedWindow(FILENAME);
	IplImage* image = cvQueryFrame(capture);
	IplImage* imageBuffer = cvCreateImage(cvGetSize(image), image->depth, image->nChannels);
	IplImage* imageShow = cvCreateImage(cvGetSize(image), image->depth, image->nChannels);
	cvCopyImage(image, imageBuffer);

	cvSetMouseCallback(
		FILENAME,
		my_mouse_callback,
		(void*)imageBuffer);
	do 
	{
		if (isDrawing)
		{
			cvCopyImage(image, imageBuffer);
			cvDrawRect(imageBuffer, p1, p2, CV_RGB(255, 0, 0), 2);
			
		}
		cvCopyImage(imageBuffer, imageShow);
		cvShowImage(FILENAME, imageShow);
		if (bDrawingFinished || cvWaitKey(1) == 27)
		{
			break;
		}
	} while (1);



	if(bDrawingFinished)
	{
		rect.x = p1.x;
		rect.y = p1.y;
		rect.width = p2.x - p1.x;
		rect.height = p2.y - p1.y;
		
		while((image = cvQueryFrame(capture))!= NULL)
		{
			
			//image = cvQueryFrame(capture);
			if(!image) break;
		
			
		
			detect.image = image;
			int count = detect.getHairColor(rect);
				cvDrawRect(image, p1, p2, CV_RGB(255, 0, 0), 2);
				cvShowImage("aaa",image);
			cout << count;
				cvWaitKey(10);
		}

	}
	return 1;
}
		//while((image = cvQueryFrame(capture))!= NULL)
		//{
		//	
		//	//image = cvQueryFrame(capture);
		//	if(!image) break;
		//
		//	
		//	detect.image = image;
		//	detect.getHairColor()

		//	
		//	cvShowImage("aaa",image);

		//		cvWaitKey(10);
		//}
	
		//	cvReleaseCapture(&capture);
		//	cvDestroyAllWindows();

			
	//IplImage* img = cvLoadImage("1.jpg",1);
	//Gauss temp;
	//img = temp.Detect(img);
	//cvShowImage("aaa",img);
	//cvWaitKey(1000);
	//return 1;




void my_mouse_callback(int event, int x, int y, int flag, void* param)
{
	IplImage* img = (IplImage*)param;

	switch (event)
	{
	case CV_EVENT_MOUSEMOVE:
		{
			if (bMoveDown)
			{
				p2.x = x;
				p2.y = y;
				isDrawing = true;
			}
		}
		break;
	case CV_EVENT_LBUTTONDOWN:
		{
			if (!isDrawing)
			{
				bMoveDown = true;
				p1.x = x;
				p1.y = y;
			}
		}
		break;
	case CV_EVENT_LBUTTONUP:
		{
			bMoveDown = false;
			if (isDrawing)
			{
				bDrawingFinished = true;
				isDrawing = false;
			}
		}
		break;
	case CV_EVENT_RBUTTONDOWN:
		break;
	}
}