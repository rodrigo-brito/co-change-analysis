library(plyr) # install.packages("plyr")

# INPUTS
data <- read.csv("../output.csv", sep=",")

tmp.go_projects = data[data$language == "go",]
tmp.c_projects = data[data$language == "c",]

tmp.rules_count_go = count(go_projects, "project_name")
tmp.rules_count_c = count(c_projects, "project_name")

options(scipen=999) # disable scientific notation
boxplot(tmp.rules_count_c$freq, tmp.rules_count_go$freq , log = "y", at = c(1,2), names = c("C", "Go"))
quantile(tmp.rules_count_c$freq)
quantile(tmp.rules_count_go$freq)

rm(list = ls(pattern = "tmp.*"))
