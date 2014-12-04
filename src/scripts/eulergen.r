###
### Euler Type 2 Rainfall generator ###
###
### Input: r15 values in an CSV file
###
### Output: Euler Type 2 rainfall as a DAT (swmm5) file

# cleanup

rm(list=ls())
library(zoo)
library(miscTools)

### Eingaben ##############################################

# workspace & Dateiname

setwd("~/Code/DynaMind/DynaMind-DynAlp/data")
file<-("intervalyear.csv")

# Bis zu welchem Minutenwert soll Regen erstellt werden? (5, 10, 15, 20, 30, ... Minuten) -> wenn in Minuten angegeben: muss durch 5 teilbar sein

limit<-1
unit<-"h"   # min, h, d

datetimebegin<-"2000 01 01 00 00"

###########################################################


# einlesen

tab<-read.csv(file)

# Umrechnung von h bzw. d in Minuten

tab[tab[,2]=="h",1]<-tab[tab[,2]=="h",1]*60
tab[tab[,2]=="h",2]<-"min"

tab[tab[,2]=="d",1]<-tab[tab[,2]=="d",1]*24*60
tab[tab[,2]=="d",2]<-"min"


annualities<-names(tab)[-(1:2)]  # vorhandene J?hrlichkeiten
minutescsv<-tab[,1]
maxmin<-max(minutescsv)  # maximaler Minutenwert


# Abfrage, ob Limit in CSV noch vorhanden, sonst Fehlermeldung

if(unit=="h")
{limit<-limit*60
 limiti<-which(tab[,1]==limit)}   # Index des Limits in der urspr?nglichen Tabelle

if(unit=="d")
{limit<-limit*24*60
 limiti<-which(tab[,1]==limit)}

if(maxmin<limit || tab[1,1]>=limit || is.na(limit)==TRUE)
{print(paste("Fehlerhaftes Limit"))}


# f?r jede J?hrlichkeit eine Matrix aufbauen
#   Spalte 1: Minutendauer in 5-Minuten-Schritten von 5 bis zum Limit
#   Spalte 2: Regenmesswert aus eingelesener CSV
#   Spalte 3: berechnete Differenz zum Vorg?nger


minutes<-seq(5,limit,5)

for(i in 1:length(annualities))   # f?r jede Wiederkehrh?ufigkeit eigene Auswertung
{
  
  mm_anno<-data.frame(Minuten=tab[1:limiti,1],mm_kum=tab[1:limiti,i+2]) # aus urspr. Tabelle ausgeschnitten; Minutenwerte mit zugeh?rigen mm, je Annualit?t eigene Tabelle
  difftab<-data.frame(Minuten=minutes,mm_kum=NA,mm_diff=NA) # Tabelle mit kumulierten mm-Werten und Differenz, 5 Minuten-Intervalle
  
  comb<-merge(mm_anno,difftab,by=intersect("Minuten","Minuten"),all.x=T,all.y=T)  # Tabelle mit 5-Min-Intervallen und wenn vorhanden zugeh?riger mm-Wert
  difftab$mm_kum<-na.approx(comb$mm_kum.x)
  #mm_kum2<-na.approx(comb$mm_kum.x)
  
  difftab[1,3]<-difftab[1,2]    # 1. Differenz entspricht Anfangswert
  
  for(j in 2:nrow(difftab))
  {
    difftab[j,3]<-round(difftab[j,2]-difftab[j-1,2],3)   # Erstellung der Differenzen
  }
   
  # Euler-Regen erstellen --> 1. Drittel in umgekehrter Reihenfolge; wenn Anzahl bis zum Limit nicht durch 3 teilbar wird auf- oder abgerundet

  euleri<-round(nrow(difftab)/3)    # bis zu diesem Index wird umgedreht
  drittelstuelp<-difftab[euleri:1,3]
  #stuelp<-as.matrix(cbind(difftab$Minuten[1:euleri],drittel1_kum,drittel1_diff))
    
  eulertab<-matrix(ncol=7,nrow=length(minutes))
  
  eulertab[,1]<-"rg1"
  eulertab[,2]<-substr(datetimebegin,1,4)
  eulertab[,3]<-substr(datetimebegin,6,7)
  eulertab[,4]<-substr(datetimebegin,9,10)
  eulertab[,5]<-substr(datetimebegin,12,13)
  eulertab[,6]<-formatC(as.numeric(substr(datetimebegin,15,16))+minutes,width=2, format="d", flag="0")
  eulertab[1:euleri,7]<-drittelstuelp
  eulertab[-(1:euleri),7]<-difftab[-(1:euleri),3]
  
  eulertab<-insertRow(eulertab,1,c("rg1",substr(datetimebegin,1,4),substr(datetimebegin,6,7),substr(datetimebegin,9,10),substr(datetimebegin,12,13),substr(datetimebegin,15,16),0))
  
  # Tabelle abspeichern
 
  write.table(eulertab,paste(getwd(),"/",substr(names(tab)[i+2],2,nchar(names(tab)[i+2])),"_to_",limit,"min.dat",sep=""),append=F,sep=" ",dec=",",row.names=F,col.names=F,quote=F)
}

