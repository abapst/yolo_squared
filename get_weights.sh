#!/bin/bash

SOURCE="http://pjreddie.com/media/files/"

wget --no-check-certificate $SOURCE"tiny-yolo-voc.weights"
mv tiny-yolo-voc.weights models/tiny-yolo-voc/tiny-yolo-voc.weights

wget --no-check-certificate $SOURCE"tiny-yolo.weights"
mv tiny-yolo.weights models/tiny-yolo-coco/tiny-yolo-coco.weights

wget --no-check-certificate $SOURCE"yolo-voc.weights"
mv yolo-voc.weights models/yolo-voc/yolo-voc.weights

wget --no-check-certificate $SOURCE"yolo.weights"
mv yolo.weights models/yolo-coco/yolo-coco.weights

echo "Done!"
