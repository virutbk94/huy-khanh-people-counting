// PeopleCount.cpp : Defines the entry point for the console application.
//
#pragma once
#include "stdafx.h"
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include "VehicleDetection.h"
#define INPUT "Input\\Thang Test_LocalHost_ch20[Clip 1]_Part1[720x240x13fps].avi"

void initializeDetectorConfiguration(VehicleDetectionConfiguration *&iVConfig, int width, int height);
void initializeImageData(IplImage *iImg, VehicleDetectionImage *iVImage);

int main()
{
	CvCapture *capture = cvCaptureFromFile(INPUT);
	if (capture == NULL)
	{
		return 0;
	}

	FILE * saveDetectionFile = fopen("detection.txt", "wt");

	int width, height;
	width = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	height = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);

	CvVideoWriter* writer  = cvCreateVideoWriter("out.avi", CV_FOURCC('X', 'v', 'i', 'D'),25, cvSize(width*2, height));

	/////////////////////////////////////////////////////////////////
	// Init
	/////////////////////////////////////////////////////////////////
	VehicleDetector *vDet;
	VehicleDetectionConfiguration *vConfig;	

	//allocate vehicle detector
	if(!allocateVehicleDetector(&vDet, &vConfig))
	{
		printf("Failed in allocation memory to detector\n");
		return -1;
	}

	//load detector
	char * detFile = "_Bin\\detector.xml";
	if(!loadVehicleDetectionClassifier(detFile, vDet))
	{
		printf("Failed in loading detector\n");
		return -1;
	}
	printf("Detector used: %s\n", detFile);

	IplImage *backgroundImg = cvLoadImage("background.jpg");
	IplImage *background = cvCreateImage(cvGetSize(backgroundImg), 8, 1);
	IplImage* hsv = cvCreateImage(cvGetSize(backgroundImg), 8, 3);		
	//cvSmooth(backgroundImg,backgroundImg, CV_BLUR, 3, 3);
	cvCvtColor(backgroundImg, background, CV_BGR2GRAY);	
	//cvCvtColor(backgroundImg, hsv, CV_BGR2HSV);
	//cvCvtPixToPlane(hsv, 0, 0, background, 0);
	
	IplImage *grayImg = cvCreateImage(cvGetSize(backgroundImg), 8, 1);
	IplImage *diffImg = cvCreateImage(cvGetSize(backgroundImg), 8, 1);
	IplImage *diffImgShow = NULL;
	IplImage* temp = NULL;
	IplImage* saveImg = cvCreateImage(cvSize(width*2,height),IPL_DEPTH_8U, 3);;
	
	int i = 0;
	IplImage* frame = NULL;
	IplConvKernel* morph = cvCreateStructuringElementEx(3,3,1,1,CV_SHAPE_RECT);
	CvMemStorage *g_storage = NULL;	
	std::vector<CvRect> rects;
	int numDetections = 0;

	while ((frame = cvQueryFrame(capture))!=NULL)
	{	
		//cvSmooth(frame,frame,CV_BLUR,3,3);
		cvCvtColor(frame, grayImg, CV_BGR2GRAY);
		//cvCvtColor(frame, hsv, CV_BGR2HSV);
		//cvCvtPixToPlane(hsv, 0, 0, diffImg, 0);

		cvAbsDiff(grayImg, background, diffImg);
		cvThreshold(diffImg, diffImg, 35 ,255, CV_THRESH_BINARY);

		cvErode(diffImg, diffImg, morph, 2);
		cvDilate(diffImg, diffImg, morph, 10);
		diffImgShow = cvCloneImage(diffImg);

		// process countour
		g_storage = cvCreateMemStorage(0);	

		CvSeq* first_contour = 0;
		cvFindContours( diffImg, g_storage, &first_contour );	

		for (CvSeq* c = first_contour; c != NULL; c = c->h_next)
		{
			CvRect bRect = cvBoundingRect(c);
			if (bRect.width > 15 && bRect.height > 15)
			{
				cvRectangle(frame,cvPoint(bRect.x,bRect.y),
					cvPoint(bRect.x+bRect.width,bRect.y+bRect.height),cvScalarAll(255));
				rects.push_back(bRect);
				numDetections++;

				//initialize detector configuration
				initializeDetectorConfiguration(vConfig, bRect.width, bRect.height);				

				//load detector configuration 
				if(!loadDetectorConfiguration(vConfig, vDet))
				{
					printf("Failed in loading configuration\n");
					return -1;
				}

				VehicleDetectionImage * vImage = new VehicleDetectionImage;
				cvSetImageROI(grayImg, bRect);	
				temp = cvCreateImage(cvSize(bRect.width,bRect.height),8,1);
				cvCopyImage(grayImg,temp);
				cvResetImageROI(grayImg);
				initializeImageData(temp, vImage);

				//allocate vehicle detection result
				VehicleDetectionResult * vDResult = NULL;
				if(!allocateVehicleDetectionResult(&vDResult))
				{
					printf("Failed in allocation memory to detector\n");
					return -1;
				}

				//detect
				if(!runVehicleDetection(vImage, vConfig, vDet, vDResult))
				{
					printf("Failed to run detector on this image \n");
					return -1;
				}

				// ok
				int numDetections = numVehicleDetections(vDResult);
				fprintf(saveDetectionFile, "Frame %d: %d detection responses\n", i++, numDetections);
				for (int t = 0; t < numDetections; t++)
				{
					VehicleDetection * vDetection = NULL;
					getVehicleDetection(vDResult, t, &vDetection);

					fprintf(saveDetectionFile, "        %d %d %d %d", 
						vDetection->mX, vDetection->mY,
						vDetection->mWidth,vDetection->mHeight);
					fprintf(saveDetectionFile, " %d %f %f %d\n", 
						vDetection->mIdentifier, vDetection->mConfidence,
						vDetection->mNumResponses, vDetection->isValid);

					cvRectangle(frame, 
						cvPoint(bRect.x + vDetection->mX, bRect.y + vDetection->mY), 
						cvPoint(bRect.x + vDetection->mX + vDetection->mWidth, bRect.y + vDetection->mY + vDetection->mHeight), 
						cvScalar(255,0,0), 1);
				}
				freeVehicleDetectionResult(vDResult);		
				cvReleaseImage(&temp);
				delete vImage;
			}
		}
		cvClearMemStorage(g_storage);
		fprintf(saveDetectionFile, "Frame %d: %d detection responses\n", i++, numDetections);
		for (int t = 0; t < numDetections; t++)
		{
			VehicleDetection * vDetection = NULL;

			fprintf(saveDetectionFile, "        %d %d %d %d", 
				rects[t].x, rects[t].y,
				rects[t].width,rects[t].height);
			fprintf(saveDetectionFile, " %d %f %f %d\n", 0, 0.0, 0.0, 1);
		}
		rects.clear();
		numDetections = 0;

		cvShowImage("video",frame);
		cvShowImage("foreground", diffImgShow);
		cvSetImageROI(saveImg,cvRect(0,0,frame->width,frame->height));
		cvCopyImage(frame,saveImg);
		cvResetImageROI(saveImg);

		cvCvtColor(diffImgShow,frame, CV_GRAY2BGR);
		cvSetImageROI(saveImg,cvRect(frame->width,0,frame->width,frame->height));
		cvCopyImage(frame,saveImg);
		cvResetImageROI(saveImg);
		cvWriteFrame(writer, saveImg);
		printf("frame: %d\n", i);
		
		if (cvWaitKey(1) == 27 )
		{
			break;
		}
	}
	freeVehicleDetector(vDet, vConfig);
	cvDestroyAllWindows();
	cvReleaseCapture(&capture);
	cvReleaseVideoWriter(&writer);
	fclose(saveDetectionFile);
	
	return 0;
}

void initializeDetectorConfiguration(VehicleDetectionConfiguration *&iVConfig, int width, int height)
{
	iVConfig->mconfigurationType = 0; // 0: more false alarms  2: less false alarms
	iVConfig->mIsFrameContinuous = false; // to detect contiously with image sequence input
	iVConfig->mWindowSearch      = 0;    // 1: search a window instead of whole image
	iVConfig->mWindowTopLeftX    = 0;
	iVConfig->mWindowTopLeftY    = 0;
	iVConfig->mWindowWidth       = width;
	iVConfig->mWindowHeight      = height;
	iVConfig->mCmPerPixel        = 0.12;  // the minimum size respect to 200x800, for eg, 0.12*200 = 24 0.12*800 = 96 is the minimum
}

void initializeImageData(IplImage *iImg, VehicleDetectionImage *iVImage)
{
	iVImage->mWidth  = iImg->width;
	iVImage->mHeight = iImg->height;
	iVImage->mData   = (unsigned char *)iImg->imageData;
	iVImage->mStride = iImg->widthStep;
}