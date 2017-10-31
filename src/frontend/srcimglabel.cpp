#include "srcimglabel.h"

void SrcImgLabel::mouseMoveEvent(QMouseEvent *event){
    if(SELECT_WAY == RECT) rectMouseMoveEvent(event);
    else if(SELECT_WAY == POLY) polyMouseMoveEvent(event);
}

void SrcImgLabel::mousePressEvent(QMouseEvent *event){
    if(SELECT_WAY == RECT) rectMousePressEvent(event);
    else if(SELECT_WAY == POLY) polyMousePressEvent(event);
}

void SrcImgLabel::mouseReleaseEvent(QMouseEvent *event){
    if(SELECT_WAY == RECT) rectMouseReleaseEvent(event);
    else if(SELECT_WAY == POLY) polyMouseReleaseEvent(event);
}

void SrcImgLabel::paintEvent(QPaintEvent *event){
    if(SELECT_WAY == RECT) rectPaintEvent(event);
    else if(SELECT_WAY == POLY) polyPaintEvent(event);
}

void SrcImgLabel::open_img(QScrollArea *scroll_area){
    ImgLabel::open_img(scroll_area);
    clear_select();
}

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

void SrcImgLabel::rectMouseMoveEvent(QMouseEvent *event){
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

void SrcImgLabel::rectMousePressEvent(QMouseEvent *event){
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


void SrcImgLabel::rectMouseReleaseEvent(QMouseEvent *event){
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

void SrcImgLabel::rectPaintEvent(QPaintEvent *event)
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

void SrcImgLabel::clear_select(){
    selectOver = false;
    needDraw = false;
    replace_point = false;
    clearPolygon(&poly);
    update();
}

void SrcImgLabel::polyMouseMoveEvent(QMouseEvent *event){
    ImgLabel::mouseMoveEvent(event);
    setCursor(Qt::ArrowCursor);
    int siz = poly.x.size();
    if(siz > 0){
        int siz = poly.x.size();
        for(int i = 0; i < siz; i++){
            int cx = poly.x[i];
            int cy = poly.y[i];
            if(nearPoint(cur_x,cur_y,cx,cy) == true){
                cur_x = cx;
                cur_y = cy;
                setCursor(Qt::CrossCursor);
                break;
            }
            else if(selectOver && i > 0 && pointDistanceSegment(cur_x,cur_y,poly.x[i-1],poly.y[i-1],cx,cy) <= MOUSE_GAP){
                double slope = getSlope(poly.x[i-1],poly.y[i-1],cx,cy);
                if(slope >= -0.1 && slope <= 0.1) setCursor(Qt::SizeVerCursor);
                else if(slope > 0.1 && slope < 10.0) setCursor(Qt::SizeBDiagCursor);
                else if(slope < -0.1 && slope > -10.0) setCursor(Qt::SizeFDiagCursor);
                else setCursor(Qt::SizeHorCursor);
            }
        }
        if(selectOver &&(event->buttons() & Qt::LeftButton) && replace_point){
            can_replace = false;
            can_merge  = false;
            if(inImg(cur_x,cur_y) && canReplacePointInPolygon(poly,replace_ind,cur_x,cur_y)){
                can_replace = true;
                if(siz > 4){
                    //if relpace point to to the line of i-1 and i+1, delete the point
                    int ind1 = replace_ind - 1;
                    int ind2 = replace_ind + 1;
                    if(ind1 < 0) ind1 = siz - 2;
                    if(pointDistanceSegment(cur_x,cur_y,poly.x[ind1],poly.y[ind1],poly.x[ind2],poly.y[ind2]) <= MOUSE_GAP){
                        double slope = getSlope(poly.x[ind1],poly.y[ind1],poly.x[ind2],poly.y[ind2]);
                        if(slope >= -0.1 && slope <= 0.1) setCursor(Qt::SizeVerCursor);
                        else if(slope > 0.1 && slope < 10.0) setCursor(Qt::SizeBDiagCursor);
                        else if(slope < -0.1 && slope > -10.0) setCursor(Qt::SizeFDiagCursor);
                        else setCursor(Qt::SizeHorCursor);
                        can_merge = true;
                    }
                }
            }
        }
        else if(selectOver &&(event->buttons() & Qt::LeftButton) && insert_point){
            can_insert = false;
            if(inImg(cur_x,cur_y) && canInsertPointInPolygon(poly,insert_ind,cur_x,cur_y))
                can_insert = true;
        }
    }
    update();
}

void SrcImgLabel::polyMousePressEvent(QMouseEvent *event){
    //cout << "Press mouse" << endl;
    getCurXY(event);
    if(inImg(cur_x,cur_y) == false) return;
    if(selectOver == true){
        int i,siz=poly.x.size();
        for(i = 0; i < siz; i++){
            int cx = poly.x[i];
            int cy = poly.y[i];
            if(nearPoint(cur_x,cur_y,cx,cy) == true){
                cur_x = cx;
                cur_y = cy;
                replace_point = true;
                replace_ind = i;
                break;
            }
            if(i > 0 && pointDistanceSegment(cur_x,cur_y,poly.x[i-1],poly.y[i-1],cx,cy) <= MOUSE_GAP){
                insert_point = true;
                insert_ind = i - 1;
                break;
            }
        }
    }
    else{
        if(poly.x.size() == 0){
            addPointToPolygon(&poly,cur_x,cur_y);
        }
        else{
            int fx = poly.x[0];
            int fy = poly.y[0];
            if(nearPoint(cur_x,cur_y,fx,fy) == true){
                cur_x = fx;
                cur_y = fy;
            }
            if(canAddPointToPolygon(poly,cur_x,cur_y)){
                addPointToPolygon(&poly,cur_x,cur_y);
                if(validPolygon(poly)){
                    selectOver = true;
                }
            }
        }
    }
}

void SrcImgLabel::polyMouseReleaseEvent(QMouseEvent *event){
    if(replace_point){
        if(can_replace){
            if(can_merge) deletePointInPolygon(&poly,replace_ind);
            else replacePointInPolygon(&poly,replace_ind,cur_x,cur_y);
        }
        replace_point = false;
    }
    else if(insert_point){
        if(can_insert){
            int ind = insert_ind;
            if(pointDistanceSegment(cur_x,cur_y,poly.x[ind],poly.y[ind],poly.x[ind+1],poly.y[ind+1]) > MOUSE_GAP)
                insertPointInPolygon(&poly,insert_ind,cur_x,cur_y);
        }
        insert_point = false;
    }
    print_poly(poly);
    update();
}

void SrcImgLabel::polyPaintEvent(QPaintEvent *event){
    QLabel::paintEvent(event);//先调用父类的paintEvent为了显示'背景'!!!
    QPainter painter(this);
    int siz = poly.x.size();
    for(int i = 0; i < siz; i++){
        painter.setPen(QPen(QColor(255, 255, 255), 1));
        painter.setBrush(QColor(255, 255, 255));
        if(!(selectOver && replace_point && (replace_ind == i || (replace_ind == 0 && i == siz -1))))
            painter.drawRect(max(0,poly.x[i]-2), max(0,poly.y[i]-2), 4, 4);
    }
    painter.setPen(QPen(Qt::green,1));
    if(selectOver == false){
        for(int i=1; i<siz; i++){
            painter.drawLine(QPoint(poly.x[i-1],poly.y[i-1]),QPoint(poly.x[i],poly.y[i]));
        }
        if(!inImg(cur_x,cur_y) || !canAddPointToPolygon(poly,cur_x,cur_y) ){
            painter.setPen(QPen(Qt::red,1));
        }
        if(poly.x.size()>0){
            painter.drawLine(QPoint(poly.x[siz-1],poly.y[siz-1]),QPoint(cur_x,cur_y));
        }
    }
    else{
        for(int i=1; i<siz; i++){
            if(replace_point && (i-1 == replace_ind || i == replace_ind ||(replace_ind == 0 && i == siz -1))){
                painter.setPen(QPen(Qt::red,1));
                if(can_replace) painter.setPen(QPen(Qt::blue,1));
                if(i-1 == replace_ind)
                    painter.drawLine(QPoint(cur_x,cur_y),QPoint(poly.x[i],poly.y[i]));
                else
                    painter.drawLine(QPoint(poly.x[i-1],poly.y[i-1]),QPoint(cur_x,cur_y));
                painter.setPen(QPen(Qt::green,1));
            }
            else if(insert_point && (i-1 == insert_ind)){
                painter.setPen(QPen(Qt::red,1));
                if(can_insert) painter.setPen(QPen(Qt::blue,1));
                painter.drawLine(QPoint(cur_x,cur_y),QPoint(poly.x[i-1],poly.y[i-1]));
                painter.drawLine(QPoint(cur_x,cur_y),QPoint(poly.x[i],poly.y[i]));
                painter.setPen(QPen(Qt::green,1));
            }
            else{
                painter.drawLine(QPoint(poly.x[i-1],poly.y[i-1]),QPoint(poly.x[i],poly.y[i]));
            }
        }
    }
}
