########################################################
# swmmrptreader.R
# 19.02.2015
########################################################

rm(list=ls())

NODE<-1
JUNCTION<-0
SUBCATCHMENT<-0

# Eingabe
########################################################
filepath<-("~/Work/test-RPT")
rptpath<-(file.path(filepath,"rptfiles/"))
outpath<-(file.path(filepath,"output"))
ending<-(".rep") # mit Punkt

xval<-c("V020","ARA1") #"V020","ARA1")   # welche Knoten sollen bei stat. Kennwerten unber?cksichtigt bleiben?
########################################################

setwd(rptpath)
list<-Sys.glob(paste("*",ending,sep=""))

summarymatrixnames=c("name","numberSubcatchments","SUMtotRunoff[Ml]","nInflowNodes","SUMmaxtotInflow[lps]","SUMtotInflowVolume[Ml]","nSurchargedNodes","surcharged[h]","nFloodingNodes", "SUMtotFloodVolume[Ml]","numberStorages","SUMaverageVol[Ml]","SUMmaxVol[Ml]","SUMmaxPcnt","nOutfalls","SUMtotOutfallVolume[Ml]","SUMtotWWTPVolume[Ml]","NSurchargeConduits","SUMhourslimitedcapacity","nPumps","SUMtotPumpVolume[Ml]")

summarymatrix<-matrix(nrow=length(list),ncol=length(summarymatrixnames))
colnames(summarymatrix)=summarymatrixnames

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

if (NODE == 1)
{
	heads<-c("  Subcatchment Runoff Summary","  Node Depth Summary","  Node Inflow Summary","  Node Surcharge Summary","  Node Flooding Summary","  Storage Volume Summary","  Outfall Loading Summary","  Link Flow Summary","  Flow Classification Summary","  Conduit Surcharge Summary","  Pumping Summary", "  Node Results")
	V[[2]]<-c("x","Node","Type","AvgD","MaxD","MaxHGL","tMaxOcc_d","t_MaxOcc_HrMin","repMaxD")
	V[[12]]<-c("x","Date","Time","Inflow[lps]","Flooding[lps]","Depth[m]","Head[m]")
}


for(file in 1:length(list)) 
{
	summarymatrix[file,1]<-substr(list[file],1,nchar(list[file])-4)

	rep<-read.table(list[file],sep="\t")
	nodelines<-grep("<<<",rep[,1])
	emptylines<-which(nchar(gsub("\\s+","",rep[,1]))==0)

	lines<-grep("-----------------------",as.character(rep[,1]))
	
	dirname<-file.path(outpath,gsub(ending,"",list[file]))
	dir.create(dirname)
	nodedir<-file.path(dirname,"NODES")
	dir.create(nodedir)

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
		source(file.path(filepath, 'sums.R'))
		zusammenf<-sums(i,file,tablelines,xval,summarymatrix)
		summarymatrix<-zusammenf
    
		if(i==4)
		{
	# Querzusammenfassung
			source(file.path(filepath, 'nodesums.R'))
			nodefl<-nodesums(nodeflooding,tablelines)
			nodeflooding<-nodefl
		}
    
		if(i==7)
		{
	# Querzusammenfassung
			source(file.path(filepath, 'outfallsums.R'))
			outfallload<-outfallsums(outfallloading,tablelines)
			outfallloading<-outfallload
		}

		if(i == 1 || i == 5 || i == 6 || i == 7 || i == 9 || i == 10 || i == 11)
		{
			colnames(tablelines)=V[[i]]
			
			filename<-file.path(dirname,paste(gsub("\\s","\\_",substr(heads[i],3,100)),".csv",sep=""))
			
			write.csv2(tablelines[,-1],filename,row.names=F)
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
			
			for(k in 1:length(ctypes))
			{
				n<-min(which(types == ctypes[k]))
				m<-n+ntypes[[k]]-1
				datasep<-strsplit(as.character(data[n:m]),"\\s+")
        
				tablelines<-matrix(nrow=length(datasep),ncol=length(datasep[[1]]))
				for(p in 1:length(datasep))
				{
					tablelines[p,]<-datasep[[p]][1:ncol(tablelines)]
				}

				if( i == 8 )
				{
				  if (ctypes[k]=="CONDUIT")
				  {
					  colnames(tablelines)<-V[[i]]
				  }

				  if(ctypes[k]=="PUMP")
				  {
					  colnames(tablelines)<-V[[i]][-c(7,9)]
				  }

				  if(ctypes[k]=="ORIFICE")
				  {
					  tablelines[is.na(tablelines)] <- 0
					  colnames(tablelines)<-V[[i]][-c(7,8)]
				  }
				  if(ctypes[k]=="WEIR")
				  {
				    colnames(tablelines)<-V[[i]][-c(7,9)]
				  }
				}
				else
				{
				  colnames(tablelines)<-V[[i]]
				}
				filename<-file.path(dirname,paste(gsub("\\s","\\_",substr(heads[i],3,100)),".csv",sep=""))
        
				write.csv2(tablelines[,-1],filename,row.names=F)   
			} 
		}
		
		if (i == 12 && NODE == 1)
		{
			for (nodenr in 1:length(nodelines))
			{
				start<-nodelines[nodenr]
				nodename<-gsub("\\s+<<< Node\\s+","",rep[start,])
				nodename<-gsub("\\s+>>>\\s*","",nodename)
				nodename<-gsub("[[:punct:]]","_",nodename)
				start<-start+5
				end<-emptylines[emptylines>start][1]-1
           
	            data<-rep[start:end,]
	            tablelineslist<-strsplit(as.character(data),"\\s+")
	            tablelines<-matrix(nrow=length(tablelineslist),ncol=length(tablelineslist[[1]]))
	            
	            for(j in 1:length(tablelineslist))
	            {
	                tablelines[j,]<-tablelineslist[[j]][1:ncol(tablelines)]
	            }
	            colnames(tablelines)=V[[i]]
	            
	            filename<-file.path(nodedir,paste(nodename,".csv",sep=""))
            
	            write.csv2(tablelines[,-1],filename,row.names=F)
	        }
		}
	}
}

write.csv2(summarymatrix,file.path(outpath, "summary.csv"),row.names=T)

colnames(nodeflooding)=c("Node","sumFloodingVol","nOcc")
write.csv2(na.omit(nodeflooding),file.path(outpath, "nodeflooding.csv"),row.names=F)

colnames(outfallloading)=c("OutfallNode","sumTotVol","n_Outfalls")
write.csv2(na.omit(outfallloading),file.path(outpath, "outfallloading.csv"),row.names=F)

print(warnings())

