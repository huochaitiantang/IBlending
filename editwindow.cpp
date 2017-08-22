#include "editwindow.h"
#include "ui_editwindow.h"
#include <QToolBar>

EditWindow::EditWindow(QWidget *parent, MainWindow * mw) :
    QMainWindow(parent),
    ui(new Ui::EditWindow)
{
    mainwindow = mw;
    ui->setupUi(this);
    editimglabel = new EditImgLabel(ui->bottom_info);
    this->adjust_widget();
    connect(this->ui->actiongo_to_poisson, SIGNAL(triggered()), this, SLOT(switch_to_poisson()));
    this->add_menu();
}

EditWindow::~EditWindow()
{
    delete ui;
}

void EditWindow::adjust_widget(){
    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect screenRect = desktopWidget->availableGeometry();
    this->resize(QSize(screenRect.width(), screenRect.height()));
    int w = this->width();
    int h = this->height();
    int MAX_X = std::max((w - 20 * 2), editimglabel->MAX_X);
    int MAX_Y = std::max((h - 100), editimglabel->MAX_Y);
    editimglabel->MAX_X = MAX_X;
    editimglabel->MAX_Y = MAX_Y;
    this->ui->img_area->setGeometry(20, 40, MAX_X, MAX_Y);
    this->ui->bottom_info->setGeometry(20, 40 + MAX_Y, MAX_X, 20);

}

void EditWindow::add_menu(){
    QAction * openAction = new QAction( tr("&Open..."), this);
    openAction->setShortcuts(QKeySequence::Open);
    connect(openAction, SIGNAL(triggered()), this, SLOT(open_img()));

    QAction * saveAction = new QAction( tr("&Save as..."), this);
    saveAction->setShortcut(QKeySequence::SaveAs);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save_img()));

    QMenu *file = menuBar()->addMenu(tr("&File"));
    file->addAction(openAction);
    file->addAction(saveAction);
}

void EditWindow::switch_to_poisson(){
    this->hide();
    this->mainwindow->show();
}

void EditWindow::open_img(){
    this->editimglabel->open_img(this->ui->img_area);
}

void EditWindow::save_img(){
    this->editimglabel->save_img();
}
