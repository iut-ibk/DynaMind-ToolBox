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
  headlines<-grep("\\[",inp[,1])
  heads<-substr(inp[headlines,1],2,nchar(as.character(inp[headlines,1]))-1)

  emptylines<-grep('^\\s*$',inp[,1])

  for(i in 1:length(heads))
  {
    if(inp[(headlines[i]+1),1]=="")
    {
      next
    }
  
    emptynext<-emptylines[emptylines>headlines[i]][1]
    headlinenext<-headlines[headlines>headlines[i]][1]
    
    if(i<length(heads))
    {
      data<-inp[(headlines[i]+1):(headlinenext-2),1]  
    }
    
    if(i==length(heads))
    {
      data<-inp[(headlines[i]+1):(emptynext-1),1]
    }
    
    if(length(grep("^;;",data)>0))
    {
      data<-data[-grep("^;;",data)]
    }
    
    if(length(data)==0)
    {
      next
    }
    
    data<-strsplit(as.character(data),"\\s+")
    
    data2<-matrix(nrow=length(data),ncol=length(data[[1]]))
    
    for(j in 1:length(data))
    {
      data2[j,]<-data[[j]][1:ncol(data2)]
    }  
    
    assign(heads[i],data2)
    
  }
}
