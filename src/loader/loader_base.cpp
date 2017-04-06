//
// Created by ise on 16-10-23.
//
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <helper/helper.h>
#include <opencv2/opencv.hpp>


#include "loader_base.h"

namespace bfs = boost::filesystem;

bool getBox = false;
bool drawing_box = false;

Loader::Loader(const string& _videos_folder)
        :videos_folder_(_videos_folder) {
    loadervideos(_videos_folder);
}

void Loader::loadervideos(const string _videos_folder){
    if(!bfs::is_directory(_videos_folder)){
        printf("ERROR - %s is not a valid directory!\n",_videos_folder.c_str());
        return;
    }

    vector<string> videos;
    find_subfolders(_videos_folder,&videos);
    printf("find %zu videos ... \n" ,videos.size());

    for(int i = 0; i < videos.size(); ++i){
        const string video_name = videos[i];
        const string video_path = _videos_folder+"/"+video_name;

        printf("Loading video : %s \n",video_name.c_str());

        Video video;
        video.video_name_ = video_path;

        const boost::regex image_filer(".*\\.jpg");
        find_matching_files(video_path,image_filer,&video.all_frames_);
        printf("%s have %zu frames\n",video_name.c_str(),video.all_frames_.size());
        videos_.push_back(video);
    }

}
void Loader::loadFirstBox(Video video,cv::Mat& image,cv::Rect& firstBox)
{
;
    const string& image_file = video.video_name_ + "/" + video.all_frames_[0];

    image = cv::imread(image_file);

    cv::Rect box;
    const string windowName = "first image";
    cv::namedWindow(windowName,CV_WINDOW_AUTOSIZE);
    cv::setMouseCallback(windowName,mouseHandler,&box);
    cv::Mat firstImage;

    while(!getBox){
        image.copyTo(firstImage);
        cv::rectangle(firstImage,box,CV_RGB(255,0,0));
        cv::imshow(windowName,firstImage);
        cv::waitKey(30);
    }
    cv::setMouseCallback(windowName,NULL,NULL);
    firstBox = box;
    printf("x1: %i  y1: %i "
                   "width: %i  height: %i \n",
          box.x,box.y,box.width,box.height);
    cv::waitKey(0);
    cv::destroyAllWindows();
    getBox = false;
}

 void Loader::mouseHandler(int event, int x, int y, int flag, void *userdata ){

    cv::Rect* box = (cv::Rect*)userdata;
    switch (event)
    {
        case CV_EVENT_MOUSEMOVE:
            if(drawing_box){
                box->width = x - box->x;
                box->height = y - box->y;
            }
            break;
        case CV_EVENT_LBUTTONDOWN:
            drawing_box = true;
            box->x = x;
            box->y = y;
            break;
        case CV_EVENT_LBUTTONUP:
            drawing_box = false;
            getBox = true;
            break;
        default:
            break;
    }
}
