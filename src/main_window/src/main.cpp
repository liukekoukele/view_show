#include "../include/mainwindow.h"
#include <QApplication>
#include <QDebug>


int main(int argc,char ** argv)
{
  QApplication app(argc, argv);

  ros::init(argc,argv,"main_window");

  MainWindow m;
  m.show();

  return app.exec();;
}
