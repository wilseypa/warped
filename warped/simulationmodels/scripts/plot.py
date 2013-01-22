# Author: Ryan Child
# Description: Plotter for WARPED simulate.sh data
#
# Requires scipy
# Linux: apt-get install python-scipy
# Windows: http://www.lfd.uci.edu/~gohlke/pythonlibs/

import csv
from pylab import *


# editable settings
# csv containing averaged results from batch simulation
filename = 'averages.csv'

# name of csv column to use for x-axis
xcol = 'msgden'
# x axis title
xaxistitle = 'Message Density'

# list of csv columns to use for y-axis (one per graph)
ycols = ['simtime', 'eventrate', 'efficiency', 'rollbacks']
# list of y axis titles (one per graph)
yaxistitles = ['Simulation Time (s)', \
               'Event Rate (thousand events/s)', \
               'Efficiency', \
               'Rollbacks/1000']

# y values are divided by this for scaling (one per graph)
divisors = [1, 1000, 1, 1000]

# list of subplots (one per graph)
subplots = ['221', '222', '223', '224']

# list of configurations to use as series data, '' for don't care
series = [['6', 'Real', 'False', 'Performance', '10'], \
          ['6', 'Real', 'False', 'Power', '10'], \
          ['6', 'Real', 'False', 'Hybrid', '10'], \
          ['6', 'Real', 'True', 'Performance', '10'], \
          ['4', 'Real', 'False', 'Performance', '10'], \
          ['4', 'Real', 'False', 'Power', '10'], \
          ['4', 'Real', 'False', 'Hybrid', '10'], \
          ['4', 'Real', 'True', 'Performance', '10']]

# labels corresponding to series above (printed in legend)
labels = ['6 lPs (Performance)', \
          '6 LPs (LowPower)', \
          '6 LPs (Hybrid)', \
          '6 LPs (Fixed)', \
          '4 lPs (Performance)', \
          '4 LPs (LowPower)', \
          '4 LPs (Hybrid)', \
          '4 LPs (Fixed)']

# line styles
linestyles = ['k-o', 'k-x', 'k-+', 'k-s', 'k-v','k-<', 'k-*', 'k-p']

# plotter - don't modify anything below here
csvfile = open(filename, 'rb')
for i in range(len(series)):
  s = series[i]
  x = []
  y = [[] for _ in ycols]
  csvfile.seek(0)
  reader = csv.DictReader(csvfile)
  for row in reader:
    if all([row[reader.fieldnames[j]].lower() == s[j].lower() or
           s[j] == '' for j in range(len(s))]):
      if row[xcol] not in x:
        x.append(row[xcol])
      for k in range(len(ycols)):
        y[k].append(float(row[ycols[k]]) / divisors[k])

  for j in range(len(ycols)):
    subplot(subplots[j])
    plot(x, y[j], linestyles[i], lw=1.0)
    xlabel(xaxistitle)
    ylabel(yaxistitles[j])
    xticks(arange(0,40,4))
    legend(labels, loc=0, frameon=False, numpoints=1, prop={'size':9})
    
show()
