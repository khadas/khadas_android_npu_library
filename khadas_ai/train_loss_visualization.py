import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
#%matplotlib inline
 
lines =18798    #Change to self generated Number of rows in train_log_loss.txt

#Adjusting the following two sets of numbers will help you view the details of the drawing
start_ite = 250 #Ignore Number of all lines starting in train_log_loss.txt
end_ite = 6000 #Ignore Number of all lines ending in train_log_loss.txt
result = pd.read_csv('train_log_loss.txt', skiprows=[x for x in range(lines) if ((x<start_ite) |(x>end_ite))] ,error_bad_lines=False, names=['loss', 'avg loss', 'rate', 'seconds', 'images'])
result.head()
 
result['loss']=result['loss'].str.split(' ').str.get(1)
result['avg']=result['avg loss'].str.split(' ').str.get(1)
result['rate']=result['rate'].str.split(' ').str.get(1)
result['seconds']=result['seconds'].str.split(' ').str.get(1)
result['images']=result['images'].str.split(' ').str.get(1)
result.head()
result.tail()
 
# print(result.head())
# print(result.tail())
# print(result.dtypes)
print(result['loss'])
#print(result['avg'])
#print(result['rate'])
#print(result['seconds'])
#print(result['images'])
 
result['loss']=pd.to_numeric(result['loss'])
result['avg']=pd.to_numeric(result['avg'])
result['rate']=pd.to_numeric(result['rate'])
result['seconds']=pd.to_numeric(result['seconds'])
result['images']=pd.to_numeric(result['images'])
result.dtypes
 
 
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
ax.plot(result['loss'].values,label='avg_loss')
ax.legend(loc='best')  
ax.set_title('The loss curves')
ax.set_xlabel('batches')
fig.savefig('avg_loss')
