#ifndef _MATRIX_H
#define _MATRIX_H

#include"common.h"
namespace TLD{
	float median(Mat m);//tested; Only work for one dimension; Not work for multiple dimension
	float median2(Mat m);//tested
	Mat filterByValue(Mat x,float value,string type);//tested
	bool anyLarger(vector<float> x,float thr);

	int maxIndex(vector<float> x);
	
	vector<int> find_index(vector<float> conf, float thr, int type); //type = 1, greater than; type = 0, less than 

	float maxValue(vector<float> x);//tested

	Mat ntuples(Mat m1,Mat m2);//tested

	Mat generateMatByStep(float x,float step,float y);//tested

	Mat normMulCol(Mat m);
	Mat distance(Mat x,Mat ex,int type);
	Mat pdist(Mat X);//tested
	Mat selectByBool(Mat x,Mat b);//tested

	void printMat(Mat x);//tested
	void printMatUchar(Mat x);//tested

	Mat roundToUchar(Mat m);//tested
	Mat roundToFloat(Mat m);//tested
	Mat ucharToFloat(Mat m);//tested
}
#endif 
