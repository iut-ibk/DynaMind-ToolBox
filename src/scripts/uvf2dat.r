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

filepath<-("U:/Datenaustausch/Tanja/Downscaling_ZRen/HADCM3_A1B/") # the path must consist another folder named UVF and DAT
setwd(paste(filepath,"1971-00/",sep=""))
list<-dir()

for (n in 1:length(list))
  {
    datei<-list[n]
    
    # erst nur die Zeilen einlesen, in der die Jahrhunderte, die Stationsnr & die Koordinaten und die Anfangs- und Endzeit stehen
    infos<-read.table(datei,header=F,sep=" ",dec=".",comment.char="$",nrows=3,skip=4,flush=F,strip.white=T,fill=T)
    
    JHDa<-infos[1,17] # Anfangsjahrhundert
    JHDe<-infos[1,18] # Endjahrhundert
    Ta<-substr(infos[3,1],1,10)   # Anfangszeitpunkt
    Te<-substr(infos[3,1],11,20)  # Endzeitpunkt
    
    # ganze Tabelle einlesen, ab den Messwerten
    messw<-read.table(datei,header=F,sep=" ",dec=".", col.names=c("T","mm"),colClasses=c("character","character"),comment.char="$",skip=7) #,nrows=500
    
    if(messw[dim(messw)[1],1]!=Te || messw[1,1]!=Ta)
    {
      print(paste("Bei ",datei," Fehler in den Daten"))
      next
    }
    
    Y<-substr(messw$T,1,2) # alle Jahreszahlen herausholen
    
    if (JHDa==JHDe)   # wenn AnfangsJHD und EndJHD gleich sind, dann bei jedem Zeitpunkt der Messdaten die ersten beiden Zahlen davor anf?gen, um auf normale Jahreszahlen zu kommen
      {
        Y<-paste(substr(JHDa,1,2),Y,sep="")
      }
    
    if (JHDa<JHDe && messw$T[1]>50)  # wenn AnfangsJHD kleiner als EndJHD, dann ab dem Wert, der mit 00 (f?r 2000) anf?ngt, 19 davorsetzen, danach 20
      {
        for(i in 2:length(Y))     # letzter Index des alten Jahrhunderts herausfinden
          {
            if(Y[i]<Y[i-1])
            {
              wechsel<-i-1
            }
          }
        
        Y[1:wechsel]<-paste(substr(JHDa,1,2),Y[1:wechsel],sep="")         # allen Werten vor dem JHD-Wechsel die AnfangsJHD-Zahl vorne anf?gen
        Y[-(1:wechsel)]<-paste(substr(JHDe,1,2),Y[-(1:wechsel)],sep="")   # allen Werten nach dem JHD-Wechsel die EndJHD-Zahl vorne anf?gen
        
      }
    
    # zu speichernde Tabelle aufbauen und f?llen
    savetab<-matrix(nrow=dim(messw)[1],ncol=7)
    savetab[,1]<-as.character(infos[2,1])
    savetab[,2]<-Y
    savetab[,3]<-substr(messw$T,3,4)  # Spalte f?r Monat
    savetab[,4]<-substr(messw$T,5,6)  # Spalte f?r Tag
    savetab[,5]<-substr(messw$T,7,8)  # Spalte f?r Stunde
    savetab[,6]<-substr(messw$T,9,10) # Spalte f?r Minute
    savetab[,7]<-messw$mm             # Spalte f?r Messwert in [mm]
    
    # Tabelle im Ordner /dat als .dat-Datei speichern
    write.table(savetab,paste(Ordnerpfad,"1971-00-dat/",infos[2,1],".dat",sep=""),append=F,sep=" ",dec=".",row.names=F,col.names=F,quote=F)
 }