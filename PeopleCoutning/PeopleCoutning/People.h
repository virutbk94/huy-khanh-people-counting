
class CPeople
{
public:
	CvPoint axis;
	int numberInside;
	bool side;
	CvRect rect;
	int pos;

	CvSeq *contour;
	bool flag;
	CPeople();
	~CPeople();

CvHistogram *hist;
//CHist *chist;
	int lable;

//void CPeople::AddPeople(CvSeq *contours);
void getRect()
{
	rect = cvBoundingRect(contour,0);
}
/*CvHistogram * getHist(IplImage *image, CvRect rect)
{
	
	return chist->computeHist(image, rect);
}
*/
};
CPeople::CPeople()
{
	//CvSize size = cvGetSize(image);
	//chist = new CHist(size);
	//chist->initHSVImage(image);
	//hist = chist->computeHist(image, rect);
	//getHist(image);
	numberInside = 1;
	flag = 0;

}

CPeople::~CPeople()
{

}

