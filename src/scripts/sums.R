###########################################################################
# Script zur Erstellung einer Zusammenfassung der Report-Files
# 19.2.2015
###########################################################################

sums<-function(i,n,tablelines,xval,zsf)
{
  if(i==1)
  {
  	zsf[n,2]<-dim(tablelines)[1] #numberSubcatchments
  	zsf[n,3]<-sum(as.numeric(tablelines[,8])) #SUMtotRunoff[Ml]
  }
  
  if(i==3)
  {
    zsf[n,4]<-dim(tablelines)[1] #numberInflowNodes
    zsf[n,5]<-sum(as.numeric(tablelines[,4])) #SUMmaxtotInflow[lps]
    zsf[n,6]<-sum(as.numeric(tablelines[,8])) #SUMtotInflowVolume[Ml]
  }
  
  if(i==4)
  {
    zsf[n,7]<-dim(tablelines)[1] #numberSurchargeNodes
    zsf[n,8]<-sum(as.numeric(tablelines[,4])) #hourssurcharged
  }
  
  if(i == 5)
  {
    zsf[n,9]<-dim(tablelines)[1] #nFloodingNodes
    zsf[n,10]<-sum(as.numeric(tablelines[,7])) #SUMtotFloodVolume[Ml]
  }
  
  if(i == 6)
  {
    zsf[n,11]<-dim(tablelines)[1] #numberStorages
    zsf[n,12]<-sum(as.numeric(tablelines[,3])) #SUMaverageVol[Ml]
    zsf[n,13]<-sum(as.numeric(tablelines[,7])) #SUMmaxVol[Ml]
    zsf[n,14]<-sum(as.numeric(tablelines[,8])) #SUMmaxPcnt
  }
  
  if(i == 7)
  {
  	zsf[n,15]<-(dim(tablelines)[1] - length(xval)) #noutfalls
    zsf[n,16]<-sum(as.numeric(tablelines[which(!(tablelines[,2] %in% xval)),6])) #SUMtotOutfallVolume[Ml] without WWTP
    zsf[n,17]<-sum(as.numeric(tablelines[which((tablelines[,2] %in% xval)),6])) #SUMtotWWTPVolume[Ml]
  }
  
  if(i == 10)
  {
  	zsf[n,18]<-dim(tablelines)[1] #NSurchargeConduits
  	zsf[n,19]<-sum(as.numeric(tablelines[,7])) #SUMhourslimitedcapacity
  }
  if(i == 11)
  {
  	zsf[n,20]<-dim(tablelines)[1] #nPumps
  	zsf[n,21]<-sum(as.numeric(tablelines[,7])) #SUMtotPumpVolume[Ml]
  }
  
  return(zsf)
}
