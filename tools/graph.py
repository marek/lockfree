#!/usr/bin/env bash
import plotly.offline as offline
import plotly.plotly as py
import plotly.graph_objs as go

import csv
import string
import argparse

parser = argparse.ArgumentParser(description='Process lockfree log')
parser.add_argument (
    '--input',
    help='input file')

parser.add_argument(
    '--output',
    help='output of graph picture'
    )

args = parser.parse_args()


# parse header
headerDefined = False
xLabels = None

def filterRow(row):
    return row[1:2] + [ s.translate(
             {ord(c): None for c in string.ascii_letters}
           ) for s in row[3:]]

data = []

for row in csv.reader(open(args.input, 'r'), delimiter=' ', skipinitialspace=True):
    row = filterRow(row)
    if not headerDefined:
        xLabels = row[1:]
        headerDefined = True
        continue

    trace = go.Scatter(
            x = xLabels,
            y = row[1:],
            name = row[0],
            line = dict(
                width = 4
                )
        )

    data.append(trace)

offline.init_notebook_mode()

layout = dict(title = 'Queue Performance',
              xaxis = dict(title = "Log statements (volume)"),
              yaxis = dict(title = 'Time (seconds)')
            )

# Plot and embed in ipython notebook!
fig = dict(data=data, layout=layout)
offline.plot(fig, auto_open=True, filename=args.output)

