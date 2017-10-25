#include "editimglabel.h"

void EditImgLabel::save_img(){
    QString sfilename = QFileDialog::getSaveFileName(this,
            tr("Save Image"),
            filename,
            tr(" *.jpg;; *.png;; *.bmp;; *.tif;; *.GIF"));
    if (sfilename.isEmpty()){
       return;
    }
    QImage * save_img = this->image;
    if(!(save_img->save(sfilename))){
        QMessageBox::information(this,
            tr("Save the image"),
            tr("Failed to save the image!"));
    }
}

void EditImgLabel::mouseMoveEvent(QMouseEvent *event){
    ImgLabel::mouseMoveEvent(event);
    return;
}

void EditImgLabel::mousePressEvent(QMouseEvent *event){
    return;
}

void EditImgLabel::mouseReleaseEvent(QMouseEvent *event){
    return;
}

void EditImgLabel::paintEvent(QPaintEvent *event){
    ImgLabel::paintEvent(event);
}
