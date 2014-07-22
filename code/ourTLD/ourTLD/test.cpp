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

	system("pause");
	return 0;
}