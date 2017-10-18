#ifndef UTIL_H
#define UTIL_H

#include <QImage>
#include "../gradient_fusion/common.hpp"

Mat QImage2cvMat(QImage *image);
QImage cvMat2QImage(const Mat& mat);
String getPureName(String s);

typedef struct pg{
    vector<int> x;
    vector<int> y;
} polygon;

static int MOUSE_GAP = 1;
static double POINT_SEGMENT_THRESH = 5.0;

bool pointOnSegment(int x, int y, int x1, int y1, int x2, int y2);
bool segmentIntersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
double pointDistanceSegment(int x, int y, int x1, int y1, int x2, int y2);
int mulv(int x1, int y1, int x2, int y2);
double modv(int x,int y);

polygon createPolygon();
bool canAddPointToPolygon(polygon pg, int x, int y);
void addPointToPolygon(polygon* pg, int x, int y);
bool validPolygon(polygon pg);
void clearPolygon(polygon* pg);
bool canReplacePointInPolygon(polygon pg, int ind, int x, int y);
void replacePointInPolygon(polygon* pg, int ind, int x, int y);
bool canInsertPointInPolygon(polygon pg, int ind, int x, int y);
void insertPointInPolygon(polygon* pg, int ind, int x, int y);
void deletePointInPolygon(polygon* pg, int ind);

void getPolygonRect(polygon pg,int* minx,int* miny, int* maxx, int* maxy);
void minPolygon(polygon* pg, int minx, int miny);
void print_poly(polygon pg);
void getPolygonMask(polygon pg, int w, int h, vector<vector<int> > &msk);

double getIntersectX(int y, int x1, int y1, int x2, int y2);
bool nearPoint(int x,int y,int cx,int cy);
double getSlope(int x1, int y1, int x2, int y2);

void copyPolygon(polygon pg, polygon * pg2);

#endif // TOOLS_H
