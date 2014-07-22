#include"detecting.h"
namespace TLD{

	/*scanns the image(I) with a sliding window, returns a list of bounding boxes and their confidences that match the object description
	*/

	void tldDetecting(vector<Mat> &dBB2, vector<float> &Conf, Config tld, int I){
		dBB2 = vector<Mat>();
		Conf = vector<float>();
		DT dt;
		Mat img = tld.img[I];
		fern(4,img,tld.control.maxbbox, tld.var, tld.tmp.conf,tld.tmp.patt);
		// evaluates Ensemble Classifier: saves sum of posteriors to 'tld.tmp.conf', 
		//saves measured codes to 'tld.tmp.patt', does not considers patches with variance < tmd.var
		vector<int> idx_dt=find_index(tld.tmp.conf,tld.model.num_trees*tld.model.thr_fern,1);
		
		//只取前100的index
		if(int(idx_dt.size())>100){
			vector<int> cmpt = vector<int>(idx_dt);
			std::sort(cmpt.begin(),cmpt.begin()+100);
			cmpt.resize(100);
			idx_dt.clear();
			for(std::vector<int>::iterator it=cmpt.begin();it!=cmpt.end();++it){
				idx_dt.push_back(*it);
			}
		}
		int num_dt = int(idx_dt.size());

		if(num_dt==0){
			tld.dt[I] = dt;
			return;
		}
		// Initialization
		



	}



}
