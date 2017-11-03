#ifndef MY_LABEL_H
#define MY_LABEL_H

#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include "camera_current_pos/camera_current_pos.h"

#include <QTimer>

const double PI = 3.1415926;

class my_label : public QLabel
{
  Q_OBJECT

public:
  explicit my_label(QLabel *parent = 0);
  ~my_label();
  void setPixmap(const QPixmap &);


public Q_SLOTS:
  void CurrentCameraPos(const camera_current_pos::camera_current_pos &);
  void timeout();
  void directionChanged(double angles);

private:
  void paintEvent(QPaintEvent *);

  QPixmap m_Pixmap;
  QTimer m_timer;

  bool m_lostFlg;
  bool m_showFlg;
  camera_current_pos::camera_current_pos m_oldPosInfo;
  camera_current_pos::camera_current_pos m_curPosInfo;

  double m_curAngles;
  double m_oldAngles;
  double m_targetAngles;

  int m_imagew;
  int m_imageh;
};


#endif // MY_LABEL_H
