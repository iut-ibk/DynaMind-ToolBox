# -*- coding: utf-8 -*-
"""
Created on Wed Nov 12 14:12:52 2014

@author: Acer
"""


import subprocess
import csv
import os
import numpy as np
import pylab as pl
from datetime import datetime
from matplotlib.dates import date2num, num2date
from itertools import cycle
from numpy import size, asarray

class TheHoleLot:

    def __init__(self):
        self.Rainevapovector = []
        self.Outputvector = []
        self.area_fractions1 = []
        self.Fractioncalculatorvec = []
        self.Lossvector = []
        self.total_area = 0.0
        self.wettingloss = 0.0
        self.depressionloss = 0.0
        
        return
        
    #killing the cd3 process (if necessary) and deleting old ouput .txt files
    def Deleter(self,location_files1='/Users/christianurich/Documents/CD3Waterbalance/simulationwithpatterns/outputfiles/'):
            os.system("taskkill /cd3.exe")
            todelete = [ f for f in os.listdir(location_files1) if f.endswith(".txt") ]
            for i in range(len(todelete)):
                os.remove(location_files1+"\%s" % todelete[i])
            return
    
    #executing programm
    def runcd3(self,cd3path = 'C:\Program Files (x86)\CityDrain3\\bin\cd3.exe', filename = 'C:\Users\Acer\Documents\GitHub\CD3Waterbalance\simulationwithpatterns\outputfiles\Test.xml'):
            print "Starting and running City Drain!"
            cd3 = r'"""'+cd3path+'"   '+filename+'""'         
            p = subprocess.Popen(cd3, shell=True)
            p.wait()
            return
        
    #Catchmentattributevec from XML_Creator (see XML-Creator.md)
    def Fractioncalculator(self,Catchmentattributevector):
            for i in range(len(Catchmentattributevector)):
                self.Fractioncalculatorvec.append([Catchmentattributevector[i][2],Catchmentattributevector[i][5],Catchmentattributevector[i][3],Catchmentattributevector[i][4]])
                self.wettingloss += Catchmentattributevector[i][9]
                self.depressionloss += Catchmentattributevector[i][8]
            self.wettingloss = self.wettingloss/(i+1)
            self.depressionloss = self.depressionloss/(i+1)
            area_fractions1_0 = 0.0
            area_fractions1_1 = 0.0
            area_fractions1_2 = 0.0
            for i in range(len(self.Fractioncalculatorvec)):
                self.total_area += float(self.Fractioncalculatorvec[i][0])
                area_fractions1_0 += float(self.Fractioncalculatorvec[i][0]*self.Fractioncalculatorvec[i][1])
                area_fractions1_1 += float(self.Fractioncalculatorvec[i][0]*self.Fractioncalculatorvec[i][2])
                area_fractions1_2 += float(self.Fractioncalculatorvec[i][0]*self.Fractioncalculatorvec[i][3])
                
            self.area_fractions1=[area_fractions1_0/self.total_area, area_fractions1_1/self.total_area, area_fractions1_2/self.total_area]
            
            return    
    
    '''
    Evapo_Model, Rain_Model need to be the file names of the filereaders and pattern implemers output
    '''    
    
    #getting model outputdata
    def getoutputdata(self,location_files1):
        file_names=os.listdir(str(location_files1)[0:])
        alltogether = []
        names = []
        for i in range(len(file_names)): 
            if file_names[i][(len(file_names[i])-3):len(file_names[i])] == 'txt':
                file_name=file_names[i]
                csv_file = open(str(location_files1) +"\%s" % file_name, "r")
                data = csv.reader(csv_file, delimiter='\t')
                mylist = list(data)
                csv_file.closed
                alltogether.append(mylist)
                names.append(file_names[i])
                csv_file.close()
        #creating vector right size
        self.Outputvector=[['error']*(len(alltogether)+1) for m in range(len(alltogether[0]))]
        #writing header
        self.Outputvector[0][0]='Time'
        for i in range(len(alltogether)):
            self.Outputvector[0][i+1]=names[i][:(len(names[i])-4)]
        #writing time colum
        for i in range(len(alltogether[0]))[1:]:
            self.Outputvector[i][0]=float(date2num(datetime.strptime(alltogether[1][i][0][:19],"%d.%m.%Y %H:%M:%S")))
        #writing Values of txtfiles in vector
        for i in range((len(alltogether)+1))[1:]:
            for n in range(len(alltogether[0]))[1:]:
                self.Outputvector[n][i]=float(alltogether[i-1][n][1])
        #Evapo Model and Rain Model are the filenames of the filereaders and pattern implemers output!!! (Filenames hove to be specified as fileout names in XML Creator)
        for i in range(len(self.Outputvector[0])):
            if self.Outputvector[0][i] == 'Evapo_Model':
                for n in range(len(self.Outputvector))[1:]:
                    self.Outputvector[n][i]=float(self.Outputvector[n][i])/1000*self.total_area
            if self.Outputvector[0][i] == 'Rain_Model':
                for n in range(len(self.Outputvector))[1:]:
                    self.Outputvector[n][i]=float(self.Outputvector[n][i])/1000*self.total_area
        self.Outputvector=np.asarray(self.Outputvector)
        print '\n'
        print"______________________________________________________________________________________________________"
        print 'Outputvector has been created'       
        return 
    
    def getinputdata(self,location_files2):
        #getting inputvector
        #location_files2='C:\Users\Acer\Documents\GitHub\CD3Waterbalance\simulationwithpatterns\inputfiles'
        file_names=os.listdir(str(location_files2)[0:])
        rainevapo=[]
        namesrainevapo=[]
        for i in range(len(file_names)): 
            if file_names[i][(len(file_names[i])-3):len(file_names[i])] == 'ixx':
                file_name=file_names[i]
                csv_file = open(str(location_files2) +"\%s" % file_name, "r")
                data = csv.reader(csv_file, delimiter='\t')
                mylist = list(data)
                rainevapo.append(mylist)
                namesrainevapo.append(file_names[i])
        #creating vector right size        
        self.Rainevapovector=[['error']*(len(namesrainevapo)+1) for m in range(size(rainevapo[0],0)-1)]
        #writing time colum    
        for i in range(size(rainevapo[0],0)-1):
            self.Rainevapovector[i][0]=float(date2num(datetime.strptime(rainevapo[1][i+1][0]+" "+rainevapo[1][i+1][1],"%d.%m.%Y %H:%M:%S")))
        #writing Values of inputfiles in vector
        for i in range((len(namesrainevapo)+1))[1:]:
            for n in range(size(rainevapo[0],0)-1):
                self.Rainevapovector[n][i]=float(rainevapo[i-1][n+1][2])
        #correcting unit and volume!
        self.Rainevapovector=np.asarray(self.Rainevapovector)
        for i in range(len(namesrainevapo)+1)[1:]:
            self.Rainevapovector[:,i]=self.Rainevapovector[:,i]/1000*self.total_area
        #giving header for future reference
        self.Rainevapovector=self.Rainevapovector.tolist()
        self.Rainevapovector.insert(0,['time']*(len(namesrainevapo)+1))
        for i in range(len(namesrainevapo)+1)[1:]:
            self.Rainevapovector[0][i]=namesrainevapo[i-1][:(len(namesrainevapo[i-1])-4)]
        self.Rainevapovector = np.asarray(self.Rainevapovector)   
        print 'RainEvapovector have been created'
        return 
        
        #tocheck (all, Evapo, Rain, Indooruse, Outdoordemand?, System)
        #area_fractions = [perv, imperv_to_storage, imperv_to_stormw]
    def Balance(self, totalstoragelist = ['Greywatertanklevels',  'Rainwatertanklevels', 'Stormwaterreservoirlevels'],
               inputERlist = ['Evapo_Model', 'Rain_Model'], outputISSPlist = ['Actual_Infiltration', 'Potable_Water_Demand', 'Sewer', 'Stormwaterdrain']):
        #tocheck=['Evapo', 'Rain', 'System']
        #Data=[Rainevapovector, Outputvector]
        Data = [self.Rainevapovector, self.Outputvector]
        colorred = "\033[01;31m{0}\033[00m"
    
        #evapotranspiration check
        #evap is the ixx files name thats the file readers input, Evapo_Model is the name specified in the XML Creator (Output Pattern Implementer)
        evapomodel = 0.0
        evapoinput = 0.0
        for i in range(len(Data)):
            for n in range(len(Data[i][0])):
                if Data[i][0][n] == 'evapo':
                    for m in range(len(Data[i][:,n]))[1:]:
                        evapoinput += float(Data[i][:,n][m])
                elif Data[i][0][n] == 'Evapo_Model':
                    for m in range(len(Data[i][:,n]))[1:]:            
                        evapomodel += float(Data[i][:,n][m])
        ErrorFRPI=(1 - evapomodel/evapoinput) * 100
        print 'The difference of given and produced Evapotranspiraten calculated by the Pattern Implementer and Filereader due to rounding errors is '+ colorred.format(str(ErrorFRPI))+' %'
        
        #rain check    
        #rain is the ixx files name thats the file readers input, Rain_Model is the name specified in the XML Creator (Output Filereader)
        rainmodel = 0.0
        raininput = 0.0
        for i in range(len(Data)):
            for n in range(len(Data[i][0])):
                if Data[i][0][n] == 'rain':
                    for m in range(len(Data[i][:,n]))[1:]:
                        raininput += float(Data[i][:,n][m])
                elif Data[i][0][n] == 'Rain_Model':
                    for m in range(len(Data[i][:,n]))[1:]:            
                        rainmodel += float(Data[i][:,n][m])
        ErrorFR=(1 - rainmodel/raininput) * 100
        print 'The difference of given and produced Rain calculated by the Filereader due to rounding errors is '+ colorred.format(str(ErrorFR))+' %'
        #total system
        #'''    
        #Lists have to be in alphabetical order
        #'''    
         
        #filenames in lists
        totalstorage = []
        inputER=[]
        outputISSP = []
        outputOD = []                               
            
        for i in range(len(Data)):
            for n in range(len(Data[i][0])):
                if Data[i][0][n] in totalstoragelist:            
                    totalstorage.append(Data[i][:,n])
                elif Data[i][0][n] in inputERlist:
                    inputER.append(Data[i][:,n])
                elif Data[i][0][n] in outputISSPlist:
                    outputISSP.append(Data[i][:,n])
                if str(repr(Data[i][0][n])[1:15]) == 'Outdoor_Demand':
                    outputOD.append(Data[i][:,n])
            
        totalstoragescalar = 0.0
        rainminusevapolosses = 0.0
        SewerStormwInfiltr = 0.0
        PWRonly = 0.0
        OutdoorD = 0.0
         
        #Speicher
        for i in range(len(totalstorage)):
            
            if totalstorage[i][0] == 'Gardenwateringstorage':
                totalstoragescalar -= float(totalstorage[i][-1])
            else:
                totalstoragescalar += float(totalstorage[i][-1])
        #Potable_Water_Demand/Sewer,Infiltr.,Stormwater
        for i in range(len(outputISSP)):
            if outputISSP[i][0] == 'Potable_Water_Demand':
                for n in range(len(outputISSP[0]))[1:]:
                    PWRonly += float(outputISSP[i][n])
            else:
                for n in range(len(outputISSP[0]))[1:]:
                    SewerStormwInfiltr -= float(outputISSP[i][n])
        #OutdoorDemand
        for i in range(len(outputOD)):
            for n in range(len(outputOD[0]))[1:]:
                OutdoorD += float(outputOD[i][n])
        #Rain and Evapo inlcuding losses
        lossstorage_perv_impervreservoir = 0.0
        lossstorage_imperstormw = 0.0
        onlyrain=0.0
        onlyevapo=0.0
        rainminusevapo = 0.0
        global effective_rain
        effective_rain = ['effective_rain']
        for i in range(len(inputER[0]))[1:]:
            if float(inputER[1][i]) > float(inputER[0][i]):
                lossstorage_perv_impervreservoir += (float(inputER[1][i]) - float(inputER[0][i]))/self.total_area*1000
                lossstorage_imperstormw += (float(inputER[1][i]) - float(inputER[0][i]))/self.total_area*1000
                if lossstorage_perv_impervreservoir > self.wettingloss:
                    rainminusevapolosses += (float(inputER[1][i])-float(inputER[0][i]))*(self.area_fractions1[0]+self.area_fractions1[1])
                    foreffectiverain1 = (float(inputER[1][i])-float(inputER[0][i]))*(self.area_fractions1[0]+self.area_fractions1[1])
                    lossstorage_perv_impervreservoir = self.wettingloss
                else:
                    foreffectiverain1=0.0
                    
                if lossstorage_imperstormw > self.depressionloss + self.wettingloss:
                    rainminusevapolosses += (float(inputER[1][i])-float(inputER[0][i]))*self.area_fractions1[2]
                    foreffectiverain2 = (float(inputER[1][i])-float(inputER[0][i]))*self.area_fractions1[2]
                    lossstorage_imperstormw = self.depressionloss + self.wettingloss
                else:
                    foreffectiverain2=0.0
                    
                #writing the effective rain height in a vector    
                effective_rain.append(foreffectiverain1+foreffectiverain2)
                    
            else:
                    
                #writing the effective rain height in a vector
                effective_rain.append(0.0)
                #simulation drying via evapotranspiration
                if lossstorage_perv_impervreservoir > 0:
                    lossstorage_perv_impervreservoir += (float(inputER[1][i]) - float(inputER[0][i]))/self.total_area*1000
                    if lossstorage_perv_impervreservoir < 0:
                        lossstorage_perv_impervreservoir = 0.0
                    else:
                        pass
                else:
                    lossstorage_perv_impervreservoir =  0.0
                    
                if lossstorage_imperstormw > 0:
                    lossstorage_imperstormw += (float(inputER[1][i]) - float(inputER[0][i]))/self.total_area*1000
                    if lossstorage_imperstormw < 0:
                        lossstorage_imperstormw = 0.0
                    else:
                        pass
                else:
                    lossstorage_imperstormw =  0.0
           
            onlyrain += float(inputER[1][i])
            if float(inputER[1][i]) >= float(inputER[0][i]):
                onlyevapo += float(inputER[0][i])
                rainminusevapo += (float(inputER[1][i])-float(inputER[0][i]))
            else:
                onlyevapo += float(inputER[1][i])
        print 'Fraction of Pervious Area: '+str(self.area_fractions1[0])
        print 'Fraction of Impervious Area to Reservoir: '+str(self.area_fractions1[1])
        print 'Fraction of Impervious Area to Stormdrain: '+str(self.area_fractions1[2])
        print 'Wetting Loss: '+str( self.wettingloss)+' mm'
        print 'Depression Loss: '+str(self.depressionloss)+' mm'
        print 'Total Rain: '+str(onlyrain) + ' = '+str(onlyevapo+rainminusevapo)+' m^3'
        print 'Evaporated Rain: '+str(onlyevapo)+' m^3'
        print 'Inital Losses only: '+str(rainminusevapo-rainminusevapolosses)+' m^3'
        print 'Potable_Water_Demand: '+str(PWRonly)+' m^3'
        print 'Outdoor_Demand: '+str(OutdoorD)+' m^3'
        print 'Rain minus all Losses: '+str(rainminusevapolosses)+' m^3'
        print 'SewerStormwInfiltr: '+str(-1*SewerStormwInfiltr)+' m^3'
        print 'Still stored in tanks: ' +str(totalstoragescalar)+' m^3 -> negativ values are caused by the garden watering module'
        print 'Absolut Error of entire balance: '+str(PWRonly-OutdoorD-totalstoragescalar+rainminusevapolosses+SewerStormwInfiltr)+' m^3'
        print 'Realtive Error of entire balance: '+str(100*(PWRonly-OutdoorD+rainminusevapolosses+SewerStormwInfiltr-totalstoragescalar)*2/(PWRonly+totalstoragescalar+OutdoorD+onlyrain+onlyevapo+(rainminusevapo-rainminusevapolosses)-SewerStormwInfiltr))+' %'
        print"______________________________________________________________________________________________________"
        
        return 
    
    
    
    #Possible Input: Outdoor_Demand, Indoor_Demand, all (plots everthing), all filenames (without endings)
    def Plotter(self,size=[12,10],limx=[0,365], limy=[0,1], toplot=['Rain_Model', 'Stormwaterdrain', 'Evapo_Model', 'effective_rain','Indoor_Demand','Outdoor_Demand'] ):
        self.listtoplot = []
        Vector1 = self.Rainevapovector
        Vector2 = self.Outputvector
        for i in range(len(toplot)):
            #searching vector headers for inputstrings, writes in plotting list
            if toplot[i] in Vector1[0]:
                for n in range(len(Vector1[0])):
                    if toplot[i]==Vector1[0][n]:
                        self.listtoplot.append([Vector1[:,0], Vector1[:,n]])                       
            elif toplot[i] in Vector2[0]:
                for n in range(len(Vector2[0])):
                    if toplot[i]==Vector2[0][n]:
                        self.listtoplot.append([Vector2[:,0], Vector2[:,n]])                        
            elif toplot[i] == 'Outdoor_Demand':
                allheaders=Vector1.tolist()[0]+Vector2.tolist()[0]
                for n in range(len(allheaders)):
                    if toplot[i]==repr(allheaders[n])[1:15]:
                        position=n
                if position<=len(Vector1[0]):
                    a=1
                elif position<=len(Vector2[0]):
                    a=2
                else:
                    a=3
                exec 'variable=Vector'+str(a)
                storageOD=asarray([0.0 for m in range(len(variable))])
                for i in range(len(variable[0])):
                    if repr(variable[0][i])[1:15] == 'Outdoor_Demand':
                        for n in range(len(variable))[1:]:
                            storageOD[n] += float(variable[n][i])
                storageOD=storageOD.tolist()
                storageOD[0]='Outdoor_Demand'
                self.listtoplot.append([variable[:,0], storageOD])
                
                #while time inbetween 2 days sum up and append
                outdoordemandsum=0.0
                dailyoutdoordemand=[]
                fulldaystart=ceil(float(variable[:,0][1]))
                fulldayend=floor(float(variable[:,0][-1]))
                i=1
                for n in range(int(fulldayend-fulldaystart)+1):
                    if float(variable[:,0][i]) < (int(fulldaystart)):
                        while float(variable[:,0][i]) <= (int(fulldaystart)+ n):
                            i+=1
                    else:
                        while float(variable[:,0][i]) >= (int(fulldaystart) + n-1) and float(variable[:,0][i]) < (int(fulldaystart) + n): 
                            outdoordemandsum += float(storageOD[i])
                            i += 1
                        dailyoutdoordemand.append(outdoordemandsum)
                        outdoordemandsum=0.0
                        dailyoutdoordemand_per_sm=mean(dailyoutdoordemand)/(self.area_fractions1[0]*self.total_area)
                print 'The average Outdoordemand per square meter for the simulated time frame is: '+str(dailyoutdoordemand_per_sm)+' m³/(m²d)'
                
            elif toplot[i] == 'all':
                for n in range(len(Vector1[0]))[1:]:
                    self.listtoplot.append([Vector1[:,0], Vector1[:,n]])                      
                for n in range(len(Vector2[0]))[1:]:
                    self.listtoplot.append([Vector2[:,0], Vector2[:,n]])                      
                    
            elif toplot[i] == 'effective_rain':
                if len(Vector1[0])==len(effective_rain):
                    self.listtoplot.append([Vector1[:,0], effective_rain])
                else:
                    self.listtoplot.append([Vector2[:,0], effective_rain])
    
                        
            else:
                print 'Error: Wrong input name!'
        #LEGEND!!!save pic if wanted
        pl.figure(figsize=(size[0], size[1]), dpi=80)
        pl.xlim(float(Vector1[1][0])+float(limx[0]), float(Vector1[1][0]) + float(limx[1]))
        pl.ylim(float(limy[0]), float(limy[1]))
        lines = ["-","--","-.",":"]
        linecycler = cycle(lines)
        for i in range(len(self.listtoplot)):
            exec 'pl.plot(asarray(self.listtoplot['+str(i)+'])[0][1:],asarray(self.listtoplot['+str(i)+'])[1][1:], linewidth=2.5, linestyle = next(linecycler), label=self.listtoplot['+str(i)+'][1][0])'
        pl.legend(loc='best')
        pl.title('Model In - and Output', fontsize=20)
        pl.xlabel('Time [d]')
        pl.ylabel('Volume [m^3]')
        pl.grid(True)
        pl.show()
        print 't=0: '+str(float(Vector1[1][0]))
        print 'The time range plotted: '+str([num2date(float(Vector1[1][0]) + float(limx[0])).strftime("%d.%m.%Y %H:%M:%S"), 
                                                 num2date(float(Vector1[1][0]) + float(limx[1])).strftime("%d.%m.%Y %H:%M:%S")])
        return

    





