# yolo_squared

Fast object detection using the YOLO architecture.
This repo is a fork of [Darknet](https://github.com/pjreddie/darknet)

I've streamlined most of the front-end code and made the API easier to understand. Video and image streaming is now done using [frame_io](https://github.com/abapst/frame_io) library. OpenCV is no longer a requirement.

Requirements:

  - GCC >= 6.0
  - CUDA >= 7.5

Optional:

  - CUDNN >= 5.0

## Install with the following commands

Clone repo:
```
git clone https://github.com/abapst/yolo_squared
cd yolo_squared
```

Edit Makefile:
  - Make sure the CUDA_DIR variable points to your CUDA installation.
  - Set the CUDNN flag if desired.

Download the YOLO model weights:
```
./get_weights.sh
```

Compile the examples:
```
make
```
You will see warnings, this is normal.

## Run the GPU test demo:
```
./scripts/run_gpu_test.sh
```

This will run inference on one image 1000 times and compute the FPS.

## Run the inference demo:
```
./scripts/process_video.sh
```

This will run box detection using the tiny-yolo-voc architecture on the data/cars.mp4 video. Box detections are stored in output/cars.csv and the detections are drawn on the video and stored in output/output.mp4
