#include "desimglabel.h"
#include "util.h"

void DesImgLabel::poisson(){
    if(this->hasSubImg == false || this->hasImg == false) return;
    QImage *srcImg = new QImage;
    *srcImg = this->subimage->scaled(this->subw, this->subh, Qt::IgnoreAspectRatio);
    Mat src, des, ans;
    vector<Mat> src_v;
    vector<Mat> des_v;
    vector<Mat> ans_v;
    src = QImage2cvMat(srcImg);
    des = QImage2cvMat(this->image);
    split(src, src_v);
    split(des,des_v);
    if(src_v.size() == 4) src_v.pop_back();
    if(des_v.size() == 4) des_v.pop_back();
    merge(src_v, src);
    merge(des_v,des);
    // 0 FR; 1 Normal Clone; 2 Mixed Clone
    ans = getPoissonMat(des, src, Rect(this->subx, this->suby, this->subw, this->subh), 1);
    split(ans,ans_v);
    Mat whiteC = 255 * Mat::ones(ans_v[0].rows, ans_v[0].cols, ans_v[0].depth());
    ans_v.push_back(whiteC);
    merge(ans_v,ans);
    //imshow("ans",ans);
    *(this->image) = cvMat2QImage(ans);
    this->hasSubImg = false;
    this->needDraw = true;
    this->setPixmap(QPixmap::fromImage(*this->image));
    this->update();
}

void DesImgLabel::save_img(char s[]){
    QString filename = QFileDialog::getSaveFileName(this,
            tr("Save Image"),
            s,
            tr(" *.jpg;; *.png;; *.bmp;; *.tif;; *.GIF"));
    if (filename.isEmpty()){
       return;
    }
    QImage * save_img = this->image;
    if(this->hasSubImg) save_img = getDisplayImage();
    if(!(save_img->save(filename))){
        QMessageBox::information(this,
            tr("Save the image"),
            tr("Failed to save the image!"));
    }
}

int DesImgLabel::inRectLine(int x, int y){
    if(inImg(x,y) == false || hasSubImg == false) return 0;
    int x1 = subx + img_anchor_x;
    int y1 = suby + img_anchor_y;
    int x2 = x1 + subw - 1;
    int y2 = y1 + subh - 1;
    if(x > x1 + MOUSE_GAP && y > y1 + MOUSE_GAP &&
       x < x2 - MOUSE_GAP && y < y2 - MOUSE_GAP) return 5;
    else if(x >= x1 - MOUSE_GAP && x <= x1 + MOUSE_GAP &&
            y >= y1 - MOUSE_GAP && y <= y1 + MOUSE_GAP){
        return 6;
    }
    else if(x >= x1 - MOUSE_GAP && x <= x1 + MOUSE_GAP &&
            y >= y2 - MOUSE_GAP && y <= y2 + MOUSE_GAP){
        return 7;
    }
    else if(x >= x2 - MOUSE_GAP && x <= x2 + MOUSE_GAP &&
            y >= y1 - MOUSE_GAP && y <= y1 + MOUSE_GAP){
        return 8;
    }
    else if(x >= x2 - MOUSE_GAP && x <= x2 + MOUSE_GAP &&
            y >= y2 - MOUSE_GAP && y <= y2 + MOUSE_GAP){
        return 9;
    }
    else if(x >= x1 - MOUSE_GAP && x <= x1 + MOUSE_GAP && y > y1 && y < y2){
        return 1;
    }
    else if(x >= x2 - MOUSE_GAP && x <= x2 + MOUSE_GAP && y > y1 && y < y2){
        return 2;
    }
    else if(y >= y1 - MOUSE_GAP && y <= y1 + MOUSE_GAP && x > x1 && x < x2) {
        return 3;
    }
    else if(y >= y2 - MOUSE_GAP && y <= y2 + MOUSE_GAP && x > x1 && x < x2) {
        return 4;
    }
    else return 0;
}

void DesImgLabel::mouseMoveEvent(QMouseEvent *event){
    getCurXY(event);
    int tmp = inRectLine(cur_x, cur_y);
    if(tmp == 1 || tmp == 2) setCursor(Qt::SizeHorCursor);
    else if(tmp == 3 || tmp == 4) setCursor(Qt::SizeVerCursor);
    else if(tmp == 5) setCursor(Qt::OpenHandCursor);
    else if(tmp == 6 || tmp == 9) setCursor(Qt::SizeFDiagCursor);
    else if(tmp == 7 || tmp == 8) setCursor(Qt::SizeBDiagCursor);
    else setCursor(Qt::ArrowCursor);
    int x,y;
    getRelativeXY(cur_x, cur_y, &x,&y);
    char ss[100];
    sprintf(ss,"x=%d, y=%d", x, y);
    QString qss(ss);
    loc_info->setText(qss);
    if(event->buttons() & Qt::LeftButton){
        needDraw = true;
        if(stretch == 5){
            subx = std::max(std::min(x - move_anchor_x, img_width - subw ), 0);
            suby = std::max(std::min(y - move_anchor_y, img_height - subh ), 0);
        }
        else if(stretch == 1 && x < subx + subw){
            subw =  subw + subx - x;
            subx = x;
        }
        else if(stretch == 2 && x > subx){
            subw = x - subx;
        }
        else if(stretch == 3 && y < suby + subh){
            subh = subh + suby - y;
            suby = y;
        }
        else if(stretch == 4 && y > suby){
            subh = y - suby;
        }
        else if(stretch == 6 && x < subx + subw && y < suby + subh){
            subw = subw + subx - x;
            subx = x;
            subh = subh + suby - y;
            suby = y;
        }
        else if(stretch == 7 && x < subx + subw && y > suby){
            subw = subw + subx - x;
            subx = x;
            subh = y - suby;
        }
        else if(stretch == 8 && x > subx && y < suby + subh){
            subw = x - subx;
            subh = subh + suby - y;
            suby = y;
        }
        else if(stretch == 9 && x > subx && y > suby){
            subw = x - subx;
            subh = y - suby;
        }
        update();
    }
}

void DesImgLabel::mousePressEvent(QMouseEvent *event){
    getCurXY(event);
    int tmp = inRectLine(cur_x,cur_y);
    stretch = tmp;
    if(stretch == 5){
        move_anchor_x = cur_x - img_anchor_x - subx;
        move_anchor_y = cur_y - img_anchor_y - suby;
    }
}

void DesImgLabel::mouseReleaseEvent(QMouseEvent *event){
    getCurXY(event);
    setCursor(Qt::ArrowCursor);
    if(stretch > 0) stretch = 0;
}

QImage * DesImgLabel::getDisplayImage(){
    QImage * DImg = new QImage(image->width(),image->height(),image->format());
    QImage * scaleImg = new QImage;
    *scaleImg = subimage->scaled(subw, subh , Qt::IgnoreAspectRatio);
    QRgb *Bigrow;
    QRgb *Smlrow;
    for(int y = 0; y < img_height; y++){
        Bigrow = (QRgb*)image->scanLine(y);
        bool yok = ((y >= suby)&&(y < suby + subh));
        if(yok) Smlrow = (QRgb*)scaleImg->scanLine(y - suby);
        for(int x = 0; x < img_width; x++){
            if( yok && x >= subx && x < subx + subw){
                DImg->setPixel(x, y, Smlrow[x - subx]);
            }
            else{
                DImg->setPixel(x, y, Bigrow[x]);
            }
        }
    }
    return DImg;
}

void DesImgLabel::paintEvent(QPaintEvent *event){
    QLabel::paintEvent(event);
    if(hasSubImg == false || needDraw == false){
        return;
    }
    if(image->format() != subimage->format()){
        return ;
    }
    if(subimage->width() > image->width() || subimage->height() > image->height()){
        hasSubImg = false;
        return;
    }
    QImage * DImg = getDisplayImage();
    this->setPixmap(QPixmap::fromImage(*DImg)); //can trigger paintevent too, needDraw can stop repeat
//    QPainter painter(this);
//    painter.setPen(QPen(Qt::green,10));
//    painter.drawRect(QRect(subx + img_anchor_x, suby + img_anchor_y, subw, subh));
    needDraw = false;
}



