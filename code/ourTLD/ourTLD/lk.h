#ifndef _LK_H
#define _LK_H
#include"common.h"
namespace TLD{
	void loadImageFromMat(Mat m, IplImage *image);

	void euclideanDistance (CvPoint2D32f *point1, CvPoint2D32f *point2, float *match, int nPts);

	void normCrossCorrelation(IplImage *imgI, IplImage *imgJ, CvPoint2D32f *points0, CvPoint2D32f *points1, int nPts, char *status, float *match,int winsize, int method) ;
	void lk(int mode);
	Mat lk(int mode, Mat imgI,Mat imgJ,Mat ptsI,Mat ptsJ,int Level=5);//tested,work for grey picture~
}
#endif