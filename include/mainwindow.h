#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopWidget>
#include "srcimglabel.h"
#include "desimglabel.h"
class EditWindow;
#include "editwindow.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Ui::MainWindow *ui;
    SrcImgLabel * srcimglabel;
    DesImgLabel * desimglabel;
    EditWindow * editwindow;

protected:
    void adjust_widget();


private slots:
    void on_open_src_img_clicked();
    void on_open_des_img_clicked();
    void on_select_ok_clicked();
    void on_poisson_clicked();
    void on_save_img_clicked();
    void on_backward_clicked();
    void on_forward_clicked();
    void on_clear_roi_clicked();
    void switch_to_edit();
    void on_rect_clicked();
    void on_poly_clicked();
    void switch_fusion_way();
};


#endif // MAINWINDOW_H
