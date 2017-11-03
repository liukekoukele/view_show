#ifndef MY_NODE_H
#define MY_NODE_H

#include <QObject>
#include <QTimer>

#include <ros/ros.h>
#include <ros/spinner.h>
#include <cv_bridge/cv_bridge.h>

#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>

#include <image_transport/image_transport.h>
#include <image_transport/subscriber_filter.h>

#include <sensor_msgs/CameraInfo.h>
#include <sensor_msgs/Image.h>
#include "camera_current_pos/camera_current_pos.h"


class my_node :public QObject
{
  Q_OBJECT
public:
  explicit my_node();
  explicit my_node(const std::string strTopicName);
  ~my_node();

  void  get_topic_callback(const sensor_msgs::ImageConstPtr & msg);
  void  get_CameraPos_callBack(const camera_current_pos::camera_current_pos &Pos);

public Q_SLOTS:
  void take_image();
  void start();
  void set_rate(double rate);

Q_SIGNALS:
void imageReceived(const cv::Mat & image);
void currentCameraPos(const camera_current_pos::camera_current_pos &);

private:

  QTimer   *mp_timer;
  bool m_startflg;
  image_transport::Subscriber m_subs;
  ros::Subscriber m_subCamaraPos;
  int m_rate;
};

#endif // MY_NODE_H
