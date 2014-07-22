#include"common.h"
#include"matrix.h"
namespace TLD{
	Mat bb_burn(Mat img, Mat bb_in, int width=0);
	Mat bb_center(Mat bb);
	Mat bb_points(Mat bb,int numM,int numN,float margin);
	Mat bb_predict(Mat BB0,Mat pt0,Mat pt1,Mat shift);
	float bb_width(Mat bb);
	float bb_height(Mat bb);
	bool bb_isdef(Mat BB2);
	bool bb_isout(Mat BB2,Size imgsize);
}