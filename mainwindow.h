#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QLabel>
#include <QPainter>

#include "../gradient_fusion/common.hpp"

using namespace std;
using namespace cv;

namespace Ui {
class MainWindow;
}

enum state_e{
    CLICK,
    CHOSE
};

Mat QImage2cvMat(QImage *image);
QImage cvMat2QImage(const Mat& mat);
String getPureName(String s);

class ImgLabel : public QLabel
{
    public:
        QImage *image;
        QString filename;
        int img_anchor_x, img_anchor_y;
        int img_width, img_height;
        int press_x, press_y;
        int release_x, release_y;
        int cur_x, cur_y;
        int minx, miny, maxx, maxy;
        bool needDraw = false;
        bool selectOver = false;
        int stretch = 0;

    protected:
        void mouseMoveEvent(QMouseEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void getCurXY(QMouseEvent *event);
        void getRelativeXY(int px, int py, int * x, int * y);
        void paintEvent(QPaintEvent *event);
        bool inImg(int x, int y);
        int inRectLine(int x, int y);
        bool validRect();
};

class DesImgLabel : public QLabel
{
    public:
        QImage *image;
        QImage *subimage;
        QString desfilename;
        int img_anchor_x, img_anchor_y;
        int img_width, img_height;
        int press_x, press_y;
        int release_x, release_y;
        int cur_x, cur_y;
        int move_anchor_x, move_anchor_y;
        int minx, miny, maxx, maxy;
        bool needDraw = false;
        bool selectOver = false;
        int stretch = 0;
        bool hasImg = false;
        bool hasSubImg = false;
        int subx, suby;
        int subw, subh;

    protected:
        void mouseMoveEvent(QMouseEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void getCurXY(QMouseEvent *event);
        void getRelativeXY(int px, int py, int * x, int * y);
        void paintEvent(QPaintEvent *event);
        bool inImg(int x, int y);
        int inRectLine(int x, int y);
        //bool validRect();
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    state_e STATE = CLICK;

    static const int MAX_X = 600;
    static const int MAX_Y = 500;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Ui::MainWindow *ui;
    ImgLabel * imglabel;
    DesImgLabel * desimglabel;


private slots:
    void on_click_clicked();
    void on_chose_clicked();
    void on_open_img_clicked();
    void on_des_open_img_clicked();
    void on_select_ok_clicked();
    void on_poisson_clicked();
    void on_save_img_clicked();


};


#endif // MAINWINDOW_H
