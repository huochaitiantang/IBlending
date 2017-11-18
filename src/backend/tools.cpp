#include "algorithms.h"

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


/*
 * Draw the IdMap and MapId
 */
void test_IdMap_MapId(vector<vector<int> > & MapId, vector<pair<int,int> > &IdMap){
    cout << "-------------------Test-------------------------" << endl;
    int rh = MapId.size();
    int rw = MapId[0].size();
    vector<string> test_pic;
    for(int i = 0; i < rh; i++){
        string s = "";
        for(int j = 0; j < rw; j++){
            s += " ";
        }
        s += "|\n";
        test_pic.push_back(s);
    }
    for(int i = 0; i < rh; i++){
        for(int j = 0; j < rw; j++){
            if(MapId[i][j] >=0 ){
                test_pic[i][j] = '*';
            }
        }
    }
    for(int i = 0; i < rh; i++) cout << test_pic[i];
    for(int i = 0; i < IdMap.size(); i++){
        test_pic[IdMap[i].first][IdMap[i].second] = '#';
    }
    for(int i = 0; i < rh; i++) cout << test_pic[i];
    cout << "-------------------End-------------------------" << endl;

}
