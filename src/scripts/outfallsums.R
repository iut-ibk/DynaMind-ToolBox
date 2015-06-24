outfallsums<-function(tab,tablelines)
{
  outfalls<-row.names(table(tablelines[,2]))
  
  for(k in 1:length(outfalls))
  {
    if(outfalls[k] %in% tab[,1])
    {
      zi<-which(tab[,1]==outfalls[k])
      tab[zi,2]<-as.numeric(tab[zi,2])+as.numeric(tablelines[which(tablelines[,2]==outfalls[k]),6])
      tab[zi,3]<-as.numeric(tab[zi,3])+1
    }
    
    if(!(outfalls[k] %in% tab[,1]))
    {
      tab<-rbind(tab,c(outfalls[k],tablelines[which(tablelines[,2]==outfalls[k]),6],1))
    }
  }
  
  return(tab)
}