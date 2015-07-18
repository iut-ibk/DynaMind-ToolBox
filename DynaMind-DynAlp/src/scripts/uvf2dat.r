###
### Convert UVF file format to DAT (swmm5) files
###
### Input: directory with UVF files
###
### Output: DAT files in the same directory
###

# cleanup

rm(list=ls())

# read

filepath<-("/home/gaelic/Code/DynaMind/DynaMind-DynAlp/data") # the path must consist another folder named UVF and DAT
setwd(filepath)

list<-dir(pattern="*.uvf")

# convert and write

for (n in 1:length(list))
  {
    file<-list[n]
    
    # read information lines
    infos<-read.table(file,header=F,sep=" ",dec=".",comment.char="$",nrows=3,skip=4,flush=F,strip.white=T,fill=T)
    
    cent_start<-infos[1,17] # Anfangsjahrhundert
    cent_end<-infos[1,18] # Endjahrhundert
    time_start<-substr(infos[3,1],1,10)   # Anfangszeitpunkt
    time_end<-substr(infos[3,1],11,20)  # Endzeitpunkt
    
    # read values
    values<-read.table(file,header=F,sep=" ",dec=".", col.names=c("T","mm"),colClasses=c("character","character"),comment.char="$",skip=7)
    
    # check sanity
    
    if(values[dim(values)[1],1]!=time_end || values[1,1]!=time_start)
    {
      print(paste(file,": error in data -> time missmatch, skipping file"))
      next
    }
    
    Y<-substr(values$T,1,2) # get year values
    
    if (cent_start==cent_end)   # if values are equal -> same century
      {
        Y<-paste(substr(cent_start,1,2),Y,sep="")
      }
    
    if (cent_start<cent_end && values$T[1]>50)  # if century switch
      {
        for(i in 2:length(Y))     # filter last index of old century
          {
            if(Y[i]<Y[i-1])
            {
              changecent<-i-1
            }
          }
        
        Y[1:changecent]<-paste(substr(cent_start,1,2),Y[1:changecent],sep="")         # values before century switch
        Y[-(1:changecent)]<-paste(substr(cent_end,1,2),Y[-(1:changecent)],sep="")   # values after century switch
        
      }
    
    # generate table to be saved
    
    savetab<-matrix(nrow=dim(values)[1],ncol=7)
    savetab[,1]<-"rg1"
    savetab[,2]<-Y
    savetab[,3]<-substr(values$T,3,4)  # Spalte f?r Monat
    savetab[,4]<-substr(values$T,5,6)  # Spalte f?r Tag
    savetab[,5]<-substr(values$T,7,8)  # Spalte f?r Stunde
    savetab[,6]<-substr(values$T,9,10) # Spalte f?r Minute
    savetab[,7]<-values$mm             # Spalte f?r valuesert in [mm]
    
    # write new file
    
    write.table(savetab,paste(filepath,"/",infos[2,1],".dat",sep=""),append=F,sep=" ",dec=".",row.names=F,col.names=F,quote=F)
 }