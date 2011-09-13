#pragma once
#include <cv.h>
#include <vector>
//#include "Histogram.h"
#include "People.h"

#define  avarageHumanSize 1000

typedef struct linklist_data
{

	CvPoint point;
	int pos;
	bool flag;
	bool isIn;
	bool side;

	linklist_data()
	{
		flag = false;
		isIn = false;
	}
	~linklist_data()
	{

	}
} DATA;

class Container
{
public:
	CvMemStorage *storageA, *storageB;
	CvSeq *contours;
	//CHist *histProcess;
	//HeadDetect detectHead;

	CvFont font;
	CvPoint *line;
	int distance1 ;
	int distance2;
	CvPoint *point;
	int countUp, countDown;
	char countUpChar[5], countDownChar[5], output[20], frameCountChar[5];
	int frame_count;
	int heigh, width;
	double sumSize;
	int sumElement;



	std::vector<CPeople> pBaseHeadColors, pBaseBGSubs;
	std::vector<CvRect> rects;
	std::vector<DATA> previous_pos;
	//linklist previous_pos;
	void Container::CreatePeople();

	Container::Container();
	Container::Container(IplImage *foreground);
	~Container();

	void Container::Process(IplImage* forgroundColorDectect,IplImage *forgroundBackgroundSubtract, IplImage* image);
	void Container::countPeoplePass(IplImage *image);
	void Container::DeletePeople(int i);
	void Container::matchPeople();
	void Container::DeleteHist(int i);
	void Container::getHist(IplImage *image);
	double Container::distance(CvPoint a, CvPoint b);
	void Container::initPoint();
	void QuickSort(	int startIndex, int endIndex);
	int SplitArray( int pivotValue, int startIndex, int endIndex);
	void swap(CPeople &a, CPeople &b);
	bool isInsideRect(CvPoint point, CvRect rect)
	{
		if (rect.x < point.x && point.x < (rect.x+ rect.width) && rect.y < point.y && point.y < (rect.y +rect.height))
			return true;
		else
			return false;
	}

};
Container::Container()
{

}
Container::Container(IplImage *foreground)
{
	contours = 0;
	countUp = countDown = 0;
	cvInitFont(&font,CV_FONT_ITALIC, 0.5,0.5,0,1,8);
	storageA = cvCreateMemStorage(0);
	storageB = cvCreateMemStorage(0);
	sumSize = 500;
	sumElement =1;
	line = new CvPoint[6];
	distance1 =	30;
	distance2 = 60;

	itoa(countUp,countUpChar,10);
	frame_count =0;

	heigh = foreground->height;
	width = foreground->width;

	//CHist *hist;
	//initial line

	// 	for (int i = 1; i <100; ++i)
	// 	{
	// 		placeHolder[i]=i;
	// 	}

	//	for (int i=0;i<20;++i)
	//		placeHolder.push_back(i);

	line[0].y=heigh/2;line[0].x=0;
	line[1].y=heigh/2;line[1].x=width/2;

	line[2].y=heigh/2-distance1;line[2].x=0;
	line[3].y=heigh/2+distance1;line[3].x=width/2;

	line[4].y=heigh/2-distance2;line[4].x=0;
	line[5].y=heigh/2+distance2;line[5].x=width/2;

	//avarageHumanSize = 1200;
}
/*void Container::initialAverageSize()
{
CvContourScanner scanner = cvStartFindContours(image, storageA, sizeof(CvContour), CV_RETR_EXTERNAL);

contours = cvFindNextContour(scanner);

CvSeq * first_contour = contours;


while (first_contour != NULL)
{
//double contourSize = 0;
tempSize+= cvContourArea(contours, CV_WHOLE_SEQ, 0);
contours =	cvFindNextContour(scanner);
first_contour = contours;
contours->h_next;
}
contours = cvEndFindContours(&scanner);
first_contour = contours;
total = contours->elem_size;
avarageHumanSize = tempSize/total;

}*/
void Container::Process(IplImage* forgroundColorDectect,IplImage *forgroundBackgroundSubtract, IplImage* image)
{

	//cvCopyImage(image, destinyImage);
	CvSeq* first_contour_A = 0;
	CvSeq* first_contour_B = 0;
	cvFindContours( forgroundColorDectect, storageA, &first_contour_A );

	++frame_count;

	for (CvSeq* c = first_contour_A; c != NULL; c = c->h_next)
	{
		CvRect bRect = cvBoundingRect(c);

		if (bRect.width > 40 && bRect.height > 40)
		{
			cvRectangle(image,cvPoint(bRect.x,bRect.y),
				cvPoint(bRect.x+bRect.width,bRect.y+bRect.height),cvScalar(0,255,0));
			CPeople pBaseHeadColor;

			pBaseHeadColor.rect = bRect;
			pBaseHeadColor.axis.x = (bRect.x+bRect.width/2);
			pBaseHeadColor.axis.y = (bRect.y+bRect.height/2);
			pBaseHeadColor.contour = c;
			pBaseHeadColor.flag = false;

			pBaseHeadColors.push_back(pBaseHeadColor);
		}
	}

	cvFindContours(forgroundBackgroundSubtract, storageB, &first_contour_B);
	for (CvSeq* c = first_contour_B; c != NULL; c = c->h_next)
	{
		CvRect bRect = cvBoundingRect(c);
		bool isAlone = true;

		if (bRect.width > 30 && bRect.height > 30)
		{
			for (int i = 0; i < pBaseHeadColors.size() && isAlone == true; i++)
				if (isInsideRect(pBaseHeadColors[i].axis, bRect))
					isAlone = false;
			if (isAlone == true)		
			{
				cvRectangle(image,cvPoint(bRect.x,bRect.y),
					cvPoint(bRect.x+bRect.width,bRect.y+bRect.height),cvScalarAll(255));
				CPeople pBaseBGSub;

				pBaseBGSub.rect = bRect;
				pBaseBGSub.axis.x = (bRect.x+bRect.width/2);
				pBaseBGSub.axis.y = (bRect.y+bRect.height/2);
				pBaseBGSub.contour = c;
				pBaseBGSub.flag = false;

				pBaseBGSubs.push_back(pBaseBGSub);
			}
		}
	}

	for (int i = 0; i < pBaseBGSubs.size(); i++)
	{
		pBaseHeadColors.push_back(pBaseBGSubs[i]);
	}

	matchPeople();
	countPeoplePass(image);
	cvClearMemStorage(storageA);
	cvClearMemStorage(storageB);
	pBaseHeadColors.clear();
	pBaseBGSubs.clear();
}
/*void Container::getHist(IplImage *image)
{
for (int i = 0; i < pBaseHeadColors.size(); ++i)
{
histProcess->initHSVImage(image);
pBaseHeadColors[i].hist = histProcess->computeHist(image, pBaseHeadColors[i].rect);
}
}*/
/*void Container::initHist(IplImage *image)
{
for (int i = 0; i < pBaseHeadColors.size(); ++i)
{
histProcess->initHSVImage(image);
pBaseHeadColors[i].hist = histProcess->computeHist(image, pBaseHeadColors[i].rect);
PreviousHist.push_back(*pBaseHeadColors[i].hist);
}

}
*/



void Container::matchPeople()
{ 
	int index =  0;
	double dis;
	double min_distance = 1000; //temporary min distance
	double mDistance;
	DATA tmpData;
	bool isMatch = false;
	int intArray[30];
	int people_size = pBaseHeadColors.size();
	int previous_pos_size = previous_pos.size();
	int t =0;

	//first match with old data position
	for (int m = 0; m < people_size && previous_pos_size>0;++m)
	{
		for (int n=0; n<previous_pos_size;n++)
		{
			mDistance = distance(previous_pos[n].point, pBaseHeadColors[m].axis);
			if (min_distance>mDistance)
			{
				min_distance = mDistance;
				tmpData.point = pBaseHeadColors[m].axis;
				tmpData.pos = n;
				//tmpData.isIn = false;
			}
		}
		if (min_distance < 40)
		{
			previous_pos[tmpData.pos].point=tmpData.point;
			previous_pos[tmpData.pos].flag=true;
			//pBaseHeadColors[m].pos = tmpData.pos;
			pBaseHeadColors[m].flag =true;
		}
		min_distance = 1000; //temporary min distance
	}

	//second match, new people got the way to store position in previous_pos list
	for (int m = 0; m < people_size;++m)
	{
		if (pBaseHeadColors[m].flag==false)
		{
			if (previous_pos_size==0)
			{
				previous_pos_size++;
				tmpData.point = pBaseHeadColors[m].axis;
				tmpData.pos = 0;
				tmpData.flag =true;
				tmpData.isIn = false;
				previous_pos.push_back(tmpData);
				isMatch = true;
			}
			for (int n=0; n<previous_pos_size && isMatch ==false;n++)
			{

				if (previous_pos[n].flag==false)
				{
					previous_pos[n].point = pBaseHeadColors[m].axis;
					previous_pos[n].flag =true;
					previous_pos[n].isIn = false;
					isMatch = true;
				}
			}
			if (!isMatch)
			{
				tmpData.point = pBaseHeadColors[m].axis;
				tmpData.pos = previous_pos_size;
				tmpData.flag =true;
				tmpData.isIn = false;
				previous_pos.push_back(tmpData);
				previous_pos_size++;
			}
			isMatch = false;
		}
	}

	//clear un-match and reset match status to fail
	for (int n=0; n<previous_pos_size;n++)
	{
		if (previous_pos[n].flag == false)
		{
			previous_pos.erase(previous_pos.begin()+n);
			previous_pos_size--;
			n--;
		}
		else
		{
			previous_pos[n].flag = false;
		}
	}
}

double Container::distance(CvPoint a, CvPoint b)
{
	double i;
	i = (a.x - b.x)*(a.x - b.x) + (a.y-b.y)*(a.y-b.y);
	return sqrt(i);
}
//void Container::matchPeople(IplImage *image)
//{ 
//int min =100;
//int index;
//int distance;
//std::vector<CPeople> temp;
//
//for (int i = 0; i < PreviousHist.size();++i)
//{
//if (PreviousHist[i].bins!=NULL)
//for (int t = 0; t < pBaseHeadColors.size();++t)
//{
//distance =  histProcess->distance(&(PreviousHist[i]),pBaseHeadColors[t].hist);
//if (min>distance)
//{
//min = distance;
//index = i;
//}
//}
//if (min < 0.8)
//{
//temp[i] = pBaseHeadColors[i];
//CvHistogram *hist;
//cvCopyHist(pBaseHeadColors[i].hist, &hist);
//PreviousHist[i] = *hist;
//DeletePeople(i);
//}
//else
//PreviousHist[i].bins = 0;
//
//min = 100;
//}
//
//if (pBaseHeadColors.size()>0)
//{
//int t = 0;
//for (int i = 0; i < pBaseHeadColors.size(); ++i)
//{
//
//while (PreviousHist[t].bins != NULL)
//++t;
//
//CvHistogram *hist;
//cvCopyHist(pBaseHeadColors[i].hist, &hist);
//PreviousHist[t] = *hist;
//++t;
//
//temp[t] = pBaseHeadColors[i];
//DeletePeople(i);
//}
//}
//pBaseHeadColors = temp;
//temp.clear();
//}


void Container::countPeoplePass(IplImage *image)
{
	/*
	cvMoments(contours, &moment, 0);
	previous_pos[i].point.x = moment.m01/moment.m00;
	previous_pos[i].point.y = moment.m10/moment.m00;
	*/


	for (int i = 0; i < previous_pos.size(); i++)
	{
		/*if (line[2].y<previous_pos[i].point.y &&  previous_pos[i].point.y< line[3].y 
		&& line[2].x<previous_pos[i].point.x && previous_pos[i].point.x< line[3].x
		&& previous_pos[i].isIn==false)
		*/
		if (line[2].y<previous_pos[i].point.y &&  previous_pos[i].point.y< line[3].y && previous_pos[i].isIn==false)

		{
			//	if (pBaseHeadColors[i].side == true)
			//	countDown =countDown + pBaseHeadColors[i].numberInside;
			//	else
			countUp = countUp + 1;
			previous_pos[i].isIn = true;
		}
		if ((previous_pos[i].point.y<line[4].y || line[5].y<previous_pos[i].point.y) && previous_pos[i].isIn==true)
		{
			previous_pos[i].isIn = false;
			//DeletePeople(i);
		}
		/*if  ((previous_pos[i].point.y<line[4].y )
		&& pBaseHeadColors[i].flag==0)
		pBaseHeadColors[i].side = true;
		else if ((line[5].y<previous_pos[i].point.y )
		&& pBaseHeadColors[i].flag==0)
		pBaseHeadColors[i].side = false;
		*/

		itoa(countUp,countUpChar,10);
		itoa(previous_pos[i].pos,countDownChar,10);
		itoa(frame_count,frameCountChar,10);
		strcpy(output, "Pass:");
		strcat(output, countUpChar);
		strcat(output,"  Frame:" );
		strcat(output, frameCountChar);

		//cvDrawContours( destinyImage, contours, CV_RGB(255,0,0), CV_RGB(0,255,0),1, 1, CV_AA, cvPoint(0,0) );
		//cvLine(image, previous_pos[i].point, previous_pos[i].point, cvScalar(0,255,255), 2, 8, 0);

		cvPutText(image, countDownChar, cvPoint(previous_pos[i].point.x,previous_pos[i].point.y),&font , cvScalar(0,255,0));
	}		
	cvPutText(image, output, cvPoint(width/10, heigh/10),&font , cvScalar(0,255,0));
	cvLine(image, line[0],line[1],cvScalar(255,0,0),1,8,0);
	//cvShowImage("contour", destinyImage);
	//cvZero(destinyImage);

}
void Container::CreatePeople()
{
	CPeople unit;
	pBaseHeadColors.push_back(unit);
}
void Container::DeletePeople(int i)
{
	pBaseHeadColors.erase(pBaseHeadColors.begin() + i);
}
/*void Container::DeleteHist(int i)
{
PreviousHist.erase(PreviousHist.begin() +i);
}*/

Container::~Container()
{
	cvReleaseMemStorage(&storageA);
	cvReleaseMemStorage(&storageB);
}
void Container::swap(CPeople &a, CPeople &b)
{
	CPeople temp;
	temp = a;
	a = b;
	b = temp;
}
void Container::QuickSort(int startIndex, int endIndex)
{
	if (endIndex>startIndex)
	{
		int pivot = pBaseHeadColors[startIndex].pos;
		int splitPoint;

		splitPoint = SplitArray(pivot, startIndex,endIndex);
		pBaseHeadColors[splitPoint].pos = pivot;
		QuickSort( startIndex, splitPoint -1);
		//		if (splitPoint+1 < endIndex)
		QuickSort(splitPoint+1, endIndex);
	}

}
int Container::SplitArray( int pivotValue, int startIndex, int endIndex)
{
	int leftBoundary = startIndex;
	int rightBoundary = endIndex;
	while (leftBoundary<rightBoundary)
	{
		while (pivotValue<pBaseHeadColors[rightBoundary].pos && rightBoundary > leftBoundary)
		{
			rightBoundary--;
		}
		swap(pBaseHeadColors[leftBoundary],pBaseHeadColors[rightBoundary]);
		while (pivotValue >= pBaseHeadColors[leftBoundary].pos && leftBoundary < rightBoundary)
		{
			leftBoundary++;
		}
		swap(pBaseHeadColors[rightBoundary], pBaseHeadColors[leftBoundary]);
	}
	return leftBoundary;
}