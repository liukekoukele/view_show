/*
 * ClassifierDarknet.h
 *
 *  Created on: May 31, 2017
 *      Author: zhuyong
 */

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "opencv2/core/version.hpp"
#include <boost/bind.hpp>
#include <boost/timer.hpp>
#include <boost/format.hpp>
#include <boost/ref.hpp>
#include <boost/foreach.hpp>
#include <boost/thread/thread.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
//#include "stdio.h "
#include <time.h>
#include <sstream>
#include <fstream>

#include <ros_darknet/image.h>

//#include <darknet/region_layer.h>
#include <ros_darknet/utils.h>
//#include <direct.h>
#include <ros_darknet/data.h>
#include "ClassifierDarknets.h"


namespace my_space_uts {

 // using namespace std;
  using namespace cv;
   char **names=NULL;
 //char *cfg = NULL;
//char *weights=NULL;
 char *buffer;
float **masks = 0; 
static float *predictions;
static float *avg;
static float *last_avg2;
static float *last_avg;
static int count =0;
  int demo_frame = 3;
  int demo_index =0;
  int demo_delay =0;
  ClassifierDarknet::ClassifierDarknet()
  {
     //darknet 初始化
     //std::string datacfg = ;
      boxes =NULL;
      probs = NULL;
      alphabet = NULL ;
     demo_detections = 0 ;
     predictions = NULL;
     thresh = 0 ;
     hier_thresh =0;
  

  }

  ClassifierDarknet::~ClassifierDarknet()
  {
     if(buffer!=NULL)
      {
          free(buffer);
      }

  }
  void ClassifierDarknet::init()
  {
     
        //ros_call_darknet_detect 
    //也获得路径  
    /*if((buffer = getcwd(NULL, 0)) == NULL)  
    {  
        perror("getcwd error");  
    }  */
    char *buffer = "/home/liuzhenkun/work/view_show";
    //读取文件
     char tmpbuffer[1000];
      //sprintf(tmpbuffer,"%s/src/ROSDarknet/data/coco.data",buffer);
     sprintf(tmpbuffer,"%s/src/ros_darknet/data/coco.data",buffer);
     list *options = read_data_cfg(tmpbuffer);//home/zy/catkin_ws/src/ros_call_darknet_detect/
    
     int classes = option_find_int(options, "classes", 20);
         memset(tmpbuffer,0,1000*sizeof(char));
     sprintf(tmpbuffer,"%s/src/ros_darknet/data/names.list",buffer);
     char *name_list = option_find_str(options, "names", tmpbuffer);//home/zy/catkin_ws/src/ros_call_darknet_detect/
     //printf("classes=%d\n",classes);
     names = get_labels(name_list);
    
    
     alphabet = load_alphabet();
     memset(tmpbuffer,0,1000*sizeof(char));
     //sprintf(tmpbuffer,"%s/src/ROSDarknet/data/yolo.cfg",buffer);
     sprintf(tmpbuffer,"%s/src/ros_darknet/data/yolo.cfg",buffer);
    // printf("%s\n",tmpbuffer);
//char * cfgfile ="data/yolo.cfg"; 
     net = parse_network_cfg(tmpbuffer);
   
     memset(tmpbuffer,0,1000*sizeof(char));
     //sprintf(tmpbuffer,"%s/src/ROSDarknet/data/yolo.weights",buffer);
      sprintf(tmpbuffer,"%s/src/ros_darknet/data/yolo.weights",buffer);
     //char *weightsfile = "src/ros_call_darknet_detect/data/yolo.weights";
     load_weights(&net, tmpbuffer);
      
     thresh = 0.24;
     hier_thresh = 0.5;
     set_batch_network(&net, 1);
     srand(2222222);
    l = net.layers[net.n-1];
    demo_detections = l.n*l.w*l.h;
    int j;
    //声明分配数组空间
    predictions = (float**)calloc(demo_frame, sizeof(float*));
    for(j = 0; j < demo_frame; ++j)
    predictions[j] = (float *) calloc(l.outputs, sizeof(float));
    boxes = (box *)calloc(l.w*l.h*l.n, sizeof(box));
    probs = (float**)calloc(l.w*l.h*l.n, sizeof(float *));
    for(j = 0; j < l.w*l.h*l.n; ++j) 
    probs[j] = (float *)calloc(l.classes+1, sizeof(float));
        if (l.coords > 4){
            masks = (float**)calloc(l.w*l.h*l.n, sizeof(float*));
            for(j = 0; j < l.w*l.h*l.n; ++j) masks[j] = (float*)calloc(l.coords-4, sizeof(float *));
        }
    avg = (float *) calloc(l.outputs, sizeof(float));
    last_avg  = (float *) calloc(l.outputs, sizeof(float));
    last_avg2 = (float *) calloc(l.outputs, sizeof(float));
  }
  
  std::vector<box> ClassifierDarknet::Detectimg(cv::Mat &imgMat)
   {
    
    std::vector<box> preBoxes;
     IplImage im(imgMat);
    float nms=.3;

     image out = ipl_to_image(&im);
     rgbgr_image(out);
    image sized = letterbox_image(out, net.w, net.h); 
    float *X=sized.data;
   
   float *prediction= network_predict(net, X);

    memcpy(predictions[demo_index], prediction, l.outputs*sizeof(float));
    mean_arrays(predictions, demo_frame, l.outputs, avg);
    l.output = last_avg2;
    if(demo_delay == 0) l.output = avg;


   get_region_boxes(l, out.w, out.h, net.w, net.h, thresh, probs, boxes, masks, 0, 0, hier_thresh, 1);
   // get_region_boxes(l, out.w, out.h, net.w, net.h, thresh, probs, boxes, 0, 0, hier_thresh, 1);
   if (nms) do_nms_obj(boxes, probs, l.w*l.h*l.n, l.classes, nms);
    //draw_detections(out, l.w*l.h*l.n, thresh, boxes, probs, names, alphabet, l.classes);
      //draw_detections(out, l.w*l.h*l.n, thresh, boxes, probs, masks, names, alphabet, l.classes);
    int num = l.w*l.h*l.n;
    int i;
    int palm_num = 0;
   // printf("pc\n");
    for(i = 0; i < num; ++i){
        int class_th = max_index(probs[i], l.classes);
        //printf("lll=%d\n",class_th);
       //if(class_th!=5)
       //  continue;
        // printf("%d\n",class_th);
        float prob = probs[i][class_th];
        if(prob > thresh)
        {
            int width = out.h * .006;

           // printf("%s: %.0f%%\n", names[class_th], prob*100);
            box b = boxes[i];

            int left  = (b.x-b.w/2.)*out.w;
            int right = (b.x+b.w/2.)*out.w;
            int top   = (b.y-b.h/2.)*out.h;
            int bot   = (b.y+b.h/2.)*out.h;

            if(left < 0) left = 0;
            if(right > out.w-1) right = out.w-1;
            if(top < 0) top = 0;
            if(bot > out.h-1) bot = out.h-1;
            b.x = left;
            b.y = top;
            b.w = right -left;
            b.h = bot - top;
            
          //  draw_box_width(im, left, top, right, bot, width, red, green, blue);
            preBoxes.push_back(b);
          // printf("up_palm\n");
           palm_num ++;
        }
    }
  //  timespec time1;
  //   clock_gettime( CLOCK_MONOTONIC, &time1);
    // char ch[500];  
    
//     sprintf(ch, "%s/src/ROSDarknet/%d%d", buffer,time1.tv_sec,time1.tv_nsec);
  //   save_image(out, ch);
    
  // int i;
   /*for(i = 0; i < out.w*out.h; ++i){
        float swap = out.data[i];
        out.data[i] = out.data[i+out.w*out.h*2];
        out.data[i+out.w*out.h*2] = swap;
    }
   
    int step = im.widthStep;
    int x, y, k;
      
     
    for(y = 0; y < out.h; ++y){
        for(x = 0; x < out.w; ++x){
            for(k= 0; k < out.c; ++k){
                im.imageData[y*step + x*out.c + k] = (unsigned char)(out.data[k*out.h*out.w + y*out.w + x]*255);
            }
        }
    }*/
    demo_index = (demo_index + 1)%demo_frame;
     free_image(out);
     free_image(sized);
     //free(boxes);
     //free_ptrs((void **)probs, l.w*l.h*l.n);
    /*imgMat = cv::Mat(&im);
    if(count % (demo_delay+1) == 0){
                float *swap = last_avg;
                last_avg  = last_avg2;
                last_avg2 = swap;
                memcpy(last_avg, avg, l.outputs*sizeof(float));
            }*/
    // cv::imshow("view",imgMat);
     int c = cvWaitKey(1);
    if (c != -1) c = c%256;
    if (c == 10){
        if(demo_delay == 0) demo_delay = 60;
        else if(demo_delay == 5) demo_delay = 0;
        else if(demo_delay == 60) demo_delay = 5;
        else demo_delay = 0;
    } else if (c == 27) {
        //demo_done = 1;
        return preBoxes;
    } else if (c == 82) {
        thresh += .02;
    } else if (c == 84) {
        thresh -= .02;
        if(thresh <= .02) thresh = .02;
    } else if (c == 83) {
        hier_thresh += .02;
    } else if (c == 81) {
        hier_thresh -= .02;
        if(hier_thresh <= .0) hier_thresh = .0;
    }
     ++count;
    return  preBoxes;
  }

}

