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
    double u,v,w,z;
    u=(x3-x1)*(y2-y1)-(x2-x1)*(y3-y1);
    v=(x4-x1)*(y2-y1)-(x2-x1)*(y4-y1);
    w=(x1-x3)*(y4-y3)-(x4-x3)*(y1-y3);
    z=(x2-x3)*(y4-y3)-(x4-x3)*(y2-y3);
    return (u*v <= 0 && w*z <= 0);
}

bool nearPoint(int x,int y,int cx,int cy){
    if(x >= cx - MOUSE_GAP && x <= cx + MOUSE_GAP &&
       y >= cy - MOUSE_GAP && y <= cy + MOUSE_GAP) return true;
    return false;
}

double modv(int x,int y){
    return sqrt(x * x + y * y);
}

int mulv(int x1, int y1, int x2, int y2){
    return (x1 * x2 + y1 * y2);
}

double pointDistanceSegment(int x, int y, int x1, int y1, int x2, int y2){
    int x_ab = x2 - x1;
    int y_ab = y2 - y1;
    int x_ap = x - x1;
    int y_ap = y - y1;
    int x_bp = x - x2;
    int y_bp = y - y2;
    double ab = modv(x_ab, y_ab);
    double ap = modv(x_ap, y_ap);
    if(ap == 0) return ap;
    double cos_bap = (double)mulv(x_ab,y_ab,x_ap,y_ap)/(ab * ap);
    if(cos_bap <= 0) return ap;
    double bp = modv(x_bp, y_bp);
    if(bp == 0) return 0;
    double cos_abp = (double)mulv(-x_ab,-y_ab,x_bp,y_bp)/(ab * bp);
    if(cos_abp <= 0) return bp;
    return (ap * sqrt(1 - cos_bap * cos_bap));
}


polygon createPolygon(){
    polygon pg;
    pg.x.clear();
    pg.y.clear();
    return pg;
}

void clearPolygon(polygon* pg){
    pg->x.clear();
    pg->y.clear();
}

bool canAddPointToPolygon(polygon pg, int x, int y){
    int siz = pg.x.size();
    if(siz <=0 ) return true;
    if(x == pg.x[0] && y == pg.y[0]){
        if(siz <= 2) return false;
        for(int i = 1 ; i < siz - 1; i++){
            //new seg & existed points dis < thresh
            if(pointDistanceSegment(pg.x[i],pg.y[i],pg.x[siz-1],pg.y[siz-1],x,y) < POINT_SEGMENT_THRESH) return false;
        }
        return true;
    }
    int lx = pg.x[siz-1];
    int ly = pg.y[siz-1];
    //new seg & existed segs intersect
    for(int i = 1; i < siz - 1; i++){
        if(segmentIntersect(pg.x[i - 1], pg.y[i - 1], pg.x[i], pg.y[i], lx, ly, x, y)) return false;
        //if(segmentIntersect(pg.x[i - 1], pg.y[i - 1], pg.x[i], pg.y[i], lx, ly, x, y)) return false;
    }
    for(int i = 1; i < siz; i++){
        //new point & existed segs dis < thresh
        if(pointDistanceSegment(x,y,pg.x[i-1],pg.y[i-1],pg.x[i],pg.y[i]) < POINT_SEGMENT_THRESH) return false;
        //new seg & existed points dis < thresh
        if(pointDistanceSegment(pg.x[i-1],pg.y[i-1],lx,ly,x,y) < POINT_SEGMENT_THRESH) return false;
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
    pg->x.push_back(x);
    pg->y.push_back(y);
}

bool canReplacePointInPolygon(polygon pg, int ind, int x, int y){
    int siz = pg.x.size();
    if(ind >= siz) return false;
    //new seg(2) & existed segs intersect
    //new point & existed segs dis < thresh
    //new seg(2) & existed points dis < thresh
    if(ind == 0){
        for(int i = 1; i < siz - 1; i++){
            if(i > 2 && segmentIntersect(x,y,pg.x[1],pg.y[1],pg.x[i-1],pg.y[i-1],pg.x[i],pg.y[i])) return false;
            if(i > 1 && i < siz - 2 && segmentIntersect(x,y,pg.x[siz-2],pg.y[siz-2],pg.x[i-1],pg.y[i-1],pg.x[i],pg.y[i])) return false;
            if(i > 1 && pointDistanceSegment(x,y,pg.x[i-1],pg.y[i-1],pg.x[i],pg.y[i]) < POINT_SEGMENT_THRESH) return false;
            if( i > 1 && pointDistanceSegment(pg.x[i],pg.y[i],x,y,pg.x[1],pg.y[1]) < POINT_SEGMENT_THRESH) return false;
            if( i < siz - 2 && pointDistanceSegment(pg.x[i],pg.y[i],x,y,pg.x[siz-2],pg.y[siz-2]) < POINT_SEGMENT_THRESH) return false;
        }
    }
    else if(ind == 1){
        for(int i = 2; i < siz; i++){
            if(i > 2 && i < siz -1 && segmentIntersect(x,y,pg.x[0],pg.y[0],pg.x[i-1],pg.y[i-1],pg.x[i],pg.y[i])) return false;
            if(i > 3 && segmentIntersect(x,y,pg.x[2],pg.y[2],pg.x[i-1],pg.y[i-1],pg.x[i],pg.y[i])) return false;
            if(i > 2 && pointDistanceSegment(x,y,pg.x[i-1],pg.y[i-1],pg.x[i],pg.y[i]) < POINT_SEGMENT_THRESH) return false;
            if( i < siz - 1 && pointDistanceSegment(pg.x[i],pg.y[i],x,y,pg.x[0],pg.y[0]) < POINT_SEGMENT_THRESH) return false;
            if( i > 2 && pointDistanceSegment(pg.x[i],pg.y[i],x,y,pg.x[2],pg.y[2]) < POINT_SEGMENT_THRESH) return false;
        }
    }
    else if(ind == siz - 2){
        for(int i = 0; i < ind; i++){
            if(i > 0 && i < ind -1 && segmentIntersect(x,y,pg.x[ind-1],pg.y[ind-1],pg.x[i-1],pg.y[i-1],pg.x[i],pg.y[i])) return false;
            if(i > 1 && segmentIntersect(x,y,pg.x[ind+1],pg.y[ind+1],pg.x[i-1],pg.y[i-1],pg.x[i],pg.y[i])) return false;
            if(i > 0 && pointDistanceSegment(x,y,pg.x[i-1],pg.y[i-1],pg.x[i],pg.y[i]) < POINT_SEGMENT_THRESH) return false;
            if( i < ind - 1 && pointDistanceSegment(pg.x[i],pg.y[i],x,y,pg.x[ind-1],pg.y[ind-1]) < POINT_SEGMENT_THRESH) return false;
            if( i > 0 && pointDistanceSegment(pg.x[i],pg.y[i],x,y,pg.x[ind+1],pg.y[ind+1]) < POINT_SEGMENT_THRESH) return false;
        }
    }
    else{
        for(int i = 0; i < ind; i++){
            if(i > 0 && i < ind -1 && segmentIntersect(x,y,pg.x[ind-1],pg.y[ind-1],pg.x[i-1],pg.y[i-1],pg.x[i],pg.y[i])) return false;
            if(i > 0 && segmentIntersect(x,y,pg.x[ind+1],pg.y[ind+1],pg.x[i-1],pg.y[i-1],pg.x[i],pg.y[i])) return false;
            if(i > 0 && pointDistanceSegment(x,y,pg.x[i-1],pg.y[i-1],pg.x[i],pg.y[i]) < POINT_SEGMENT_THRESH) return false;
            if( i < ind - 1 && pointDistanceSegment(pg.x[i],pg.y[i],x,y,pg.x[ind-1],pg.y[ind-1]) < POINT_SEGMENT_THRESH) return false;
            if( pointDistanceSegment(pg.x[i],pg.y[i],x,y,pg.x[ind+1],pg.y[ind+1]) < POINT_SEGMENT_THRESH) return false;
        }
        for(int i = ind + 1; i < siz; i++){
            if(i > ind + 1 && segmentIntersect(x,y,pg.x[ind-1],pg.y[ind-1],pg.x[i-1],pg.y[i-1],pg.x[i],pg.y[i])) return false;
            if(i > ind + 2 && segmentIntersect(x,y,pg.x[ind+1],pg.y[ind+1],pg.x[i-1],pg.y[i-1],pg.x[i],pg.y[i])) return false;
            if(i > ind + 1 && pointDistanceSegment(x,y,pg.x[i-1],pg.y[i-1],pg.x[i],pg.y[i]) < POINT_SEGMENT_THRESH) return false;
            if( pointDistanceSegment(pg.x[i],pg.y[i],x,y,pg.x[ind-1],pg.y[ind-1]) < POINT_SEGMENT_THRESH) return false;
            if( i > ind + 1 && pointDistanceSegment(pg.x[i],pg.y[i],x,y,pg.x[ind+1],pg.y[ind+1]) < POINT_SEGMENT_THRESH) return false;
        }
    }
    return true;
}

void replacePointInPolygon(polygon* pg, int ind, int x, int y){
    int siz = pg->x.size();
    if(ind >= siz) return;
    pg->x[ind] = x;
    pg->y[ind] = y;
    if(ind == 0){
       pg->x[siz-1] = x;
       pg->y[siz-1] = y;
    }
}

void getPolygonRect(polygon pg,int* minx,int* miny, int* maxx, int* maxy){
    int x1,y1,x2,y2;
    x1 = x2 = pg.x[0];
    y1 = y2 = pg.y[0];
    for(int i = 1; i < pg.x.size() - 1; i++){
        int cx = pg.x[i];
        int cy = pg.y[i];
        x1 = min(x1,cx);
        y1 = min(y1,cy);
        x2 = max(x2,cx);
        y2 = max(y2,cy);
    }
    *minx = x1;
    *miny = y1;
    *maxx = x2;
    *maxy = y2;
}

void minPolygon(polygon* pg, int minx, int miny){
    for(int i = 0; i < pg->x.size(); i++){
        pg->x[i] -= minx;
        pg->y[i] -= miny;
    }
}

void print_poly(polygon pg){
    for(int i=0;i<pg.x.size();i++){
        cout << pg.x[i] << "," << pg.y[i] << " ";
    }
    cout << endl;
}

double getIntersectX(int y, int x1, int y1, int x2, int y2){
    if(y == y1 && y1 == y2) return -1;
    if(x1 == x2) return x1;
    return (double)(x2-x1)*(y-y1)/(y2-y1)+x1 ;
}

void getPolygonMask(polygon pg, int w, int h, vector<vector<int> > &msk){
    msk.clear();
    int siz = pg.x.size();
    cout << "w:" << w << " h:" << h << endl;
    for(int i = 0; i < h; i++){
        vector<int> cur;
        for(int j = 0; j < w; j++){
            cur.push_back(0);
        }
        msk.push_back(cur);
    }
    for(int i = 0; i < h; i++){
        // line y = i intersect with poly
        vector<double> inter_x;
        for(int j = 1; j < siz; j++){
            if(segmentIntersect(0,i,w-1,i,pg.x[j-1],pg.y[j-1],pg.x[j],pg.y[j])){
                double cur_x = getIntersectX(i,pg.x[j-1],pg.y[j-1],pg.x[j],pg.y[j]);
                if(cur_x >= 0){
                    //cross problem
                    if(cur_x == (double)pg.x[j]){
                        if(pg.y[j-1] < i){
                            inter_x.push_back(cur_x);
                        }
                    }
                    else if(cur_x == (double)pg.x[j-1]){
                        if(pg.y[j] < i){
                            inter_x.push_back(cur_x);
                        }
                    }
                    else{
                        inter_x.push_back(cur_x);
                    }
                }
            }
        }
        sort(inter_x.begin(),inter_x.end());
        for(int j = 0; j < inter_x.size(); j += 2){
            int st = (int)inter_x[j];
            int ed = (int)inter_x[j+1];
            for(int k = st; k <= ed; k++){
                msk[i][k] = 1;
            }
        }
    }
}

void copyPolygon(polygon pg, polygon * pg2){
    pg2->x.clear();
    pg2->y.clear();
    for(int i=0;i<pg.x.size();i++){
        pg2->x.push_back(pg.x[i]);
        pg2->y.push_back(pg.y[i]);
    }
}


