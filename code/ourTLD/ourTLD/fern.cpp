#include "math.h"
#include"common.h"
#include "stdio.h"
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <map>
#include <set>
#include "tld.h"


static double thrN;
static int nBBOX;
static int mBBOX;
static int nTREES;
static int nFEAT;
static int nSCALE;
static int iHEIGHT;
static int iWIDTH;
static int *BBOX = NULL;
static int *OFF  = NULL;
static double *IIMG = 0;
static double *IIMG2 = 0;
static vector<vector <double> > WEIGHT;
static vector<vector <int> > nP;
static vector<vector <int> > nN;
static int BBOX_STEP = 7;
static int nBIT = 1; // number of bits per feature


#define sub2idx(row,col,height) ((int) (floor((row)+0.5) + floor((col)+0.5)*(height)))

void update(double *x, int C, int N) {
	for (int i = 0; i < nTREES; i++) {

		int idx = (int) x[i];

		(C==1) ? nP[i][idx] += N : nN[i][idx] += N;

		if (nP[i][idx]==0) {
			WEIGHT[i][idx] = 0;
		} else {
			WEIGHT[i][idx] = ((double) (nP[i][idx])) / (nP[i][idx] + nN[i][idx]);
		}
	}
}


double measure_forest(double *idx) {
	double votes = 0;
	for (int i = 0; i < nTREES; i++) { 
		votes += WEIGHT[i][idx[i]];
	}
	return votes;
}


int measure_tree_offset(unsigned char *img, int idx_bbox, int idx_tree) {
	int index = 0;
	int *bbox = BBOX + idx_bbox*BBOX_STEP;
	int *off = OFF + bbox[5] + idx_tree*2*nFEAT;
	for (int i=0; i<nFEAT; i++) {
		index<<=1; 
		int fp0 = img[off[0]+bbox[0]];
		int fp1 = img[off[1]+bbox[0]];
		if (fp0>fp1) { index |= 1;}
		off += 2;
	}
	return index;	
}


double measure_bbox_offset(unsigned char *blur, int idx_bbox, double minVar, double *tPatt) {

	double conf = 0.0;
	double bboxvar = bbox_var_offset(IIMG,IIMG2,BBOX+idx_bbox*BBOX_STEP);
	if (bboxvar < minVar) {	return conf; }

	for (int i = 0; i < nTREES; i++) { 
		int idx = measure_tree_offset(blur,idx_bbox,i);
		tPatt[i] = idx;
		conf += WEIGHT[i][idx];
	}
	return conf;
}

int* create_offsets(double *scale0, double *x0) {

	int *offsets = (int*) malloc(nSCALE*nTREES*nFEAT*2*sizeof(int));
	int *off = offsets;

	for (int k = 0; k < nSCALE; k++){
		double *scale = scale0+2*k;
		for (int i = 0; i < nTREES; i++) {
			for (int j = 0; j < nFEAT; j++) {
				double *x  = x0 +4*j + (4*nFEAT)*i;
				*off++ = sub2idx((scale[0]-1)*x[1],(scale[1]-1)*x[0],iHEIGHT);
				*off++ = sub2idx((scale[0]-1)*x[3],(scale[1]-1)*x[2],iHEIGHT);
			}
		}
	}
	return offsets;
}

int* create_offsets_bbox(double *bb0) {

	int *offsets = (int*) malloc(BBOX_STEP*nBBOX*sizeof(int));
	int *off = offsets;

	for (int i = 0; i < nBBOX; i++) {
		double *bb = bb0+mBBOX*i;
		*off++ = sub2idx(bb[1]-1,bb[0]-1,iHEIGHT);
		*off++ = sub2idx(bb[3]-1,bb[0]-1,iHEIGHT);
		*off++ = sub2idx(bb[1]-1,bb[2]-1,iHEIGHT);
		*off++ = sub2idx(bb[3]-1,bb[2]-1,iHEIGHT);
		*off++ = (int) ((bb[2]-bb[0])*(bb[3]-bb[1]));
		*off++ = (int) (bb[4]-1)*2*nFEAT*nTREES; // pointer to features for this scale
		*off++ = bb[5]; // number of left-right bboxes, will be used for searching neighbours
	}
	return offsets;
}


double randdouble() 
{ 
	return rand()/(double(RAND_MAX)+1); 
}

// default
void fern(){
	printf("CLEANUP: function(0);\n");
	printf("INIT: function(1, img, bb, features, scales)\n");
	printf("UPDATE: Conf = function(2,X,Y,Margin,Bootstrap,Idx)\n");
	printf("EVALUATE: Conf = function(3,X)\n");
	printf("DETECT: function(4,img,maxBBox,minVar,Conf,X)\n");
	printf("GET PATTERNS: patterns = fern(5,img,idx,minVar)\n");
	return;
}

// mode = 0; clear up
void fern(int mode){
	if(mode == 0){
		srand(0); // fix state of random generator
		thrN = 0; nBBOX = 0; mBBOX = 0; nTREES = 0; nFEAT = 0; nSCALE = 0; iHEIGHT = 0; iWIDTH = 0;

		free(BBOX); BBOX = 0;
		free(OFF); OFF = 0;
		free(IIMG); IIMG = 0;
		free(IIMG2); IIMG2 = 0;
		WEIGHT.clear();
		nP.clear();
		nN.clear();
		return;
	}
}

// mode = 1 % allocate structures
void fern(int mode, Mat img, Mat grid, Mat features, Mat scales){
	if(mode == 1){
		// c++ 不好像matlab确定传入参数个数，所以要在调用的时候保证参数传入正确； 这里传入参数的个数需要是5个
		if (BBOX!=0) { printf("fern: already initialized.\n"); return; } 
		
		iHEIGHT    = img.rows; 
		iWIDTH     = img.cols;
		nTREES     = features.cols; //mxGetN(mxGetField(prhs[3],0,"x"));
		nFEAT      = features.rows/4; //mxGetM(mxGetField(prhs[3],0,"x")) / 4; // feature has 2 points: x1,y1,x2,y2
		thrN       = 0.5 * nTREES;
		nSCALE     = scales.cols; //mxGetN(prhs[4]);

		IIMG       = (double*) malloc(iHEIGHT*iWIDTH*sizeof(double));
		IIMG2      = (double*) malloc(iHEIGHT*iWIDTH*sizeof(double));

		// BBOX
		mBBOX      =  grid.rows;//mxGetM(prhs[2]); 
		nBBOX      =  grid.cols;//mxGetN(prhs[2]);
		BBOX	   = create_offsets_bbox(grid.ptr<double>(0));
		double *x  = features.ptr<double>(0); //mxGetPr(mxGetField(prhs[3],0,"x"));
		double *s  = scales.ptr<double>(0); //mxGetPr(prhs[4]);
		OFF		   = create_offsets(s,x);

		for (int i = 0; i<nTREES; i++) {
			WEIGHT.push_back(vector<double>(pow(2.0,nBIT*nFEAT), 0));
			nP.push_back(vector<int>(pow(2.0,nBIT*nFEAT), 0));
			nN.push_back(vector<int>(pow(2.0,nBIT*nFEAT), 0));
		}

		for (int i = 0; i<nTREES; i++) {
			for (int j = 0; j < WEIGHT[i].size(); j++) {
				WEIGHT[i].at(j) = 0;
				nP[i].at(j) = 0;
				nN[i].at(j) = 0;
			}
		}
		return;
			 }
	}

// UPDATE  训练更新随机厥时候用 
// mode=2  num_para = 5; 无返回值
void fern(int mode, Mat XX, Mat YY, float thr_fern, int bootstrap){
		double *X     = XX.ptr<double>(0); //mxGetPr(prhs[1]);
		int numX      = XX.cols; //mxGetN(prhs[1]);
		double *Y     = YY.ptr<double>(0); //mxGetPr(prhs[2]);
		double thrP   = (double)thr_fern *nTREES; //*mxGetPr(prhs[3]) * nTREES;
		//int bootstrap = (int) *mxGetPr(prhs[4]);

		int step = numX / 10;

		//if (nrhs == 5) {
			for (int j = 0; j < bootstrap; j++) {
				for (int i = 0; i < step; i++) {
					for (int k = 0; k < 10; k++) {
					
						int I = k*step + i;
						double *x = X+nTREES*I;
						if (Y[I] == 1) {
							if (measure_forest(x) <= thrP)
								update(x,1,1);
						} else {
							if (measure_forest(x) >= thrN)
								update(x,0,1);
						}
					}
				}
			}
		//}
}

// UPDATE  训练更新随机厥时候用 
// mode=2  num_para = 5; 返回Mat
vector<double> fern(int mode, Mat XX, Mat YY, float thr_fern, int bootstrap, bool type){
		double *X     = XX.ptr<double>(0); //mxGetPr(prhs[1]);
		int numX      = XX.cols; //mxGetN(prhs[1]);
		double *Y     = YY.ptr<double>(0); //mxGetPr(prhs[2]);
		double thrP   = (double)thr_fern *nTREES; //*mxGetPr(prhs[3]) * nTREES;
		//int bootstrap = (int) *mxGetPr(prhs[4]);

		int step = numX / 10;

		//if (nrhs == 5) {
			for (int j = 0; j < bootstrap; j++) {
				for (int i = 0; i < step; i++) {
					for (int k = 0; k < 10; k++) {
					
						int I = k*step + i;
						double *x = X+nTREES*I;
						if (Y[I] == 1) {
							if (measure_forest(x) <= thrP)
								update(x,1,1);
						} else {
							if (measure_forest(x) >= thrN)
								update(x,0,1);
						}
					}
				}
			}
		//}
	vector<double> output;
	for (int i = 0; i < numX; i++) {
		output.push_back(measure_forest(X+nTREES*i));
	}
	return output;
}
//// 6个参数，mode = 2
void fern(int mode, Mat XX, Mat YY, float thr_fern, int bootstrap, Mat idxMat){
	double *X     = XX.ptr<double>(0); //mxGetPr(prhs[1]);
		int numX      = XX.cols; //mxGetN(prhs[1]);
		double *Y     = YY.ptr<double>(0); //mxGetPr(prhs[2]);
		double thrP   = (double)thr_fern *nTREES; //*mxGetPr(prhs[3]) * nTREES;
		//int bootstrap = (int) *mxGetPr(prhs[4]);

		int step = numX / 10;

		double *idx   = idxMat.ptr<double>(0); //mxGetPr(prhs[5]);
		int nIdx      = idxMat.cols*idxMat.rows;//mxGetN(prhs[5])*mxGetM(prhs[5]);

		for (int j = 0; j < bootstrap; j++) {
			for (int i = 0; i < nIdx; i++) {
				int I = idx[i]-1;
				double *x = X+nTREES*I;
				if (Y[I] == 1) {
					if (measure_forest(x) <= thrP)
						update(x,1,1);
					} else {
						if (measure_forest(x) >= thrN)
							update(x,0,1);
					}
				}
			}
}

// num_para=7, mode=2, 有返回值
vector<double> fern(int mode, Mat XX, Mat YY, float thr_fern, int bootstrap, Mat idxMat,bool type){
	double *X     = XX.ptr<double>(0); //mxGetPr(prhs[1]);
		int numX      = XX.cols; //mxGetN(prhs[1]);
		double *Y     = YY.ptr<double>(0); //mxGetPr(prhs[2]);
		double thrP   = (double)thr_fern *nTREES; //*mxGetPr(prhs[3]) * nTREES;
		//int bootstrap = (int) *mxGetPr(prhs[4]);

		int step = numX / 10;

		double *idx   = idxMat.ptr<double>(0); //mxGetPr(prhs[5]);
		int nIdx      = idxMat.cols*idxMat.rows;//mxGetN(prhs[5])*mxGetM(prhs[5]);

		for (int j = 0; j < bootstrap; j++) {
			for (int i = 0; i < nIdx; i++) {
				int I = idx[i]-1;
				double *x = X+nTREES*I;
				if (Y[I] == 1) {
					if (measure_forest(x) <= thrP)
						update(x,1,1);
					} else {
						if (measure_forest(x) >= thrN)
							update(x,0,1);
					}
				}
			}
	vector<double> output;
	for (int i = 0; i < numX; i++) {
		output.push_back(measure_forest(X+nTREES*i));
	}
	return output;
}




