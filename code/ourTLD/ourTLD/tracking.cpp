#include"tracking.h"
void tld::tldTracking(Rect &BB2,float &Conf,int Valid,Config TLD,Rect BB1,int I,int J){
//Estimates motion of bounding box BB1 from frame I to frame J
	//initialize output variables
	BB2 = Rect(); // estimated bounding 
	Conf = 0; // confidence of prediction
	Valid  = 0;  // is the predicted bounding box valid? if yes, learning will take place ...
}