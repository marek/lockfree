import plotly.plotly as py
import plotly.graph_objs as go
import csv


# parse header
import fileinput

gotHeader = False
Y_label = 

for line in csv.reader(fileinput.input()):
    if not gotHeader:
        Y label.