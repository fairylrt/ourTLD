#include"common.h"
#include"Config.h"
namespace TLD{
	Mat bb_points(Rect bb,int numM,int numN,float margin){
		return Mat();
	};
	Rect bb_predict(Rect BB0,Mat pt0,Mat pt1){
		return Rect();
	}
	bool bb_isdef(Rect BB2){
		return true;
	}
	bool bb_isout(Rect BB2,Size imgsize){
		return false;
	}
}
