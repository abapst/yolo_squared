#!/usr/bin/bash

YOLO=`readlink -f .`
MODEL="tiny-yolo-voc"
VID="cars28"

FRCNN_DIR="/project/other/dragoneye/testing"
FRCNN_CSV="$FRCNN_DIR/vdet_results/"$VID".csv"
VIDEO="$FRCNN_DIR/videos/"$VID".mp4"

YOLO_DIR="/project/other/dragoneye/testing_yolo/vdet_results/yolo-voc"
YOLO_CSV="$YOLO_DIR/"$VID".csv"

$YOLO/build/run_draw_boxes $VIDEO $FRCNN_CSV temp.mp4
$YOLO/build/run_draw_boxes temp.mp4 $YOLO_CSV output.mp4
rm temp.mp4

stty sane
