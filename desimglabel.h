#ifndef DESIMGLABEL_H
#define DESIMGLABEL_H

#include "imglabel.h"

class DesImgLabel : public ImgLabel
{
    public:
        QImage *subimage;
        int move_anchor_x, move_anchor_y;
        bool hasSubImg = false;
        int subx, suby;
        int subw, subh;
        void poisson();
        DesImgLabel(QLabel *info):ImgLabel(info){}

    protected:
        int inRectLine(int x, int y);
        void mouseMoveEvent(QMouseEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void paintEvent(QPaintEvent *event);
};

#endif // DESIMGLABEL_H
