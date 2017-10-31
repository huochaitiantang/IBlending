#include "algorithms.h"

//============ drag and drop =================

/* dragDrop(drag and drop not implementation)
 *
 */
void dragDrop(Mat &img_front, Mat &img_back, Mat &mask, Rect roi, Point pt, Mat &ans){
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
