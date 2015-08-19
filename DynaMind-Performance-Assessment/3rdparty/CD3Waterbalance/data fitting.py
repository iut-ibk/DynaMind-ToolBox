# -*- coding: utf-8 -*-
"""
Created on Fri Oct 17 09:32:18 2014

@author: Acer
"""

import csv
import os
from scipy.interpolate import interp1d
from datetime import datetime

file_names=os.listdir('C:\Users\Acer\Documents\GitHub\CD3Waterbalance\simulation with patterns\pattern')[0:]
list_csv=zeros((len(file_names),1)).tolist()
final_list=zeros((len(file_names),1)).tolist()

for i in range(len(file_names)): 
    file_name=file_names[i]
    
    with open("C:\Users\Acer\Documents\GitHub\CD3Waterbalance\simulation with patterns\pattern\%s" % file_name) as csvfile:
        data=csv.reader(csvfile, delimiter=',', quotechar='$')     
        
        global list_csv
        
        for row in data:
            list_csv[i].append(', '.join(row))
    if list_csv[i][1][0] == ' ':
        final_list[i] = list_csv[i][1:]
    else:
        final_list[i] = list_csv[i][1:]

date_2_num=[[1]*len(final_list[0]) for m in range(len(file_names))]
for i in range(len(final_list)):
    for n in range(len(final_list[0])):
        date_2_num[i][n]=([date2num(datetime.strptime(final_list[i][n].split()[0]+" "+final_list[i][n].split()[1],"%d.%m.%Y %H:%M:%S")),float(final_list[i][n].split()[2]),float(final_list[i][n].split()[3]),float(final_list[i][n].split()[4]),float(final_list[i][n].split()[5]),float(final_list[i][n].split()[6]),float(final_list[i][n].split()[7])])

date=[]
bath=[]
shower=[]
toilet=[]
tap=[]
washing_m=[]
dishwasher=[]
for i in range(len(date_2_num[0])):
    date.append(date_2_num[0][i][0])
    bath.append(date_2_num[0][i][1])
    shower.append(date_2_num[0][i][2])
    toilet.append(date_2_num[0][i][3])
    tap.append(date_2_num[0][i][4])
    washing_m.append(date_2_num[0][i][5])
    dishwasher.append(date_2_num[0][i][6])
    
bath_int = interp1d(date, bath)
shower_int = interp1d(date, shower)
toilet_int = interp1d(date, toilet)
tap_int = interp1d(date, tap)
washing_m_int = interp1d(date, washing_m)
dishwasher_int = interp1d(date, dishwasher)

spacing=linspace(date[0],date[-1],24*10+1)

bath_int=bath_int(spacing)
shower_int = shower_int(spacing)
toilet_int = toilet_int(spacing)
tap_int = tap_int(spacing)
washing_m_int = washing_m_int(spacing)
dishwasher_int =dishwasher_int(spacing)

indoor_use=transpose((spacing, bath_int, shower_int, toilet_int, tap_int, washing_m_int, dishwasher_int))

#import matplotlib.pyplot as plt
#plt.plot(spacing,shower_int,'o')
#plt.show()

np.savetxt('test.txt', indoor_use,fmt='%10.10f%10.2f%10.2f%10.2f%10.2f%10.2f%10.2f', delimiter="/t")








