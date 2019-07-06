library(plyr) # install.packages("plyr")

# INPUTS
options(scipen=999) # disable scientific notation
data <- read.csv("../output.csv", sep=",")

tmp.go_projects = data[data$language == "go",]
tmp.c_projects = data[data$language == "c",]

# Convert project names
tmp.go_projects[] <- lapply(tmp.go_projects, as.character)
tmp.c_projects[] <- lapply(tmp.c_projects, as.character)

# Go
tmp.rules_equal_count = count(tmp.go_projects[tmp.go_projects$lhs_file==tmp.go_projects$rhs_file,], "project_name")
tmp.rules_diff_count = count(tmp.go_projects[tmp.go_projects$lhs_file!=tmp.go_projects$rhs_file,], "project_name")

par(mar=c(3,5,1,1)) # margin
boxplot(tmp.rules_equal_count$freq, tmp.rules_diff_count$freq , log = "y", at = c(1,2), names = c("Same file", "Different file"), ylab="Number of rules")
quantile(tmp.rules_equal_count$freq)
quantile(tmp.rules_diff_count$freq)

# C
tmp.rules_equal_count = count(tmp.c_projects[tmp.c_projects$lhs_file==tmp.c_projects$rhs_file,], "project_name")
tmp.rules_diff_count = count(tmp.c_projects[tmp.c_projects$lhs_file!=tmp.c_projects$rhs_file,], "project_name")

par(mar=c(3,5,1,1)) # margin
boxplot(tmp.rules_equal_count$freq, tmp.rules_diff_count$freq , log = "y", at = c(1,2), names = c("Same file", "Different file"), ylab="Number of rules")
quantile(tmp.rules_equal_count$freq)
quantile(tmp.rules_diff_count$freq)


rm(list = ls(pattern = "tmp.*"))