#include"tracking.h"
namespace TLD{
	float median2(Mat m){
		return 0;
	}
	Range filterByValue(Mat xFJ3,float medFB,Mat xFJ4,float medNCC){
		return Range();
	}
//Estimates motion of bounding box BB1 from frame I to frame J
void tldTracking(Mat &BB2,float &Conf,int Valid,Config tld,Mat BB1,int I,int J){
	BB2=Mat();
	Conf=0;
	Valid  = 0;

	Mat xFI = TLD::bb_points(BB1,10,10,5); 
	Mat xFJ = lk(2,tld.img[I],tld.img[J],xFI,xFI);
	float medFB  = median2(xFJ.row(3)); 
	float medNCC = median2(xFJ.row(4)); 
	Range idxF   = filterByValue(xFJ.row(3),medFB,xFJ.row(4),medNCC); // get indexes of reliable points
	BB2 = bb_predict(BB1,xFI.colRange(idxF),xFJ.rowRange(1,2).colRange(idxF)); // estimate BB2 using the reliable points only

	tld.xFJ = xFJ.colRange(idxF);//save selected points (only for display purposes)

	// detect failures
	if (!bb_isdef(BB2) || bb_isout(BB2,tld.imgsize)){
		BB2 = Mat(); return;// bounding box out of image
	}

	//ÎÊÌâ
	//if tld.control.maxbbox > 0 && medFB > 10, BB2 = []; return; end  % too unstable predictions

	
	// estimate confidence and validity
	Mat patchJ   = tldGetPattern(tld.img[J],BB2,tld.model.patchsize); // sample patch in current image
	float t_Conf;
	tldNN(t_Conf,Conf,patchJ,tld); // estimate its Conservative Similarity (considering 50% of positive patches only)

	// Validity
	Valid = tld.valid[I]; // copy validity from previous frame
	if (Conf > tld.model.thr_nn_valid)
		Valid = 1; // tracker is inside the 'core'
}
	/*
	//initialize output variables
	BB2 = Mat(); // estimated bounding 
	Conf = 0; // confidence of prediction
	Valid  = 0;  // is the predicted bounding box valid? if yes, learning will take place ...

	// estimate BB2
	Mat xFI    = bb_points(BB1,10,10,5); // generate 10x10 grid of points within BB1 with margin 5 px
	Mat xFJ    = lk(2,tld.img{I}.input,tld.img{J}.input,xFI,xFI); % track all points by Lucas-Kanade tracker from frame I to frame J, estimate Forward-Backward error, and NCC for each point
	float medFB  = median2(xFJ(3,:)); // get median of Forward-Backward error
	float medNCC = median2(xFJ(4,:)); // get median for NCC
	Range idxF   = xFJ(3,:) <= medFB & xFJ(4,:)>= medNCC; // get indexes of reliable points
	BB2    = bb_predict(BB1,xFI.colRange(idxF),xFJ.rowRange(1,2).colRange(idxF); // estimate BB2 using the reliable points only

	tld.xFJ = xFJ(:,idxF); % save selected points (only for display purposes)

	// detect failures
	if ~bb_isdef(BB2) || bb_isout(BB2,tld.imgsize), BB2 = []; return; end % bounding box out of image
	if tld.control.maxbbox > 0 && medFB > 10, BB2 = []; return; end  % too unstable predictions

	// estimate confidence and validity
	patchJ   = tldGetPattern(tld.img{J},BB2,tld.model.patchsize); % sample patch in current image
	[~,Conf] = tldNN(patchJ,tld); % estimate its Conservative Similarity (considering 50% of positive patches only)

	// Validity
	Valid    = tld.valid(I); % copy validity from previous frame
	if Conf > tld.model.thr_nn_valid, Valid = 1; end % tracker is inside the 'core'
	}
	*/

	Mat tldGetPattern(Mat img,Mat bb,Size patchsize,bool flip=0){
		int nBB=bb.cols;
		Mat pattern=Mat::zeros(Size(patchsize.height*patchsize.width,nBB),CV_32F);
		for (int i=0;i<nBB;i++){
			Mat patch=img_patch(img,bb.col(i));
			if (flip)
				patch=Mat();//fliplr
			pattern.col(i) = tldPatch2Pattern(patch,patchsize);
		}
		return Mat();
	}

	Mat norm(Mat m){
		Mat x(1,m.cols,CV_32F,0);
		for(int i=0;i<m.cols;i++){
			x.at<float>(i)=0;
			for(int j=0;j<m.cols;j++){
				x.at<float>(i)+=m.at<float>(i,j)*m.at<float>(i,j);
			}
			x.at<float>(i)=sqrt(x.at<float>(i));
		}
		return x;
	}

	vector<float> distance(Mat x,Mat ex,int type){
		Mat dst;
		gemm(x.t(),ex,1,Mat(x.t()),0,dst);
		Mat x1=norm(x);
		Mat x2=norm(ex);
		dst/=x1.at<float>(0);
		dst/=x2;
		return dst;
	}

	void tldNN(vector<float> &conf1,vector<float> &conf2,Mat x,Config tld){
		Mat isin(3,x.cols,CV_32F);
		conf1=Mat(1,x.cols,CV_32F);
		conf2=Mat(1,x.cols,CV_32F);
		for (int i=0;i<x.cols;i++){
			
		}
		
	}
	Mat img_patch(Mat img, Mat bb, float randomize=0,float p_par=0){
		return Mat();
	}

	Mat tldPatch2Pattern(Mat patch,Size patchsize){
		resize(patch,patch,patchsize); // 'bilinear' is faster
		patch.convertTo(patch,CV_32F);
		Mat pattern = patch.reshape(0,1);
		pattern = pattern - mean(pattern);
		return pattern.t();
	}
}