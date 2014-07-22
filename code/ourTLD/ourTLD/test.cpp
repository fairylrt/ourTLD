#include"tld.h"

Mat onebb;//一个bb
Mat mulbb;//好几个bb

int main(){

	float temp[4]={100.3,20.4,130.66,30.12};
	onebb=Mat(1,4,CV_32F,temp);
	float temp2[2][4]={{100.3,20.4,130.66,30.12},{89,70,130.3,120.2}};
	mulbb=Mat(2,4,CV_32F,temp2);

	Mat x=bb_points(onebb,5,5,2);

	//printMat(x);

	lk(0);

	float temp3[4]={109,46,242,192};
	Mat BB1(1,4,CV_32F,temp3);

	Mat xFI = TLD::bb_points(BB1,10,10,5); // generate 10x10 grid of points within BB1 with margin 5 px
	//printMat(xFI);
	Mat xFJ = lk(2,imread("img1.png",0),imread("img2.png",0),xFI,xFI);
	float medFB  = median2(xFJ.col(2)); // get median of Forward-Backward error
	float medNCC = median2(xFJ.col(3)); // get median for NCC
	Mat idxF   = filterByValue(xFJ.col(2),medFB,"<=").mul(filterByValue(xFJ.col(3),medNCC,">=")); // get indexes of reliable points
	printMatUchar(idxF);
	Mat	BB2 = bb_predict(BB1,selectByBool(xFI,idxF),selectByBool(xFJ.colRange(0,2),idxF)); // estimate BB2 using the reliable points only

	system("pause");
	return 0;
}