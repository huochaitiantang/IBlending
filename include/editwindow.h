#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include <QMainWindow>
class MainWindow;
#include "mainwindow.h"
#include "editimglabel.h"
namespace Ui {
class EditWindow;
}

class EditWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditWindow(QWidget *parent = 0, MainWindow * mw = 0);
    ~EditWindow();
    MainWindow * mainwindow;
    EditImgLabel * editimglabel;

protected:
    void adjust_widget();
    void add_menu();

private slots:
    void switch_to_poisson();
    void open_img();
    void save_img();

private:
    Ui::EditWindow *ui;
};

#endif // EDITWINDOW_H
