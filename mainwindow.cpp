#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

static MainWindow* s_main_window;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    s_main_window = this;
    image = new QImage;
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_click_clicked()
{
    STATE = CLICK;
    ui->bottom_info->setText("click on click button");
    cout << "switch to click state" << endl;
}

void MainWindow::on_chose_clicked()
{
    STATE = CHOSE;
    cout << "switch to chose state" << endl;
    ui->bottom_info->setText("click on chose button");
}

void MainWindow::myMouseMoveEvent(int x, int y){
    char ss[100];
    sprintf(ss,"x=%d,y=%d",x,y);
    QString qss(ss);
    this->ui->bottom_info->setText(qss);
}

void MainWindow::on_open_img_clicked(){
    QString filename;
    filename = QFileDialog::getOpenFileName(this, tr("Chose img"), "", tr("Images (*.png *.bmp *.jpg *.tif *.GIF )"));
    if(filename.isEmpty()){
        return;
    }
     if(! ( image->load(filename) ) ){
        QMessageBox::information(this, tr("Open failed!"), tr("Open failed"));
        return;
     }
     ImgLabel * label = new ImgLabel();
     label->setPixmap(QPixmap::fromImage(*image));
     label->setAlignment(Qt::AlignCenter);
     label->setMouseTracking(true);
     ui->img_area->setWidget(label);
     ui->img_area->setMaximumHeight(MAX_Y);
     ui->img_area->setMaximumWidth(MAX_X);
     img_height = image->height();
     img_width = image->width();
     if(img_height < MAX_Y){
         img_anchor_y = (MAX_Y - img_height)/2;
     }
     else{
         img_anchor_y = 0;
     }
     if(img_width < MAX_X){
         img_anchor_x = (MAX_X - img_width)/2;
     }
     else{
         img_anchor_x = 0;
     }
}

void ImgLabel::mouseMoveEvent(QMouseEvent *event){
    //cout << event->x() << " " << event->y() << endl;
    int x, y;
    //cout << this->pos().x() << " " << this->pos().y() << endl;
    x = event->x() - s_main_window->img_anchor_x;
    y = event->y() - s_main_window->img_anchor_y;
    cout << x << " " << y << endl;
    if( x < 0 || y < 0 ||
        x >= s_main_window->img_width ||
        y >= s_main_window->img_height){
        x = 0.0; y = 0.0;
    }
    s_main_window->myMouseMoveEvent(x,y);
}
