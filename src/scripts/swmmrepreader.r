########################################################
# reportreader_besser.R
# 19.02.2015
########################################################

rm(list=ls())

# Eingabe
########################################################
filepath<-("C:/Users/c8451151/Desktop/arbeiten/RSIDW_17_7/Reportfiles/") 
ending<-(".rep") # mit Punkt

xval<-c("V020","ARA1")   # welche Knoten sollen bei stat. Kennwerten unberücksichtigt bleiben?
########################################################

setwd(filepath)
list<-Sys.glob(paste("*",ending,sep=""))

zsf<-matrix(nrow=length(list),ncol=8)
nodeflooding<-matrix(nrow=1,ncol=3)
outfallloading<-matrix(nrow=1,ncol=3)

heads<-c("  Subcatchment Runoff Summary","  Node Depth Summary","  Node Inflow Summary","  Node Surcharge Summary","  Node Flooding Summary","  Storage Volume Summary","  Outfall Loading Summary","  Link Flow Summary","  Flow Classification Summary","  Conduit Surcharge Summary","  Pumping Summary")
V<-list()
V[[1]]<-c("x","Node","totPrec","totRunon","totEvap","totInfil","totRunoff","totRunoff2","PeakRunoff","RunoffCoeff")
V[[2]]<-c("x","Node","Type","AvgD","MaxD","MaxHGL","tMaxOcc_d","t_MaxOcc_HrMin")
V[[3]]<-c("x","Node","Type","MaxLatInflow","MaxTotInflow","tMaxOcc_d","t_MaxOcc_HrMin","LatInflowVol","TotInflowVol","FlowBalErr")
V[[4]]<-c("x","Node","Type","hSurch","MaxHabCrown","MinDbelRim")
V[[5]]<-c("x","Node","hFlooded","MaxRate","tMaxOcc_d","t_MaxOcc_HrMin","TotFloodV","MaxPondD")
V[[6]]<-c("x","StorUnit","AvgVol","AvgPcntFull","EvapPcntLoss","ExfilPcntLoss","MaxVol","MaxPcntFull","tMaxOcc_d","t_MaxOcc_HrMin","MaxOutflow")
V[[7]]<-c("x","OutfallNode","FlowFreqPcnt","AvgFlow","MaxFlow","TotVol")
V[[8]]<-c("x","Link","Type","MaxFlow","tMaxOcc_d","t_MaxOcc_HrMin","MaxVeloc","MaxFullFlow","MaxFullDepth")
V[[9]]<-c("x","Conduit","AdjActL","F_Dry","F_UpDry","F_DownDry","F_SubCrit","F_SupCrit","F_UpCrit","F_DownCrit","F_NormLtd","F_InletCtrl")
V[[10]]<-c("x","Conduit","HF_BothEnds","HF_Upstream","HF_Dnstream","HabFullNormF","HCapLim")
V[[11]]<-c("x","Pump","PctUt","NStartups","MinFlow","AvgFlow","MaxFlow","TotVol","PowerUsage","PctTimeOff_PumpLow","PctTimeOff_CurveHigh")

for(file in 1:length(list)) 
{

zsf[file,1]<-substr(list[file],1,nchar(list[file])-4) 

rep<-read.table(list[file],sep="\t")

emptylines<-which(nchar(gsub("\\s+","",rep[,1]))==0)
lines<-grep("-----------------------",as.character(rep[,1]))

for(i in 1:length(heads))
  {
    name<-which(rep[,1]==heads[i])
    start<-lines[lines>name][2]+1
    end<-emptylines[emptylines>start][1]-1
    
    if(i==7)
    {
      end<-emptylines[emptylines>start][1]-3
    }
    
    data<-rep[start:end,]
    tablelineslist<-strsplit(as.character(data),"\\s+")
    
    tablelines<-matrix(nrow=length(tablelineslist),ncol=length(tablelineslist[[1]]))
    
    for(j in 1:length(tablelineslist))
    {
      tablelines[j,]<-tablelineslist[[j]][1:ncol(tablelines)]
    }
    
    # Zusammenfassung
    source('C:/Users/c8451151/Desktop/arbeiten/RSIDW_17_7/sums.R')
    zusammenf<-sums(i,file,tablelines,xval,zsf)
    zsf<-zusammenf
    
    
    if(i==4)
    {
      # Querzusammenfassung
      source('C:/Users/c8451151/Desktop/arbeiten/RSIDW_17_7/nodesums.R')
      nodefl<-nodesums(nodeflooding,tablelines)
      nodeflooding<-nodefl
    }
    
    if(i==7)
    {
      # Querzusammenfassung
      source('C:/Users/c8451151/Desktop/arbeiten/RSIDW_17_7/outfallsums.R')
      outfallload<-outfallsums(outfallloading,tablelines)
      outfallloading<-outfallload
    }
    
    
    
    
    
    if(i == 1 || i == 5 || i == 6 || i == 7 || i == 9 || i == 10 || i == 11)
    {
      colnames(tablelines)=V[[i]]
      
      write.csv2(tablelines[,-1],paste(gsub(ending,"",list[file]),"_",gsub("\\s","\\_",substr(heads[i],3,100)),".csv",sep=""),row.names=F)
    }
    
    
    if(i == 2 || i == 3 || i == 4 || i == 8)     # Unterscheidung nach Type
    {
      types<-vector(length=length(tablelineslist))
      for(j in 1:length(tablelineslist))
      {
        types[j]<-tablelineslist[[j]][3]
      }
      
      ctypes<-row.names(table(types))
      ntypes<-table(types)
      
      n<-1
      
      for(k in 1:length(ctypes))
      {
        m<-n+ntypes[[k]]-1
        datasep<-strsplit(as.character(data[n:m]),"\\s+")
        
        tablelines<-matrix(nrow=length(datasep),ncol=length(datasep[[1]]))
        
        
        for(p in 1:length(datasep))
        {
          tablelines[p,]<-datasep[[p]][1:ncol(tablelines)]
        }
        
        if((i==8 && ctypes[k]=="CONDUIT") || i!=8)
        {
          colnames(tablelines)=V[[i]]
        }
        
        if(i==8 && ctypes[k]=="PUMP")
        {
          colnames(tablelines)=V[[i]][-c(7,9)]
        }
        
        if(i==8 && ctypes[k]=="ORIFICE" || ctypes[k]=="WEIR")
        {
          colnames(tablelines)=V[[i]][-c(7,8)]
        }        
        
        write.csv2(tablelines[,-1],paste(gsub(ending,"",list[file]),"_",gsub("\\s","\\_",substr(heads[i],3,100)),"_",ctypes[k],".csv",sep=""),row.names=F)
        
        n<-m+1        
      } 
    }
  }
}

colnames(zsf)=c("name","Sum_Tot_Runoff","n_Surchargings","Sum_Hours_Surcharged","n_Floodings","Sum_Hours_Flooding","Sum_Volume_Flooding","Sum_Tot_Outfallvolume")
write.csv2(zsf,"C:/Users/c8451151/Desktop/arbeiten/RSIDW_17_7/zsf.csv",row.names=T)

colnames(nodeflooding)=c("Node","sumFloodingVol","nOcc")
write.csv2(na.omit(nodeflooding),"C:/Users/c8451151/Desktop/arbeiten/RSIDW_17_7/nodeflooding.csv",row.names=F)

colnames(outfallloading)=c("OutfallNode","sumTotVol","n_Outfalls")
write.csv2(na.omit(outfallloading),"C:/Users/c8451151/Desktop/arbeiten/RSIDW_17_7/outfallloading.csv",row.names=F)
