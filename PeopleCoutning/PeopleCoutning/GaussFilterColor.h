#include "cv.h"
#include "highgui.h"
#include <fstream>
using namespace std;

//#define PI 3.14

#pragma once

class GaussFilterColor
{
public:	
	double InvCov[2][2];
	
	double sub[3];
	double rsl[3];
	double b;

	double phi;
	double logOfSqrt2PiVariance;
	double threshold;

	CvMat* mean;
	CvMat* corrvariant;

	void TrainData(char *prefix, char *suffix, int number_images, int start_index, int end_index, char* fileOutput);
	void LoadData(char* fileOutput);
	//IplImage* Classify(IplImage *imgColor, IplImage *mask);
	IplImage* Classify(IplImage *imgColor,  float thresh);
	//IplImage* Classify(IplImage *imgColor);
	void SetThreshold(double threshold);

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
	
	GaussFilterColor(void);
	~GaussFilterColor(void);
};

#include "GaussFilterColor.h"

GaussFilterColor::GaussFilterColor(void)
{
	threshold = -6.5;	
}

GaussFilterColor::~GaussFilterColor(void)
{
	cvReleaseMat(&mean);
	cvReleaseMat(&corrvariant);
}

void GaussFilterColor::SetThreshold(double thresh)
{
	this->threshold = thresh;
}


IplImage* GaussFilterColor::Classify(IplImage *img,  float thresh)
{
	IplImage* gray = cvCreateImage(cvGetSize(img), img->depth, 1);

	//Convert To Hsv Color System
	IplImage* imgHsv = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);

	//cvSmooth(img, imgHsv, CV_MEDIAN, 5, 5);
	//cvCopy(imgHsv, img);

	cvCvtColor(img, imgHsv, CV_BGR2HSV);
	
	//Inverse Covariance.	
	InvCov[0][0] = 1/ (*(float *)corrvariant->data.ptr);	
	InvCov[1][1] = 1/ *((float *)(corrvariant->data.ptr + corrvariant->step + sizeof(corrvariant->type)));
	
	for(int i = 0; i < imgHsv->height; i++)
	{
		//char* ptrMaskTmp = mask->imageData + i*mask->widthStep;
		uchar* ptrTmp = (uchar*)(imgHsv->imageData + i*imgHsv->widthStep);
		uchar* ptrGrayTmp = (uchar*)(gray->imageData + i*gray->widthStep);
		
		for(int j = 0; j < imgHsv->width; j++)
		{			
			//char* ptrMask = ptrMaskTmp + j;
			uchar* ptrGray = ptrGrayTmp + j;

		//	if(ptrMask[0] == 0){
				//Read 2 Images Pixel				
				uchar* ptr = ptrTmp + (j*imgHsv->nChannels);//Read HSV
				
				sub[0] = ptr[1] - *(float *)(mean->data.ptr); // (*(float*)CV_MAT_ELEM_PTR(*mean, 0, 0));
				sub[1] = ptr[2] - *(float *)(mean->data.ptr + mean->step);//(*(float*)CV_MAT_ELEM_PTR(*mean, 1, 0));

				rsl[0] = sub[0]*InvCov[0][0];
				rsl[1] = sub[1]*InvCov[1][1];

				b = rsl[0]*sub[0] + rsl[1]*sub[1];
				b = -0.5*b;

				double temp = logOfSqrt2PiVariance + b;
				
				if (temp > thresh)
				{				
					*ptrGray = 255;
				}
				else
				{
					*ptrGray = 0;
				}
		//	}
			//else
				//*ptrGray = 0;
		}
	}
				
	cvReleaseImage(&imgHsv);
	return gray;
}



void GaussFilterColor::LoadData(char* fileOutput)
{
	ifstream fin;
	fin.open(fileOutput);
	if(fin.is_open())
	{
		mean = cvCreateMat(2, 1, CV_32FC1);
		corrvariant = cvCreateMat(2, 2, CV_32FC1);

		//read mean
		fin>>(*(float*)CV_MAT_ELEM_PTR(*mean, 0, 0))>>(*(float*)CV_MAT_ELEM_PTR(*mean, 1, 0));
		//read covarriance
		fin>>(*(float*)CV_MAT_ELEM_PTR(*corrvariant, 0, 0))
			>>(*(float*)CV_MAT_ELEM_PTR(*corrvariant, 0, 1))
			>>(*(float*)CV_MAT_ELEM_PTR(*corrvariant, 1, 0))
			>>(*(float*)CV_MAT_ELEM_PTR(*corrvariant, 1, 1));

		fin.close();
	}

	double xyCorv = sqrt((*(float*)CV_MAT_ELEM_PTR(*corrvariant, 0, 0))*(*(float*)CV_MAT_ELEM_PTR(*corrvariant, 1, 1)));
	phi = (*(float*)CV_MAT_ELEM_PTR(*corrvariant, 0, 1))/xyCorv;
	double temp = 2*3.14*sqrt(xyCorv);
	logOfSqrt2PiVariance = log(1.0/temp);
}

void GaussFilterColor::TrainData(char *prefix, char *suffix, int number_images, int start_index, int end_index, char* fileOutput)
{
	unsigned char *pImg;
	unsigned char *pHsv;

	unsigned char *p;//Pixel of Img
	unsigned char *q;//Pixel of ImgHsv

	int row;
	int col;
	int k;

	//s, v
	CvMat *mean;	
	CvMat *corrvariant;

	mean = cvCreateMat(2, 1, CV_32FC1);
	corrvariant = cvCreateMat(2, 2, CV_32FC1);

	double sumS = 0;
	double sumV = 0;
	double total = 0;

	char filename[200] = "\0", number[8];
	int prefix_length;
	prefix_length = strlen(prefix);
 
	int i = 0, j = 0;
 
	strcat(filename, prefix);
	IplImage *imgHSV;
	IplImage *img;

	//train means
	for (i = start_index; i <= end_index; i++) 
	{
		itoa(i,number,10);
		strcat(filename, number);
		strcat(filename, suffix);
		
		img = cvLoadImage(filename);
			
		//Convert To Hsv Color System
		imgHSV = cvCloneImage(img);
		cvSmooth(img, imgHSV, CV_MEDIAN, 5, 5);
		cvCopy(imgHSV, img);
		cvCvtColor(img, imgHSV, CV_RGB2HSV);

		//compute mean for h and s
		for(row = 0; row < img->height; row++)
		{
			for(col = 0; col < img->width; col++)
			{
				uchar* data = &CV_IMAGE_ELEM(imgHSV, uchar, row, col*3);	//get hsv
				
				int s = data[1];
				int v = data[2];
				
				sumS += s;
				sumV += v;

				total++;
			}
		}

		cvReleaseImage(&imgHSV);
		cvReleaseImage(&img);
		filename[prefix_length] = '\0';
	}

	sumS = 1.0*sumS/total;
	sumV = 1.0*sumV/total;

	(*(float*)CV_MAT_ELEM_PTR(*mean, 0, 0)) = sumS;
	(*(float*)CV_MAT_ELEM_PTR(*mean, 1, 0)) = sumV;
		
	//train covarriance		
		
	CvMat *color = cvCreateMat(2, 1, CV_32FC1);
	CvMat *C_Sub_Mean = cvCloneMat(color);
	CvMat *C_Sub_Mean_T = cvCreateMat(1, 2, CV_32FC1);	
	CvMat *tmp = cvCreateMat(2, 2, CV_32FC1);
	cvZero(corrvariant);
	for (i = start_index; i <= end_index; i++) 
	{
		itoa(i,number,10);
		strcat(filename, number);
		strcat(filename, suffix);
		
		img = cvLoadImage(filename);
			
		//Convert To Hsv Color System
		imgHSV = cvCloneImage(img);
		cvSmooth(img, imgHSV, CV_MEDIAN, 5, 5);
		cvCopy(imgHSV, img);
		cvCvtColor(img, imgHSV, CV_RGB2HSV);	
		
		for(row = 0; row < img->height; row++)
		{
			for(col = 0; col < img->width; col++)
			{
				uchar* data = &CV_IMAGE_ELEM(imgHSV, uchar, row, col*3);	//get hsv
				
				int s = data[1];
				int v = data[2];
				
				(*(float*)CV_MAT_ELEM_PTR(*color, 0, 0)) = s;
				(*(float*)CV_MAT_ELEM_PTR(*color, 1, 0)) = v;
				
				cvSub(color, mean, C_Sub_Mean);
				
				cvTranspose(C_Sub_Mean, C_Sub_Mean_T);			
				
				
				cvMatMul(C_Sub_Mean, C_Sub_Mean_T, tmp);
				cvAdd(tmp, corrvariant, corrvariant);
			}
		}

		cvReleaseImage(&imgHSV);
		cvReleaseImage(&img);
		filename[prefix_length] = '\0';
	}
	
	cvScale(corrvariant, corrvariant, (1.0/(total-1)));
	
	ofstream file;
	file.open(fileOutput);
	if(file.is_open())
	{
		//write mean
		file<<(*(float*)CV_MAT_ELEM_PTR(*mean, 0, 0))<<' '<<(*(float*)CV_MAT_ELEM_PTR(*mean, 1, 0))<<endl;
		//write covarriance
		file<<(*(float*)CV_MAT_ELEM_PTR(*corrvariant, 0, 0))<<' '
			<<(*(float*)CV_MAT_ELEM_PTR(*corrvariant, 0, 1))<<' '
			<<(*(float*)CV_MAT_ELEM_PTR(*corrvariant, 1, 0))<<' '
			<<(*(float*)CV_MAT_ELEM_PTR(*corrvariant, 1, 1))<<endl;
		file.close();
	}

	cvReleaseMat(&mean);
	cvReleaseMat(&corrvariant);
	cvReleaseMat(&C_Sub_Mean);
	cvReleaseMat(&C_Sub_Mean_T);
	cvReleaseMat(&tmp);
	cvReleaseMat(&color);

	cvReleaseImage(&img);
	cvReleaseImage(&imgHSV);
}
