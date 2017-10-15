#ifndef SRCIMGLABEL_H
#define SRCIMGLABEL_H

#include "imglabel.h"

class SrcImgLabel : public ImgLabel
{
    public:
        int minx, miny, maxx, maxy;
        bool selectOver = false;
        SrcImgLabel(QLabel *info):ImgLabel(info){}
        void clear_select();
        void open_img(QScrollArea *scroll_area);
        bool replace_point = false;
        int replace_ind = 0;
        bool can_replace = false;

    protected:
        int inRectLine(int x, int y);
        void mouseMoveEvent(QMouseEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void paintEvent(QPaintEvent *event);

        void rectMouseMoveEvent(QMouseEvent *event);
        void rectMousePressEvent(QMouseEvent *event);
        void rectMouseReleaseEvent(QMouseEvent *event);
        void rectPaintEvent(QPaintEvent *event);

        void polyMouseMoveEvent(QMouseEvent *event);
        void polyMousePressEvent(QMouseEvent *event);
        void polyMouseReleaseEvent(QMouseEvent *event);
        void polyPaintEvent(QPaintEvent *event);

};

#endif // SRCIMGLABEL_H
