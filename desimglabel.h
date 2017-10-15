#ifndef DESIMGLABEL_H
#define DESIMGLABEL_H

#include "imglabel.h"
#include <vector>
//for forward and backward
typedef struct{
    QImage * img;
    QImage * subimg;
    int x, y, w, h;
    bool withsub;
} desimg_t;

class DesImgLabel : public ImgLabel
{
    public:
        QImage *subimage;
        int move_anchor_x, move_anchor_y;
        bool hasSubImg = false;
        int subx, suby;
        int subw, subh;
        std::vector<desimg_t> img_buf;
        int img_buf_index;
        DesImgLabel(QLabel *info):ImgLabel(info){}
        void poisson();
        void save_img(char s[]);
        void open_img(QScrollArea *scroll_area);
        void forward_backward(int offset);
        vector<vector<int> > submask;

    protected:
        int inRectLine(int x, int y);
        QImage * getDisplayImage();
        void mouseMoveEvent(QMouseEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void paintEvent(QPaintEvent *event);
};

#endif // DESIMGLABEL_H
