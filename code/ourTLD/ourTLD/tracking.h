#ifndef _TRACKING_H
#define _TRACKING_H
#include"common.h"
#include"bbox.h"
#include"Config.h"
#include"lk.h"
#include"matrix.h"
#include<math.h>

namespace TLD{
	void warp_image_roi(Mat image, int w, int h, Mat H,
						double xmin, double xmax, double ymin, double ymax,
						double fill, Mat result);
	Mat warp(Mat img,Mat H,Mat box);
	void tldTracking(Mat &BB2,vector<float> &Conf,int Valid,Config tld,Mat BB1,int I,int J);
	Mat tldGetPattern(Mat img,Mat bb,Size patchsize,bool flip=0);
	void tldNN(vector<float> &conf1,vector<float> &conf2,Mat x,Config tld);
	Mat img_patch(Mat img, Mat bb);
	Mat tldPatch2Pattern(Mat patch,Size patchsize);
}
#endif