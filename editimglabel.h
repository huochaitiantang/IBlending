#ifndef EDITIMGLABEL_H
#define EDITIMGLABEL_H

#include "imglabel.h"

class EditImgLabel : public ImgLabel
{
    public:
        EditImgLabel(QLabel *info):ImgLabel(info){}
        void save_img();

    protected:
        void mouseMoveEvent(QMouseEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void paintEvent(QPaintEvent *event);

};


#endif // EDITIMGLABEL_H
