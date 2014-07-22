#include"common.h"

namespace TLD{
	float median(Mat m);
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
	vector<int> find_index(vector<float> conf, float thr, int type); //type = 1, greater than; type = 0, less than 

}
