#include "../include/my_node.h"
#include <QDebug>

my_node::my_node()
{
  mp_timer = new QTimer();
  m_startflg = false;
  m_rate = 40;

  ros::NodeHandle nh;
  image_transport::ImageTransport it(nh);

  m_subs = it.subscribe(nh.resolveName("openpose_image"), 1, &my_node::get_topic_callback, this);

  connect(mp_timer,SIGNAL(timeout()),this,SLOT(take_image()));
  mp_timer->start((1000/40));

}

my_node::my_node(const std::string strTopicName)
{
  mp_timer = new QTimer();
  m_startflg = false;
  m_rate = 40;

  if("CameraPos" == strTopicName)
  {
    ros::NodeHandle h;
    m_subCamaraPos = h.subscribe(strTopicName,1,&my_node::get_CameraPos_callBack,this);
  }
  else if("image_raw" == strTopicName || "openpose_image" == strTopicName
          || "darknet_image" == strTopicName)
  {
    ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);
    m_subs = it.subscribe(nh.resolveName(strTopicName), 1, &my_node::get_topic_callback, this);
  }

  connect(mp_timer,SIGNAL(timeout()),this,SLOT(take_image()));
  mp_timer->start((1000/40));
}

my_node::~my_node()
{
  delete mp_timer;
  mp_timer = NULL;
}

void my_node::get_topic_callback(const sensor_msgs::ImageConstPtr &msg)
{
  if(msg->data.size())
  {
    cv_bridge::CvImageConstPtr ptr = cv_bridge::toCvShare(msg);
    if(msg->encoding.compare(sensor_msgs::image_encodings::BGR8) == 0)
    {
      cv::Mat cpy = ptr->image.clone();
      Q_EMIT imageReceived(cpy);
    }
    else if(msg->encoding.compare(sensor_msgs::image_encodings::RGB8) == 0)
    {
      cv::Mat bgr;
      cv::cvtColor(ptr->image, bgr, cv::COLOR_RGB2BGR);
      Q_EMIT imageReceived(bgr);
    }
    else
    {
      ROS_INFO("find_object_ros: Encoding \"%s\" detected. Supported image encodings are bgr8 and rgb8...", msg->encoding.c_str());
    }
  }
}


void my_node::get_CameraPos_callBack(const camera_current_pos::camera_current_pos &Pos)
{
  Q_EMIT currentCameraPos(Pos);
}



void my_node::set_rate(double rate)
{
  if(rate < 0)
  {
    m_rate = 0;
  }
  else
  {
    m_rate = rate;
  }

  if(m_startflg)
  {
    mp_timer->setInterval(m_rate?(1000/m_rate):0);
  }

}

void my_node::take_image()
{
  ros::spinOnce();
}

void my_node::start()
{
  if(m_startflg)
  {
    m_startflg = false;
    mp_timer->stop();
  }
  else
  {
    m_startflg = true;
   // mp_timer->setInterval();
    mp_timer->start(m_rate?(1000/m_rate):0);
  }

}

