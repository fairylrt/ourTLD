#ifndef _DETECTING_H
#define _DETECTING_H
#include"common.h"
#include"bbox.h"
#include"Config.h"
#include"matrix.h"
#include"fern.h"

namespace TLD{
	struct DT{
		Mat bb;
		vector<int> idx;
		Mat conf1;
		Mat conf2;
		Mat isin;
		Mat patch;
		vector<float> patt;
	};
	void tldDetecting(vector<Mat> &dBB2, vector<float> &Conf, Config tld, int I);
	
}
#endif
