#!/usr/bin/env bash




Deploy=nets/tracker.prototxt
Pretrain_model=~/tracking/GOTURN/scripts/nets/models/pretrained_model/tracker.caffemodel
videos_folder=~/code/video2image/bin/result/
gpu_id=0

#start_video_num=0
#pause_val = 1

bin/runTracker $Deploy $Pretrain_model $videos_folder $gpu_id
