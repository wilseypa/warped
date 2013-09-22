# Combines and averages the given csv file(s) using the given settings

from __future__ import print_function
import csv, sys
import itertools, operator
import subprocess
import Gnuplot

###### Settings go here ######
TITLE = "RAID Load Balancing Performance"
# What to filter by (what data to use) - ex only use threads = 16
FILTER = 1
FILTERCOLUMN = "Threads"
FILTERVALUE = "48"

# Lines (what to split into lines by) - enter column header
LINES = "LoadBalancing"
LINEPREFACE = "Load Balancing " ## Appended to beginning of line name in plot

# X-axis
XAXIS = "ScheduleQCount"
XAXISLABEL = "Number of LTSF queues"

# Y-axis (to take median of)
YAXIS = "Runtime"
YAXISLABEL = "Simulation time (s)"

###### Don't edit below here ######

def getIndex(aList, text):
	'''Returns the index of the requested text in the given list'''
	for i,x in enumerate(aList):
		if x == text:
			return i

def median(mylist):
    sorts = sorted(mylist)
    length = len(sorts)
    if length == 0:
        return 0
    if not length % 2:
        return (sorts[length / 2] + sorts[length / 2 - 1]) / 2.0
    return sorts[length / 2]

def plot(data, title, xaxisLabel, yaxisLabel, linePreface, outFileName):
	g = Gnuplot.Gnuplot()
	g.title(title)
	g("set terminal postscript eps size 3.5,2.62 enhanced color font 'Helvetica,20' linewidth 2")
	g("set key right top")
	g.xlabel(xaxisLabel)
	g.ylabel(yaxisLabel)
	g('set output "graph.eps"')
	d = []
	print(data)
	for key in data['data']:
		result = Gnuplot.Data(data['header'],data['data'][key],with_="line",title=linePreface+key)
		d.append(result)
	g.plot(*d)

def main():
	inFileName = sys.argv[1]
	outFileName = sys.argv[2]
	reader = csv.reader(open(inFileName,'rb'))
	header = reader.next()

	writer = open("avg_data", "wb")

	# Get Column Values for use below
	nFilterColumn = getIndex(header, FILTERCOLUMN)
	nLines= getIndex(header, LINES)
	nXaxis = getIndex(header, XAXIS)

	# Column to take median of
	MEDCOL = getIndex(header, YAXIS)

	if FILTER:
		reader = [i for i in reader if i[nFilterColumn] == FILTERVALUE]
	reader = sorted(reader, key=lambda x: x[nLines], reverse=False)
	reader = sorted(reader, key=lambda x: int(x[nXaxis]), reverse=False)

	outData = {'header':[],'data':{}}
	# First sorting criteria (loadBalancing) - different lines
	for sqCount, data in itertools.groupby(reader, lambda x: x[nXaxis]):
		# Label column
		outData['header'].append(int(sqCount))
		# Second sorting criteria (sqCount) - x-axis
		for loadBalancing, lbdata in itertools.groupby(data, lambda x: x[nLines]):
			subList = []
			for row in lbdata:
				if row[MEDCOL] != '': # Filter out empty results
					subList.append(float(row[MEDCOL]))
				label = row[nLines]
			medVal = median(subList)
			if label not in outData['data']:
				outData['data'][label] = []
			outData['data'][label].append(medVal)

	plot(outData, TITLE, XAXISLABEL, YAXISLABEL, LINEPREFACE, outFileName)

if __name__ == "__main__":
    main()
