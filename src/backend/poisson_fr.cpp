#include "algorithms.h"

//============ poisson with FR solver =================



/* Compute matrix b for Ax = b,
 * if mask is not rect then fill out with background gradient
 * img_front : front img
 * img_back : background img
 * mask : interesting area, rect or poly
 * roi : roi of front
 * pt : anchor of background
 * B : result
 */
void getBWithMask( Mat &img_front, Mat &img_back, Mat &mask, Rect roi, Point pt, Mat &B){
    Mat Lap;
    Mat Lap_back;
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
    filter2D( img_back, Lap_back, -1, lk);
    double * BPtr = B.ptr<double>();
    for(int i = 0; i < rh; i++){
        for( int j = 0; j < rw; j++){
            double tmp = 0.0;
            if(mask.at<double>(i, j) > 0){
                tmp += Lap.at<double>( (i + roi.y ), ( j + roi.x ) );
            }
            else{
                tmp += Lap_back.at<double>((i + pt.y), (j + pt.x));
            }
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
 * mask : mask of interest, rect or poly
 * roi : roi of front
 * pt : anchor of background
 * ans : result img
 */
void poissonFR(Mat &img_front, Mat &img_back, Mat &mask, Rect roi, Point pt, Mat &ans){
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
        //cout << " For rgb[" << k << "]..." << endl;
        getBWithMask(rgb_f[k], rgb_b[k], mask, roi, pt, B);
        Mat ans3;
        double delta = 0.00001;
        // 1 guassian solve
        //solve( A, B, ans, DECOMP_LU );

        // 2 solve FR with big A
        //solve_FR( A, B, ans2, delta );

        // 3 solve FR with sparse A
        //long t3 = time(NULL);
        solve_FR_SparseA(rw, rh, B, ans3, delta );
        //long t4 = time(NULL);
        //cout << "\tCost " << (t4 - t3) * 1000 << " ms." << endl;
        ans3 = ans3.reshape(0, rh);
        result.push_back( ans3 );
    }
    end = time(NULL);
    cout << " Poisson cost " << (end - start) * 1000 << " ms." << endl;
    cout << " ------------------------------------------------------ " << endl;
    merge( result, ans );
}
