#include "algorithms.h"

//============ api =================
/* API for using algorithms
 */
Mat getFusionMat(const Mat &back, const Mat &front, const Mat &mask, Rect b_roi, int type){
    Mat ans = back.clone();
    Rect roi = Rect(0, 0, b_roi.width, b_roi.height);
    Point center( b_roi.x + b_roi.width / 2, b_roi.y + b_roi.height / 2 );
    long t1 = time(NULL);
    if(type == 0){
        cout << "Poisson Opencv3.2 Normal Clone " << endl;
        seamlessClone( front, back, mask, center, ans, NORMAL_CLONE );
    }
    else if(type == 1){
        cout << "Poisson Opencv3.2 Mixed Clone " << endl;
        seamlessClone( front, back, mask, center, ans, MIXED_CLONE );
    }
    else if(type == 2 || type == 3){
        Mat res, in1, in2, msk;
        vector<Mat> msk_v;
        back.convertTo(in1, CV_64FC3);
        front.convertTo(in2, CV_64FC3);
        mask.convertTo(msk, CV_64FC3);
        split(msk,msk_v);
        msk = msk_v[0];
        print_mat_info( in1, "background" );
        print_mat_info( in2, "roi_front" );
        print_mat_info( msk, "mask" );
        if(type == 2){
            cout << "Poisson Own Rect ROI By FR Solver " << endl;
            poisson(in2, in1, roi, b_roi.tl(), res);
        }
        else{
            cout << "Poisson Own Poly ROI By FR Solver " << endl;
            polygonPoisson(in2, in1, msk, roi, b_roi.tl(), res);
        }
        res.convertTo(res, CV_8UC3);
        Mat roimat = ans(b_roi);
        res.copyTo(roimat);
    }
    else if(type == 4){
        cout << "Poisson Own Drag Drop Solver" << endl;
        Mat res, msk, in1, in2;
        vector<Mat> msk_v;
        split(mask,msk_v);
        msk = msk_v[0];
        in1 = back;
        in2 = front;
        edgePoisson(in2, in1, msk, roi, b_roi.tl(), ans);
    }
    else{
         cout << "No type [" << type << "] fusion!" << endl;
    }
    long t2 = time(NULL);
    cout << "Fusion type [" << type << "] Cost "<<(t2-t1)*1000 << " ms." << endl;
    return ans;
}


//============ poisson =================
/* Compute the index
 * i,j : cordinate
 * w : size of column
 */
int get_ind(int i, int j, int w){
    return i * w + j;
}

/* Get the matrix A in Ax = b
 * A : result
 * h : img height
 * w : img width
 */
void  getA(Mat &A, int h, int w){
    A = Mat::eye( h * w, h * w, CV_64FC1);
    A *= -4;
    //corner has 2 neighbors, border has 3 neighbors, other has 4 neighbors
    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
            int label = get_ind(i, j, w);
            double * ALinePtr = A.ptr<double>(label);
            if ( i > 0 ) ALinePtr[ get_ind( i - 1, j, w ) ] = 1;
            if ( j > 0 ) ALinePtr[ get_ind( i, j - 1, w ) ] = 1;
            if ( i < h - 1 ) ALinePtr[ get_ind( i + 1, j, w ) ] = 1;
            if ( j < w - 1 ) ALinePtr[ get_ind( i, j + 1, w ) ] = 1;
        }
    }
}

/* Get the sparse matrix A in Ax = b
 * A : result
 * h : img height
 * w : img width
 */
vector< vector< int > >  get_sparseA(int h, int w){
    vector< vector< int > > ans;
    //corner has 2 neighbors, border has 3 neighbors, other has 4 neighbors
    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
            vector<int> ind;
            if ( i > 0 ) ind.push_back( get_ind( i - 1, j, w ) );
            if ( j > 0 ) ind.push_back( get_ind( i, j - 1, w ) );
            if ( i < h - 1 ) ind.push_back( get_ind( i + 1, j, w ) );
            if ( j < w - 1 ) ind.push_back( get_ind( i, j + 1, w ) );
            ans.push_back(ind);
        }
    }
    return ans;
}

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
 * B : result
 */
void getB( Mat &img_front, Mat &img_back, Rect roi, Point pt, Mat &B){
    Mat Lap;
    int rh = roi.height;
    int rw = roi.width;
    Mat lk = Mat::zeros( 3, 3, CV_64FC1 );
    double * lkPtr = lk.ptr<double>();
    lkPtr[1] = 1.0;
    lkPtr[3] = 1.0;
    lkPtr[5] = 1.0;
    lkPtr[7] = 1.0;
    lkPtr[4] = -4.0;
    B = Mat::zeros( rh * rw, 1, CV_64FC1);
    //get Lap matrix
    filter2D( img_front, Lap, -1, lk);
    double * BPtr = B.ptr<double>();
    for(int i = 0; i < rh; i++){
        for( int j = 0; j < rw; j++){
            double tmp = 0.0;
            tmp += Lap.at<double>( (i + roi.y ), ( j + roi.x ) );
            //border lap value should substract background pixel value
            if( i == 0){
                int cur = max(i - 1 + pt.y, 0);
                tmp -= img_back.at<double>(cur, j + pt.x );
            }
            else if( i == rh - 1){
                int cur = min(i + 1 + pt.y, img_back.rows - 1);
                tmp -= img_back.at<double>(cur, j + pt.x );
            }
            if( j == 0){
                int cur = max(j - 1 + pt.x, 0);
                tmp -= img_back.at<double>( i + pt.y, cur);
            }
            else if( j == rw - 1 && pt.x < img_back.cols - 1){
                int cur = min(j + 1 + pt.x, img_back.cols - 1);
                tmp -= img_back.at<double>( i + pt.y, cur);
            }
            BPtr[ get_ind( i, j, rw ) ] = tmp;
        }
    }
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

/*
 * Poisson fusion(Rect area)
 * img_front : front img
 * img_back : background img
 * roi : roi of front
 * pt : anchor of background
 * ans : result img
 */
void poisson(Mat &img_front, Mat &img_back, Rect roi, Point pt, Mat &ans){
    int rh = roi.height;
    int rw = roi.width;
    Mat A, B;
    long start, end;
    //getA( A, rh, rw);
    print_mat_info(A,"A");
    vector <Mat> rgb_f,rgb_b,result;
    split(img_front, rgb_f);
    split(img_back, rgb_b);
    start = time(NULL);
    for(int k = 0; k < img_back.channels(); k++){
        cout << " For rgb[" << k << "]..." << endl;
        getB(rgb_f[k], rgb_b[k], roi, pt, B);
        Mat ans3;
        double delta = 0.00001;
        // 1 guassian solve
        //solve( A, B, ans, DECOMP_LU );

        // 2 solve FR with big A
        //solve_FR( A, B, ans2, delta );

        // 3 solve FR with sparse A
        long t3 = time(NULL);
        solve_FR_SparseA(rw, rh, B, ans3, delta );
        long t4 = time(NULL);
        cout << "\tCost " << (t4 - t3) * 1000 << " ms." << endl;
        ans3 = ans3.reshape(0, rh);
        result.push_back( ans3 );
    }
    end = time(NULL);
    cout << " Poisson cost " << (end - start) * 1000 << " ms." << endl;
    cout << " ------------------------------------------------------ " << endl;
    merge( result, ans );
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
        cout << " For rgb[" << k << "]..." << endl;
        getPolyB(rgb_f[k], rgb_b[k], roi, pt, B, MapId, IdMap);
        Mat res;
        Mat res_rect = Mat::zeros(rh, rw, CV_64FC1);
        double delta = 0.00001;
        long t0 = time(NULL);
        solve_FR_PolySparseA(rw, rh, B, A, res, delta );
        long t1 = time(NULL);
        cout << "\tCost " << (t1 - t0) * 1000 << " ms.\n" << endl;
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

/* EdgePoisson(drag and drop not implementation)
 *
 */
void edgePoisson(Mat &img_front, Mat &img_back, Mat &mask, Rect roi, Point pt, Mat &ans){
    int rh = roi.height;
    int rw = roi.width;
    long start, end;
    Mat grab_mask, bgdModel, fgdModel;
    Mat obj_mask, edge_mask;
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
                EPtr[j] = OPtr[j] = Ptr[j] = 0 ;
            else if(Ptr[j] == GC_PR_BGD){
                Ptr[j] = 128;
                OPtr[j] = 0;
                EPtr[j] = 255;
            }
            else if(Ptr[j] == GC_PR_FGD) {
                OPtr[j] = Ptr[j] = 255;
                EPtr[j] = 0;
            }
        }
    }
    imshow("grab_mask",grab_mask);
    //imshow("edge_mask",edge_mask);
    imshow("obj_mask",obj_mask);

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
    Point center( pt.x + rw / 2, pt.y + rh / 2 );
    seamlessClone(img_front,img_back,obj_mask,center, ans, NORMAL_CLONE);

    //imshow("ans",ans);
    //img_front.copyTo(ans);
}


//============ lmath =================

/* get matrix like
 * -4 1 0 0 0 0
 * 1 -4 1 0 0 0
 * 0 1 -4 1 0 0
 * 0 0 1 -4 1 0
 * 0 0 0 1 -4 1
 * 0 0 0 0 1 -4
 */
void get_a(Mat &a, int w){
    a = Mat::zeros( w, w, CV_64FC1 );
    for(int i = 0; i < w; i++){
        double * aLinePtr = a.ptr<double>(i);
        aLinePtr[i] = -4;
        if(i > 0) aLinePtr[i-1] = 1;
        if(i < w - 1) aLinePtr[i+1] = 1;
    }
}

/* calculate the det(a)
 */
double cal_a_det(int n){
    double sq3, x1, x2, y1, y2, res;
    sq3 = sqrt(3);
    x1 = 2 + ( 7.0 / 6 ) * sq3;
    x2 = 2 - ( 7.0 / 6 ) * sq3;
    y1 = -2 - sq3;
    y2 = -2 + sq3;
    //cout << x1 << " " << x2 << " " << y1 << " " << y2 << endl;
    res = x1 * pow( y1, n - 1 ) + x2 * pow( y2, n - 1 );
    res = -res;
    return res;
}

/* print a float mat
 */
void print_float_mat(Mat m){
    cout << "[" << endl;
    for(int i = 0; i < m.rows; i++) {
        for( int j = 0; j < m.cols; j++){
            double x = m.at<double>(i,j);
            if((x == 0.0)||(x>0 && x<0.0001)||(x<0 && x>-0.0001)) cout << "0, ";
            //else cout << setprecision(10) << (x*abs(cal_a_det(m.rows))) << ", ";
            else cout << setprecision(5) << (int)(1/x) << ", ";
        }
        cout << ";" << endl;
    }
    cout << "]" << endl;

}

/* calculate the invert of a
 */
void cal_a_inv(int n, Mat &ans){
    ans = Mat::zeros( n, n, CV_64FC1 );
    double fac[10000];
    for( int i = 0; i <= n; i++ ) fac[i] = abs( cal_a_det(i) );
    for ( int i = 0; i < n; i++ ){
        for ( int j = i; j < n; j++ ){
            ans.at<double>(i,j) = - fac[i] * fac[n-j-1] / fac[n];
        }
    }
    for( int i = 0; i < n; i++ ){
        for( int j = i + 1; j < n; j++) ans.at<double>(j,i) = ans.at<double>(i,j);
    }
    //return ans;
}

/* judge if x < y */
bool less_than( Mat &x, Mat &y ){
    int h = std::min( x.rows, y.cols );
    double * xPtr = x.ptr<double>();
    double * yPtr = y.ptr<double>();
    for( int i = 0; i < h; i++ ){
        if( abs(xPtr[i]) > yPtr[i] ) return false;
    }
    return true;
}

/*
 * conjugate gradient method solve for Ax = b
 * A, ans, b : matrix A x b
 * delta : precision
 */
void solve_FR( Mat &A, Mat &b, Mat &ans, double delta ){
    if ( A.cols != A.rows || A.rows != b.rows || b.cols != 1 ){
        cout << "ERROR : Solver_FR Failed!" << endl;
        return;
    }
    int k = 0, h = b.rows;
    Mat del = Mat::ones( h, 1, CV_64FC1 );
    del *= delta;
    Mat tmp;
    Mat x = Mat::zeros( h, 1, CV_64FC1 );
    Mat p = Mat::zeros( h, 1, CV_64FC1 );
    Mat r = b.clone();
    double rTr_2, alpha;
    tmp = r.t() * r ;
    double rTr_1 = tmp.at<double>(0,0);
    while( less_than( r , del ) == false ){
        k++;
        if( k == 1 ){
            r.copyTo( p );
        }
        else{
            p = ( r + ( rTr_1 / rTr_2 ) * p );
        }
        Mat Ap = A * p;
        tmp = p.t() * Ap;
        alpha = rTr_1 / tmp.at<double>(0,0);
        x = ( x + alpha * p );
        r = ( r - alpha * Ap );

        rTr_2 = rTr_1;
        tmp = r.t() * r;
        rTr_1 = tmp.at<double>(0,0);
    }
    cout << " solve FR for step " << k << " ." << endl;
    x.copyTo(ans);
}

/* calculate for a*p
 */
Mat cal_ap( int rw, Mat &p ){
    Mat ans = Mat::zeros( rw, 1, CV_64FC1 );
    double * pPtr = p.ptr<double>();
    double * ansPtr = ans.ptr<double>();
    ansPtr[0] = -4 * pPtr[0] + pPtr[1];
    for( int i = 0; i < rw -1; i++ )
        ansPtr[i] = -4 * pPtr[i] + pPtr[i - 1] + pPtr[i + 1];
    ansPtr[rw - 1] = -4 * pPtr[rw - 1] + pPtr[rw - 2];
    return ans;
}

/* calculate for A*p
 * A like
 * a i o o o,
 * i a i o o,
 * o i a i 0,
* o o i a i,
 * o o o i a,
 * i,a : size(w,w)
 * p : size(w*h,1)
 * A can not be stored because it is too large and sparse
 */
Mat cal_Ap( int rw, int rh, Mat &p ){
    Mat a,ans;
    vector<Mat> P;
    vector<Mat> ANS;
    //get_a( a, rw );
    for( int i = 0; i < rh; i++ ){
        Mat tmp;
        p( Rect( 0 , rw * i , 1, rw ) ).copyTo(tmp);
        P.push_back(tmp);
    }
    //ANS.push_back( a * P[0] + P[1] );
    ANS.push_back( cal_ap( rw, P[0] ) + P[1] );
    for( int i = 1; i < rh - 1; i++ ){
        //ANS.push_back( P[i-1] + a * P[i] + P[i+1] );
        ANS.push_back( P[i-1] + cal_ap( rw, P[i] ) + P[i+1] );
    }
    //ANS.push_back( P[rh-2] + a * P[rh-1] );
    ANS.push_back( P[rh-2] + cal_ap( rw, P[rh-1] ) );
    int k = 0;
    ans = Mat::zeros( rw * rh, 1, CV_64FC1 );
    double * ansPtr = ans.ptr<double>();
    for( int i = 0; i < rh; i++ ){
        double * tmpPtr = ANS[i].ptr<double>();
        for( int j = 0; j < rw; j++ ){
            ansPtr[k++] = tmpPtr[j];
        }
    }
    return ans;
}

/* slower than cal_Ap */
Mat cal_Ap2( vector< vector< int > > A, Mat &p ){
    Mat ans = Mat::zeros( A.size(), 1, CV_64FC1 );
    double * pPtr = p.ptr<double>();
    double * ansPtr = ans.ptr<double>();
    for(int i = 0; i < A.size(); i++ ){
        double tmp = -4 * pPtr[i];
        for(int j = 0; j < A[i].size(); j++ ){
            tmp += pPtr[ A[i][j] ];
        }
        ansPtr[i] = tmp;
    }
    return ans;
}

/*
 * conjugate gradient method solve for Ax = b, A
 * ans, b : matrix x b
 * rw : child matrix width
 * rh : num of chlid matrix in each line
 * delta : precision
 */
void solve_FR_SparseA( int rw, int rh, Mat &b, Mat &ans, double delta ){
    if ( b.rows != rw * rh || b.cols != 1 ){
        cout << "ERROR : Solver_FR Failed!" << endl;
        return;
    }
    int k = 0, h = b.rows;
    //vector< vector < int > > A = get_sparseA( rh, rw );
    Mat del = Mat::ones( h, 1, CV_64FC1 );
    del *= delta;
    Mat tmp;
    Mat x = Mat::zeros( h, 1, CV_64FC1 );
    Mat p = Mat::zeros( h, 1, CV_64FC1 );
    Mat r = b.clone();
    double rTr_2, alpha;
    tmp = r.t() * r ;
    double rTr_1 = tmp.at<double>();
    while( less_than( r , del ) == false ){
        k++;
        if( k == 1 ){
            r.copyTo( p );
        }
        else{
            p = ( r + ( rTr_1 / rTr_2 ) * p );
        }
        Mat Ap = cal_Ap( rw, rh, p);
        //Mat Ap = cal_Ap2( A, p);
        tmp = p.t() * Ap;
        alpha = rTr_1 / tmp.at<double>();
        x = ( x + alpha * p );
        r = ( r - alpha * Ap );

        rTr_2 = rTr_1;
        tmp = r.t() * r;
        rTr_1 = tmp.at<double>();
    }
    cout << " solve FR_SpareA for step " << k << " ." << endl;
    x.copyTo(ans);
}

/*
 * conjugate gradient method solve for Ax = b, A
 * ans, b : matrix x b
 * rw : child matrix width
 * rh : num of chlid matrix in each line
 * A : coefficient matrix
 * delta : precision
 */
void solve_FR_PolySparseA( int rw, int rh, Mat &b, vector<vector<int> > &A, Mat &ans, double delta ){
    int k = 0, h = b.rows;
    Mat del = Mat::ones( h, 1, CV_64FC1 );
    del *= delta;
    Mat tmp;
    Mat x = Mat::zeros( h, 1, CV_64FC1 );
    Mat p = Mat::zeros( h, 1, CV_64FC1 );
    Mat r = b.clone();
    double rTr_2, alpha;
    tmp = r.t() * r ;
    double rTr_1 = tmp.at<double>();
    while( less_than( r , del ) == false ){
        k++;
        if( k == 1 ){
            r.copyTo( p );
        }
        else{
            p = ( r + ( rTr_1 / rTr_2 ) * p );
        }
        Mat Ap = cal_Ap2( A, p);
        tmp = p.t() * Ap;
        alpha = rTr_1 / tmp.at<double>();
        x = ( x + alpha * p );
        r = ( r - alpha * Ap );

        rTr_2 = rTr_1;
        tmp = r.t() * r;
        rTr_1 = tmp.at<double>();
    }
    cout << " solve FR_PolySpareA for step " << k << " ." << endl;
    x.copyTo(ans);
}


//============ tools =================

/* Print mat infomation
 * mat: img
 * s: mat name
 */
int print_mat_info(Mat mat, const char* s){
    cout << " -- " << s << ":" << endl;
    cout << "\tcti:" << mat.isContinuous();
    cout << " cha:" << mat.channels();
    cout << " col:" << mat.cols;
    cout << " row:" << mat.rows << endl;
    cout << endl;
    return 0;
}

/* Simply replace the part of img obj with the part of img src
 * obj : object img
 * src : source img
 * pt : object cordinate anchor
 * roi : source roi part
 * ans : result img
 */
void simple_replace(const Mat& obj, Point pt, const Mat &src, Rect roi, Mat& ans){
    ans = obj.clone();
    int c = src.channels();
    int orow = obj.rows;
    int ocol = obj.cols;
    if(obj.channels() != src.channels()){
        cout << "ERROR: obj and src channels not matched!" << endl;
        return;
    }
    for(int i = pt.y; i < pt.y + roi.height && i < orow; i++){
        for(int j = pt.x; j < pt.x + roi.width && j < ocol; j++){
            for(int k = 0; k < c; k++){
                ans.at<Vec3b>(i,j)[k] = src.at<Vec3b>( i - pt.y + roi.y, j - pt.x + roi.x )[k];
            }
        }
    }
}

/*
 * Check if the roi of img is valid
 */
bool valid_roi(const Mat& img, Rect roi){
    if(	roi.x < 0 || roi.x >= img.cols ||
        roi.y < 0 || roi.y >= img.rows ||
        roi.x + roi.width > img.cols ||
        roi.y + roi.height > img.rows ) return false;
    else return true;
}

