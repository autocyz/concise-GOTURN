//
// Created by ise on 16-10-21.
//

#include <opencv2/opencv.hpp>
#include <string>
#include "network/regressor.h"
#include "tracker/tracker.h"
#include "helper/bounding_box.h"
#include "loader/loader_base.h"

using std::string;

int main(int argc, char *argv[]){

    ::google::InitGoogleLogging(argv[0]);

    const string model_file = argv[1];
    const string pretrain_file = argv[2];
    const string video_folder = argv[3];

    int gpu_id = 0;
    if (argc >= 5) {
        gpu_id = atoi(argv[4]);
    }

    int start_video_num = 0;
    if (argc >= 6) {
        start_video_num = atoi(argv[5]);
    }

    int pause_val = 1;
    if (argc >= 7) {
        pause_val = atoi(argv[6]);
    }

    Regressor regressor(model_file,pretrain_file,gpu_id, false);

    Tracker tracker(false);

    Loader loaderVideo(video_folder);

    printf("%zu \n", loaderVideo.videos_[0].all_frames_.size());

    for(int video_num = 1; video_num < loaderVideo.videos_.size(); ++video_num ){
        Video video = loaderVideo.videos_[video_num];
        printf("video is: %s \n",video.video_name_.c_str());

        cv::Rect targetBox;
        cv::Mat image_curr;
        loaderVideo.loadFirstBox(video,image_curr,targetBox);

        BoundingBox bbox_gt;
        bbox_gt.x1_ = targetBox.x;
        bbox_gt.y1_ = targetBox.y;
        bbox_gt.x2_ = targetBox.x+targetBox.width;
        bbox_gt.y2_ = targetBox.y+targetBox.height;

        tracker.Init(image_curr,bbox_gt,&regressor);

//        string outName = "/media/ise/myfiles/xingtu/"+std::to_string(video_num)+".avi";
//        cv::VideoWriter outVideo;
//        outVideo.open(outName,CV_FOURCC('M','J','P','G'),25,
//                      cv::Size(image_curr.cols,image_curr.rows));


        for(int frame_num = 1; frame_num < video.all_frames_.size(); ++frame_num ){

            // Get image for the current frame.
            // (The ground-truth bounding box is used only for visualization).


            const string& image_file = video.video_name_ + "/" + video.all_frames_[frame_num];
            image_curr = cv::imread(image_file);

            // Track and estimate the target's bounding box location in the current image.
            // Important: this method cannot receive bbox_gt (the ground-truth bounding box) as an input.
            BoundingBox bbox_estimate_uncentered;
            tracker.Track(image_curr, &regressor, &bbox_estimate_uncentered);

            targetBox.x = (int)bbox_estimate_uncentered.x1_;
            targetBox.y = (int)bbox_estimate_uncentered.y1_;
            targetBox.width = (int)bbox_estimate_uncentered.x2_ - targetBox.x;
            targetBox.height = (int)bbox_estimate_uncentered.y2_ - targetBox.y;

            cv::Mat display;
            image_curr.copyTo(display);
            cv::rectangle(display,targetBox,CV_RGB(255,0,0), 2);
            cv::imshow("tracking out",display);

//            outVideo.operator<<(display);

//            std::cout<<image_file<<std::endl;
//            printf("x1: %.0f  y1: %.0f "
//                           "x2: %.0f  y2: %.0f \n",
//                   bbox_estimate_uncentered.x1_,bbox_estimate_uncentered.y1_,
//                   bbox_estimate_uncentered.x2_,bbox_estimate_uncentered.y2_);
            cv::waitKey(1);
        }

        cv::destroyAllWindows();

    }

    return 0;

}