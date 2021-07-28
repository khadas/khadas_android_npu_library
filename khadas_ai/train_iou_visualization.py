import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
#%matplotlib inline
 
lines = 234429    #Change to self generated Number of rows in train_log_iou.txt

#Adjusting the following two sets of numbers will help you view the details of the drawing
start_ite = 1 #Ignore Number of all lines starting in train_log_iou.txt
end_ite = 234429 #Ignore Number of all lines ending in train_log_iou.txt
result = pd.read_csv('train_log_iou.txt', skiprows=[x for x in range(lines) if ((x<start_ite) |(x>end_ite)) ] ,error_bad_lines=False, names=['IOU', 'count', 'class_loss', 'iou_loss', 'total_loss'])
result.head()
 
result['IOU']=result['IOU'].str.split(': ').str.get(1)
result['count']=result['count'].str.split(': ').str.get(1)
result['class_loss']=result['class_loss'].str.split('= ').str.get(1)
result['iou_loss']=result['iou_loss'].str.split('= ').str.get(1)
result['total_loss']=result['total_loss'].str.split('= ').str.get(1)
result.head()
result.tail()
 
# print(result.head())
# print(result.tail())
# print(result.dtypes)
print(result['IOU'])
#print(result['count'])
#print(result['class_loss'])
#print(result['iou_loss'])
#print(result['total_loss'])
 
result['IOU']=pd.to_numeric(result['IOU'])
result['count']=pd.to_numeric(result['count'])
result['class_loss']=pd.to_numeric(result['class_loss'])
result['iou_loss']=pd.to_numeric(result['iou_loss'])
result['total_loss']=pd.to_numeric(result['total_loss'])
result.dtypes
 
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
ax.plot(result['IOU'].values,label='IOU')
ax.legend(loc='best')
# ax.set_title('The Region Avg IOU curves')
ax.set_title('The Region Avg IOU curves')
ax.set_xlabel('batches')
# fig.savefig('Avg IOU')
fig.savefig('Avg_IOU')