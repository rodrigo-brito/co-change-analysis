library(plyr) # install.packages("plyr")

# INPUTS
data <- read.csv("../output.csv", sep=",")

tmp.go_projects = data[data$language == "go",]
tmp.c_projects = data[data$language == "c",]

options(scipen=999) # disable scientific notation
boxplot(tmp.c_projects$support, tmp.go_projects$support, log = "y", at = c(1,2), ylim=c(1,300), names = c("C", "Go"))
quantile(tmp.c_projects$support)
quantile(tmp.go_projects$support)

boxplot(tmp.c_projects$confidence, tmp.go_projects$confidence, at = c(1,2), names = c("C", "Go"))
quantile(tmp.c_projects$confidence)
quantile(tmp.go_projects$confidence)


rm(list = ls(pattern = "tmp.*"))
