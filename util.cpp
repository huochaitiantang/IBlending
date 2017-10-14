#include "util.h"

QImage cvMat2QImage(const Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        //qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB32);
        return image.copy();
    }
    else
    {
        //qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}
Mat QImage2cvMat(QImage *image)
{
    Mat mat;
    switch(image->format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = Mat(image->height(), image->width(), CV_8UC4, (void*)image->constBits(), image->bytesPerLine());
        //cout << "Format_ARGB" << endl;
        break;
    case QImage::Format_RGB888:
        mat = Mat(image->height(), image->width(), CV_8UC3, (void*)image->constBits(), image->bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = Mat(image->height(), image->width(), CV_8UC1, (void*)image->constBits(), image->bytesPerLine());
        break;
    }
    return mat;
}

String getPureName(String s){
    int begin = s.rfind('/');
    int end = s.rfind('.');
    String ans = s.substr(begin+1, end-begin-1);
    return ans;

}

bool pointOnSegment(int x, int y, int x1, int y1, int x2, int y2){
    if( (x - x1) * (y2 - y1) == (x2 - x1) * (y - y1)
        && min(x1 , x2) <= x && x <= max(x1 , x2)
        && min(y1 , y2) <= y && y <= max(y1 , y2)
      )
        return true;
    else
        return false;
}

bool segmentIntersect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4){
    if(!(   min(x1,x2)<=max(x3,x4)
         && min(x3,x4)<=max(x1,x2)
         && min(y1,y2)<=max(y3,y4)
         && min(y3,y4)<=max(y1,y2)
         ) )
           return false;
    int u,v,w,z;
    u=(x3-x1)*(y2-y1)-(x2-x1)*(y3-y1);
    v=(x4-x1)*(y2-y1)-(x2-x1)*(y4-y1);
    w=(x1-x3)*(y4-y3)-(x4-x3)*(y1-y3);
    z=(x2-x3)*(y4-y3)-(x4-x3)*(y2-y3);
    return (u*v <= 0 && w*z <= 0);
}

polygon createPolygon(){
    polygon pg;
    pg.collinear = true;
    pg.x.clear();
    pg.y.clear();
    return pg;
}

void clearPolygon(polygon* pg){
    pg->collinear = true;
    pg->x.clear();
    pg->y.clear();
}

bool canAddPointToPolygon(polygon pg, int x, int y){
    int siz = pg.x.size();
    if(siz <=0 ) return true;
    if(x == pg.x[0] && y == pg.y[0]){
        if(siz <= 2) return false;
        if(pg.collinear == true) return false;
        return true;
    }
    bool intersect = false;
    int lx = pg.x[siz-1];
    int ly = pg.y[siz-1];
    for(int i = 1; i < siz - 1; i++){
        bool cur = segmentIntersect(pg.x[i - 1], pg.y[i - 1], pg.x[i], pg.y[i], lx, ly, x, y);
        if (cur == true){
            intersect = true;
            break;
        }
    }
    if(intersect == true) return false;
    if(siz >= 2 && pointOnSegment(x,y,pg.x[siz-1],pg.y[siz-1],pg.x[siz-2],pg.y[siz-2])){
        cout << "on Segment so cannot add" << endl << x << ","<< y<< endl;
        return false;
    }
    return true;
}

bool validPolygon(polygon pg){
    if(pg.x.size() != pg.y.size()) return false;
    int siz = pg.x.size();
    if(siz <= 3) return false;
    if(pg.x[0] != pg.x[siz-1]) return false;
    if(pg.y[0] != pg.y[siz-1]) return false;
    return true;
}

void addPointToPolygon(polygon* pg, int x, int y){
    int siz = pg->x.size();
    if(siz >= 2 && pg->collinear == true){
        pg->collinear = pointOnSegment(x, y, pg->x[0], pg->y[0], pg->x[1], pg->y[1]);
    }
    pg->x.push_back(x);
    pg->y.push_back(y);
}

bool nearPoint(int x,int y,int cx,int cy){
    if(x >= cx - MOUSE_GAP && x <= cx + MOUSE_GAP &&
       y >= cy - MOUSE_GAP && y <= cy + MOUSE_GAP) return true;
    return false;
}
