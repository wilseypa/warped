# Author: Ryan Child
# Description: averages output from WARPED simulate.sh

import csv


# number of input parameters
numparams = 6
# number of outputs to average
numoutput = 4
# number of values to average
n = 10

reader = csv.reader(open('data.csv','rb'))
writer = csv.writer(open('averages.csv','wb'))
count = 0
avgs = [0 for _ in range(numoutput)]
writer.writerow(reader.next())
lastrow = []
for row in reader:
  for i in range(numparams, len(row)):
    avgs[i-numparams] = avgs[i-numparams] + float(row[i])
  count = count + 1
  if count == n:
    newrow = lastrow[0:numparams]
    for val in avgs:
      val = val / n
      newrow.append(val)
    writer.writerow(newrow)
    avgs = [0 for _ in range(numoutput)]
    count = 0
  lastrow = row
