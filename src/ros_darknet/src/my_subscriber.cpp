    #include <ros/ros.h>  
    #include <image_transport/image_transport.h>  
    #include <opencv2/highgui/highgui.hpp>  
    #include <opencv2/core/core.hpp>
    #include <opencv/cv.h>
    #include <opencv2/opencv.hpp>
    #include <opencv2/imgproc/imgproc.hpp>
    #include <cv_bridge/cv_bridge.h>  
    //#include <darknet/box.h>
   
   #include"ClassifierDarknets.h"
    #include "ros_darknet/Bounding_box.h"  
   //using namespace std;
    using  namespace cv;
    using  namespace my_space_uts;
    ros::Publisher detected_rect_pub;
    cv::Mat  _img;
    ClassifierDarknet  DarknetDetect;
    double t0,t1;
    int framenum = 0;
    box last_savebox;

    image_transport::Publisher publish_darkmet;
  
    float bboxOverlap(box fistBox,box secondBox)
    {
        float xA = fistBox.x;
        float yA = fistBox.y;
        float widthA = fistBox.w;
        float heightA = fistBox.h;

        float xB = secondBox.x;
        float yB = secondBox.y;
        float widthB = secondBox.w;
        float heightB = secondBox.h;

        float endx = (xA+widthA>xB+widthB?xA+widthA:xB+widthB);
        float startx =(xA<xB?xA:xB);
        float width = widthA+widthB-(endx-startx);

        float endy = yA+heightA>yB+heightB?yA+heightA:yB+heightB;
        float starty = yA<yB?yA:yB;
        float height = heightA+heightB-(endy-starty);

        float ratio = 0.0f;
        float Area,AreaA,AreaB;

        if (width<=0||height<=0)
            return 0.0f;
        else
        {
            Area = width*height;
            AreaA = widthA*heightA;
            ratio = Area/AreaA;
            //AreaB = widthB*heightB;
            //ratio = Area /(op::fastMin(AreaA,AreaB));
        }
        return ratio;
         
    }

  void image_detect_palm_Callback(const sensor_msgs::ImageConstPtr& msg)
  {
    //printf("&&&&&&&&&&&&&&&\n");
    cv_bridge::CvImagePtr cv_ptr;
    try {
        cv_ptr = cv_bridge::toCvCopy(msg, "bgr8");
    } catch (cv_bridge::Exception& e) {
        return;
    }
    if (cv_ptr->image.empty()) return;
     _img  = cv_ptr->image;
    // cv::Mat showimg;
    // _img.copyTo(showimg);
    // printf("image =%d,%d\n",showimg.cols,showimg.rows);
    //printf("%d\n",framenum++);
      //  _img = cv_bridge::toCvShare(msg,"bgr8")->image;
    std::vector<box> preBox = DarknetDetect.Detectimg(_img);
     
         for(int j = 0;j< preBox.size();j++)
        {
           cv::rectangle(_img,
                           cv::Point((int)preBox[j].x,(int)preBox[j].y),
                           cv::Point{(int)(preBox[j].x + preBox[j].w),
                                     (int)(preBox[j].y + preBox[j].h)},
                           cv::Scalar{255.f,0.f,0.f}, 2);
        }
     
         //cv::imshow("view",_img);

         //add by liuzhenkun
         cv_ptr->image = _img;
         publish_darkmet.publish(cv_ptr->toImageMsg());
         //end bu add
         cv::waitKey(1);
  }
   
      
    int main(int argc, char **argv)  
    {  
      ros::init(argc, argv, "ros_darknet");  
      ros::NodeHandle nh;  
      ros::NodeHandle nad;  
      //cv::namedWindow("view");
      //cv::startWindowThread();  
      printf("000\n");
      last_savebox.x =0;
      last_savebox.y =0;

      last_savebox.w=0;
      last_savebox.h=0;
      //memset(last_savebox,0,sizeof(box));
      image_transport::ImageTransport it(nh);  
      //image_transport::Subscriber subdetect_palm = it.subscribe("detect_img_me", 1, image_detect_palm_Callback);
      
       detected_rect_pub = nad.advertise<ros_darknet::Bounding_box>("detect_aim",1);
       
      image_transport::Subscriber sub = it.subscribe("image_raw", 1, image_detect_palm_Callback);
      DarknetDetect.init();
     
      image_transport::ImageTransport nh_darknet(nh);
      publish_darkmet = nh_darknet.advertise("darknet_image",1);

      ros::spin();  
      //cv::destroyWindow("view");
     return 0; 
    }  
