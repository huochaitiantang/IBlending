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
    bool collinear;
} polygon;

static int MOUSE_GAP = 1;

bool pointOnSegment(int x, int y, int x1, int y1, int x2, int y2);
bool segmentIntersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);

polygon createPolygon();
bool canAddPointToPolygon(polygon pg, int x, int y);
void addPointToPolygon(polygon* pg, int x, int y);
bool validPolygon(polygon pg);
void clearPolygon(polygon* pg);

bool nearPoint(int x,int y,int cx,int cy);

#endif // TOOLS_H
