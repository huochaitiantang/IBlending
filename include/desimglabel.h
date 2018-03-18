#ifndef DESIMGLABEL_H
#define DESIMGLABEL_H

#include "imglabel.h"
#include <vector>
//for forward and backward
typedef struct{
    QImage * img;
    QImage * subimg;
    QImage * subimgmsk;
    QImage * subimgscale;
    QImage * subimgmskscale;
    int x, y, w, h;
    select_way_e selectway;
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
        QImage *subimagemask;
        QImage *subimagescale;
        QImage *subimagemaskscale;
        algorithm_e ALGO = POISSON_FR;

    protected:
        int inRectLine(int x, int y);
        QImage * getDisplayImage();
        void mouseMoveEvent(QMouseEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void paintEvent(QPaintEvent *event);
};

#endif // DESIMGLABEL_H
