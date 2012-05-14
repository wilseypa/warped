import os
import fnmatch

numfiles = 0
for f in os.listdir('.'):
  if fnmatch.fnmatch(f,'lp*.csv'):
    numfiles = numfiles + 1

files = ['lp%d.csv' % x for x in range(numfiles)]
lines = []
for f in files:
  lines.append(open(f, 'r').readlines())

numlines = len(lines[0])
for x in range(numlines - 2):
  uwiavg = 0.
  outline = ''
  for fnum in range(numfiles):
    (uwi, freq) =  lines[fnum][x + 1].strip().split(',')
    uwiavg = uwiavg + float(uwi)
    outline = outline + '%s,%s,' % (uwi, freq)
  uwiavg = uwiavg / numfiles
  outline = '%f: %s' % (uwiavg, outline[0:-1])
  print outline
