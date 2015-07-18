########################################################
# inputfilereader.R
# 24.02.2015
#
# Verbesserung
# 26.2.2015
########################################################

rm(list=ls())

# Eingabe
########################################################
filepath<-("C:/Users/c8451151/Desktop/arbeiten/RSIDW_17_7/Inputfiles")
gp_file<-("C:/Users/c8451151/Desktop/arbeiten/RSIDW_17_7/Shape/Gitterpunkte_INCA_M28.csv")
ending<-(".inp") # mit Punkt
########################################################

setwd(filepath)
list<-Sys.glob(paste("*",ending,sep=""))
options(useFancyQuotes=F)

gpcsv<-as.matrix(read.csv2(gp_file,header=F,col.names=c("ID","X","Y"),dec="."))


for(file in 1:length(list)) 
{
  inp<-read.table(list[file],sep="\t",blank.lines.skip=F)
    
  # suche [Polygons] und nimm Daten (Subcatchment-Bezeichnung & X-Y-Koordinaten) heraus
  polygonsline<-which(inp[,1]=="[Polygons]")
  symbolsline<-which(inp[,1]=="[SYMBOLS]")
  
  coorddata<-inp[(polygonsline+3):(symbolsline-2),1]
  
  tablelineslist<-strsplit(as.character(coorddata),"\\s+")
  
  tablelines<-matrix(nrow=length(tablelineslist),ncol=length(tablelineslist[[1]]))
  
    for(j in 1:length(tablelineslist))
    {
      tablelines[j,]<-tablelineslist[[j]][1:ncol(tablelines)]
    }
  
  subcnames<-rownames(table(tablelines[,1]))
  coords<-matrix(nrow=length(subcnames),ncol=3)

  coords[,1]<-subcnames
  
    for(j in 1:length(subcnames))
    {
      coords[j,2]<-tablelines[which(tablelines[,1]==coords[j,1])[1],2]
      coords[j,3]<-tablelines[which(tablelines[,1]==coords[j,1])[1],3]
    }
  
  colnames(coords)=c("SubcName","X","Y")
  
  # suche f�r jedes Subcatchment den Gitterpunkt mit dem geringsten Abstand und speichere Subcatchment mit jeweiligem GP in minima
  minima<-matrix(nrow=length(subcnames),ncol=2)
  minima[,1]<-subcnames
  
  for(i in 1:length(subcnames))
  {
    x<-as.numeric(coords[i,2])
    y<-as.numeric(coords[i,3])
    
    distance<-matrix(nrow=dim(gpcsv)[1],ncol=2)
    distance[,1]<-gpcsv[,1]
    distance[,2]<-sqrt(abs(x-gpcsv[,2])^2+abs(y-gpcsv[,3])^2)
    
    minima[i,2]<-gpcsv[which(distance[,2]==min(distance[,2])),1]
  }
  
  
  ##### schreibe Tabelle mit neuen 'Rain Gage' so wie bisher in der Inputfile f�r [Subcatchment]
  
  subcline<-which(inp[,1]=="[SUBCATCHMENTS]")
  subarealine<-which(inp[,1]=="[SUBAREAS]")
  
  subcdatalist<-strsplit(as.character(inp[(subcline+3):(subarealine-2),1]),"\\s+")
  
  subcdatamatrix<-matrix(nrow=length(subcdatalist),ncol=length(subcdatalist[[1]]))
  
  for(j in 1:length(subcdatalist))
  {
    subcdatamatrix[j,]<-subcdatalist[[j]][1:ncol(subcdatamatrix)]
  }
  
  # �ndere 2. Spalte
  
  minima_text<-sprintf("%03d",as.numeric(minima[,2]))
  
  for(j in 1:dim(subcdatamatrix)[1])
  {
    subcdatamatrix[j,2]<-paste("RG",minima_text[which(minima[,1]==subcdatamatrix[j,1])],sep="")
  }
  
  
  # erweitere [Raingages]
  rg<-rownames(table(subcdatamatrix[,2]))
  rg2<-as.numeric(rownames(table(minima[,2])))
  
  raingages<-matrix(nrow=length(rg),ncol=8)
  raingages[,1]<-rg
  raingages[,2]<-"VOLUME"  
  raingages[,3]<-"0:05"
  raingages[,4]<-"1.0"
  raingages[,5]<-"FILE"
  raingages[,6]<-paste("gp_",which(raingages[,1]==raingages[,1]),".dat",sep="")
  raingages[,7]<-paste("rg",rg2,sep="")
  raingages[,8]<-"MM"
  
  # Setze neue Datei zusammen
  raingagesline<-which(inp[,1]=="[RAINGAGES]")
  
  part1<-as.matrix(inp[1:(raingagesline+2),1])
  
  # bilde 1 Spalte aus Raingages-Daten mit Spaltenbreite wie die Bindestriche unter deren �berschriften
  dash1<-inp[raingagesline+2,]
  dash2<-as.vector(strsplit(as.character(dash1),"\\s+"))
  widths<-sapply(dash2,nchar)
  part2<-matrix(ncol=1,nrow=dim(raingages)[1])
  
  raingages[,6]<-dQuote(raingages[,6])
  
  for(j in 1:dim(raingages)[1])
  {
    
    raingageline<-matrix(ncol=8,nrow=1)
    
    for(k in 1:8)
    {
      if(k<=length(widths))
      {
        raingageline[1,k]<-sprintf(paste("%-",widths[k],"s",sep=""),raingages[j,k])
        
        if(k==1)
        {
          part2[j,1]<-raingageline[1,k]
        }
        
        if(k!=1)
        {
          part2[j,1]<-paste(part2[j,1],raingageline[1,k],sep=" ")  
        } 
      }
      
      if(k>length(widths))
      {
        part2[j,1]<-paste(part2[j,1],raingages[j,k],sep=" ")  
      }
    } 
  }
  
  
  part3<-as.matrix(inp[(subcline-1):(subcline+2),1])
                   
  # bilde 1 Spalte aus Subcatchments-Daten mit Spaltenbreite wie die Bindestriche unter deren �berschriften
  dash1<-inp[subcline+2,]
  dash2<-as.vector(strsplit(as.character(dash1),"\\s+"))
  widths<-sapply(dash2,nchar)
  part4<-matrix(ncol=1,nrow=dim(subcdatamatrix)[1])
  
  for(j in 1:dim(subcdatamatrix)[1])
  {
     
    subctextline<-matrix(ncol=8,nrow=1)
    
    for(k in 1:8)
    {
     subctextline[1,k]<-sprintf(paste("%-",widths[k],"s",sep=""),subcdatamatrix[j,k])
     
     if(k==1)
     {
       part4[j,1]<-subctextline[1,k]
     }
     
     if(k!=1)
     {
       part4[j,1]<-paste(part4[j,1],subctextline[1,k],sep=" ")  
     } 
    } 
  }
  
  part5<-as.matrix(inp[(subarealine-1):dim(inp)[1],1])
  
  complete<-rbind(part1,part2,part3,part4,part5)
 
  write.table(complete,paste(filepath,"/Inputfiles_new/",substr(list[file],1,nchar(list[file])-4),"_new.inp",sep=""),quote=F,row.names=F,col.names=F)
  
}
