import inspect
import os
import random
import sys
def extract_log(log_file,new_log_file,key_word):
    with open(log_file, 'r') as f:
      with open(new_log_file, 'w') as train_log:
  #f = open(log_file)
    #train_log = open(new_log_file, 'w')
        for line in f:
          if 'Syncing' in line:
            continue
          if 'nan' in line:
            continue
          if 'Region 82 Avg' in line:
            continue
          if 'Region 94 Avg' in line:
            continue
          if 'Region 106 Avg' in line:
            continue
          if 'total_bbox' in line:
            continue  
          if 'Loaded' in line:
            continue              
          if key_word in line:
            train_log.write(line)
    f.close()
    train_log.close()

def extract_log2(log_file,new_log_file,key_word):
    with open(log_file, 'r') as f:
      with open(new_log_file, 'w') as train_log:
  #f = open(log_file)
    #train_log = open(new_log_file, 'w')
        for line in f:
          if 'Syncing' in line:
            continue
          if 'nan' in line:
            continue
          if 'Region 94 Avg' in line:
            continue
          if 'Region 106 Avg' in line:
            continue
          if 'total_bbox' in line:
            continue  
          if 'Loaded' in line:
            continue     
          if 'IOU: 0.000000' in line:
            continue                          
          if key_word in line:
            del_num=line.replace("v3 (mse loss, Normalizer: (iou: 0.75, obj: 1.00, cls: 1.00) Region 82 Avg (", "")
            train_log.write(del_num.replace(")", ""))
    f.close()
    train_log.close() 
extract_log('train_yolov3.log','train_log_loss.txt','images')
extract_log2('train_yolov3.log','train_log_iou.txt','IOU')
