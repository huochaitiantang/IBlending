#ifndef ALGORITHMS_H
#define ALGORITHMS_H
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <math.h>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace cv;
using namespace std;

//api.cpp
Mat getFusionMat(const Mat &back, const Mat &front, const Mat &mask, Rect b_roi, int type);

//tools.cpp
int print_mat_info(Mat mat, const char* s);
void simple_replace(const Mat& obj, Point pt, const Mat &src, Rect roi, Mat& ans);
bool valid_roi(const Mat& img, Rect roi);
void test_IdMap_MapId(vector<vector<int> > & MapId, vector<pair<int,int> > &IdMap);

//solve_equation.cpp
void get_a(Mat &a, int w);
double cal_a_det(int n);
void print_float_mat(Mat m);
void cal_a_inv(int n, Mat &ans);
bool less_than( Mat &x, Mat &y );
void solve_FR( Mat &A, Mat &b, Mat &ans, double delta );
Mat cal_Ap( int rw, int rh, Mat &p );
void solve_FR_SparseA( int rw, int rh, Mat &b, Mat &ans, double delta );
void solve_FR_PolySparseA( int rw, int rh, Mat &b, vector<vector<int> > &A, Mat &ans, double delta );

//poisson_rect.cpp
int get_ind(int i, int j, int w);
void getA(Mat &A, int h, int w);
vector< vector< int > >  get_sparseA(int h, int w);
void getB( Mat &img_front, Mat &img_back, Rect roi, Point pt, Mat &B);
void poisson(Mat &img_front, Mat &img_back, Rect roi, Point pt, Mat &ans);

//poisson_poly.cpp
void getMaskMapTable(Mat &Mask, Rect roi, vector<vector<int> > & MapId, vector<pair<int,int> > &IdMap);
void polygonPoisson(Mat &img_front, Mat &img_back, Mat &mask, Rect roi, Point pt, Mat &ans);
void getPolyB( Mat &img_front, Mat &img_back, Rect roi, Point pt, Mat &B, vector<vector<int> > &MapId, vector<pair<int,int> > &IdMap);
vector<vector<int> > getPolySparseA( vector<vector<int> > &MapId, vector<pair<int,int> > &IdMap);

//poisson.cpp
void getBWithMask( Mat &img_front, Mat &img_back, Mat &mask, Rect roi, Point pt, Mat &B);
void poissonFR(Mat &img_front, Mat &img_back, Mat &mask, Rect roi, Point pt, Mat &ans);

//drag_drop.cpp
void getBandMask(Mat &img_front, Mat &mask, Mat &edge_mask, Mat &obj_mask);
void getMaskBoundary(Mat &mask, vector<vector<int> > & MapId, vector<pair<int,int> > &IdMap, vector<pair<int,int> > &BoundaryIdMap);
void dragDrop(Mat &img_front, Mat &img_back, Mat &mask, Rect roi, Point pt, Mat &ans);


#endif // ALGORITHMS_H
