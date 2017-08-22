#ifndef IMGLABEL_H
#define IMGLABEL_H

#include <QLabel>
#include <QScrollArea>
#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QPen>
#include <QPainter>


static int MOUSE_GAP = 1;
class ImgLabel : public QLabel
{
    public:
        QImage *image;
        QString filename;
        QLabel * loc_info;
        int img_anchor_x, img_anchor_y;
        int img_width, img_height;
        int press_x, press_y;
        int release_x, release_y;
        int cur_x, cur_y;
        bool needDraw = false;
        int stretch = 0;
        bool hasImg = false;
        int MAX_X = 600;
        int MAX_Y = 500;
        void open_img(QScrollArea *scroll_area);
        ImgLabel(QLabel *info);

    protected:
        void getCurXY(QMouseEvent *event);
        void getRelativeXY(int px, int py, int * x, int * y);
        bool inImg(int x, int y);
        void mouseMoveEvent(QMouseEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void paintEvent(QPaintEvent *event);
        int inRectLine(int x, int y);
};
#endif // IMGLABEL_H
