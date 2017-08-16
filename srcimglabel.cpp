#include "srcimglabel.h"

int SrcImgLabel::inRectLine(int x, int y){
    if(inImg(x,y) == false || selectOver == false) return 0;
    if(x == minx && y >= miny && y <= maxy) return 1;
    if(x == maxx && y >= miny && y <= maxy) return 2;
    if(y == miny && x >= minx && x <= maxx) return 3;
    if(y == maxy && x >= minx && x <= maxx) return 4;
    return 0;
}

void SrcImgLabel::mouseMoveEvent(QMouseEvent *event){
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
    loc_info->setText(qss);
    if(event->buttons() & Qt::LeftButton){
        needDraw = true;
        if(stretch == 1){
            minx = std::max(std::min(maxx,cur_x),img_anchor_x);
        }
        else if(stretch == 2){
            maxx = std::min(std::max(minx,cur_x),img_anchor_x + img_width - 1);
        }
        else if(stretch == 3){
            miny = std::max(std::min(maxy,cur_y),img_anchor_y);
        }
        else if(stretch == 4){
            maxy = std::min(std::max(miny,cur_y),img_anchor_y + img_height - 1);
        }
        update();
    }
}

void SrcImgLabel::mousePressEvent(QMouseEvent *event){
    //cout << "Press mouse" << endl;
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


void SrcImgLabel::mouseReleaseEvent(QMouseEvent *event){
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
    }
}

void SrcImgLabel::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);//先调用父类的paintEvent为了显示'背景'!!!
    if(stretch > 0 || selectOver){
        QPainter painter(this);
        painter.setPen(QPen(Qt::green,1));
        painter.drawRect(QRect(minx, miny, maxx-minx+1, maxy-miny+1));
        //cout << "draw.... " << minx << " " << miny << " " << maxx << " " << maxy<< endl;
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
        maxx = std::min(std::max(press_x, final_x),img_anchor_x+img_width-1);
        maxy = std::min(std::max(press_y, final_y),img_anchor_y+img_height-1);
        QPainter painter(this);
        painter.setPen(QPen(Qt::green,1));
        painter.drawRect(QRect(minx, miny, maxx-minx+1, maxy-miny+1));
        needDraw = false;
    }
}
