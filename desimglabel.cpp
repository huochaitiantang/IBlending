#include "desimglabel.h"
#include "util.h"

void DesImgLabel::poisson(){
    if(this->hasSubImg == false || this->hasImg == false) return;
    //QImage *srcImg = new QImage;
    //QImage *srcImgMsk = new QImage;
    //*srcImg = this->subimagescale;
    Mat src, des, ans, msk;
    vector<Mat> src_v;
    vector<Mat> des_v;
    vector<Mat> ans_v;
    vector<Mat> msk_v;
    src = QImage2cvMat(subimagescale);
    des = QImage2cvMat(this->image);
    split(src, src_v);
    split(des,des_v);
    if(src_v.size() == 4) src_v.pop_back();
    if(des_v.size() == 4) des_v.pop_back();
    merge(src_v, src);
    merge(des_v, des);
    if(SELECT_WAY == POLY){
        //*srcImgMsk = this->subimagemaskscale;
        msk = QImage2cvMat(subimagemaskscale);
        split(msk,msk_v);
        if(msk_v.size() == 4) msk_v.pop_back();
        merge(msk_v, msk);
        // 0 FR; 1 Normal Clone; 2 Mixed Clone
        ans = getPolygonPoissonMat(des, src, msk, Rect(this->subx, this->suby, this->subw, this->subh), 1);
    }
    else{
        // 0 FR; 1 Normal Clone; 2 Mixed Clone
        ans = getPoissonMat(des, src, Rect(this->subx, this->suby, this->subw, this->subh), 1);
    }
    split(ans,ans_v);
    Mat whiteC = 255 * Mat::ones(ans_v[0].rows, ans_v[0].cols, ans_v[0].depth());
    ans_v.push_back(whiteC);
    merge(ans_v,ans);
    //imshow("ans",ans);
    desimg_t cur_old, cur_new;
    while(img_buf_index < img_buf.size() - 1) img_buf.pop_back();
    cur_old.img = image;
    cur_old.subimg = subimage;
    cur_old.subimgmsk = subimagemask;
    cur_old.subimgscale = subimagescale;
    cur_old.subimgmskscale = subimagemaskscale;
    cur_old.selectway = SELECT_WAY;
    cur_old.withsub = true;
    cur_old.x = subx;
    cur_old.y = suby;
    cur_old.w = subw;
    cur_old.h = subh;
    img_buf.push_back(cur_old);
    image = new QImage();
    *image = cvMat2QImage(ans);
    cur_new.img = image;
    cur_new.withsub = false;
    img_buf.push_back(cur_new);
    img_buf_index += 2;

    this->hasSubImg = false;
    this->needDraw = true;
    this->setPixmap(QPixmap::fromImage(*image));
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

void DesImgLabel::open_img(QScrollArea *scroll_area){
    ImgLabel::open_img(scroll_area);
    img_buf.clear();
    desimg_t cur;
    cur.img = image;
    cur.withsub = false;
    img_buf.push_back(cur);
    img_buf_index = 0;
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
        if((stretch >= 1 && stretch <=4 ) ||(stretch >=6 && stretch <= 9)){
            *subimagescale = subimage->scaled(subw, subh , Qt::IgnoreAspectRatio);
            if(SELECT_WAY == POLY)
                *subimagemaskscale = subimagemask->scaled(subw, subh, Qt::IgnoreAspectRatio);
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
    QRgb *Bigrow;
    QRgb *Smlrow;
    QRgb *SmlMskrow;
    for(int y = 0; y < img_height; y++){
        Bigrow = (QRgb*)image->scanLine(y);
        bool yok = ((y >= suby)&&(y < suby + subh));
        if(yok) {
            Smlrow = (QRgb*)subimagescale->scanLine(y - suby);
            if(SELECT_WAY == POLY)
                SmlMskrow = (QRgb*)subimagemaskscale->scanLine(y - suby);
        }
        for(int x = 0; x < img_width; x++){
            if( yok && x >= subx && x < subx + subw){
                DImg->setPixel(x, y, Smlrow[x - subx]);
                if(SELECT_WAY == POLY && SmlMskrow[x - subx] == qRgb(0,0,0)){
                        QColor qc(Bigrow[x]);
                        qc.setRed((qc.red()+150)/2);
                        qc.setGreen((qc.green()+150)/2);
                        qc.setBlue((qc.blue()+150)/2);
                        DImg->setPixel(x, y, qc.rgb());
                }
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

void DesImgLabel::forward_backward(int offset){
    if (offset != 1 && offset != -1) return;
    int img_buf_size = img_buf.size();
    int tmp = img_buf_index + offset;
    if(tmp < 0 || tmp >= img_buf_size ) return;
    image = img_buf[tmp].img;
    hasSubImg = img_buf[tmp].withsub;
    if(hasSubImg){
        subimage = img_buf[tmp].subimg;
        subimagemask = img_buf[tmp].subimgmsk;
        subimagescale = img_buf[tmp].subimgscale;
        subimagemaskscale = img_buf[tmp].subimgmskscale;
        SELECT_WAY = img_buf[tmp].selectway;
        subx = img_buf[tmp].x;
        suby = img_buf[tmp].y;
        subw = img_buf[tmp].w;
        subh = img_buf[tmp].h;
        QImage * DImg = getDisplayImage();
        this->setPixmap(QPixmap::fromImage(*DImg));
    }
    else{
        this->setPixmap(QPixmap::fromImage(*image));
    }
    img_buf_index = tmp;
}


