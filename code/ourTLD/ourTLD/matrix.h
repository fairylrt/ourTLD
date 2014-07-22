#ifndef _MATRIX_H
#define _MATRIX_H

#include"common.h"
namespace TLD{
	float median(Mat m);//tested; Only work for one dimension; Not work for multiple dimension
	float median2(Mat m);
	Mat filterByValue(Mat x,float value,string type);
	bool anyLarger(vector<float> x,float thr);

	int maxIndex(vector<float> x);

	float maxValue(vector<float> x);

	Mat ntuples(Mat m1,Mat m2);

	Mat generateMatByStep(float x,float step,float y);

	Mat normMulCol(Mat m);
	Mat distance(Mat x,Mat ex,int type);
	Mat pdist(Mat X);
	Mat selectByBool(Mat x,Mat b);

	void printMat(Mat x);
}
#endif 