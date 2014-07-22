#include"common.h"
#include"Config.h"
#include"bbox.h"

#define max(a,b) a<b?b:a
#define min(a,b) a<b?a:b
namespace TLD{
	void bb_burn(Mat img, Mat bb_in, int width){
		for (int i=0;i<bb_in.rows;i++){
			Mat bb=bb_in.row(i).clone();
			bb.at<float>(0)=max(1,bb.at<float>(0));
			bb.at<float>(1)=max(1,bb.at<float>(1));
			bb.at<float>(2)=min(img.cols,bb.at<float>(2));
			bb.at<float>(3)=max(img.rows,bb.at<float>(3));
			
			int bb0=(int)bb.at<float>(0);
			int bb1=(int)bb.at<float>(1);
			int bb2=(int)bb.at<float>(2);
			int bb3=(int)bb.at<float>(3);

			img.rowRange(bb1-1,bb1+width).colRange(bb0-1,bb2)=255;
			img.rowRange(bb3-width-1,bb3).colRange(bb0-1,bb2)=255;
			img.rowRange(bb1-1,bb3).colRange(bb0-1,bb0+width)=255;
			img.rowRange(bb1-1,bb3).colRange(bb2-width-1,bb2)=255;
		}
	}//ok~ bb是整数矩阵怎么办？
	Mat bb_center(Mat bb){
		Mat cp(bb.rows,2,CV_32F);
		cp.col(0)=(bb.col(0)+bb.col(2))/2;
		cp.col(1)=(bb.col(1)+bb.col(3))/2;
		return cp;
	}//tested~ok~

	//bb是'一'个框！
	Mat bb_points(Mat bb1,int numM,int numN,float margin){
		//Generates numM x numN points on BBox.
		Mat bb=bb1.clone();
		bb.colRange(0,2)+=margin;
		bb.colRange(2,4)-=margin;

		if (numM==1 && numN==1)
			return bb_center(bb);

		if (numM==1 && numN>1){
			Mat c=bb_center(bb);
			float stepW=(bb.at<float>(2)-bb.at<float>(0))/(numN-1);
			return ntuples(generateMatByStep(bb.at<float>(0),stepW,bb.at<float>(2)),c.col(1));
		}

		if (numM>1 && numN==1){
			Mat c=bb_center(bb);
			float stepH=(bb.at<float>(3)-bb.at<float>(1))/(numM-1);
			return ntuples(c.col(0),generateMatByStep(bb.at<float>(1),stepH,bb.at<float>(3)));
		}

		float stepW=(bb.at<float>(2)-bb.at<float>(0))/(numN-1);
		float stepH=(bb.at<float>(3)-bb.at<float>(1))/(numM-1);
		return ntuples(generateMatByStep(bb.at<float>(0),stepW,bb.at<float>(2)),generateMatByStep(bb.at<float>(1),stepH,bb.at<float>(3))).clone();
	}

	//BB0是一维的。草！
	Mat bb_predict(Mat BB0,Mat pt0,Mat pt1,Mat shift){
		Mat of=pt1-pt0;
		float dx=median(of.col(0));
		float dy=median(of.col(1));

		Mat d1=pdist(pt0);
		Mat d2=pdist(pt1);
		float s=median(d2/d1);

		float s1=0.5*(s-1)*bb_width(BB0);
		float s2=0.5*(s-1)*bb_height(BB0);

		float temp[4]={BB0.at<float>(0)-s1+dx,BB0.at<float>(1)-s2+dy,BB0.at<float>(2)+s1+dx,BB0.at<float>(3)+s2+dy};
		Mat ans(1,4,CV_32F,temp);
		float temp2[2]={s1,s2};
		shift=Mat(1,2,CV_32F,temp2,1);
		return ans.clone();
	}
	Mat bb_predict(Mat BB0,Mat pt0,Mat pt1){
		Mat of=pt1-pt0;
		float dx=median(of.col(0));
		float dy=median(of.col(1));

		Mat d1=pdist(pt0);
		Mat d2=pdist(pt1);
		float s=median(d2/d1);

		float s1=0.5*(s-1)*bb_width(BB0);
		float s2=0.5*(s-1)*bb_height(BB0);

		float temp[4]={BB0.at<float>(0)-s1+dx,BB0.at<float>(1)-s2+dy,BB0.at<float>(2)+s1+dx,BB0.at<float>(3)+s2+dy};
		Mat ans(1,4,CV_32F,temp);
		//float temp2[2]={s1,s2};
		//shift=Mat(1,2,CV_32F,temp2);
		return ans.clone();
	}
	float bb_width(Mat bb){
		return bb.at<float>(2)-bb.at<float>(0)+1;
	}
	float bb_height(Mat bb){
		return bb.at<float>(3)-bb.at<float>(1)+1;
	}
	bool bb_isdef(Mat BB2){
		return !BB2.empty();
	}
	bool bb_isout(Mat bb,Size imsize){
		return (bb.at<float>(0)>imsize.height)
			||(bb.at<float>(1)>imsize.width)
			||(bb.at<float>(2)<1)
			||(bb.at<float>(3)<1);
	}
}
