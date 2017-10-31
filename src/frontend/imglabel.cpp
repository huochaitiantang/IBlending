#include "imglabel.h"

ImgLabel::ImgLabel(QLabel *info){
     this->loc_info = info;
}

void ImgLabel::open_img(QScrollArea *scroll_area){
    this->hasImg = false;
    this->image = new QImage;
    this->filename = QFileDialog::getOpenFileName(this, tr("Chose img"), "", tr("Images (*.png *.bmp *.jpg *.tif *.GIF )"));
    if(this->filename.isEmpty()){
        return;
    }
     if(! ( this->image->load(this->filename) ) ){
        QMessageBox::information(this, tr("Open failed!"), tr("Open failed"));
        return;
     }
     this->setPixmap(QPixmap::fromImage(*this->image));
     this->setAlignment(Qt::AlignCenter);
     this->setMouseTracking(true);
     int h = this->image->height();
     int w = this->image->width();
     this->img_height = h;
     this->img_width = w;
     if(h < MAX_Y)  {
         this->img_anchor_y = (MAX_Y - h)/2;
         if( w >= MAX_X) this->img_anchor_y -= 10;
     }
     else   this->img_anchor_y = 0;
     if(w < MAX_X)  {
         this->img_anchor_x = (MAX_X - w)/2;
         if(h >= MAX_Y) {
             this->img_anchor_x -= 10;
         }
     }
     else this->img_anchor_x = 0;
     scroll_area->setWidget(this);
     scroll_area->setMaximumHeight(MAX_Y);
     scroll_area->setMaximumWidth(MAX_X);
     this->hasImg = true;
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
    //cout << "ImgLabel:" << cur_x << " " << cur_y << "\n";
    int x,y;
    getRelativeXY(cur_x, cur_y, &x,&y);
    char ss[100];
    sprintf(ss,"x=%d, y=%d", x, y);
    QString qss(ss);
    loc_info->setText(qss);
    return;
}

void ImgLabel::mousePressEvent(QMouseEvent *event){
    return;
}

void ImgLabel::mouseReleaseEvent(QMouseEvent *event){
    return;
}

void ImgLabel::paintEvent(QPaintEvent *event){
    QLabel::paintEvent(event);
}
