#!/usr/bin/python

import csv
import os

output = open("output.csv","w")
output.write("language,project_name,lhs_file,lhs_function,rhs_file,rhs_function,support,confidence\n")

for root, dirs, files in os.walk("./data"):
	for file in files:
		with open("%s/%s" % (root, file)) as csv_file:
			if "/go" in root:
				language = "go"
			elif "/c" in root:
				language = "c"
			else:
				raise "invalid project"

			csv_reader = csv.reader(csv_file, delimiter=',')
			line_count = 0
			for row in csv_reader:
				if len(row) < 5:
					continue

				if row[0] == "transactions_count" or row[0] == "lhs_file":
					continue

				project = file.replace('.csv','')
				output.write(",".join([language, project] + row[:6]))
				output.write("\n")