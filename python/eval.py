import numpy as np
import pandas
from operator import itemgetter
import copy
import matplotlib.pyplot as plt

"""
Return a dict with frame number as keys and a list of box detections as
the values. Box detections are parameterized as:
    [left,top,width,height,conf,class]
"""
def read_csv(filename,add_class_col=0):
    df = pandas.read_csv(filename) 
    
    """
    Add a dummy class column if this is an old-style csv from 2016 dragoneye
    """
    if add_class_col:
        df['class'] = pandas.Series(np.ones(len(df)),index=df.index)

    f = np.unique(df['frame'])
    c = np.unique(df['class'])
    
    data = {}
    for cls in c:
        data[cls] = {}
        for frame in f:
            data[cls][frame] = df.loc[(df['frame'] == frame) & (df['class'] == cls)].values[:,1:6].tolist() 

    return data

"""
Compute intersection-over-union metric for a single pair of boxes of the form
    [left,top,width,height]
"""
def compute_IOU_one(b1,b2):

    b1_area = b1[2]*b1[3]
    b2_area = b2[2]*b2[3]

    xl = max(b1[0],b2[0])
    yt = max(b1[1],b2[1])
    xr = min(b1[0]+b1[2],b2[0]+b2[2])
    yb = min(b1[1]+b1[3],b2[1]+b2[3])
    iw = xr-xl
    ih = yb-yt
    if ((iw <= 0.) | (ih <= 0.)):
        return 0.
    else:
        inter_area = iw*ih
        iou = (inter_area)/(b1_area + b2_area - inter_area)
        return iou

"""
Compute intersection-over-union metric for a list of bounding boxes U with
a single bounding box b. Returns a vector of size len(U). Boxes are of the form
    [left,top,width,height,conf]
Conf is not used.
"""
def compute_IOU(U,b,match_thresh):
    IOU = [0]*len(U)
    for u in xrange(len(U)):
        iou = compute_IOU_one(U[u][:4],b[:4])
        w = float(U[u][2])
        h = float(U[u][3])
        thr = min(match_thresh,w*h/((w+10)*(h+10)))
        if  (iou >= thr):
            IOU[u] = iou

    assert (len(IOU) == len(U))
    return IOU

"""
Declare detection as matches or not matches to the corresponding
ground truth using the greedy algorithm described by the ImageNet
object detection challenge. Detections have already been sorted
in descending order by confidence.
"""
def match_detections_frame(ground_truth,detections,match_thresh):

    if detections == []:
        matches = []
        return matches
    if ground_truth == []:
        matches = [0]*len(detections)
        return matches

    U = copy.deepcopy(ground_truth) # set of unmatched ground_truth
    matches = [0]*len(detections)

    for j in xrange(len(detections)):
        if U:
            iou = compute_IOU(U,detections[j],match_thresh)
            if (sum(iou) > 0):
                matches[j] = 1 
                k = np.argmax(iou)
                U.pop(k)

    return matches

"""
Generate (0,1) matches for every detection in every class/frame pair.
This uses the greedy match algorithm described by the ImageNet object
detection challenge evaluation paper.

"Russakovsky et al., 'ImageNet Large Scale Visual Recognition Challenge', Int J Comput Vis, 2015"
"""
def generate_matches(ground_truth,detections,match_thresh=0.5):
    matches = {}

    for cls in ground_truth.keys():
        matches[cls] = {}
        for frame in ground_truth[cls].keys():
            matches[cls][frame] = match_detections_frame(ground_truth[cls][frame],detections[cls][frame],match_thresh)

    return matches
        
"""
Make the ground truth and detection dicts have the same class and frame
keys (i.e. if a key exists in one but not the other, make an empty key
in the other)
"""
def equalize_data(ground_truth,detections):

    # add empty frames to detections (missing detections)
    for cls in ground_truth.keys():
        if cls not in detections.keys():
            detections[cls] = {}
        for frame in ground_truth[cls].keys():
            if frame not in detections[cls].keys():
                detections[cls][frame] = []

    # add empty frames to ground_truth (spurious detections)
    assert (detections.keys() == ground_truth.keys())
    for cls in ground_truth.keys():
        for frame in detections[cls].keys():
            if frame not in ground_truth[cls].keys():
                ground_truth[cls][frame] = []

    for cls in ground_truth.keys():
        assert (detections[cls].keys() == ground_truth[cls].keys())

    return (ground_truth,detections)

def compute_metrics(ground_truth,detections,matches,n=100):

    P,R,FPR,correct = {},{},{},{}
    thresh = np.linspace(0,1,100)

    for cls in ground_truth.keys():
        P[cls],R[cls],FPR[cls] = [0]*n,[0]*n,[0]*n
        correct[cls] = {}
        Ntrue = float(sum(len(ground_truth[cls][f]) for f in ground_truth[cls].keys()))
        Ndet = float(sum(len(matches[cls][f]) for f in matches[cls].keys()))
        for ii,t in enumerate(thresh):
            for frame in ground_truth[cls].keys(): 
                correct[cls][frame] = [(i[4] >= t) for i in detections[cls][frame]]                 
                
            n_tp = float(sum(sum(i*j for i,j in zip(correct[cls][f],matches[cls][f])) for f in ground_truth[cls].keys()))
            n_precision = float(sum(sum(correct[cls][f]) for f in ground_truth[cls].keys()))
            n_fp = Ndet - n_tp
            if n_precision == 0:
                P[cls][ii] = 1.
            else:
                P[cls][ii] = n_tp/n_precision
            R[cls][ii] = n_tp/Ntrue
            FPR[cls][ii] = n_fp/(845-Ntrue)
             
    return(P,R,FPR) 

def do_eval(csv_truth,csv_pred):

    truth = read_csv(csv_truth,add_class_col=1)
    pred = read_csv(csv_pred,add_class_col=1)
    results = {}

    """
    Fill in empty detections and spurious detections
    """ 
    (truth,pred) = equalize_data(truth,pred)

    """
    Sort detections in each frame by descending confidence
    """
    for cls in pred.keys():
        for frame in pred[cls].keys():
            pred[cls][frame] = sorted(pred[cls][frame],key=itemgetter(-1),reverse=True)

    """
    Greedily generate matches for predictions against ground truth boxes
    """
    matches = generate_matches(truth,pred,match_thresh=0.5)

    """
    Compute precision/recall/FPR
    """
    (P,R,FPR) = compute_metrics(truth,pred,matches,n=100)

    results['truth'] = truth
    results['pred'] = pred
    results['matches'] = matches
    results['P'] = P
    results['R'] = R
    results['AP'] = [np.mean(cls) for cls in P.keys()]
    results['ntruth'] = [sum(len(truth[cls][f]) for f in truth[cls].keys()) for cls in truth.keys()]
    results['npred'] = [sum(len(pred[cls][f]) for f in pred[cls].keys()) for cls in pred.keys()]
    results['nmatches'] = [sum(sum(matches[cls][f]) for f in matches[cls].keys()) for cls in matches.keys()]

    meanwidth_truth = np.mean([b[2] for f in truth[cls].keys() for cls in truth.keys() for b in truth[cls][f]])

    meanheight_truth = np.mean([b[3] for f in truth[cls].keys() for cls in truth.keys() for b in truth[cls][f]])

    meanwidth_pred = np.mean([b[2] for f in pred[cls].keys() for cls in pred.keys() for b in pred[cls][f]])

    meanheight_pred = np.mean([b[3] for f in pred[cls].keys() for cls in pred.keys() for b in pred[cls][f]])

    results['mean_detection_truth'] = [meanwidth_truth,meanheight_truth]
    results['mean_detection_pred'] = [meanwidth_pred,meanheight_pred]

    return results

def test_simple():
    a = []

    a.append([0,0,150,150,.9])
    a.append([0,300,150,150,.66])
    a.append([300,0,150,150,.55])
    a.append([300,300,150,150,.33])
    b = copy.deepcopy(a)

    m = match_detections_frame(a,b)
    print("test_results={}".format(m))

def test_yolo():
    rcnn_dir = "/project/other/dragoneye/testing/vdet_results/"
    yolo_dir = "/project/other/dragoneye/testing_yolo/vdet_results/"
    video = "cars20.csv"
    
    results = do_eval(rcnn_dir+video,yolo_dir+video)

    return results

if __name__ == "__main__":
    test_simple()
