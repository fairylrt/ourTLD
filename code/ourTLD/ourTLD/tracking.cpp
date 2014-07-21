#include"tracking.h"
namespace TLD{
//Estimates motion of bounding box BB1 from frame I to frame J
void tldTracking(Rect &BB2,float &Conf,int Valid,Config tld,Rect BB1,int I,int J){
	BB2=Rect();//initialize output variables
	Conf=0;
	BB2 = Rect(); // estimated bounding 
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
	

	
}