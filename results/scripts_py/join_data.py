#!/usr/bin/python

import csv
import os

output = open("../output.csv",'w')
output.write("language,project_name,lhs_file,lhs_function,rhs_file,rhs_function,support,confidence\n")

empty_c_projects = 0
empty_go_projects = 0

for root, dirs, files in os.walk("../data"):
	for file in files:
		with open("%s/%s" % (root, file)) as csv_file:
			if "/go" in root:
				language = "go"
			elif "/c" in root:
				language = "c"
			else:
				raise "invalid file/project"

			csv_reader = csv.reader(csv_file, delimiter=',')
			line_count = 0
			for row in csv_reader:
				if len(row) < 5:
					continue

				if row[0] == "transactions_count" or row[0] == "lhs_file":
					continue

				line_count+=1

				project = file.replace('.csv','')
				output.write(",".join([language, project] + row[:6]))
				output.write("\n")
		
			# Count number of empty projects
			if line_count == 0:
				if language == "c":
					empty_c_projects+=1
				elif language == "go":
					empty_go_projects+=1

print("total empty Go projects = %d" % empty_go_projects)
print("total empty C projects = %d" % empty_c_projects)
print("process finished.")
