#include "cv.h"
#include "highgui.h"
#include "math.h"
#include"common.h"
#include <limits>

const int MAX_COUNT = 500;
const int MAX_IMG   = 2;
int win_size = 4;
CvPoint2D32f* points[3] = {0,0,0};
static IplImage **IMG = 0;
static IplImage **PYR = 0;

namespace TLD{
	void loadImageFromMat(Mat m, IplImage *image) {
		*image=m;
	}

	void euclideanDistance (CvPoint2D32f *point1, CvPoint2D32f *point2, float *match, int nPts) {

		for (int i = 0; i < nPts; i++) {

			match[i] = sqrt((point1[i].x - point2[i].x)*(point1[i].x - point2[i].x) + 
			(point1[i].y - point2[i].y)*(point1[i].y - point2[i].y) );

		}
	}

	void normCrossCorrelation(IplImage *imgI, IplImage *imgJ, CvPoint2D32f *points0, CvPoint2D32f *points1, int nPts, char *status, float *match,int winsize, int method) {


		IplImage *rec0 = cvCreateImage( cvSize(winsize, winsize), 8, 1 );
		IplImage *rec1 = cvCreateImage( cvSize(winsize, winsize), 8, 1 );
		IplImage *res  = cvCreateImage( cvSize( 1, 1 ), IPL_DEPTH_32F, 1 );

		for (int i = 0; i < nPts; i++) {
			if (status[i] == 1) {
				cvGetRectSubPix( imgI, rec0, points0[i] );
				cvGetRectSubPix( imgJ, rec1, points1[i] );
				cvMatchTemplate( rec0,rec1, res, method );
				match[i] = ((float *)(res->imageData))[0]; 

			} else {
				match[i] = 0.0;
			}
		}
		cvReleaseImage( &rec0 );
		cvReleaseImage( &rec1 );
		cvReleaseImage( &res );

	}

	void lk(int mode){
		if (mode==0){
			if (IMG!=0 && PYR!=0) {

					for (int i = 0; i < MAX_IMG; i++) {
						cvReleaseImage(&(IMG[i])); IMG[i] = 0;
						cvReleaseImage(&(PYR[i])); PYR[i] = 0;
					}
					free(IMG); IMG = 0;
					free(PYR); PYR = 0;
					//mexPrintf("LK: deallocated\n");
				}

				IMG = (IplImage**) calloc(MAX_IMG,sizeof(IplImage*));
				PYR = (IplImage**) calloc(MAX_IMG,sizeof(IplImage*));
				//mexPrintf("LK: initialized\n");

				return;
		}
	}

	Mat lk(int mode, Mat imgI,Mat imgJ,Mat ptsI,Mat ptsJ,int Level=5)
	{

		float nan = std::numeric_limits<float>::quiet_NaN();
		float inf = std::numeric_limits<float>::infinity();
	
		if (mode==1){
			if (IMG == 0 ) {
					printf("lk(2,imgI,imgJ,ptsI,ptsJ,Level)\n");
				//            0 1    2    3    4   
				return Mat();
			}

			int I       = 0;
			int J       = 1;
			int Winsize = 10;

			// Images
			if (IMG[I] != 0) {
				loadImageFromMat(imgI,IMG[I]);
			} else {
				CvSize imageSize = cvSize(imgI.rows,imgI.cols);
				IMG[I] = cvCreateImage( imageSize, 8, 1 );
				PYR[I] = cvCreateImage( imageSize, 8, 1 );
				loadImageFromMat(imgI,IMG[I]);
			}

			if (IMG[J] != 0) {
				loadImageFromMat(imgJ,IMG[J]);
			} else {
				CvSize imageSize = cvSize(imgJ.rows,imgJ.cols);
				IMG[J] = cvCreateImage( imageSize, 8, 1 );
				PYR[J] = cvCreateImage( imageSize, 8, 1 );
				loadImageFromMat(imgJ,IMG[J]);
			}

			// Points
			//float *ptsI = mxGetPr(prhs[3]); 
			int nPts = ptsI.rows;
			//float *ptsJ = mxGetPr(prhs[4]); 

			if (nPts != ptsJ.rows) {
				printf("Inconsistent input!\n");
				return Mat();
			}

			points[0] = (CvPoint2D32f*)cvAlloc(nPts*sizeof(CvPoint2D32f)); // template
			points[1] = (CvPoint2D32f*)cvAlloc(nPts*sizeof(CvPoint2D32f)); // target
			points[2] = (CvPoint2D32f*)cvAlloc(nPts*sizeof(CvPoint2D32f)); // forward-backward

			for (int i = 0; i < nPts; i++) {
				points[0][i].x = ptsI.at<float>(i,0); points[0][i].y = ptsI.at<float>(i,1);
				points[1][i].x = ptsJ.at<float>(i,0); points[1][i].y = ptsJ.at<float>(i,1);
				points[2][i].x = ptsI.at<float>(i,0); points[2][i].y = ptsI.at<float>(i,1);
			}
		
			float *ncc    = (float*) cvAlloc(nPts*sizeof(float));
			float *ssd    = (float*) cvAlloc(nPts*sizeof(float));
			float *fb     = (float*) cvAlloc(nPts*sizeof(float));
			char  *status = (char*)  cvAlloc(nPts);

			cvCalcOpticalFlowPyrLK( IMG[I], IMG[J], PYR[I], PYR[J], points[0], points[1], nPts, cvSize(win_size,win_size), Level, status, 0, cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), CV_LKFLOW_INITIAL_GUESSES);
			cvCalcOpticalFlowPyrLK( IMG[J], IMG[I], PYR[J], PYR[I], points[1], points[2], nPts, cvSize(win_size,win_size), Level, 0     , 0, cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03), CV_LKFLOW_INITIAL_GUESSES | CV_LKFLOW_PYR_A_READY | CV_LKFLOW_PYR_B_READY );
		
			normCrossCorrelation(IMG[I],IMG[J],points[0],points[1],nPts, status, ncc, Winsize,CV_TM_CCOEFF_NORMED);
			//normCrossCorrelation(IMG[I],IMG[J],points[0],points[1],nPts, status, ssd, Winsize,CV_TM_SQDIFF);
			euclideanDistance( points[0],points[2],fb,nPts);
		

			// Output
			int M = 4;
			Mat ans = Mat(nPts, M, CV_32F);
			for (int i = 0; i < nPts; i++) {
				if (status[i] == 1){
					ans.at<float>(i,0) = (float) points[1][i].x;
					ans.at<float>(i,1) = (float) points[1][i].y;
					ans.at<float>(i,2) = (float) fb[i];
					ans.at<float>(i,3) = (float) ncc[i];
					//output[M*i+4] = (float) ssd[i];
				} else {
					ans.at<float>(i,0)   = nan;
					ans.at<float>(i,1) = nan;
					ans.at<float>(i,2) = nan;
					ans.at<float>(i,3) = nan;
					//output[M*i+4] = nan;
				}
			}
			return ans;
		}
		return Mat();
	}

}


