###
### Convert UVF File format to DAT (SWMM5) Files
###
### Input: Folder with UVF Files
###
### Output: DAT Files in the same Folder
###

# cleanup

rm(list=ls())

# read
filepath<-("/home/gaelic/Code/DynaMind/DynaMind-DynAlp/data") # the path must consist another folder named UVF and DAT
setwd(filepath)

list<-dir(pattern="*.uvf")

for (n in 1:length(list))
  {
    file<-list[n]
    
    # erst nur die Zeilen einlesen, in der die Jahrhunderte, die Stationsnr & die Koordinaten und die Anfangs- und Endzeit stehen
    infos<-read.table(file,header=F,sep=" ",dec=".",comment.char="$",nrows=3,skip=4,flush=F,strip.white=T,fill=T)
    
    cent_start<-infos[1,17] # Anfangsjahrhundert
    cent_end<-infos[1,18] # Endjahrhundert
    time_start<-substr(infos[3,1],1,10)   # Anfangszeitpunkt
    time_end<-substr(infos[3,1],11,20)  # Endzeitpunkt
    
    # ganze Tabelle einlesen, ab den valueserten
    values<-read.table(file,header=F,sep=" ",dec=".", col.names=c("T","mm"),colClasses=c("character","character"),comment.char="$",skip=7) #,nrows=500
    
    if(values[dim(values)[1],1]!=time_end || values[1,1]!=time_start)
    {
      print(paste(file,": error in data -> time missmatch, skipping file"))
      next
    }
    
    Y<-substr(values$T,1,2) # alle Jahreszahlen herausholen
    
    if (cent_start==cent_end)   # wenn AnfangsJHD und EndJHD gleich sind, dann bei jedem Zeitpunkt der Messdaten die ersten beiden Zahlen davor anf?gen, um auf normale Jahreszahlen zu kommen
      {
        Y<-paste(substr(cent_start,1,2),Y,sep="")
      }
    
    if (cent_start<cent_end && values$T[1]>50)  # wenn AnfangsJHD kleiner als EndJHD, dann ab dem Wert, der mit 00 (f?r 2000) anf?ngt, 19 davorsetzen, danach 20
      {
        for(i in 2:length(Y))     # letzter Index des alten Jahrhunderts herausfinden
          {
            if(Y[i]<Y[i-1])
            {
              changecent<-i-1
            }
          }
        
        Y[1:changecent]<-paste(substr(cent_start,1,2),Y[1:changecent],sep="")         # allen Werten vor dem JHD-changecent die AnfangsJHD-Zahl vorne anf?gen
        Y[-(1:changecent)]<-paste(substr(cent_end,1,2),Y[-(1:changecent)],sep="")   # allen Werten nach dem JHD-changecent die EndJHD-Zahl vorne anf?gen
        
      }
    
    # zu speichernde Tabelle aufbauen und f?llen
    savetab<-matrix(nrow=dim(values)[1],ncol=7)
    savetab[,1]<-"rg1"
    savetab[,2]<-Y
    savetab[,3]<-substr(values$T,3,4)  # Spalte f?r Monat
    savetab[,4]<-substr(values$T,5,6)  # Spalte f?r Tag
    savetab[,5]<-substr(values$T,7,8)  # Spalte f?r Stunde
    savetab[,6]<-substr(values$T,9,10) # Spalte f?r Minute
    savetab[,7]<-values$mm             # Spalte f?r valuesert in [mm]
    
    # Tabelle im Ordner als .dat-Datei speichern
    write.table(savetab,paste(filepath,"/",infos[2,1],".dat",sep=""),append=F,sep=" ",dec=".",row.names=F,col.names=F,quote=F)
 }