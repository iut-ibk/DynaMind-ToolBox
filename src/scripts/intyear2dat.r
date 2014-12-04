###
### Euler Type 2 Rainfall converter ###
###
### Input: interval values in an CSV file sorted by annuality
###
### Output: Euler Type 2 rainfall as a DAT (swmm5) file

# cleanup

rm(list=ls())

# necessary libraries

library(zoo)
library(miscTools)

# workspace & Dateiname

setwd("~/Code/DynaMind/DynaMind-DynAlp/data")
file<-("intervalyear.csv")

limit<-1	# limit until (if unit=min val must be divisible by 5)
unit<-"h"	# min, h, d
datetimebegin<-"2000 01 01 00 00"

# read file

tab<-read.csv(file)

# convert from hours or days to minutes

tab[tab[,2]=="h",1]<-tab[tab[,2]=="h",1]*60
tab[tab[,2]=="h",2]<-"min"

tab[tab[,2]=="d",1]<-tab[tab[,2]=="d",1]*24*60
tab[tab[,2]=="d",2]<-"min"


annualities<-names(tab)[-(1:2)]  # existing annualities
minutescsv<-tab[,1]
maxmin<-max(minutescsv)  # maximum minutes value in file


# check if limit is not bigger than the table

if(unit=="h")
{
	limit<-limit*60
	limiti<-which(tab[,1]==limit)
}

if(unit=="d")
{
	limit<-limit*24*60
	limiti<-which(tab[,1]==limit)
}

if(maxmin<limit || tab[1,1]>=limit || is.na(limit)==TRUE)
{
	print(paste("limit error"))
}


# built matrix for every annuality available

minutes<-seq(5,limit,5)

for(i in 1:length(annualities))
{
	# gen dataframe with values for actual annuality
	mm_anno<-data.frame(Minuten=tab[1:limiti,1],mm_kum=tab[1:limiti,i+2])
	# transform to cummulated values
	difftab<-data.frame(Minuten=minutes,mm_kum=NA,mm_diff=NA)
	# combine to 1 table with 5 min values (if exist, else NA)
	comb<-merge(mm_anno,difftab,by=intersect("Minuten","Minuten"),all.x=T,all.y=T)
	# fill missing values
	difftab$mm_kum<-na.approx(comb$mm_kum.x)
  	# first diff = first value
	difftab[1,3]<-difftab[1,2]
	
    # calculate diffs
	for(j in 2:nrow(difftab))
	{
		difftab[j,3]<-round(difftab[j,2]-difftab[j-1,2],3)
	}
	
    # index for first third
	euleri<-round(nrow(difftab)/3)
	# revert first third
	difftab[1:euleri,3]<-difftab[euleri:1,3]

	# fill matrix
	eulertab<-matrix(ncol=7,nrow=length(minutes))
  
	eulertab[,1]<-"rg1"
	eulertab[,2]<-substr(datetimebegin,1,4)
	eulertab[,3]<-substr(datetimebegin,6,7)
	eulertab[,4]<-substr(datetimebegin,9,10)
	eulertab[,5]<-substr(datetimebegin,12,13)
	eulertab[,6]<-formatC(as.numeric(substr(datetimebegin,15,16))+minutes,width=2, format="d", flag="0")
	eulertab[,7]<-difftab[,3]
  
	eulertab<-insertRow(eulertab,1,c("rg1",substr(datetimebegin,1,4),substr(datetimebegin,6,7),substr(datetimebegin,9,10),substr(datetimebegin,12,13),substr(datetimebegin,15,16),0))
  
# save table to file
 
	write.table(eulertab,paste(getwd(),"/",substr(names(tab)[i+2],2,nchar(names(tab)[i+2])),"_to_",limit,"min.dat",sep=""),append=F,sep=" ",dec=",",row.names=F,col.names=F,quote=F)

}

