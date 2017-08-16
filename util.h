#ifndef UTIL_H
#define UTIL_H

#include <QImage>
#include "../gradient_fusion/common.hpp"

Mat QImage2cvMat(QImage *image);
QImage cvMat2QImage(const Mat& mat);
String getPureName(String s);

#endif // TOOLS_H
