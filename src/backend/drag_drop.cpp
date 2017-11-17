#include "algorithms.h"

//============ drag and drop =================

/*
 * get aera between boundary given by user and boundary given by grabcut
 */
 void getBandMask(Mat &img_front, Mat &mask, Mat &edge_mask, Mat &obj_mask){
     int rh = img_front.rows;
     int rw = img_front.cols;
     Mat grab_mask, bgdModel, fgdModel;
     mask.copyTo(grab_mask);
     for(int i = 0; i < rh; i++){
         uchar * Ptr = grab_mask.ptr<uchar>(i);
         for(int j = 0; j < rw; j++){
             if(Ptr[j] == 0)
                 Ptr[j] = GC_BGD;
             else
                 Ptr[j] = GC_PR_FGD;
         }
     }
     grabCut( img_front, grab_mask, Rect(0, 0, rw, rh), bgdModel, fgdModel, 1, GC_INIT_WITH_MASK );
     grabCut( img_front, grab_mask, Rect(0, 0, rw, rh), bgdModel, fgdModel, 10);
     grab_mask.copyTo(obj_mask);
     grab_mask.copyTo(edge_mask);
     for(int i = 0; i < rh; i++){
         uchar * Ptr = grab_mask.ptr<uchar>(i);
         uchar * OPtr = obj_mask.ptr<uchar>(i);
         uchar * EPtr = edge_mask.ptr<uchar>(i);
         for(int j = 0; j < rw; j++){
             if(Ptr[j] == GC_BGD)
                 EPtr[j] = OPtr[j] = 0 ;
             else if(Ptr[j] == GC_PR_BGD){
                 OPtr[j] = 0;
                 EPtr[j] = 255;
             }
             else if(Ptr[j] == GC_PR_FGD) {
                 OPtr[j]= 255;
                 EPtr[j] = 0;
             }
         }
     }
 }

/*
 * given a mask and the map relationship, get the boundary point set
 */
void getMaskBoundary(vector<vector<int> > & MapId, vector<pair<int,int> > &IdMap, vector<pair<int,int> > &BoundaryIdMap){
    int h = MapId.size();
    int w = MapId[0].size();
    BoundaryIdMap.clear();
    for(int i = 0; i < IdMap.size(); i++){
        int x = IdMap[i].first;
        int y = IdMap[i].second;
        if(x == 0 || y == 0 || x == h - 1 || y == w -1){
            pair<int,int> p(x,y);
            BoundaryIdMap.push_back(p);
        }
        else if(MapId[x-1][y] == -1 || MapId[x+1][y] == -1 ||
                MapId[x][y-1] == -1 || MapId[x][y+1] == -1){
            pair<int,int> p(x,y);
            BoundaryIdMap.push_back(p);
        }
    }
    int siz = BoundaryIdMap.size();
    cout << "Boundary Num:" << siz << endl;
    for(int j = 0; j < h; j++){
    for(int k = 0; k < w; k++){
    	int i;
	for(i = 0; i < siz; i++){
    		if(j == BoundaryIdMap[i].first && k == BoundaryIdMap[i].second) break;
    	}
	if(i < siz) cout << "*";
	else cout << " ";
    }
    cout << endl;
	}

}

/* dragDrop(drag and drop not implementation)
 *
 */
void dragDrop(Mat &img_front, Mat &img_back, Mat &mask, Rect roi, Point pt, Mat &ans){
    int rh = roi.height;
    int rw = roi.width;

    Mat obj_mask, edge_mask;
    getBandMask(img_front,mask,edge_mask,obj_mask);

    imshow("mask",mask);
    imshow("edge_mask",edge_mask);
    imshow("obj_mask",obj_mask);

    vector<vector<int> > MapId;
    vector<pair<int,int> > IdMap;
    vector<pair<int,int> > BoundaryIdMap;
    getMaskMapTable(mask,roi,MapId,IdMap);
    getMaskBoundary(MapId, IdMap, BoundaryIdMap);



    //Mat new_back, s_new_back, new_front;
    //img_back.copyTo(new_back);
    //img_front.copyTo(new_front);

    //Mat roimat = new_back(Rect(pt.x, pt.y, rw, rh));
    //roimat.copyTo(s_new_back);
    //s_new_back.copyTo(new_front, edge_mask);
    //new_front.copyTo(s_new_back, obj_mask);
    //s_new_back.copyTo(roimat);

    //imshow("s_new_back", s_new_back);
    //imshow("new_front", new_front);
    //imshow("new_back", new_back);
    //Point center( pt.x + rw / 2, pt.y + rh / 2 );
    //seamlessClone(img_front,img_back,obj_mask,center, ans, NORMAL_CLONE);

    //imshow("ans",ans);
    //img_front.copyTo(ans);
}
