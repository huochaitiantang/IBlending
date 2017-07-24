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
    //ui->bottom_info->setText("click on click button");
    cout << "switch to click state" << endl;
}

void MainWindow::on_chose_clicked()
{
    STATE = CHOSE;
    //cout << "switch to chose state" << endl;
    ui->bottom_info->setText("click on chose button");
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
     int h = image->height();
     int w = image->width();
     label->img_height = h;
     label->img_width = w;
     if(h < MAX_Y)  label->img_anchor_y = (MAX_Y - h)/2;
     else   label->img_anchor_y = 0;
     if(w < MAX_X)  label->img_anchor_x = (MAX_X - w)/2;
     else label->img_anchor_x = 0;
     cout << label->img_anchor_x << " " << label->img_anchor_y << endl;
     ui->img_area->setWidget(label);
     ui->img_area->setMaximumHeight(MAX_Y);
     ui->img_area->setMaximumWidth(MAX_X);
}

void ImgLabel::getCurXY(QMouseEvent *event){
    cur_x = event->x();
    cur_y = event->y();
}

void ImgLabel::getRelativeXY(int px, int py, int * x, int * y){
    *x = px - img_anchor_x;
    *y = py - img_anchor_y;
    if( *x < 0 ) *x = 0;
    if( *y < 0 ) *y = 0;
    if( *x >= img_width ) *x = img_width - 1;
    if( *y >= img_height ) *y = img_height - 1;
}

bool ImgLabel::inImg(int x, int y){
    if(x < img_anchor_x ||
       y < img_anchor_y ||
       x >= img_anchor_x + img_width ||
       y >= img_anchor_y + img_height) return false;
    else return true;
}

void ImgLabel::mouseMoveEvent(QMouseEvent *event){
    getCurXY(event);
    int x,y;
    getRelativeXY(cur_x, cur_y, &x,&y);
    char ss[100];
    sprintf(ss,"x=%d, y=%d", x, y);
    QString qss(ss);
    s_main_window->ui->bottom_info->setText(qss);
    if(event->buttons() & Qt::LeftButton){
        needDraw = true;
        update();
    }
}

void ImgLabel::mousePressEvent(QMouseEvent *event){
    cout << "Press mouse" << endl;
    selectOver = false;
    press_x = cur_x;
    press_y = cur_y;
}

void ImgLabel::mouseReleaseEvent(QMouseEvent *event){
    cout << "Release mouse" << endl;
    getCurXY(event);
    int x,y;
    getRelativeXY(cur_x, cur_y, &x, &y);
    release_x = x + img_anchor_x;
    release_y = y + img_anchor_y;
    selectOver = true;
    needDraw = true;
    update();
}

void ImgLabel::paintEvent(QPaintEvent *event)
{

    QLabel::paintEvent(event);//先调用父类的paintEvent为了显示'背景'!!!
    if(needDraw && inImg(press_x,press_y)){
        int minx = std::max(std::min(press_x, cur_x), img_anchor_x);
        int miny = std::max(std::min(press_y, cur_y), img_anchor_y);
        int maxx = std::min(std::max(press_x, cur_x),img_anchor_x+img_width);
        int maxy = std::min(std::max(press_y, cur_y),img_anchor_y+img_height);
        QPainter painter(this);
        painter.setPen(QPen(Qt::green,1));
        painter.drawRect(QRect(minx, miny, maxx-minx, maxy-miny));
        needDraw = false;
    }
    if(selectOver && inImg(press_x,press_y)){
        int minx = std::max(std::min(press_x, release_x), img_anchor_x);
        int miny = std::max(std::min(press_y, release_y), img_anchor_y);
        int maxx = std::min(std::max(press_x, release_x),img_anchor_x+img_width);
        int maxy = std::min(std::max(press_y, release_y),img_anchor_y+img_height);
        QPainter painter(this);
        painter.setPen(QPen(Qt::green,1));
        painter.drawRect(QRect(minx, miny, maxx-minx, maxy-miny));
        needDraw = false;
    }
}


