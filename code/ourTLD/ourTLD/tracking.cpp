#include"tracking.h"
namespace TLD{
	#define nextrow(tmp, width, height) ((tmp)+width)
	#define nextcol(tmp, width, height) ((tmp)+1)
	#define nextr_c(tmp, width, height) ((tmp)+width+1)

	/* Warps image of size w x h, using affine transformation matrix (2x2 part) 
   and offset (center of warping) ofsx, ofsy. Result is the region of size 
   defined with roi. */
	void warp_image_roi(Mat image, int w, int h, Mat H,
						double xmin, double xmax, double ymin, double ymax,
						double fill, Mat result)
	{
	   double curx, cury, curz, wx, wy, wz, ox, oy, oz;
	   int x, y;
	   unsigned char *tmp;
	   float *output= (float*)(result.data);
	   double i, j, xx, yy;
	   /* precalulate necessary constant with respect to i,j offset 
		  translation, H is column oriented (transposed) */   
	   ox = H.at<float>(0,2);
	   oy = H.at<float>(1,2);
	   oz = H.at<float>(2,2);

	   yy = ymin;
	   for (j=0; j<(int)(ymax-ymin+1); j++)
	   {
		  /* calculate x, y for current row */
		  curx = H.at<float>(0,1)*yy + ox;
		  cury = H.at<float>(1,1)*yy + oy;
		  curz = H.at<float>(2,1)*yy + oz;
		  xx = xmin; 
		  yy = yy + 1;
		  for (i=0; i<(int)(xmax-xmin+1); i++)
		  {
			 /* calculate x, y in current column */
			 wx = H.at<float>(0,0)*xx + curx;
			 wy = H.at<float>(1,0)*xx + cury;
			 wz = H.at<float>(2,0)*xx + curz;
	//       printf("%g %g, %g %g %g\n", xx, yy, wx, wy, wz);
			 wx /= wz; wy /= wz;
			 xx = xx + 1;
         
			 x = (int)floor(wx);
			 y = (int)floor(wy);

			 if (x>=0 && y>=0)
			 {
				wx -= x; wy -= y; 
				if (x+1==w && wx==1)
				   x--;
				if (y+1==h && wy==1)
				   y--;
				if ((x+1)<w && (y+1)<h)
				{
				   tmp = image.data+x*w+y;
				   /* image[x,y]*(1-wx)*(1-wy) + image[x+1,y]*wx*(1-wy) +
					  image[x,y+1]*(1-wx)*wy + image[x+1,y+1]*wx*wy */
				   *output++ = 
					  (*(tmp) * (1-wx) + *nextcol(tmp, w, h) * wx) * (1-wy) +
					  (*nextrow(tmp,w,h) * (1-wx) + *nextr_c(tmp,w,h) * wx) * wy;
				} else 
				   *output++ = fill;
			 } else 
				*output++ = fill;
		  }
	   }
	}

	Mat warp(Mat img,Mat H,Mat box){
		int w=img.cols;
		int h=img.rows;
		
		float xmin=box.at<float>(0),ymin=box.at<float>(1),xmax=box.at<float>(2),ymax=box.at<float>(3);

		float fill=0;
		Mat result(Size((xmax-xmin+1),(int)(ymax-ymin+1)),CV_32F);
		warp_image_roi(img,w,h,H,xmin,xmax,ymin,ymax,fill,result);

		return result;
	}

//Estimates motion of bounding box BB1 from frame I to frame J
void tldTracking(Mat &BB2,vector<float> &Conf,int Valid,Config tld,Mat BB1,int I,int J){
	//initialize output variables
	BB2=Mat();// estimated bounding
	Conf=vector<float>(); // confidence of prediction
	Valid  = 0;  // is the predicted bounding box valid? if yes, learning will take place ...


	// estimate BB2
	Mat xFI = TLD::bb_points(BB1,10,10,5); // generate 10x10 grid of points within BB1 with margin 5 px
	Mat xFJ = lk(2,tld.img[I],tld.img[J],xFI,xFI);// track all points by Lucas-Kanade tracker from frame I to frame J, estimate Forward-Backward error, and NCC for each point
	float medFB  = median2(xFJ.col(3)); // get median of Forward-Backward error
	float medNCC = median2(xFJ.col(4)); // get median for NCC
	Range idxF   = filterByValue(xFJ.col(3),medFB,xFJ.col(4),medNCC); // get indexes of reliable points
	BB2 = bb_predict(BB1,xFI.rowRange(idxF),xFJ.colRange(1,2).rowRange(idxF)); // estimate BB2 using the reliable points only

	tld.xFJ = xFJ.rowRange(idxF);//save selected points (only for display purposes)

	// detect failures
	if (!bb_isdef(BB2) || bb_isout(BB2,tld.imgsize)){
		BB2 = Mat(); return;// bounding box out of image
	}

	//ÎÊÌâ
	//if tld.control.maxbbox > 0 && medFB > 10, BB2 = []; return; end  % too unstable predictions

	
	// estimate confidence and validity
	Mat patchJ   = tldGetPattern(tld.img[J],BB2,tld.model.patchsize); // sample patch in current image
	vector<float> t_Conf;
	tldNN(t_Conf,Conf,patchJ,tld); // estimate its Conservative Similarity (considering 50% of positive patches only)

	// Validity
	Valid = tld.valid[I]; // copy validity from previous frame
	if (Conf[0] > tld.model.thr_nn_valid)
		Valid = 1; // tracker is inside the 'core'
}

	Mat tldGetPattern(Mat img,Mat bb,Size patchsize,bool flip=0){
		int nBB=bb.rows;
		Mat pattern=Mat::zeros(Size(nBB,patchsize.height*patchsize.width),CV_32F);
		for (int i=0;i<nBB;i++){
			Mat patch=img_patch(img,bb.row(i));
			if (flip)
				patch=Mat();//fliplr
			pattern.row(i) = tldPatch2Pattern(patch,patchsize);
		}
		return Mat();
	}

	

	void tldNN(vector<float> &conf1,vector<float> &conf2,Mat x,Config tld){
		Mat isin(x.rows,3,CV_32F,-1);
		conf1.clear();
		conf2.clear();
		conf1=vector<float>(x.rows);
		conf2=vector<float>(x.rows);
		for (int i=0;i<x.cols;i++){// fore every patch that is tested
			vector<float> nccP=TLD::distance(x.row(i),tld.pex,1);// measure NCC to positive examples
			vector<float> nccN=TLD::distance(x.row(i),tld.nex,1);//measure NCC to negative examples

			if (anyLarger(nccP,tld.model.ncc_thesame))//
				isin.at<float>(i,1)=1;//IF the query patch is highly correlated with any positive patch in the model THEN it is considered to be one of them
			isin.at<float>(i,2)=maxIndex(nccP);// get the index of the maximall correlated positive patch
			if (anyLarger(nccN,tld.model.ncc_thesame))
				isin.at<float>(i,3)=1;//IF the query patch is highly correlated with any negative patch in the model THEN it is considered to be one of them

			// measure Relative Similarity
			float dN = 1 - maxValue(nccN);
			float dP = 1 - maxValue(nccP);
			conf1[i] = dN / (dN + dP);

			// measure Conservative Similarity
			float maxP = maxValue(vector<float>( nccP.begin(),nccP.begin()+int(ceil(tld.model.valid*tld.pex.rows))));
			dP = 1 - maxP;
			conf2[i] = dN / (dN + dP);

		}
		
	}
	Mat img_patch(Mat img, Mat bb, float randomize=0,float p_par=0){
		//All coordinates are integers
		Mat cp(Size(1,2),CV_32F);
		cp.at<float>(0)=(bb.at<float>(0)+bb.at<float>(2))/2-1;
		cp.at<float>(1)=(bb.at<float>(1)+bb.at<float>(3))/2-1;
		
		float data[3][3]={{1,0,-cp.at<float>(0)},{0,1,-cp.at<float>(1)},{0,0,1}};
		Mat H(3,3,CV_32F,data);
		
		float bbW=bb.at<float>(2)-bb.at<float>(0);
		float bbH=bb.at<float>(3)-bb.at<float>(1);
		if (bbW<=0||bbH<=0)
			return Mat();

		float data1[4]={-bbW/2,bbW/2,-bbH/2,bbH/2};
		Mat box(1,4,CV_32F,data1);

		Mat patch;
		H=H.inv();
		if (img.channels()==3){
			vector<Mat> channels;
			split(img,channels);
			for(int i=0;i<3;i++){
				channels[i]=warp(channels[i],H,box);
				//uint8(channels[i]);
			}
			merge(channels,patch);
		}
		else{
			patch=warp(img,H,box);
			//patch=(int)patch;
		}
		return patch;
	}

	Mat tldPatch2Pattern(Mat patch,Size patchsize){
		resize(patch,patch,patchsize); // 'bilinear' is faster
		//patch.convertTo(patch,CV_32F);
		Mat pattern = patch.reshape(1,1);
		pattern = pattern - mean(pattern);
		return pattern;
	}
}