#include "algorithms.h"

//============ poisson poly =================

/* Get the sparse matrix A in Ax = b
 * return A: if n,m(both in mask) are neighbors,
 *          then exists i,j make that A[n][i] = m & A[m][j] = n.
 */
vector<vector<int> > getPolySparseA( vector<vector<int> > &MapId, vector<pair<int,int> > &IdMap){
    vector<vector<int> > ans;
    int h = MapId.size();
    int w = MapId[0].size();
    for(int i = 0; i < IdMap.size(); i++){
        vector<int> ind;
        int x = IdMap[i].first;
        int y = IdMap[i].second;
        int cur;
        if( x > 0){
            cur = MapId[x-1][y];
            if(cur >= 0) ind.push_back(cur);
        }
        if( y > 0){
            cur = MapId[x][y-1];
            if(cur >= 0) ind.push_back(cur);
        }
        if( x < h -1){
            cur = MapId[x+1][y];
            if(cur >= 0) ind.push_back(cur);
        }
        if( y < w - 1){
             cur = MapId[x][y+1];
            if(cur >= 0) ind.push_back(cur);
        }
        ans.push_back(ind);
    }
    return ans;
}



/* Compute matrix b for Ax = b
 * img_front : front img
 * img_back : background img
 * roi : roi of front
 * pt : anchor of background
 * B : result, only include pixel in mask.
 */
void getPolyB( Mat &img_front, Mat &img_back, Rect roi, Point pt, Mat &B, vector<vector<int> > &MapId, vector<pair<int,int> > &IdMap){
    Mat Lap;
    int rh = roi.height;
    int rw = roi.width;
    int siz = IdMap.size();
    Mat lk = Mat::zeros( 3, 3, CV_64FC1 );
    double * lkPtr = lk.ptr<double>();
    lkPtr[1] = 1.0;
    lkPtr[3] = 1.0;
    lkPtr[5] = 1.0;
    lkPtr[7] = 1.0;
    lkPtr[4] = -4.0;
    B = Mat::zeros( siz, 1, CV_64FC1);
    //get Lap matrix
    filter2D( img_front, Lap, -1, lk);
    double * BPtr = B.ptr<double>();
    for(int i = 0; i < siz; i++){
        int x = IdMap[i].first;
        int y = IdMap[i].second;
        double tmp = Lap.at<double>(x, y);
        // pixel in mask are neighbor with pixel not in mask.
        // substract the pixel out of mask.
        if(x == 0 || (x > 0 && MapId[x-1][y] < 0)){
            int cur = max(0, pt.y + x -1);
            tmp -= img_back.at<double>(cur, pt.x + y);
        }
        if(x == rh -1 || (x < rh -1 && MapId[x+1][y] < 0)){
            int cur = min(x + 1 + pt.y, img_back.rows - 1);
            tmp -= img_back.at<double>(cur, pt.x + y);
        }
        if(y == 0 || (y > 0 && MapId[x][y-1] < 0)){
            int cur = max(0, y - 1 + pt.x);
            tmp -= img_back.at<double>(x + pt.y, cur);
        }
        if(y == rw - 1 || (y < rw - 1 && MapId[x][y+1] < 0)){
            int cur = min(y + 1 + pt.x, img_back.cols - 1);
            tmp -= img_back.at<double>(x + pt.y, cur);
        }
        BPtr[i] = tmp;
    }
}


/* Get the MapId & IdMap for poly(mask) poisson
 * MapId : MapId(x,y)=-1 if pixel not in mask but in rect
 *         MapId(x,y)=id if pixel in mask, id is the order
 * IdMap : Pixel in mask ordered by i,
 *         corresponding to pixel(x = IdMap[i].first, y = IdMap.second) in rect area.
 */
void getMaskMapTable(Mat &Mask, Rect roi, vector<vector<int> > & MapId, vector<pair<int,int> > &IdMap){
    int rh = roi.height;
    int rw = roi.width;
    int ind = 0;
    MapId.clear();
    IdMap.clear();
    // ind is the No. of the pixel in the mask
    for(int i = 0; i < rh; i++){
        vector<int> cur_row;
        for(int j = 0; j < rw; j++){
            if(Mask.at<double>(i,j) == 0){
                cur_row.push_back(-1);
            }
            else{
                cur_row.push_back(ind);
                pair<int,int> p(i,j);
                IdMap.push_back(p);
                ind ++;
            }
        }
        MapId.push_back(cur_row);
    }
}

/*
 * Poisson fusion(Poly mask area)
 * img_front : front img
 * img_back : background img
 * mask : front mask
 * roi : roi of front
 * pt : anchor of background
 * ans : result img
 */
void polygonPoisson(Mat &img_front, Mat &img_back, Mat &mask, Rect roi, Point pt, Mat &ans){
    int rh = roi.height;
    int rw = roi.width;
    vector<vector<int> > A;
    Mat B;
    long start, end;
    vector <Mat> rgb_f,rgb_b,result;
    split(img_front, rgb_f);
    split(img_back, rgb_b);
    vector<vector<int> > MapId;
    vector<pair<int,int> > IdMap;

    //imshow("msk",mask);
    cout << "rh:" << rh << " rw:" << rw << endl;
    cout << "mask: rows:" << mask.rows << ", cols:" << mask.cols << endl;
    getMaskMapTable(mask,roi,MapId,IdMap);
    cout << "Rect pixel : " << MapId.size() * MapId[0].size() << endl;
    cout << "IdMap size( num of x) : " << IdMap.size() << endl;

    A = getPolySparseA(MapId, IdMap);
    start = time(NULL);
    for(int k = 0; k < img_back.channels(); k++){
        //cout << " For rgb[" << k << "]..." << endl;
        getPolyB(rgb_f[k], rgb_b[k], roi, pt, B, MapId, IdMap);
        Mat res;
        Mat res_rect = Mat::zeros(rh, rw, CV_64FC1);
        double delta = 0.00001;
        long t0 = time(NULL);
        solve_FR_PolySparseA(rw, rh, B, A, res, delta );
        long t1 = time(NULL);
        //cout << "\tCost " << (t1 - t0) * 1000 << " ms.\n" << endl;
        for(int i = 0; i < rh; i++){
            double * Ptr = res_rect.ptr<double>(i);
            for(int j = 0; j < rw; j++){
                if(MapId[i][j] < 0) Ptr[j] = rgb_b[k].at<double>(pt.y + i, pt.x + j);
                else Ptr[j] = res.at<double>(MapId[i][j],0);
            }
        }
        result.push_back( res_rect );
    }
    end = time(NULL);
    cout << "Poly Poisson cost " << (end - start) * 1000 << " ms." << endl;
    cout << " ------------------------------------------------------ \n" << endl;
    merge( result, ans );
}
