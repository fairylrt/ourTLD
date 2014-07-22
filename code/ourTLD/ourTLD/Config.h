#ifndef _CONFIG_H
#define _CONFIG_H
#include"common.h"

class Config{
public:
	Mat bb;
<<<<<<< HEAD
	vector<Mat> img;
=======
	vector<Mat> img;//img[I]��ʾ������ͼ��
>>>>>>> origin/lrt
	Mat xFJ;
	Size imgsize;
	struct Model{
		Size patchsize;
		float thr_nn_valid;
		float ncc_thesame;
<<<<<<< HEAD
		float valid;  
		float thr_fern; 
		float thr_nn;
		int num_trees;
		int num_features;
		int min_win; 
		int fliplr;

=======
		float valid;
>>>>>>> origin/lrt
	} model;
	vector<int> valid;
	Mat pex;
	Mat nex;
<<<<<<< HEAD
	struct Control{
		float maxbbox; //fraction of evaluated bounding boxes in every frame, maxbox = 0 means detector is truned off, if you don't care about speed set it to 1 
		int update_detector; //% online learning on/off, of 0 detector is trained only in the first frame and then remains fixed
		int drop_img; // drop_img = 1;
		int repeat; // repeat=1;
	} control;
	Mat grid; // store bounding box;
	Mat scales; // 
	Mat feature; // 原来里面定义的features还有个成员是type, default是"forest", 不知道有没有用，为了方便先定义为mat;
	float var; //  Variance threshold
	vector<float> tmpConf; // 原来的tld.tmp.conf
	Mat tmpPatt; // 原来的tld.tmp.patt

	
};
=======
};
#endif
>>>>>>> origin/lrt
