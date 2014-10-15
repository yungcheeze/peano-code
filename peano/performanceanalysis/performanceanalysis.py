import sys
import re
import pylab
import networkx 



searchPatternNumberOfCells         = ".*peano::performanceanalysis::DefaultAnalyser::endIteration.*cells=\("
searchPatternNumberOfLocalVertices = ".*peano::performanceanalysis::DefaultAnalyser::endIteration.*local-vertices="



numberOfInnerLeafCells  = {}
numberOfInnerCells      = {}
numberOfOuterLeafCells  = {}
numberOfOuterCells      = {}
numberOfLocalCells      = {}
numberOfLocalVertices   = {}
  
  
  
#
# Display constants. Might be reset later
#
AlphaValue = 1.0



def parseInputFile():
  #data = {k: [] for k in range(2)}

  print "prepare data structures",
  for rank in range(0,numberOfRanks):
    numberOfInnerLeafCells[rank]  = []
    numberOfInnerCells[rank]      = []
    numberOfOuterLeafCells[rank]  = []
    numberOfOuterCells[rank]      = []
    numberOfLocalCells[rank]      = []
    numberOfLocalVertices[rank]   = []
    print ".",
  print " done"
   
  try:
    inputFile = open( inputFileName,  "r" )
    print "parse ",
    for line in inputFile:
      for rank in range(0,numberOfRanks):
        if ("DefaultAnalyser" in line):
          m = re.search( "rank:" + str(rank) + " +" + searchPatternNumberOfCells, line )
          if (m):
            token = line.replace("(",",").replace(")","").strip().split(",")
            numberOfInnerLeafCells[rank].append(float(token[-5]))
            numberOfOuterLeafCells[rank].append(float(token[-4]))
            numberOfInnerCells[rank].append(float(token[-3]))
            numberOfOuterCells[rank].append(float(token[-2]))
            numberOfLocalCells[rank].append(float(token[-1]))
            print ".",
          m = re.search( "rank:" + str(rank) + " +" + searchPatternNumberOfLocalVertices, line )
          if (m):
            token = line.strip().split("=")
            numberOfLocalVertices[rank].append(float(token[-1]))
            print ".",
          
    print " done"
  except Exception as inst:
    print "failed to read " + inputFileName
    print inst



def plotLogicalTopology():
  topologyGraph = networkx.MultiDiGraph()
  topologyGraph.add_node(0, color='blue')
  
  try:
    inputFile = open( inputFileName,  "r" )
    print "parse topology",
    for line in inputFile:
        searchPatternAddEdge    = "peano::performanceanalysis::DefaultAnalyser::addWorker.*(d)"
        searchPatternRemoveEdge = "peano::performanceanalysis::DefaultAnalyser::removeWorker.*"
        xxx
      if ("DefaultAnalyser" in line):
          m = re.search( "rank:" + str(rank) + " +" + searchPatternNumberOfCells, line )
          if (m):
            token = line.replace("(",",").replace(")","").strip().split(",")
            numberOfInnerLeafCells[rank].append(float(token[-5]))
            numberOfOuterLeafCells[rank].append(float(token[-4]))
            numberOfInnerCells[rank].append(float(token[-3]))
            numberOfOuterCells[rank].append(float(token[-2]))
            numberOfLocalCells[rank].append(float(token[-1]))
            print ".",
          m = re.search( "rank:" + str(rank) + " +" + searchPatternNumberOfLocalVertices, line )
          if (m):
            token = line.strip().split("=")
            numberOfLocalVertices[rank].append(float(token[-1]))
            print ".",
          
    print " done"
  except Exception as inst:
    print "failed to read " + inputFileName
    print inst
  
  
  #G.add_edge(1,2)
  #>>> G.add_edge(1,3)
  #>>> G[1][3]['color']='blue'
  #>>> G[1][3]['weight']='blue'
  pylab.clf()
  pylab.title( "Logical topology" )
  networkx.draw_circular(topologyGraph)
  pylab.savefig( outputFileName + ".topology.png" )
  pylab.savefig( outputFileName + ".topology.pdf" )
  


def setGeneralPlotSettings():
  pylab.xlabel('t')
  pylab.grid(True)
  try:
    pylab.legend(fontsize=9, framealpha=0.5)
  except:
    # old pylab version
    l = pylab.legend(prop={'size':9})
    l.get_frame().set_alpha(0.5)



def plotGlobalGridOverview():
  pylab.clf()
  pylab.title( "Global cells" )
  x = pylab.arange(0, len(numberOfInnerLeafCells[0]), 1.0)
  pylab.plot(x, numberOfInnerLeafCells[0], 'o-',  markersize=10, color='r', label='#inner leaf cells' )
  pylab.plot(x, numberOfOuterLeafCells[0], '+-',  markersize=10, color='g', label='#outer leaf cells' )
  pylab.plot(x, numberOfInnerCells[0],     '.-',  markersize=10, color='r', label='#inner cells' )
  pylab.plot(x, numberOfOuterCells[0],     'x-',  markersize=10, color='g', label='#outer cells' )
  setGeneralPlotSettings()
  pylab.savefig( outputFileName + ".grid-overview.png" )
  pylab.savefig( outputFileName + ".grid-overview.pdf" )

  pylab.clf()
  pylab.title( "Local cells" )
  startRank = 1
  if (numberOfRanks==1):
    startRank = 0
  for rank in range(startRank,numberOfRanks):
    x = pylab.arange(0, len(numberOfLocalCells[rank]), 1.0)
    pylab.plot(x, numberOfLocalCells[rank], 'o',  color='#000000', alpha=AlphaValue, markersize=10)
  pylab.xlabel('t')
  pylab.savefig( outputFileName + ".local-cells.png" )
  pylab.savefig( outputFileName + ".local-cells.pdf" )

  pylab.clf()
  pylab.title( "Local vertices" )
  startRank = 1
  if (numberOfRanks==1):
    startRank = 0
  for rank in range(startRank,numberOfRanks):
    x = pylab.arange(0, len(numberOfLocalVertices[rank]), 1.0)
    pylab.plot(x, numberOfLocalVertices[rank], 'o',  color='#000000', alpha=AlphaValue, markersize=10)
  pylab.xlabel('t')
  pylab.savefig( outputFileName + ".local-vertices.png" )
  pylab.savefig( outputFileName + ".local-vertices.pdf" )



#
# main
#
if ( len(sys.argv)<=2 ):
  print "please specify input file name and number of ranks"
else:
  inputFileName   = sys.argv[1]
  outputFileName  = sys.argv[1] + ".html"
  numberOfRanks   = int(sys.argv[2])

  outFile        = open( outputFileName, "w" )
  
  AlphaValue     = 1.0/numberOfRanks
  if (AlphaValue<0.01):
    AlphaValue   = 0.01
  
  print "read input file " + inputFileName + " for " + str(numberOfRanks) + " rank(s)"
  parseInputFile()
  
  #
  # Header of report
  #
  print "write report header"
  outFile.write( 
    "<html>\
       <body>\
       <h1>Peano Performance Report</h1>\
       <p>Peano's default performance analysis</p>\
       <p>Author: Tobias Weinzierl</p>\
       <p>Data file: " + inputFileName + "</p>\
       <p>Ranks: " + str(numberOfRanks) + "</p>\
       <center><img src=\"http://www.peano-framework.org/logo.png\" />\
       <br /><a href=\"http://www.peano-framework.org\" >www.peano-framework.org</a></center>\
    ")

  outFile.write( "\
     <h2 id=\"table-of-contents\">Table of contents</h2>\
     <ul>\
       <li><a href=\"#global-grid-overview\">Global grid overview</a></li>\
       <li><a href=\"#logical-topology\">Logical topology</a></li>\
     </ul>\
   ")


  #
  # Grid information
  #
  print "plot global grid overview"
  plotGlobalGridOverview()
  outFile.write( "\
    <h2 id=\"global-grid-overview\">Global grid overview</h2>\
    <img src=\"" + outputFileName + ".grid-overview.png\" />\
    <img src=\"" + outputFileName + ".local-cells.png\" />\
    <img src=\"" + outputFileName + ".local-vertices.png\" />\
    <br /><br />\
    <p>\
    The darker the plots, the more ranks exhibit a certain characteristics. \
    If the points spread out, this is an indicator that your load balancing is \
    inadequate. \
    If you run Peano on multiple ranks, rank 0 typically degenerates to a pure \
    administrative rank, i.e. it is o.k. if there is close to 0 vertices and cells \
    for one particular rank. \
    </p>\
    <a href=\"#table-of-contents\">To table of contents</a>\
    ")

  #
  # Grid information
  #
  print "plot logical grid topology"
  plotLogicalTopology()
  outFile.write( "\
    <h2 id=\"logical-topology\">Logical topology</h2>\
    <img src=\"" + outputFileName + ".topology.png\" />\
    <br /><br />\
    <p>\
    Yet to be written. \
    </p>\
    <a href=\"#table-of-contents\">To table of contents</a>\
    ")

  #
  # Trailor of report
  #
  print "write report trailer"
  outFile.write( "</body>" )
  outFile.write( "</html>" )
  outFile.close()
  