########################################################
# swmmrptreader.R
########################################################
rm(list=ls())
library("tools")
# options(error=traceback)
########################################################

wrkpath <- ("/home/gaelic/DynAlp-v11")
reppath <- (file.path(wrkpath,"repfiles"))
outpath <- (file.path(wrkpath,"output"))

NODES <- 0 # node detail reports
#JUNCTION <- 0
#SUBCATCHMENT <- 0
wwtp <- c("V020","ARA1") # wwtps are handled differently

#######################################################
crosstotal<-function(tab,sectiontable,valcol)
{
    for(k in 1:dim(sectiontable)[1])
        
    {
        value <- as.numeric(sectiontable[k,valcol])
        if (!value > 0) next
        nodename <- sectiontable[k,1]
        ni<-which(tab[,1]==nodename)
        
        if(length(ni))
        {
            ni<-which(tab[,1]==nodename)
            tab[ni,2] = as.numeric(tab[ni,2])+value
            tab[ni,3] = as.numeric(tab[ni,3])+1
        }
        else
        {
            values <-c(nodename,value,1)
            tab <- rbind(tab,values)
        }
    }
    return(tab)
}

repfiles <- Sys.glob(file.path(reppath,"*.rep"))

summarymatrix<-matrix(nrow=length(repfiles),ncol=0)
floodingsummary<-matrix(nrow=0,ncol=3)
outfallsummary<-matrix(nrow=0,ncol=3)

for(filenr in 1:length(repfiles))
{
    print(paste(filenr," of ",length(repfiles)," - ",repfiles[filenr]))
    srow <- c(basename(file_path_sans_ext(repfiles[filenr])))
    smn <- c("name")

	rep <- read.table(repfiles[filenr],sep="\t")
	
	emptylines <- which(nchar(gsub("\\s+","",rep[,1]))==0)
	breaklines <- grep("-----------------------",as.character(rep[,1]))
	nodelines <- grep("<<<",rep[,1])
	
	S<-list()
	S[["Subcatchment Runoff Summary"]]<-c("Subcatchment","totPrec","totRunon","totEvap","totInfil","totRunoff","totRunoff2","PeakRunoff","RunoffCoeff")
	S[["Node Depth Summary"]]<-c("Node","Type","AvgD","MaxD","MaxHGL","tMaxOcc_d","t_MaxOcc_HrMin","MaxDRep")
	S[["Node Inflow Summary"]]<-c("Node","Type","MaxLatInflow","MaxTotInflow","tMaxOcc_d","t_MaxOcc_HrMin","LatInflowVol","TotInflowVol","FlowBalErr")
	S[["Node Surcharge Summary"]]<-c("Node","Type","hSurch","MaxHabCrown","MinDbelRim")
	S[["Node Flooding Summary"]]<-c("Node","hFlooded","MaxRate","tMaxOcc_d","t_MaxOcc_HrMin","TotFloodV","MaxPondD")
	S[["Storage Volume Summary"]]<-c("StorUnit","AvgVol","AvgPcntFull","EvapPcntLoss","ExfilPcntLoss","MaxVol","MaxPcntFull","tMaxOcc_d","t_MaxOcc_HrMin","MaxOutflow")
	S[["Outfall Loading Summary"]]<-c("OutfallNode","FlowFreqPcnt","AvgFlow","MaxFlow","TotVol")
	S[["Link Flow Summary"]]<-c("Link","Type","MaxFlow","tMaxOcc_d","t_MaxOcc_HrMin","MaxVeloc","MaxFullFlow","MaxFullDepth")
	S[["Flow Classification Summary"]]<-c("Conduit","AdjActL","F_Dry","F_UpDry","F_DownDry","F_SubCrit","F_SupCrit","F_UpCrit","F_DownCrit","F_NormLtd","F_InletCtrl")
	S[["Conduit Surcharge Summary"]]<-c("Conduit","HF_BothEnds","HF_Upstream","HF_Dnstream","HabFullNormF","HCapLim")
	S[["Pumping Summary"]]<-c("Pump","PctUt","NStartups","MinFlow","AvgFlow","MaxFlow","TotVol","PowerUsage","PctTimeOff_PumpLow","PctTimeOff_CurveHigh")
	S[["Node Results"]]<-c("Date","Time","Inflow[lps]","Flooding[lps]","Depth[m]","Head[m]")
	S[["Conduit Results"]]<-c("Date","Time")
	
	currentdir <- file.path(outpath,basename(file_path_sans_ext(repfiles[filenr])))
	dir.create(currentdir)
	
	for (section in names(S))
	{
	    if ( length(grep(section,rep[,1])) )
	        {
	        if ( section == "Node Results" )
	        {
	            S[["Node Depth Summary"]]<-c("Node","Type","AvgD","MaxD","MaxHGL","tMaxOcc_d","t_MaxOcc_HrMin","MaxrepD")
	            nodedir <- file.path(currentdir,"NODES")
	            dir.create(nodedir)
	        }
	    }
	    else
	    {
	        S[c(section)] <- NULL
	    }
	}
	
	for (section in names(S))
	{
	    sectionstart<-grep(section,rep[,1])
	    start<-breaklines[breaklines>sectionstart][2]+1
	    end<-emptylines[emptylines>start][1]-1
	
		if (section == "Outfall Loading Summary")
		{
			end<-emptylines[emptylines>start][1]-3
		}
	    
        data<-rep[start:end,] # get section
        data<-gsub("(^[[:space:]]+|[[:space:]]+$)", "", data) #trim leading and trailing spaces
        data<-strsplit(as.character(data),"\\s+") #split data

        sectiontable<-matrix(nrow=length(data),ncol=length(data[[1]]))

	    for (j in 1:length(data)) { sectiontable[j,]<-data[[j]][1:ncol(sectiontable)] }
        sectiontable[is.na(sectiontable)] <- 0
    
    # the following section will be split by types
	    if (section == "Node Depth Summary" ||
	        section == "Node Inflow Summary" ||
	        section == "Node Surcharge Summary"||
	        section == "Link Flow Summary")
	    {
	        types<-vector(length=dim(sectiontable)[1])
			
		    for (j in 1:dim(sectiontable)[1]) { types[j]<-sectiontable[j,2] }
      
		    ctypes<-row.names(table(types))
		    ntypes<-table(types)
			
		    for(typenr in 1:length(ctypes))
		    {
			    typestart <- min(which(types == ctypes[typenr]))
			    typeend <- typestart+ntypes[[typenr]]-1
			    typetable <- sectiontable[typestart:typeend,]
			    
			    colnames(typetable) <- S[[section]]
			    filename<-file.path(currentdir,paste(gsub("\\s","\\_",section),"-",ctypes[typenr],".csv",sep=""))
			    write.csv2(typetable,filename,row.names=F)
		    }
	    }
		
		else if (section == "Node Results" & NODES == 1)
		{
			for (nodenr in 1:length(nodelines))
			{
				start <- nodelines[nodenr]
				nodename <- gsub("\\s+<<< Node\\s+","",rep[start,])
				nodename <- gsub("\\s+>>>\\s*","",nodename)
				nodename <- gsub("[[:punct:]]","_",nodename)
				start <- start + 5
				end <- emptylines[emptylines>start][1] - 1
           
	            nodedata <- rep[start:end,]
	            nodedata<-gsub("(^[[:space:]]+|[[:space:]]+$)", "", nodedata) #trim leading and trailing spaces
	            nodedata<-strsplit(as.character(nodedata),"\\s+")
	            nodetable<-matrix(nrow=length(nodedata),ncol=length(nodedata[[1]]))
	            
	            for(j in 1:dim(nodetable)[1]) {	nodetable[j,]<-nodedata[[j]][1:ncol(nodetable)] }
	            colnames(nodetable)=S[[section]]
	            
	            filename<-file.path(nodedir,paste(nodename,".csv",sep=""))
            
	            write.csv2(nodetable,filename,row.names=F)
	        }
		}
	    else
	    {
	        colnames(sectiontable) <- S[[section]]
	        
	        filename<-file.path(currentdir,paste(gsub("\\s","\\_",section),".csv",sep=""))
	        write.csv2(sectiontable,filename,row.names=F)
	    }
        
        ## Summarys handled below
        
	    if (section == "Subcatchment Runoff Summary")
	    {
	        srow <- cbind(srow, dim(sectiontable)[1]); smn <-c(smn,"Nsubcatchments")
	        srow <- cbind(srow, sum(as.numeric(sectiontable[,6]))); smn<-c(smn,"SUMtotrunoff[Ml]")
	    }
        else if (section == "Node Inflow Summary")
        {
            srow <- cbind(srow, dim(sectiontable)[1]); smn <- c(smn,"Ninflownodes")
            srow <- cbind(srow, sum(as.numeric(sectiontable[,4]))); smn <- c(smn, "SUMmaxtotinflow[lps]")
            srow <- cbind(srow, sum(as.numeric(sectiontable[,8]))); smn <- c(smn, "SUMmaxtotinflowvolume[Ml]")
        }
        else if (section == "Node Surcharge Summary")
        {
            srow <- cbind(srow, dim(sectiontable)[1]); smn <- c(smn, "Nsurchargenodes")#numberSurchargeNodes
            srow <- cbind(srow, max(as.numeric(sectiontable[,3]))); smn <- c(smn, "MAXhourssurcharge")#MAXhourssurcharges
            srow <- cbind(srow, sum(as.numeric(sectiontable[,3]))); smn <- c(smn, "SUMhourssurcharge")#SUMhourssurcharged
        }
        else if (section == "Node Flooding Summary") # Count of Surcharge nodes through all Simulations
        {
            srow <- cbind(srow, dim(sectiontable)[1]); smn <- c(smn, "Nfloodingnodes") #numberFloodingNodes
            srow <- cbind(srow, max(as.numeric(sectiontable[,2]))); smn <- c(smn, "MAXhoursflooded") #MAXhoursflooded
            srow <- cbind(srow, sum(as.numeric(sectiontable[,2]))); smn <- c(smn, "SUMhoursflooded") #SUMhoursflooded
            srow <- cbind(srow, sum(as.numeric(sectiontable[,6]))); smn <- c(smn, "SUMfloodvolume[Ml]") #SUMFloodVolume
            floodingsummary<-crosstotal(floodingsummary,sectiontable,6)
        }
        else if (section == "Storage Volume Summary")
        {
            srow <- cbind(srow, dim(sectiontable)[1]); smn <- c(smn, "Nstorages") #numberStorages
            srow <- cbind(srow, sum(as.numeric(sectiontable[,2]))); smn <- c(smn, "SUMaveragestoragevol[Ml]") #SUMaverageVol[Ml]
            srow <- cbind(srow, sum(as.numeric(sectiontable[,6]))); smn <- c(smn, "SUMmaxstoragevol[Ml]") #SUMmaxVol[Ml]
            srow <- cbind(srow, sum(as.numeric(sectiontable[,7]))); smn <- c(smn, "SUMmaxstoragepcnt") #SUMmaxPcnt
        }
        else if (section == "Outfall Loading Summary") # Count of Outfall events per node through all Simulations
        {
            srow <- cbind(srow, (dim(sectiontable)[1] - length(wwtp))); smn <- c(smn, "Noutfalls") #noutfalls
            srow <- cbind(srow, sum(as.numeric(sectiontable[which(!(sectiontable[,1] %in% wwtp)),5]))); smn <- c(smn, "SUMcsovolume[Ml]") #SUMtotOutfallVolume[Ml] without WWTP
            srow <- cbind(srow, sum(as.numeric(sectiontable[which((sectiontable[,1] %in% wwtp)),5]))); smn <- c(smn, "SUMwwtpvolume[Ml]") #SUMtotWWTPVolume[Ml]
            outfallsummary<-crosstotal(outfallsummary,sectiontable,5)
        }
        else if (section == "Conduit Surcharge Summary")
        {
            srow <- cbind(srow, dim(sectiontable)[1]); smn <- c(smn, "Nsurchargeconduits") #NSurchargeConduits
            srow <- cbind(srow, sum(as.numeric(sectiontable[,6]))); smn <- c(smn, "SUMhourslimcapconduits") #SUMhourslimitedcapacity
        }
        else if (section == "Pumping Summary")
        {
            srow <- cbind(srow, dim(sectiontable)[1]); smn <- c(smn, "Npumps") #nPumps
            srow <- cbind(srow, sum(as.numeric(sectiontable[,7]))); smn <- c(smn, "SUMpumpvolume[Ml]") #SUMtotPumpVolume[Ml]
        }
	}
	
	if (filenr == 1) summarymatrix <- matrix(ncol=dim(srow)[2],nrow=0)
    
	summarymatrix <- rbind(summarymatrix, srow)
	colnames(summarymatrix) <- smn
}

write.csv2(summarymatrix,file.path(outpath, "summary.csv"),row.names=T)

colnames(floodingsummary)=c("Node","sumFloodingVol","nOcc")
write.csv2(floodingsummary,file.path(outpath, "floodingsummary.csv"),row.names=F)

colnames(outfallsummary)=c("OutfallNode","sumTotVol","n_Outfalls")
write.csv2(outfallsummary,file.path(outpath, "outfallsummary.csv"),row.names=F)

# print(warnings())
