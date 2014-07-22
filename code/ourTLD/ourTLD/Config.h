#ifndef _CONFIG_H
#define _CONFIG_H
#include"common.h"
class Config{
public:
	Mat bb;
	vector<Mat> img;//img[I]��ʾ������ͼ��
	Mat xFJ;
	Size imgsize;
	struct Model{
		Size patchsize;
		float thr_nn_valid;
		float ncc_thesame;
		float valid;
	} model;
	vector<int> valid;
	Mat pex;
	Mat nex;
};
#endif