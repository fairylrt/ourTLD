#include"common.h"
namespace TLD{
	vector<Point2d> bb_points(Rect bb,int numM,int numN,float margin);
	Rect bb_predict(Rect BB0,Mat pt0,Mat pt1);
}