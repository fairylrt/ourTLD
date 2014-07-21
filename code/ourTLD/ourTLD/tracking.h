#include"common.h"
#include"bbox.h"
#include"Config.h"
#include"lk.h"
#include<math.h>

namespace TLD{
	void tldTracking(Mat &BB2,float &Conf,int Valid,Config TLD,Mat BB1,int I,int J);
}