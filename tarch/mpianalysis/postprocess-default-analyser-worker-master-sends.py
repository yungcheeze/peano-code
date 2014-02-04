from PIL import Image, ImageDraw, ImageFont

import csv
import sys
import pydot
import re


inputFilename  = sys.argv[1]
numberOfRanks  = int(sys.argv[2])

fontsize = "0.8"


graph              = pydot.Dot(graph_type='digraph')
sparseMaxGraph     = pydot.Dot(graph_type='digraph')
sparseAverageGraph = pydot.Dot(graph_type='digraph')

for rank in range(0,numberOfRanks):
  graph.add_node(pydot.Node( str(rank), style="filled", fillcolor="grey", fontcolor="blue" ))
  sparseMaxGraph.add_node(pydot.Node( str(rank), style="filled", fillcolor="grey", fontcolor="blue" ))
  sparseAverageGraph.add_node(pydot.Node( str(rank), style="filled", fillcolor="grey", fontcolor="blue" ))


totalMax     = 0
totalAverage = 0
totalCount   = 0

for master in range(0,numberOfRanks):
  for worker in range(0,numberOfRanks):
    count   = 0
    max     = 0
    average = 0
    inFile  = open( inputFilename,  "r" )
    data = "   "
    while ( not data=="" ):
      data = inFile.readline()
      if (
        re.search( "tarch::mpianalysis::DefaultAnalyser::dataWasNotReceivedFromWorker", data ) and 
        re.search( "rank:" + str(master) + " ", data ) and
        re.search( "worker " + str(worker) + " ", data ) 
      ):
        time = float(data.split( "for" )[-1].split("s")[0])
        count      = count + 1
        totalCount = totalCount + 1 
        if ( time > max ):
          max      = time
        if ( time > totalMax ):
          totalMax   = time
        average      = average + time
        totalAverage = totalAverage + time

    if count>2:
      edge = pydot.Edge(str(worker),str(master), label="(" + str(count) + "," + str(max) + "," + str(float(average) / float(count)) + ")", fontsize=fontsize, labelfontcolor="blue" )
      graph.add_edge(edge)
        
  
  
for master in range(0,numberOfRanks):
  for worker in range(0,numberOfRanks):
    count   = 0
    max     = 0
    average = 0
    inFile  = open( inputFilename,  "r" )
    data = "   "
    while ( not data=="" ):
      data = inFile.readline()
      if (
        re.search( "tarch::mpianalysis::DefaultAnalyser::dataWasNotReceivedFromWorker", data ) and 
        re.search( "rank:" + str(master) + " ", data ) and
        re.search( "worker " + str(worker) + " ", data ) 
      ):
        time = float(data.split( "for" )[-1].split("s")[0])
        count      = count + 1
        if ( time > max ):
          max      = time
        average      = average + time


    if count>2 and (float(average) / float(count) > float(totalAverage) / float(totalCount)):
      edge = pydot.Edge(str(worker),str(master), label="(" + str(count) + "," + str(max) + "," + str(float(average) / float(count)) + ")", fontsize=fontsize, labelfontcolor="blue" )
      sparseAverageGraph.add_edge(edge)

    if count>2 and ( float(max) > 0.9 * float(totalMax) ):
      edge = pydot.Edge(str(worker),str(master), label="(" + str(count) + "," + str(max) + "," + str(float(average) / float(count)) + ")", fontsize=fontsize, labelfontcolor="blue" )
      sparseMaxGraph.add_edge(edge)
      
        
graph.write_png(sys.argv[1]+'.worker-master.png', prog='fdp')
sparseMaxGraph.write_png(sys.argv[1]+'.worker-master-sparse-max.png', prog='fdp')
sparseAverageGraph.write_png(sys.argv[1]+'.worker-master-sparse-average.png', prog='fdp')


#dot
#twopi
#neato
#fdp

