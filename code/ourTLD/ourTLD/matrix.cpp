#include"matrix.h"
namespace TLD{
	float median(Mat m){
		vector<float> temp=m;
		float ans=0;
		if (temp.size() %2==1){
			int pos=temp.size()/2;
			nth_element(temp.begin(),temp.begin()+pos,temp.end());
			ans=temp[pos];
			temp.clear();
			return ans;
		}
		else{
			int pos=temp.size()/2;
			nth_element(temp.begin(),temp.begin()+pos-1,temp.end());
			ans=temp[pos-1];
			nth_element(temp.begin(),temp.begin()+pos,temp.end());
			ans=(ans+temp[pos])/2;
			temp.clear();
			return ans;
		}
	}
	float median2(Mat m){
		return median(m);
	}
	Mat filterByValue(Mat x,float value,string type){
		Mat y=x.clone();
		if (type=="<="){
			for(int i=0;i<x.cols;i++)
				if (y.at<float>(i)<=value)
					y.at<float>(i)=1;
				else
					y.at<float>(i)=0;
		}
		else if (type==">="){
			for(int i=0;i<x.cols;i++)
				if (y.at<float>(i)>=value)
					y.at<float>(i)=1;
				else
					y.at<float>(i)=0;
		} 
		else if (type=="=="){
			for(int i=0;i<x.cols;i++)
				if (y.at<float>(i)==value)
					y.at<float>(i)=1;
				else
					y.at<float>(i)=0;
		}
		return y;
	}

	bool anyLarger(vector<float> x,float thr){
		for(int i=0;i<x.size();i++)
			if (x[i]>thr)
				return true;
		return false;
	}

	int maxIndex(vector<float> x){
		int ans=0;
		float max=x[0];
		for(int i=1;i<x.size();i++)
			if (x[i]>max){
				max=x[i];
				ans=i;
			}
		return ans;
	}

	float maxValue(vector<float> x){
		float max=x[0];
		for(int i=1;i<x.size();i++)
			if (x[i]>max){
				max=x[i];
			}
		return max;
	}

	Mat ntuples(Mat m1,Mat m2){
		Mat t1=m1.reshape(0,1);
		Mat t2=m2.reshape(0,1);
		Mat ans(t1.cols*t2.cols,2,CV_32F);
		int width=t2.cols;
		for(int i=0;i<t1.cols;i++)
			for(int j=0;j<t2.cols;j++){
				ans.at<float>(i*width+j,0)=t1.at<float>(i);
				ans.at<float>(i*width+j,1)=t2.at<float>(j);
			}
		return ans;
	}

	Mat generateMatByStep(float x,float step,float y){
		int n=floor((y-x)/step)+1;
		Mat ans(1,n,CV_32F);
		for(int i=0;i<ans.cols;i++){
			ans.at<float>(i)=x+step*i;
		}
		return ans;
	}
	Mat normMulCol(Mat m){
		vector<float> x(m.rows);
		for(int i=0;i<m.rows;i++){
			x[i]=norm(m.row(i));
			//for(int j=0;j<m.cols;j++){
			//	x[i]+=m.at<float>(i,j)*m.at<float>(i,j);
			//}
			//x[i]=sqrt(x[i]);
		}
		Mat ans(x,true);
		//Guarantee to be a row
		if (ans.cols==1)
			ans=ans.t();
		return ans;
	}

	Mat distance(Mat x,Mat ex,int type){
		Mat dst;
		gemm(ex,x.t(),1,Mat(x.t()),0,dst);
		float x1=norm(x);
		Mat x2=normMulCol(ex);
		dst/=x1;
		dst/=x2;
		return dst;
	}
	Mat pdist(Mat X){
		int n=X.rows;
		int cnt=0;
		Mat ans(X.rows*(X.rows-1)/2,1,CV_32F);
		for(int i=0;i<n-1;i++)
			for(int j=i+1;j<n;j++){
				ans.at<float>(cnt)=norm(X.row(i)-X.row(j));
				cnt++;
			}
		return ans;
	}

	Mat selectByBool(Mat x,Mat b){
		if (b.cols!=1)
			b=b.reshape(0,b.cols);
		Mat ans;
		for(int i=0;i<x.rows;i++){
			if (b.at<float>(i)==1)
				ans.push_back(x.row(i));
		}
		return ans;
	}

	void printMat(Mat x){
		for(int i=0;i<x.rows;i++){
			for(int j=0;j<x.cols;j++)
				printf("%6.3f ",x.at<float>(i,j));
			printf("\n");
		}
	}
}