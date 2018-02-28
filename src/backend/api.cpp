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
    else if(type == 2 || type == 3 || type == 5){
        Mat res, in1, in2, msk;
        Mat back_with_border;
        vector<Mat> msk_v;
        //add border
        copyMakeBorder(back, back_with_border, 1, 1, 1, 1, BORDER_REPLICATE, Scalar(1,1,1));
        //imshow("back",back);
        //imshow("back-border",back_with_border);
        back_with_border.convertTo(in1, CV_64FC3);
        front.convertTo(in2, CV_64FC3);
        mask.convertTo(msk, CV_64FC3);
        split(msk,msk_v);
        msk = msk_v[0];
        print_mat_info( in1, "background" );
        print_mat_info( in2, "roi_front" );
        print_mat_info( msk, "mask" );
        Point tl_with_border = Point(b_roi.x + 1, b_roi.y + 1);
        if(type == 2){
            cout << "Poisson Own Rect ROI By FR Solver " << endl;
            poisson(in2, in1, roi, tl_with_border, res);
        }
        else if(type == 3){
            cout << "Poisson Own Poly ROI By FR Solver " << endl;
            polygonPoisson(in2, in1, msk, roi, tl_with_border, res);
        }
        else {
            cout << "Poisson By FR Solver " << endl;
            poissonFR(in2, in1, msk, roi, tl_with_border, res);
        }
        res.convertTo(res, CV_8UC3);
        Mat roimat = ans(b_roi);
        res.copyTo(roimat);
    }
    else if(type == 4){
        cout << "Poisson Own Drag Drop Solver" << endl;
        Mat res, msk, in1, in2;
        vector<Mat> msk_v;
        mask.convertTo(msk, CV_64FC3);
        split(msk,msk_v);
        msk = msk_v[0];
        in1 = back;
        in2 = front;
        dragDrop(in2, in1, msk, roi, b_roi.tl(), ans);
    }
    else{
         cout << "No type [" << type << "] fusion!" << endl;
    }
    long t2 = time(NULL);
    cout << "Fusion type [" << type << "] Cost "<<(t2-t1)*1000 << " ms." << endl;
    return ans;
}
