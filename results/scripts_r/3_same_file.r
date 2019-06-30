library(plyr) # install.packages("plyr")

# INPUTS
data <- read.csv("../output.csv", sep=",")

tmp.go_projects = data[data$language == "go",]
tmp.c_projects = data[data$language == "c",]

tmp.go_projects[] <- lapply(tmp.go_projects, as.character)
tmp.c_projects[] <- lapply(tmp.c_projects, as.character)

# Same file
tmp.rules_equal_count_go = count(tmp.go_projects[tmp.go_projects$lhs_file==tmp.go_projects$rhs_file,], "project_name")
tmp.rules_equal_count_c = count(tmp.c_projects[tmp.go_projects$lhs_file==tmp.go_projects$rhs_file,], "project_name")

options(scipen=999) # disable scientific notation
boxplot(tmp.rules_equal_count_c$freq, tmp.rules_equal_count_go$freq , log = "y", at = c(1,2), names = c("C", "Go"))
quantile(tmp.rules_equal_count_c$freq)
quantile(tmp.rules_equal_count_go$freq)

# Different file
tmp.rules_equal_count_go = count(tmp.go_projects[tmp.go_projects$lhs_file!=tmp.go_projects$rhs_file,], "project_name")
tmp.rules_equal_count_c = count(tmp.c_projects[tmp.go_projects$lhs_file!=tmp.go_projects$rhs_file,], "project_name")

options(scipen=999) # disable scientific notation
boxplot(tmp.rules_equal_count_c$freq, tmp.rules_equal_count_go$freq , log = "y", at = c(1,2), names = c("C", "Go"))
quantile(tmp.rules_equal_count_c$freq)
quantile(tmp.rules_equal_count_go$freq)

rm(list = ls(pattern = "tmp.*"))