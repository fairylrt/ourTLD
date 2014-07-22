#ifndef _FERN_H
#define _FERN_H
#include"common.h"

namespace TLD{
	void update(double *x, int C, int N);
	double measure_forest(double *idx);
	int measure_tree_offset;
	double measure_bbox_offset;
	int* create_offsets;
	int* create_offsets_bbox;
	double randdouble() ;
	void fern(); // 没有参数; 
	void fern(int mode); // mode = 0
	void fern(); // 2个参数
	void fern(); // 4个参数
	void fern(int mode, Mat img, Mat grid, Mat features, Mat scales); // 5个参数 mode = 1
	void fern(int mode, Mat XX, Mat YY, float thr_fern, int bootstrap); // 5个参数 mode =2 
	vector<double> fern((int mode, Mat XX, Mat YY, float thr_fern, int bootstrap, bool type);) // 6个参数 mode =2  type=1 有返回值
	void fern(int mode, Mat XX, Mat YY, float thr_fern, int bootstrap, Mat idxMat); // 6个参数，mode = 2
	vector<double> fern(int mode, Mat XX, Mat YY, float thr_fern, int bootstrap, Mat idxMat, bool type); // 7个参数， mode=2 type=1 有返回值
	void fern(int mode, Mat img, float maxbbox, float var, vector<float> tmpConf, Mat tmpPatt); // 6个参数

}
#endif
