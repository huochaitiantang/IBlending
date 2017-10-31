#include "algorithms.h"

//============ poisson rect =================

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
