#ifndef SRCIMGLABEL_H
#define SRCIMGLABEL_H

#include "imglabel.h"

class SrcImgLabel : public ImgLabel
{
    public:
        int minx, miny, maxx, maxy;
        bool selectOver = false;
        SrcImgLabel(QLabel *info):ImgLabel(info){}

    protected:
        int inRectLine(int x, int y);
        void mouseMoveEvent(QMouseEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void paintEvent(QPaintEvent *event);

};

#endif // SRCIMGLABEL_H
