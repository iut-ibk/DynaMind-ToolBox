########################################################
# swmmrptreader.R
########################################################
rm(list=ls())
library("tools")
# options(error=traceback)
########################################################

wrkpath <- ("~/Work/Current/reportreader")
reppath <- (file.path(wrkpath,"repfiles"))
outpath <- (file.path(wrkpath,"output"))

NODES <- 1 # node detail reports
#JUNCTION <- 0
#SUBCATCHMENT <- 0
wwtp <- c("V020","ARA1") # wwtps are handled differently

#######################################################
crosstotal<-function(tab,tablelines,valcol)
{
    for(k in 1:dim(tablelines)[1])
    {
        value <- as.numeric(tablelines[k,valcol])
        if (!value > 0) next
        
        nodename <- tablelines[k,1]
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

summarymatrixnames=c("name","numberSubcatchments","SUMtotRunoff[Ml]","nInflowNodes","SUMmaxtotInflow[lps]","SUMtotInflowVolume[Ml]","nSurchargedNodes","surcharged[h]","nFloodingNodes", "SUMtotFloodVolume[Ml]","numberStorages","SUMaverageVol[Ml]","SUMmaxVol[Ml]","SUMmaxPcnt","nOutfalls","SUMtotOutfallVolume[Ml]","SUMtotWWTPVolume[Ml]","NSurchargeConduits","SUMhourslimitedcapacity","nPumps","SUMtotPumpVolume[Ml]")

summarymatrix<-matrix(nrow=length(repfiles),ncol=length(summarymatrixnames))
colnames(summarymatrix)=summarymatrixnames

floodingsummary<-matrix(nrow=0,ncol=3)
outfallsummary<-matrix(nrow=0,ncol=3)

for(filenr in 1:length(repfiles))
{
    print(repfiles[filenr])
	summarymatrix[filenr,1] <- basename(file_path_sans_ext(repfiles[filenr]))

	rep <- read.table(repfiles[filenr],sep="\t")
	
	emptylines <- which(nchar(gsub("\\s+","",rep[,1]))==0)
	breaklines <- grep("-----------------------",as.character(rep[,1]))
	nodelines <- grep("<<<",rep[,1])
	
	S<-list()
	S[["Subcatchment Runoff Summary"]]<-c("Subcatchment","totPrec","totRunon","totEvap","totInfil","totRunoff","totRunoff2","PeakRunoff","RunoffCoeff")
	S[["Node Depth Summary"]]<-c("Node","Type","AvgD","MaxD","MaxHGL","tMaxOcc_d","t_MaxOcc_HrMin")
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
	    if ( length(grep(section,rep[,1])) && section == "Node Results" )
	    {
	        S[["Node Depth Summary"]]<-c("Node","Type","AvgD","MaxD","MaxHGL","tMaxOcc_d","t_MaxOcc_HrMin","MaxrepD")
	        nodedir <- file.path(currentdir,"NODES")
	        dir.create(nodedir)
	    }
	}
	
	for (section in names(S))
	{
	    sectionstart<-grep(section,rep[,1])
	    if (!length(sectionstart))
	    {
	        S[c(section)] <- NULL
	        next
	    }
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
    
    # Summary
	    source(file.path(wrkpath, 'sums.R'))
	    summarymatrix<-sums(section,file,sectiontable,wwtp,summarymatrix)

	    if (section == "Node Flooding Summary") # Count of Surcharge nodes through all Simulations
	    {
		    floodingsummary<-crosstotal(floodingsummary,sectiontable,6)
	    }
    
	    else if (section == "Outfall Loading Summary") # Count of Outfall events per node through all Simulations
	    {
		    outfallsummary<-crosstotal(outfallsummary,sectiontable,5)
	    }
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
			    typedata <- strsplit(as.character(data[typestart:typeend]),"\\s+")
        
			    typetable <- matrix(nrow=length(typedata),ncol=length(typedata[[1]]))
			    for(j in 1:dim(typetable)[1]) {	typetable[j,]<-typedata[[j]][1:ncol(typetable)] }
				
				if (ctypes[typenr] == "PUMP") { colnames(typetable) <- S[[section]][-c(6,8)] }
                else if (ctypes[typenr] == "ORIFICE" ) { colnames(typetable) <- S[[section]][-c(6,7)] }
				else if (ctypes[typenr] == "WEIR" ) { colnames(typetable) <- S[[section]][-c(6,7)] }
				else { colnames(typetable) <- S[[section]] }
		    }
		    filename<-file.path(currentdir,paste(gsub("\\s","\\_",section),".csv",sep=""))
		    write.csv2(typetable,filename,row.names=F)
	    }
		
		else if (section == "Node Results")
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
	}
}

write.csv2(summarymatrix,file.path(outpath, "summary.csv"),row.names=T)

colnames(floodingsummary)=c("Node","sumFloodingVol","nOcc")
write.csv2(floodingsummary,file.path(outpath, "floodingsummary.csv"),row.names=F)

colnames(outfallsummary)=c("OutfallNode","sumTotVol","n_Outfalls")
write.csv2(outfallsummary,file.path(outpath, "outfallsummary.csv"),row.names=F)

# print(warnings())