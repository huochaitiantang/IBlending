#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QLabel>
#include <QPainter>
using namespace std;

namespace Ui {
class MainWindow;
}

enum state_e{
    CLICK,
    CHOSE
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    state_e STATE = CLICK;
    QImage *image;
    static const int MAX_X = 600;
    static const int MAX_Y = 500;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Ui::MainWindow *ui;


private slots:
    void on_click_clicked();

    void on_chose_clicked();

    void on_open_img_clicked();


};

class ImgLabel : public QLabel
{
    public:
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

#endif // MAINWINDOW_H
