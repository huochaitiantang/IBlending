#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QLabel>
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
    int img_anchor_x = 0;
    int img_anchor_y = 0;
    int img_width;
    int img_height;
    int press_x;
    int press_y;
    static const int MAX_X = 600;
    static const int MAX_Y = 500;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void myMouseMoveEvent(int x, int y);


private slots:
    void on_click_clicked();

    void on_chose_clicked();

    void on_open_img_clicked();

private:
    Ui::MainWindow *ui;
};

class ImgLabel : public QLabel
{
protected:
    void mouseMoveEvent(QMouseEvent *event);
    //void mousePressEvent(QMouseEvent *event);
    //void mouseReleaseEvent(QMouseEvent *event);
};

#endif // MAINWINDOW_H
