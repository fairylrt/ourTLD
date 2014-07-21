#include"common.h"
namespace TLD{
	Mat bb_points(Mat bb,int numM,int numN,float margin);
	Mat bb_predict(Mat BB0,Mat pt0,Mat pt1);
	bool bb_isdef(Mat BB2);
	bool bb_isout(Mat BB2,Size imgsize);
}