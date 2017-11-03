#include "../include/mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  //slam
  mp_slamLabelView = new my_label();
  ui->verticalLayout_slam->addWidget(mp_slamLabelView);

  mp_slamImageNode = new my_node("image_raw");
  connect(mp_slamImageNode,SIGNAL(imageReceived(cv::Mat)),this,SLOT(slam_imageRecv(cv::Mat)));
  mp_slamCameraPosNode = new my_node("CameraPos");
  connect(mp_slamCameraPosNode,SIGNAL(currentCameraPos(camera_current_pos::camera_current_pos)),
          mp_slamLabelView,SLOT(CurrentCameraPos(camera_current_pos::camera_current_pos)));

  //openpose
  mp_openposeNode = new my_node("openpose_image");
  connect(mp_openposeNode,SIGNAL(imageReceived(cv::Mat)),this,SLOT(openpose_imageRecv(cv::Mat)));

  //darknet
  mp_darknetNode = new my_node("darknet_image");
  connect(mp_darknetNode,SIGNAL(imageReceived(cv::Mat)),this,SLOT(darknet_imageRecv(cv::Mat)));
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::slam_imageRecv(const cv::Mat &image)
{
  QPixmap qmp = QPixmap::fromImage(main_window::cvtCvMat2QImage(image.clone()));
  qmp = qmp.scaled(ui->tab_slam->width(),ui->tab_slam->height());
  mp_slamLabelView->setPixmap(qmp);
}

void MainWindow::openpose_imageRecv(const cv::Mat &image)
{
  QPixmap qmp = QPixmap::fromImage(main_window::cvtCvMat2QImage(image.clone()));
  ui->label_openpose->setPixmap(qmp);
}

void MainWindow::darknet_imageRecv(const cv::Mat &image)
{
  QPixmap qmp = QPixmap::fromImage(main_window::cvtCvMat2QImage(image.clone()));
  ui->label_darknet->setPixmap(qmp);
}

namespace main_window {

QImage cvtCvMat2QImage(const cv::Mat & image, bool isBgr)
{
  QImage qtemp;
  if(!image.empty() && image.depth() == CV_8U)
  {
    if(image.channels()==3)
    {
      const unsigned char * data = image.data;
      if(image.channels() == 3)
      {
        qtemp = QImage(image.cols, image.rows, QImage::Format_RGB32);
        for(int y = 0; y < image.rows; ++y, data += image.cols*image.elemSize())
        {
          for(int x = 0; x < image.cols; ++x)
          {
            QRgb * p = ((QRgb*)qtemp.scanLine (y)) + x;
            if(isBgr)
            {
              *p = qRgb(data[x * image.channels()+2], data[x * image.channels()+1], data[x * image.channels()]);
            }
            else
            {
              *p = qRgb(data[x * image.channels()], data[x * image.channels()+1], data[x * image.channels()+2]);
            }
          }
        }
      }
    }
    else if(image.channels() == 1)
    {
      // mono grayscale
      qtemp = QImage(image.data, image.cols, image.rows, image.cols, QImage::Format_Indexed8).copy();
      QVector<QRgb> my_table;
      for(int i = 0; i < 256; i++) my_table.push_back(qRgb(i,i,i));
      qtemp.setColorTable(my_table);
    }
    else
    {
      printf("Wrong image format, must have 1 or 3 channels\n");
    }
  }
  return qtemp;
}

}

