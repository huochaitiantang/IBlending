#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

static MainWindow* s_main_window;
static int cookie = 1;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    s_main_window = this;
    desimglabel = new DesImgLabel();
    imglabel = new ImgLabel();
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
    //cout << "switch to click state" << endl;
}

void MainWindow::on_chose_clicked()
{
    STATE = CHOSE;
    //cout << "switch to chose state" << endl;
    //ui->bottom_info->setText("click on chose button");
}

void MainWindow::on_open_img_clicked(){
    imglabel->selectOver = false;
    //imglabel->needDraw = false;
    imglabel->image = new QImage;
    imglabel->filename = QFileDialog::getOpenFileName(this, tr("Chose img"), "", tr("Images (*.png *.bmp *.jpg *.tif *.GIF )"));
    if(imglabel->filename.isEmpty()){
        return;
    }
     if(! ( imglabel->image->load(imglabel->filename) ) ){
        QMessageBox::information(this, tr("Open failed!"), tr("Open failed"));
        return;
     }

     imglabel->setPixmap(QPixmap::fromImage(*imglabel->image));
     imglabel->setAlignment(Qt::AlignCenter);
     imglabel->setMouseTracking(true);
     int h = imglabel->image->height();
     int w = imglabel->image->width();
     imglabel->img_height = h;
     imglabel->img_width = w;
     if(h < MAX_Y)  {
         imglabel->img_anchor_y = (MAX_Y - h)/2;
         if( w >= MAX_X) imglabel->img_anchor_y -= 10;
     }
     else   imglabel->img_anchor_y = 0;
     if(w < MAX_X)  {
         imglabel->img_anchor_x = (MAX_X - w)/2;
         if(h >= MAX_Y) {
             imglabel->img_anchor_x -= 10;
         }
     }
     else imglabel->img_anchor_x = 0;
     //imglabel->resize(w,h);
     cout << "anchor_img_x:" << imglabel->img_anchor_x << endl;
     ui->img_area->setWidget(imglabel);
     ui->img_area->setMaximumHeight(MAX_Y);
     ui->img_area->setMaximumWidth(MAX_X);
}

void MainWindow::on_des_open_img_clicked(){
    desimglabel->hasSubImg = false;
    desimglabel->image = new QImage;
    desimglabel->desfilename = QFileDialog::getOpenFileName(this, tr("Chose img"), "", tr("Images (*.png *.bmp *.jpg *.tif *.GIF )"));
    if(desimglabel->desfilename.isEmpty()){
        return;
    }
     if(! ( desimglabel->image->load(desimglabel->desfilename) ) ){
        QMessageBox::information(this, tr("Open failed!"), tr("Open failed"));
        return;
     }

     desimglabel->setPixmap(QPixmap::fromImage(*desimglabel->image));
     desimglabel->setAlignment(Qt::AlignCenter);
     desimglabel->setMouseTracking(true);
     int h = desimglabel->image->height();
     int w = desimglabel->image->width();
     desimglabel->img_height = h;
     desimglabel->img_width = w;
     desimglabel->hasImg = true;
     if(h < MAX_Y)  {
         desimglabel->img_anchor_y = (MAX_Y - h)/2;
         if(w >= MAX_X) desimglabel->img_anchor_y -= 10;
     }
     else   desimglabel->img_anchor_y = 0;
     if(w < MAX_X) {
         desimglabel->img_anchor_x = (MAX_X - w)/2;
         if( h >= MAX_Y) desimglabel->img_anchor_x -= 10;
     }
     else desimglabel->img_anchor_x = 0;
     ui->des_img_area->setWidget(desimglabel);
     ui->des_img_area->setMaximumHeight(MAX_Y);
     ui->des_img_area->setMaximumWidth(MAX_X);
}

void MainWindow::on_select_ok_clicked(){
    if(imglabel->selectOver == false) return;
    if(desimglabel->hasImg == false) return;
    desimglabel->hasSubImg = true;
    desimglabel->subx = 0;
    desimglabel->suby = 0;
    //desimglabel->scale_x = 1.0;
    //desimglabel->scale_y = 1.0;

    cout << " format: " << imglabel->image->format() << endl;
    int imgx = imglabel->minx - imglabel->img_anchor_x;
    int imgy = imglabel->miny - imglabel->img_anchor_y;
    int w = imglabel->maxx - imglabel->minx + 1;
    int h = imglabel->maxy - imglabel->miny + 1;
    desimglabel->subw = w;
    desimglabel->subh = h;

    desimglabel->subimage = new QImage(w, h, imglabel->image->format());
    for(int y = imgy; y < imgy + h ; y++){
        QRgb *rowData = (QRgb*)imglabel->image->scanLine(y);
        for(int x = imgx; x < imgx + w; x++){
            desimglabel->subimage->setPixel(x-imgx,y-imgy,rowData[x]);
        }
    }
    desimglabel->needDraw = true;
    desimglabel->update();
}

void MainWindow::on_poisson_clicked(){
    if(desimglabel->hasSubImg == false || desimglabel->hasImg == false) return;
    QImage *srcImg = new QImage;
    *srcImg = desimglabel->subimage->scaled(desimglabel->subw,
                                            desimglabel->subh,
                                            Qt::IgnoreAspectRatio);
    Mat src, des, ans;
    vector<Mat> src_v;
    vector<Mat> des_v;
    vector<Mat> ans_v;
    src = QImage2cvMat(srcImg);
    des = QImage2cvMat(desimglabel->image);
    split(src, src_v);
    split(des,des_v);
    if(src_v.size() == 4) src_v.pop_back();
    if(des_v.size() == 4) des_v.pop_back();
    merge(src_v, src);
    merge(des_v,des);
    // 0 FR; 1 Normal Clone; 2 Mixed Clone
    ans = getPoissonMat(des, src, Rect(desimglabel->subx,
                                 desimglabel->suby,
                                 desimglabel->subw,
                                 desimglabel->subh), 1);
    split(ans,ans_v);
    Mat whiteC = 255 * Mat::ones(ans_v[0].rows, ans_v[0].cols, ans_v[0].depth());
    ans_v.push_back(whiteC);
    merge(ans_v,ans);
    imshow("ans",ans);
    *(desimglabel->image) = cvMat2QImage(ans);
    desimglabel->hasSubImg = false;
    desimglabel->needDraw = true;
    desimglabel->setPixmap(QPixmap::fromImage(*desimglabel->image));
    desimglabel->update();
}

void MainWindow::on_save_img_clicked(){
    char ss[128];
    String fs1 = getPureName(imglabel->filename.toStdString());
    String fs2 = getPureName(desimglabel->desfilename.toStdString());
    const char* s1 = fs1.c_str();
    const char* s2 = fs2.c_str();
    sprintf(ss,"%s_%s_%d.jpg",s1,s2,cookie++);
    cout << ss << endl;
    desimglabel->image->save(ss);
}

void ImgLabel::getCurXY(QMouseEvent *event){
    cur_x = event->x();
    cur_y = event->y();
}

void DesImgLabel::getCurXY(QMouseEvent *event){
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

void DesImgLabel::getRelativeXY(int px, int py, int * x, int * y){
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

bool DesImgLabel::inImg(int x, int y){
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

int DesImgLabel::inRectLine(int x, int y){
    if(inImg(x,y) == false || hasSubImg == false) return 0;
    int x1 = subx + img_anchor_x;
    int y1 = suby + img_anchor_y;
    int x2 = x1 + subw - 1;
    int y2 = y1 + subh - 1;
    if(x > x1 && y > y1 && x < x2 && y < y2) return 5;
    else if(x == x1 && y > y1 && y < y2) return 1;
    else if(x == x2 && y > y1 && y < y2) return 2;
    else if(y == y1 && x > x1 && x < x2) return 3;
    else if(y == y2 && x > x1 && x < x2) return 4;
    else if(x == x1 && y == y1) return 6;
    else if(x == x1 && y == y2) return 7;
    else if(x == x2 && y == y1) return 8;
    else if(x == x2 && y == y2) return 9;
    else return 0;
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
    s_main_window->ui->des_bottom_info->setText(qss);
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

void DesImgLabel::mousePressEvent(QMouseEvent *event){
    getCurXY(event);
    int tmp = inRectLine(cur_x,cur_y);
    stretch = tmp;
    if(stretch == 5){
        move_anchor_x = cur_x - img_anchor_x - subx;
        move_anchor_y = cur_y - img_anchor_y - suby;
    }
}

void ImgLabel::mouseReleaseEvent(QMouseEvent *event){
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

void DesImgLabel::mouseReleaseEvent(QMouseEvent *event){
    getCurXY(event);
    setCursor(Qt::ArrowCursor);
    if(stretch > 0) stretch = 0;
}

void ImgLabel::paintEvent(QPaintEvent *event)
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

void DesImgLabel::paintEvent(QPaintEvent *event){
    QLabel::paintEvent(event);
    //cout << "Heool:" << needDraw << endl;
    if(hasSubImg == false || needDraw == false){
        return;
    }
    if(image->format() != subimage->format()){
        cout << "format not matched!" << endl;
        return ;
    }
    if(subimage->width() > image->width() || subimage->height() > image->height()){
        hasSubImg = false;
        return;
    }
    QImage * DImg = new QImage(image->width(),image->height(),image->format());
    QImage * scaleImg = new QImage;
    *scaleImg = subimage->scaled(subw, subh , Qt::IgnoreAspectRatio);
    //cout << " " << subimage->width() << " " << subimage->height() << " "
    //     << " " << scaleImg->width() << " " << scaleImg->height() << endl;
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

    this->setPixmap(QPixmap::fromImage(*DImg)); //can trigger paintevent too, needDraw can stop repeat
//    QPainter painter(this);
//    painter.setPen(QPen(Qt::green,10));
//    painter.drawRect(QRect(subx + img_anchor_x, suby + img_anchor_y, subw, subh));
    needDraw = false;
}


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


