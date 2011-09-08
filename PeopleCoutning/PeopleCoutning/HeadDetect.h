
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


	//CvPoint		minloc, maxloc;
	//double		minval, maxval;
	IplImage* tpl = cvLoadImage("Hair.png",CV_LOAD_IMAGE_COLOR);
	int			img_width, img_height;;
	int			tpl_width, tpl_height;
	int			res_width, res_height;

	if( image == 0 ) {
		fprintf( stderr, "Cannot load file %s!\n");
		//return 1; 
	}

	if( tpl == 0 ) {
		fprintf( stderr, "Cannot load file %s!\n");
		//return 1;
	}

	/* get image's properties */
	img_width  = image->width;
	img_height = image->height;
	tpl_width  = tpl->width;
	tpl_height = tpl->height;
	res_width  = img_width - tpl_width + 1;
	res_height = img_height - tpl_height + 1;

	/* create new image for template matching computation */
	res = cvCreateImage( cvSize( res_width, res_height ), IPL_DEPTH_32F, 1 );

	/* choose template matching method to be used */
	cvMatchTemplate( image, tpl, res, CV_TM_SQDIFF_NORMED );

	//	cvMinMaxLoc( res, &minval, &maxval, &minloc, &maxloc, 0 );


	/* wait until user press a key to exit */
	int number =getContour(rect);

	//return 0;
	return number;
};


int HeadDetect::getContour(CvRect rect)
{
	CvMemStorage* storage;
	storage = cvCreateMemStorage(0);

	cvSetImageROI(res, rect);

	IplImage* cropRes = cvCreateImage(cvGetSize(res),res->depth,res->nChannels);

	cvCopy(res,cropRes,NULL);

	IplImage* res2 = cvCreateImage( cvGetSize(res), 8, 1 );
	//IplImage *res3 = cvCreateImage( cvGetSize(res), 32, 1 );

	//cvShowImage("res trc",res);
	cvThreshold(cropRes,cropRes,0.5,1,CV_THRESH_BINARY_INV);
	//cvShowImage("res sau",res);
	cvConvertScale(res,res2);


	CvContourScanner scanner = cvStartFindContours(res2,storage,sizeof(CvContour), CV_RETR_EXTERNAL);
	contours = cvFindNextContour(scanner);

	int numb = cvFindContours(res2,storage,&contours,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

	contours = cvFindNextContour(scanner);
	cvResetImageROI(res);
	cvReleaseMemStorage(&storage);

	return numb;
}

