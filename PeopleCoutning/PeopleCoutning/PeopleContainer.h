#pragma once
#include <cv.h>
#include <vector>
#include "Histogram.h"
#include "HeadDetect.h"
#include <time.h>
#include "People.h"
//#include "LinkList.h"

class Container
{
public:
	CvMemStorage *storage;
	CvSeq *contours;
	CHist *histProcess;
	HeadDetect detectHead;

	//std::vector<int> placeHolder;
	CvFont font;
	IplImage *destinyImage;
	int avarageHumanSize;
	CvPoint *line;
	int distance1 ;
	int distance2;
	CvPoint *point;
	int countUp, countDown;
	int heigh, width;
	double sumSize;
	int sumElement;
	


	std::vector<CPeople> people;
//	LinkList pointList;
	void Container::CreatePeople();

	Container::Container();
	Container::Container(IplImage *foreground);
	~Container();

	void Container::Process(IplImage* forground,IplImage* image );
	void Container::countPeoplePass();
	void Container::DeletePeople(int i);
	void Container::matchPeople();
	void Container::DeleteHist(int i);
	void Container::getHist(IplImage *image);
	double Container::distance(CvPoint a, CvPoint b);
	void Container::initPoint();
	void QuickSort(	int startIndex, int endIndex);
	int SplitArray( int pivotValue, int startIndex, int endIndex);
	void swap(CPeople &a, CPeople &b);

};

Container::Container()
{
	
}
Container::Container(IplImage *foreground)
{
	contours = 0;
	countUp = countDown = 0;
	destinyImage = cvCreateImage(cvGetSize(foreground),8,3);
	cvInitFont(&font,CV_FONT_ITALIC, 0.5,0.5,0,1,8);
	storage = cvCreateMemStorage(0);
	sumSize = 500;
	sumElement =1;
	line = new CvPoint[6];
	distance1 = 7;
	distance2 = 60;
	
	

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
	line[1].y=heigh/2;line[1].x=width;

	line[2].y=heigh/2-distance1;line[2].x=0;
	line[3].y=heigh/2+distance1;line[3].x=width;

	line[4].y=heigh/2-distance2;line[4].x=0;
	line[5].y=heigh/2+distance2;line[5].x=width;
	
	
	
	
	//avarageHumanSize = 1200;
}
/*void Container::initialAverageSize()
{
CvContourScanner scanner = cvStartFindContours(image, storage, sizeof(CvContour), CV_RETR_EXTERNAL);

contours = cvFindNextContour(scanner);

CvSeq * test = contours;


while (test != NULL)
{
//double contourSize = 0;
tempSize+= cvContourArea(contours, CV_WHOLE_SEQ, 0);
contours =	cvFindNextContour(scanner);
test = contours;
contours->h_next;
}
contours = cvEndFindContours(&scanner);
test = contours;
total = contours->elem_size;
avarageHumanSize = tempSize/total;

}*/
void Container::Process(IplImage* forground,IplImage* image)

{

	cvCopyImage(image, destinyImage);

	CvContourScanner scanner = cvStartFindContours(forground, storage, sizeof(CvContour), CV_RETR_EXTERNAL);
	contours = cvFindNextContour(scanner);

	int i = 0;
	CvSeq * test = contours;
	int numberInside =1;

	//avarageHumanSize = contourSize/cvGetSize(contours).width;
	while (test!=NULL)
	{
	/*	double contourSize = cvContourArea(contours, CV_WHOLE_SEQ, 0);
		if (contourSize>100)
		{
			sumElement += 1;
			sumSize +=  contourSize;
			avarageHumanSize = sumSize/sumElement;
		}
		if (contourSize < avarageHumanSize/3)
		{
			test = 	cvFindNextContour(scanner);

			//test = contours;
		}
		else if ((avarageHumanSize*2)<contourSize)
		{
			// 			if (people.empty())
			// 				CreatePeople();
			// 		
			// 			if (people.size()<i)
			// 		
			// 				CreatePeople();

			//numberInside = 2;
			numberInside = detectHead.getHairColor(cvBoundingRect(contours));
			goto N;
		}
		else
		{
N:
			if (people.size()<=i)
				CreatePeople();

			people[i].rect = cvBoundingRect(contours);
			people[i].axis.x = (people[i].rect.x+people[i].rect.width);
			people[i].axis.y = (people[i].rect.y+people[i].rect.height/2);
			//people[i].numberInside = numberInside;
			people[i].numberInside = detectHead.getHairColor(people[i].rect);
			people[i].contour = contours;
		*/	
			//contours =	cvFindNextContour(scanner);
			contours = contours->h_next;

			contours =	cvFindNextContour(scanner);
			test = contours;
			++i;
		}
	//}
	while (i< people.size())
	{
		DeletePeople(i);
		++i;
	}
	i=0;
	contours = cvEndFindContours(&scanner);
	cvDrawContours( destinyImage, contours, CV_RGB(255,0,0), CV_RGB(0,255,0),1, 1, CV_AA, cvPoint(0,0) );
	cvShowImage("contour", destinyImage);
	//matchPeople();
	//countPeoplePass();
}

void Container::getHist(IplImage *image)
{
	for (int i = 0; i < people.size(); ++i)
	{
		histProcess->initHSVImage(image);
		people[i].hist = histProcess->computeHist(image, people[i].rect);
	}
}
/*void Container::initHist(IplImage *image)
{
for (int i = 0; i < people.size(); ++i)
{
histProcess->initHSVImage(image);
people[i].hist = histProcess->computeHist(image, people[i].rect);
PreviousHist.push_back(*people[i].hist);
}

}
*/

void Container::initPoint()
{
	int size = people.size();
	for (int i = 0; i < size; ++i)
	{
//		pointList.append(people[i].axis);
		people[i].pos = i;
	}
}
/*
void Container::matchPeople()
{ 

	int index =  0;
	double dis;
	double min=100;
	int intArray[30];
	int size = people.size();
	for (int i = 0; i < size;++i)
	{
		if (pointList.head == NULL)
		{
			initPoint();
			break;
		}
		pointList.current=pointList.head;
		min = distance(pointList.current->data, people[i].axis);
		int t = 0;
		if (pointList.current!=NULL)
		{
		while (pointList.current->link != NULL)
		{
			++t;
			pointList.current = pointList.current->link;
			dis = distance(pointList.current->data, people[i].axis);
			if (min>dis)
			{
				min = dis;
				index = t;
				pointList.match = pointList.current;
			}
		}
		}

		if (min < 5)		
		{
			people[i].pos= index;
			pointList.match->match=true;
			
		}	
		else
		{
			pointList.append(people[i].axis);
			people[i].pos = t+1;
		}
	}

	pointList.delNotMatchNode();
	pointList.resetMatchValue();
	
	QuickSort(0,people.size()-1);
	int a;
}
*/
double Container::distance(CvPoint a, CvPoint b)
{
	double i;
	i = (a.x - b.x)*(a.x - b.x) + (a.y-b.y)*(a.y-b.y);
	return sqrt(i);
}
/*void Container::matchPeople(IplImage *image)
{ 
int min =100;
int index;
int distance;
std::vector<CPeople> temp;

for (int i = 0; i < PreviousHist.size();++i)
{
if (PreviousHist[i].bins!=NULL)
for (int t = 0; t < people.size();++t)
{
distance =  histProcess->distance(&(PreviousHist[i]),people[t].hist);
if (min>distance)
{
min = distance;
index = i;
}
}
if (min < 0.8)
{
temp[i] = people[i];
CvHistogram *hist;
cvCopyHist(people[i].hist, &hist);
PreviousHist[i] = *hist;
DeletePeople(i);
}
else
PreviousHist[i].bins = 0;

min = 100;
}

if (people.size()>0)
{
int t = 0;
for (int i = 0; i < people.size(); ++i)
{

while (PreviousHist[t].bins != NULL)
++t;

CvHistogram *hist;
cvCopyHist(people[i].hist, &hist);
PreviousHist[t] = *hist;
++t;

temp[t] = people[i];
DeletePeople(i);
}
}
people = temp;
temp.clear();
}
*/

void Container::countPeoplePass()
{
	/*
	cvMoments(contours, &moment, 0);
	people[i].axis.x = moment.m01/moment.m00;
	people[i].axis.y = moment.m10/moment.m00;
	*/
	char countUpChar[5], countDownChar[5], output[20];

	for (int i = 0; i < people.size(); ++i)
	{
		if (line[2].y<people[i].axis.y &&  people[i].axis.y< line[3].y 
			&& line[2].x<people[i].axis.x && people[i].axis.x< line[3].x
			&& people[i].flag==0)
		{
			if (people[i].side == true)
			countDown =countDown + people[i].numberInside;
			else
				countUp = countUp + people[i].numberInside;
			people[i].flag = 1;
		}
		if ((people[i].axis.y<line[4].y || line[5].y<people[i].axis.y)
			&& people[i].flag==1)
		{
			people[i].flag = 0;
			//DeletePeople(i);
		}
		if  ((people[i].axis.y<line[4].y )
			&& people[i].flag==0)
			people[i].side = true;
		else if ((line[5].y<people[i].axis.y )
			&& people[i].flag==0)
			people[i].side = false;


		itoa(countUp,countUpChar,10);
		itoa(countDown,countDownChar,10);
		strcpy(output, "Up:");
		strcat(output, countUpChar);
		strcat(output,"  Down:" );
		strcat(output, countDownChar);
		
		cvDrawContours( destinyImage, contours, CV_RGB(255,0,0), CV_RGB(0,255,0),1, 1, CV_AA, cvPoint(0,0) );
		cvLine(destinyImage, people[i].axis, people[i].axis, cvScalar(0,255,255), 2, 8, 0);
		cvPutText(destinyImage, output, cvPoint(width/10, heigh/10),&font , cvScalar(0,255,0));
		
	}		
	cvLine(destinyImage, line[0],line[1],cvScalar(255,0,0),1,8,0);
	//cvShowImage("contour", destinyImage);
	//cvZero(destinyImage);
}

void Container::CreatePeople()
{
	CPeople unit;
	people.push_back(unit);
}

void Container::DeletePeople(int i)
{
	people.erase(people.begin() + i);
}

/*void Container::DeleteHist(int i)
{
	PreviousHist.erase(PreviousHist.begin() +i);
}*/

Container::~Container()
{
	cvReleaseImage(&destinyImage);
	cvReleaseMemStorage(&storage);
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
		int pivot = people[startIndex].pos;
	int splitPoint;
		
		splitPoint = SplitArray(pivot, startIndex,endIndex);
		people[splitPoint].pos = pivot;
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
		while (pivotValue<people[rightBoundary].pos && rightBoundary > leftBoundary)
		{
			rightBoundary--;
		}
		swap(people[leftBoundary],people[rightBoundary]);
		while (pivotValue >= people[leftBoundary].pos && leftBoundary < rightBoundary)
		{
			leftBoundary++;
		}
		swap(people[rightBoundary], people[leftBoundary]);
	}
	return leftBoundary;
}