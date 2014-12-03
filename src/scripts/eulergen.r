###
### Euler Type 2 Rainfall generator ###
###
### Input: r15 values in an CSV file
###
### Output: Euler Type 2 rainfall as a DAT (swmm5) file

# cleanup

rm(list=ls())

file <- ("/home/gaelic/Code/DynaMind/DynaMind-DynAlp/data/intervalyear.csv")

# read

intervalyear <- read.csv("~/Code/DynaMind/DynaMind-DynAlp/data/intervalyear.csv")

# write

