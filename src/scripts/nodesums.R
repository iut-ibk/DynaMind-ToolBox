nodesums<-function(tab,tablelines)
{
  nodes<-row.names(table(tablelines[,2]))
  
  for(k in 1:length(nodes))
  {
    if(nodes[k] %in% tab[,1])
    {
      zi<-which(tab[,1]==nodes[k])
      tab[zi,2]<-as.numeric(tab[zi,2])+as.numeric(tablelines[which(tablelines[,2]==nodes[k]),6])
      tab[zi,3]<-as.numeric(tab[zi,3])+1
    }
    
    if(!(nodes[k] %in% tab[,1]))
    {
      tab<-rbind(tab,c(nodes[k],tablelines[which(tablelines[,2]==nodes[k]),6],1))
    }
  }
  
  return(tab)
}