#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../include/my_label.h"
#include "../include/my_node.h"
#include <QDebug>

namespace main_window {
QImage cvtCvMat2QImage(const cv::Mat & image, bool isBgr = true);
}



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

public Q_SLOTS:
  void slam_imageRecv(const cv::Mat & image);

  void openpose_imageRecv(const cv::Mat & image);

  void darknet_imageRecv(const cv::Mat & image);
private:
  Ui::MainWindow *ui;

  //slam
  my_node *mp_slamImageNode;
  my_node *mp_slamCameraPosNode;
  my_label *mp_slamLabelView;

  //openpose
  my_node *mp_openposeNode;

  //darknet
  my_node *mp_darknetNode;
};

#endif // MAINWINDOW_H
