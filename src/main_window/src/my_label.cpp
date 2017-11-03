#include "../include/my_label.h"
#include <QDebug>
#include <math.h>
#include <QString>

my_label::my_label(QLabel *parent) :
  QLabel(parent)
{
  m_oldPosInfo.cos2Angles = 0;
  m_oldPosInfo.sin2Angles = 0;
  m_oldPosInfo.FactorDepth = 0;
  m_oldPosInfo.RelativeDepth = 0;

  m_lostFlg = true;

  m_curAngles = 0;
  m_oldAngles = 0;
  m_targetAngles = 0;
  m_showFlg = true;

  m_imagew = 1280;
  m_imageh = 720;

  m_timer.start(300);
  connect(&m_timer,SIGNAL(timeout()),this,SLOT(timeout()));
}

my_label::~my_label()
{
}

void my_label::setPixmap(const QPixmap &pixmap)
{
  m_Pixmap = pixmap;
  this->repaint();
 // m_Pixmap = m_Pixmap.scaled(1080,1920,Qt::KeepAspectRatio);
}

void my_label::CurrentCameraPos(const camera_current_pos::camera_current_pos &Pos)
{
  if(-0.0000001 < Pos.FactorDepth && 0.0000001 > Pos.FactorDepth)
  {
    m_lostFlg = true;
  }
  else
  {
    m_lostFlg = false;

    m_curPosInfo = Pos;

    if(m_curPosInfo.sin2Angles < 0)
    {
      m_curAngles = -m_curPosInfo.cos2Angles;
    }
    else
    {
      m_curAngles = m_curPosInfo.cos2Angles;
    }
  }
}

void my_label::timeout()
{
  m_showFlg = !m_showFlg;
}

void my_label::directionChanged(double angles)
{

  if(angles < 0.001 && angles > -0.001)
  {

  }
  else
  {
    m_oldAngles += angles;
    if(m_oldAngles > 180)
    {
      m_oldAngles = 180.0 - m_oldAngles;
    }
  }
  qDebug() << "m_oldAngles = " << m_oldAngles;
}


void my_label::paintEvent(QPaintEvent *event)
{
  if(m_Pixmap.isNull())
  {
    return  ;
  }
  QPainter painter;
  QPolygon polygon;

  painter.begin(this);
  m_imagew = m_Pixmap.width();
  m_imageh = m_Pixmap.height();

  this->resize(m_Pixmap.size());
  painter.drawImage(0,0,m_Pixmap.toImage());
  {
    painter.setPen(Qt::red);
    painter.setFont(QFont("Lost",10));
    QString str = "Angles:";
    str += QString::number(m_curAngles,13,3);
    str += " Depth:";
    str += QString::number((m_curPosInfo.FactorDepth*m_curPosInfo.RelativeDepth),10,5);
    painter.drawText((m_imagew*0.7),20,str);
  }

  if(m_showFlg)
  {
    return ;
  }

  if(m_lostFlg)
  {
    painter.setPen(Qt::red);
    painter.setFont(QFont("Lost",30));
    painter.drawText(30,(m_imageh*0.85),"Track Lost");
  }
  else
  {

#if 1
    if(m_curAngles < (m_oldAngles - 5.0))
    {
      painter.setBrush(Qt::green);
      polygon.clear();
      polygon << QPoint(00,60) << QPoint(60,0) << QPoint(60,35) << QPoint(150,30) << QPoint(150,90)
                  << QPoint(60,85) << QPoint(60,120);
      //right
      painter.translate((m_imagew*0.7),(m_imageh*0.2+120));
      painter.rotate(180);
      painter.drawPolygon(polygon);
    }
    else if(m_curAngles > (m_oldAngles + 5.0))
    {
      painter.setBrush(Qt::green);
      polygon.clear();
      polygon << QPoint(00,60) << QPoint(60,0) << QPoint(60,35) << QPoint(150,30) << QPoint(150,90)
                  << QPoint(60,85) << QPoint(60,120);
      //left
      painter.translate((m_imagew*0.3),(m_imageh*0.2));
      painter.drawPolygon(polygon);
    }
    else
    {
      //up
      painter.setBrush(Qt::green);
      polygon.clear();
      polygon << QPoint(30,60) << QPoint(60,0) << QPoint(60,35) << QPoint(120,20) << QPoint(120,100)
                  << QPoint(60,85) << QPoint(60,120);
      painter.translate(((m_imagew*0.5)+30),(m_imageh-300));
      painter.rotate(90);
      painter.drawPolygon(polygon);
    }
#endif
    painter.end();

  }


}
