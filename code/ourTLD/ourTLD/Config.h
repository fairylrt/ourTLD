#include"common.h"
class Config{
public:
	Mat bb;
	vector<Mat> img;//img[I]��ʾ������ͼ��
	Mat xFJ;
	Size imgsize;
	struct Model{
		Size patchsize;
		float thr_nn_valid
	} model;
	vector<int> valid;
	vector<Mat> pex;
	vector<Mat> nex;
};