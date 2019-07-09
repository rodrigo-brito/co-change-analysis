library(plyr) # install.packages("plyr")

# INPUTS
data <- read.csv("../output.csv", sep=",")

min_confidence = 0.5

tmp.go_projects = data[data$language == "go" & data$confidence >= min_confidence,]
tmp.c_projects = data[data$language == "c" & data$confidence >= min_confidence,]

# Convert project names
tmp.go_projects[] <- lapply(tmp.go_projects, as.character)
tmp.c_projects[] <- lapply(tmp.c_projects, as.character)

tmp.rules_count_go = count(tmp.go_projects, "project_name")
tmp.rules_count_c = count(tmp.c_projects, "project_name")

# include empty rules projects
no_rules_go_projects = 19
no_rules_c_projects = 2

# inclunding empty counting for Python files
for (i in 1:no_rules_go_projects){
  tmp.rules_count_go[nrow(tmp.rules_count_go) + 1,] = list(project_name=as.character(sprintf("empty_%d",i)),freq=0)
}

# inclunding empty counting for C files
for (i in 1:no_rules_c_projects){
  tmp.rules_count_c[nrow(tmp.rules_count_c) + 1,] = list(project_name=as.character(sprintf("empty_%d",i)),freq=0)
}

options(scipen=999) # disable scientific notation
par(mar=c(3,5,1,1)) # margin
boxplot(tmp.rules_count_c$freq, tmp.rules_count_go$freq, at = c(1,2), names = c("C", "Go"), outline = FALSE, ylab="Number of co-changes")
quantile(tmp.rules_count_c$freq)
quantile(tmp.rules_count_go$freq)

rm(list = ls(pattern = "tmp.*"))
