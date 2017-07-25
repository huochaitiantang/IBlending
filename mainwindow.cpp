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

bool ImgLabel::validRect(){
    if(minx >= img_anchor_x &&
       maxx >= img_anchor_x &&
       miny >= img_anchor_y &&
       maxy >= img_anchor_y &&
       minx < img_anchor_x + img_width &&
       maxx < img_anchor_x + img_width &&
       miny < img_anchor_y + img_height &&
       maxy < img_anchor_y + img_height) return true;
    else return false;
}

int ImgLabel::inRectLine(int x, int y){
    if(inImg(x,y) == false || selectOver == false) return 0;
    if(x == minx && y >= miny && y <= maxy) return 1;
    if(x == maxx && y >= miny && y <= maxy) return 2;
    if(y == miny && x >= minx && x <= maxx) return 3;
    if(y == maxy && x >= minx && x <= maxx) return 4;
    return 0;
}


void ImgLabel::mouseMoveEvent(QMouseEvent *event){
    getCurXY(event);
    int tmp = inRectLine(cur_x,cur_y);
    if(tmp == 1 || tmp == 2){
        setCursor(Qt::SizeHorCursor);
    }
    else if(tmp == 3 || tmp == 4){
        setCursor(Qt::SizeVerCursor);
    }
    else{
        setCursor(Qt::ArrowCursor);
    }
    int x,y;
    getRelativeXY(cur_x, cur_y, &x,&y);
    char ss[100];
    sprintf(ss,"x=%d, y=%d", x, y);
    QString qss(ss);
    s_main_window->ui->bottom_info->setText(qss);
    if(event->buttons() & Qt::LeftButton){
        needDraw = true;
        if(stretch == 1){
            minx = std::max(std::min(maxx,cur_x),img_anchor_x);
            //maxx = std::max(maxx,cur_x);
        }
        else if(stretch == 2){
            maxx = std::min(std::max(minx,cur_x),img_anchor_x + img_width);
            //minx = std::min(minx,cur_x);
        }
        else if(stretch == 3){
            miny = std::max(std::min(maxy,cur_y),img_anchor_y);
            //maxy = std::max(maxy,cur_y);
        }
        else if(stretch == 4){
            maxy = std::min(std::max(miny,cur_y),img_anchor_y + img_height);
            //miny = std::min(miny,cur_y);
        }
        update();
    }
}



void ImgLabel::mousePressEvent(QMouseEvent *event){
    cout << "Press mouse" << endl;
    getCurXY(event);
    int tmp = inRectLine(cur_x,cur_y);
    if(tmp == 0){
        selectOver = false;
        press_x = cur_x;
        press_y = cur_y;
    }
    else{
        stretch = tmp;
    }

}

void ImgLabel::mouseReleaseEvent(QMouseEvent *event){
    cout << "Release mouse" << endl;
    getCurXY(event);
    setCursor(Qt::ArrowCursor);
    if(stretch > 0){
        stretch = 0;
    }
    else{
        int x,y;
        getRelativeXY(cur_x, cur_y, &x, &y);
        release_x = x + img_anchor_x;
        release_y = y + img_anchor_y;
        if(inImg(press_x,press_y))  selectOver = true;
        //needDraw = true;
        //update();
    }
    cout << "Over:" << selectOver
         << " press: " << press_x
         << " " << press_y
         << " release:" << release_x
         << " " << release_y << endl;

}

void ImgLabel::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);//先调用父类的paintEvent为了显示'背景'!!!
    if(stretch > 0 || selectOver){
        QPainter painter(this);
        painter.setPen(QPen(Qt::green,1));
        painter.drawRect(QRect(minx, miny, maxx-minx, maxy-miny));
        cout << "draw.... " << minx << " " << miny << " " << maxx << " " << maxy<< endl;
    }
    else if(needDraw&&inImg(press_x,press_y)){
        int final_x = cur_x;
        int final_y = cur_y;
        if(selectOver){
            final_x = release_x;
            final_y = release_y;
        }
        minx = std::max(std::min(press_x, final_x), img_anchor_x);
        miny = std::max(std::min(press_y, final_y), img_anchor_y);
        maxx = std::min(std::max(press_x, final_x),img_anchor_x+img_width);
        maxy = std::min(std::max(press_y, final_y),img_anchor_y+img_height);
        QPainter painter(this);
        painter.setPen(QPen(Qt::green,1));
        painter.drawRect(QRect(minx, miny, maxx-minx, maxy-miny));
        needDraw = false;
    }
}


