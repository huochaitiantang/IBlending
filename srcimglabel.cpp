#include "srcimglabel.h"

int SrcImgLabel::inRectLine(int x, int y){
    if(inImg(x,y) == false || selectOver == false) return 0;
    else if(x >= minx - MOUSE_GAP && x <= minx + MOUSE_GAP &&
            y >= miny - MOUSE_GAP && y <= miny + MOUSE_GAP){
        return 6;
    }
    else if(x >= minx - MOUSE_GAP && x <= minx + MOUSE_GAP &&
            y >= maxy - MOUSE_GAP && y <= maxy + MOUSE_GAP){
        return 7;
    }
    else if(x >= maxx - MOUSE_GAP && x <= maxx + MOUSE_GAP &&
            y >= miny - MOUSE_GAP && y <= miny + MOUSE_GAP){
        return 8;
    }
    else if(x >= maxx - MOUSE_GAP && x <= maxx + MOUSE_GAP &&
            y >= maxy - MOUSE_GAP && y <= maxy + MOUSE_GAP){
        return 9;
    }
    else if(x >= minx - MOUSE_GAP && x <= minx + MOUSE_GAP && y > miny && y < maxy){
        return 1;
    }
    else if(x >= maxx - MOUSE_GAP && x <= maxx + MOUSE_GAP && y > miny && y < maxy){
        return 2;
    }
    else if(y >= miny - MOUSE_GAP && y <= miny + MOUSE_GAP && x > minx && x < maxx){
        return 3;
    }
    else if(y >= maxy - MOUSE_GAP && y <= maxy + MOUSE_GAP && x > minx && x < maxx){
        return 4;
    }
    return 0;
}

void SrcImgLabel::mouseMoveEvent(QMouseEvent *event){
    getCurXY(event);
    int tmp = inRectLine(cur_x,cur_y);
    if(tmp == 1 || tmp == 2) setCursor(Qt::SizeHorCursor);
    else if(tmp == 3 || tmp == 4) setCursor(Qt::SizeVerCursor);
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
        else if(stretch == 6){
            minx = std::max(std::min(maxx,cur_x),img_anchor_x);
            miny = std::max(std::min(maxy,cur_y),img_anchor_y);
        }
        else if(stretch == 7){
            minx = std::max(std::min(maxx,cur_x),img_anchor_x);
            maxy = std::min(std::max(miny,cur_y),img_anchor_y + img_height - 1);
        }
        else if(stretch == 8){
            maxx = std::min(std::max(minx,cur_x),img_anchor_x + img_width - 1);
            miny = std::max(std::min(maxy,cur_y),img_anchor_y);
        }
        else if(stretch == 9){
            maxx = std::min(std::max(minx,cur_x),img_anchor_x + img_width - 1);
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
