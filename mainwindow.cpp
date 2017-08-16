#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "util.h"

static int cookie = 1;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    srcimglabel = new SrcImgLabel(ui->src_bottom_info);
    desimglabel = new DesImgLabel(ui->des_bottom_info);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_open_src_img_clicked(){
    srcimglabel->selectOver = false;
    srcimglabel->open_img(ui->src_img_area);
}

void MainWindow::on_open_des_img_clicked(){
    desimglabel->hasSubImg = false;
    desimglabel->open_img(ui->des_img_area);
}

void MainWindow::on_select_ok_clicked(){
    if(srcimglabel->selectOver == false) return;
    if(desimglabel->hasImg == false) return;
    desimglabel->hasSubImg = true;
    desimglabel->subx = 0;
    desimglabel->suby = 0;
    int imgx = srcimglabel->minx - srcimglabel->img_anchor_x;
    int imgy = srcimglabel->miny - srcimglabel->img_anchor_y;
    int w = srcimglabel->maxx - srcimglabel->minx + 1;
    int h = srcimglabel->maxy - srcimglabel->miny + 1;
    desimglabel->subw = w;
    desimglabel->subh = h;
    desimglabel->subimage = new QImage(w, h, srcimglabel->image->format());
    for(int y = imgy; y < imgy + h ; y++){
        QRgb *rowData = (QRgb*)srcimglabel->image->scanLine(y);
        for(int x = imgx; x < imgx + w; x++){
            desimglabel->subimage->setPixel(x-imgx,y-imgy,rowData[x]);
        }
    }
    desimglabel->needDraw = true;
    desimglabel->update();
}

void MainWindow::on_poisson_clicked(){
    desimglabel->poisson();
}

void MainWindow::on_save_img_clicked(){
    char ss[128];
    string fs1 = getPureName(srcimglabel->filename.toStdString());
    string fs2 = getPureName(desimglabel->filename.toStdString());
    const char* s1 = fs1.c_str();
    const char* s2 = fs2.c_str();
    sprintf(ss,"%s_%s_%d.jpg",s1,s2,cookie++);
    printf("save img [ %s ]\n",ss);
    desimglabel->image->save(ss);
}






