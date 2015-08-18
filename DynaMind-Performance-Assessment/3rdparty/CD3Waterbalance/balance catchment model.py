# -*- coding: utf-8 -*-
"""
Created on Thu Oct 02 08:41:08 2014

@author: Acer
"""

# import data
# collect it
# export data

import csv
import os
import numpy as np
import pylab as pl
import decimal

file_names=os.listdir('C:\Users\Acer\Documents\GitHub\CD3Waterbalance\check')[0:]
list_csv=zeros((len(file_names),1)).tolist()
final_list=zeros((len(file_names),1)).tolist()

for i in range(len(file_names)): 
    file_name=file_names[i]
    
    with open("C:\Users\Acer\Documents\GitHub\CD3Waterbalance\check\%s" % file_name) as csvfile:
        data=csv.reader(csvfile, delimiter=',', quotechar='$')     
        
        global list_csv
        
        for row in data:
            list_csv[i].append(', '.join(row))
    if list_csv[i][1][0] == ' ':
        final_list[i] = list_csv[i][2:-1]
    else:
        final_list[i] = list_csv[i][2:-1]

date_2_num=[[1]*len(final_list[0]) for m in range(len(file_names))]

from datetime import datetime, date, time

for i in range(len(final_list)):
    for n in range(len(final_list[1])):
        date_2_num[i][n]=([date2num(datetime.strptime(final_list[i][n].split()[0]+" "+final_list[i][n].split()[1],"%d.%m.%Y %H:%M:%S")),float(final_list[i][n].split()[2])])
   
a=np.asarray(date_2_num[0])
b=np.asarray(date_2_num[1])
c=np.asarray(date_2_num[2])
d=np.asarray(date_2_num[3])
e=np.asarray(date_2_num[4])
f=np.asarray(date_2_num[5])
g=np.asarray(date_2_num[6])
#catchment are [m²]
catchment_area = 1
#perv. and imperv. fractions []
perv = 0.4
imperv_raintank = 0.2
imperv_runoff = 0.4
#time step of simulation [s]
dt=360
#[rain,evap,outdoor use,infiltr,runoff,collected]
sum_array=[[0]*len(b) for m in range(6)]


for i in range(len(f)):
    if f[i][1]<=c[i][1]:
        sum_array[0][i]=-f[i][1]*perv*catchment_area
        sum_array[1][i]=c[i][1]*perv*catchment_area
        sum_array[2][i]=-d[i][1]
    else:
        sum_array[0][i]=-f[i][1]*catchment_area
        sum_array[1][i]=c[i][1]*catchment_area
        sum_array[2][i]=-d[i][1]
        sum_array[3][i]=a[i][1]
        sum_array[4][i]=g[i][1]
        sum_array[5][i]=b[i][1]
        
  
balance_for_each_time=sum(sum_array,axis=0)
sum_array_absolut=sum_array
for i in range(len(balance_for_each_time)):
    balance_for_each_time[i]=abs(balance_for_each_time[i])
    sum_array_absolut[0][i]=abs(sum_array_absolut[0][i])
    sum_array_absolut[1][i]=abs(sum_array_absolut[1][i])
    sum_array_absolut[2][i]=abs(sum_array_absolut[2][i])
    sum_array_absolut[3][i]=abs(sum_array_absolut[3][i])
    sum_array_absolut[4][i]=abs(sum_array_absolut[4][i])
    sum_array_absolut[5][i]=abs(sum_array_absolut[5][i])
    
#absolut value
cumulated_absolut_error=sum(balance_for_each_time)
#perncet value
relative_averaged_error=2*cumulated_absolut_error / (sum(sum_array_absolut[0])+sum(sum_array_absolut[1])+sum(sum_array_absolut[2])+sum(sum_array_absolut[3])+sum(sum_array_absolut[4])+sum(sum_array_absolut[5]))/100

days=float(dt)/60/60/24*len(balance_for_each_time)
print''
print 'The duration of the simulation was %s days' %days
print 'The absolut cumulated error value is %s m³' %cumulated_absolut_error 
print 'The realtive averaged error is %s percent' %relative_averaged_error
print''