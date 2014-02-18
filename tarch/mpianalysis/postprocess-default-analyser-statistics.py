import sys
import os
import os.path
import re


inputFilename  = sys.argv[1]
outputFilename = "report-" + inputFilename + ".html"

outFile        = open( outputFilename, "w" )


forks = 0
joins = 0
workingRanks = 1
sumOfWorkingRanks = 1
maxRanks     = 1
data = "   "
forksPerLevel = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ]

inFile         = open( inputFilename,  "r" )
while ( not data=="" ):
  data = inFile.readline()
  if (re.search( "tarch::mpianalysis::DefaultAnalyser::addWorker", data )):
    forks             = forks + 1
    workingRanks      = workingRanks + 1
    sumOfWorkingRanks = sumOfWorkingRanks + workingRanks
    level             = int(data.split("level:")[-1].split("]")[0])
    forksPerLevel[level] = forksPerLevel[level] + 1
    if workingRanks > maxRanks: 
      maxRanks = workingRanks
  if (re.search( "tarch::mpianalysis::DefaultAnalyser::removeWorker", data )):
    joins = joins + 1
    workingRanks = workingRanks - 1
    sumOfWorkingRanks = sumOfWorkingRanks + workingRanks
inFile.close()


outFile.write( "<html>" )
outFile.write( "<body>" )


outFile.write( "<h1>Report on " + inputFilename + "</h1>" )
outFile.write( "<p>Peano mpianalysis default postprocessing<br />" )
outFile.write( "   Authors: Kristof Unterweger, Tobias Weinzierl, Roland Wittmann</p>" )

outFile.write( "<p>\
The following data are extracted from the file " + inputFilename + ".\
If you don't see any output, ensure that you have the tracing\
enabled and that you do not filter out info information from the\
tarch::mpianalysis component.\
Some postprocessing steps also require the pydot python module, i.e. \
you have to ensure that the pydot path is set via PYTHONPATH.\
</p>" )



outFile.write( "<center>" )
outFile.write( "<table border=\"1\">" )
outFile.write( "<tr>" )
outFile.write( "<td>Ranks:</td><td>" + sys.argv[2] + "</td>" )
outFile.write( "<tr>" )
outFile.write( "<td>Forks total:</td><td>" + str(forks) + "</td>" )
outFile.write( "</tr><tr>" )
outFile.write( "<td>Joins total:</td><td>" + str(joins) + "</td>" )
outFile.write( "</tr><tr>" )
outFile.write( "<td>Max. working ranks:</td><td>" + str(maxRanks) + "</td>" )
outFile.write( "</tr><tr>" )
outFile.write( "<td>Average working ranks after each reblancing:</td><td>" + str(sumOfWorkingRanks/(forks+joins)) + "</td>" )
outFile.write( "</tr>" )
outFile.write( "</table>" )


outFile.write( "<table border=\"1\">" )
outFile.write( "<tr>" )
for i in range(0,10):
  outFile.write( "</tr><tr>" )
  outFile.write( "<td>Forks on level " + str(i) + ":</td><td>" )
  outFile.write( str(forksPerLevel[i]) )
  outFile.write( "</td>" )

outFile.write( "</tr>" )
outFile.write( "</table>" )
outFile.write( "</center>" )


outFile.write( "<p>\
<i>Performance hint: </i>\
If the maximal number of working ranks is significantly smaller than the total number of ranks, your application might \
have ran into a weak scaling issue. The problem then was too small. If the problem is sufficiently big, and the number \
if idle ranks still remains high, it might had happened that Peano was not able to fork on a coarse enough level. \
See the section on MPI tuning in the wiki that discusses multiscale concurrency. In such a case, it often helps \
to make the computational grid more regular to some degree, i.e. to prescribe a finer maximum mesh size. \
</p>" )


outFile.write( "<h2>Fork-join evaluation</h2>" )
outFile.write( "<img src=\"" + inputFilename + ".joins-and-forks.png\" />" )

outFile.write( "<center>" )
outFile.write( "<table border=\"1\">" )
outFile.write( "<tr> <td><b>Event type</b></td> <td><b>Master rank</b></td> <td><b>Worker rank</b></td> <td><b>Worker's domain</b></td> </td>" )
inFile         = open( inputFilename,  "r" )
data           = "  "
while ( not data=="" ):
  data = inFile.readline()
  if (re.search( "tarch::mpianalysis::DefaultAnalyser::addWorker", data )):
     outFile.write( "<tr>" )
     outFile.write( "<td>fork</td>" )
     outFile.write( "<td>" )
     outFile.write( data.split( "->" )[0].split( ")" )[-1] )
     outFile.write( "</td>" )
     outFile.write( "<td>" )
     outFile.write( data.split( "+" )[1].split( "[" )[0] )
     outFile.write( "</td>" )
     outFile.write( "<td>" )
     outFile.write( data.split( "worker's domain:" )[1] )
     outFile.write( "</td>" )
     outFile.write( "</tr>" )
inFile.close()
outFile.write( "</table>" )
outFile.write( "</center>" )


outFile.write( "<p>\
<i>Performance hint: </i>\
Joins and forks are expensive operations in terms of walltime. Evaluating the load balancing \
information also is not for free. Hence, try to reduce the number of joins and forks as you \
switch on rebalancing only from time to time, and reduce the load balancing overhead. See \
the section on 'Disable load balancing' in the wiki. \
</p>" )



outFile.write( "<h2>Worker tree</h2>" )
outFile.write( "<p>The following diagram gives an overview of your logical topology. Snapshots at multiple time-steps are merged, i.e. it is a static representation. Image might not be available if pydot is not available to your Python installation.</p>" )
outFile.write( "<img src=\"" + inputFilename + ".workertree.png\" />" )


outFile.write( "<p>\
<i>Performance hint: </i>\
Study this graph together with a visualisation of the real grid decomposition (such as the graph \
below for 2d). Ensure that your decomposition is balanced any take the wiki's remarks on \
load balancing and node weights into account. \
</p>" )

outFile.write( "<h2>Domain decomposition</h2>" )
outFile.write( "<p>The image below is a snapshot of the domain decomposition. In 2d it " )
outFile.write( "illustrates the exact domain decomposition. For higher dimensions it shows "  )
outFile.write( "a 2d cut. </p>" )
outFile.write( "<img src=\"" + inputFilename + ".2d-dd.png\" />" )




outFile.write( "<h2>Master-worker statistics - late worker</h2>" )
outFile.write( "<p>If an edge points from a to b, it means that master b had to wait for its worker a. The labels are wait times in seconds. </p>" )
outFile.write( "<img src=\"" + inputFilename + ".worker-master.png\" />" )
outFile.write( "<p>The following graph holds only edges whose average is beyond the average of averages.</p>" )
outFile.write( "<img src=\"" + inputFilename + ".worker-master-sparse-average.png\" />" )
outFile.write( "<p>The following graph holds only edges whose maximum weight is with 10% of the total maximum weight.</p>" )
outFile.write( "<img src=\"" + inputFilename + ".worker-master-sparse-max.png\" />" )


outFile.write( "<p>\
Peano makes the global rank 0 become a pure administrative rank, i.e. it deploys all workload to another mpi \
rank and afterwards handles load balancing, global algorithm control, and so forth. As a consequence \
there is usually always one edge pointing towards rank 0. This edge does not illustrate a performance issue. \
</p>" )


outFile.write( "<p>\
Several researchers recommend to introduce pure administrative ranks in Peano, i.e. make some ranks deploy \
all their workload to other nodes and act as administrator only. Such ranks always should have incoming \
edges from all their children here. \
</p>" )


outFile.write( "<p>\
<i>Performance hint: </i>\
The edges here illustrate the critical communication path for one traversal, i.e. long graphs running from a node to rank 0 indicate \
that along these graphs the Peano traversal is serialised. Eliminate these edges by reducing the \
workload of the involved nodes. See remark in wiki on 'Optimise worker-master communication' or \
'Avoid reductions'. \
</p>" )




outFile.write( "<h2>Boundary exchange statistics - late senders</h2>" )

outFile.write( "<p>\
The follow diagrams display an edge whenever a node A had to wait for a neighbouring node B. \
The first entry of the label counters how often it had to work, the second the maximum data \
cardinality, i.e. vertices, and the last one the average wait cardinality. If A points to B, \
B hat to wait for data coming from A. The directed edges illustrate the data flow. As mentioned, \
all figures enlist data cardinalities. They are not wait times but number of records missing. \
</p>" )

outFile.write( "<p>In all diagrams, singular events, i.e. events waits only once or twice, are omitted.</p>" )

outFile.write( "<img src=\"" + inputFilename + ".boundary-exchange.png\" />" )
outFile.write( "<p>The graph below holds only those edges that have an average that is bigger than the average of averages.</p>" )
outFile.write( "<img src=\"" + inputFilename + ".boundary-exchange-sparse-average.png\" />" )
outFile.write( "<p>The last graph illustrates those late senders that have max values that fall into the upper 10% of max values.</p>" )
outFile.write( "<img src=\"" + inputFilename + ".boundary-exchange-sparse-max.png\" />" )


outFile.write( "<p>\
Peano is designed to exchange all boundary data in the background of the computation. \
In the ideal case, the above graph hence should be empty or very sparse. \
If it is pretty dense, your algorithm is bandwidth-bound. This is not a problem \
per se, if the bandwidth-bound nodes do not slow down their masters, i.e. \
the diagrams above have to be read in context with the worker-master graphs. \
</p>" )


outFile.write( "<p>\
<i>Performance hint: </i>\
The global rank 0 should not have adjacent edges. If it has, ensure you've followed the 'Avoid communications \
with rank 0' recipes from the wiki. \
</p>" )

outFile.write( "<p>\
<i>Performance hint: </i>\
If this graph is a clique, your mpi buffer sizes might be too small or too big. Cf. \
SendReceiveBufferPool's setBufferSize(). Its default values might be ill-suited for your \
application's memory footprint. \
</p>" )

outFile.write( "<p>\
<i>Performance hint: </i>\
If single nodes are the hot-spots making the others wait (many outgoing edges), those nodes might either have \
a significantly higher load than others (cf. balancing remark above) or they might have a significant higher surface to \
other nodes that has to be exchanged. Adopt load balancing and see 'Check the load balancing and node weights' \
in the wiki. \
</p>" )

outFile.write( "<p>\
<i>Performance hint: </i>\
If edges point from workers to their direct parents (cf. logical topology), the local master-worker balancing might be ill-balanced. Such \
edges are in the first place not that important - any node might have a severe workload where it can only deploy \
smaller parts of the work. The involved worker then has, by definition, to be smaller than its master (it got only \
a fragment of the total work) and then waits for the master's boundary data for the subsequent iteration. A \
critical situation (from a performance point of view) would occur, if the master had to wait for the worker's \
finished message. Cf. Master-worker statistics above. \
</p>" )

outFile.write( "<p>\
<i>Performance hint: </i>\
If nodes delay their masters but have no significant heavy edges in the boundary graph, study their individual runtime \
profile carefully. If these profiles also indicate that the data exchange is not signficiant, your worker-master \
data exchange suffers from worker-master latency. See section in wiki on 'Optimise worker-master communication'. \
</p>" )




outFile.write( "<h2>Runtime profiles</h2>" )

outFile.write( "<p> \
The diagram below gives a summative overview over all busy and idle times. \
It is once given with standard y-axis, and once with a logarithmically scaled \
axis. As the rank iteration numbers are not synchronised (each worker counts \
locally how many traversals have been done), the individual measurements can \
be translated to each other. All timings are once given as cpu time stemming \
from the clock tics, and once in calendar time. \
</p>" )


outFile.write( "<p> \
Each Peano traversal consists of the following phases: \
<ul> \
  <li> \
    <i>Busy time</i>: This is the time the node is actually traversing the tree, i.e. \
    the time between a start message received from its master and the time until the finished \
    message is sent back to the master. Busy times also comprise message exchanges, but these \
    message exchanges (boundary, load balancing, and so forth) run parallel to or are merged \
    into the grid traversal. If the workload is homogeneous (i.e. roughly the same number of \
    operations per spacetree node or at least leaf), the busy time usually directly correlates \
    to the number of cells, i.e. to the workload distribution. \
    The only fragment of the busy time that is illustrated separately is the time consumed by \
    the heap data exchange. \
  </li> \
  <li> \
    Definition <i>Work within domain</i>: Part of the busy time where the inner domain's bounding \
    box is traversed and not the embedding cells. Peano embeds all domains into a greater box to \
    simplify the boundary handling. This means, a traversal has to run through the actual domain \
    (work within domain time) and to run through boundary/shadow layers. Busy time comprises the \
    whole run while work within domain is only a subset. \
  </li> \
  <li> \
    Definition <i>Communication time</i>: As soon as a node has sent the information back to the master \
    that it has finished its traversal, it starts to clean up all the pending sends of boundary \
    data to the neighbours, receives the boundary data from all adjacent ranks, switches local \
    data structures, and so forth. It prepares all the data for the subsequent iteration. The \
    communication time comprises the \
    <ul> \
      <li> exchange of <i>join</i> data and the </li>  \
      <li> exchange of <i>boundary</i> vertices. </li> \
    </ul> \
  </li> \
  <li> \
    Definition <i>Idle time</i>: As soon as all data is communicated, the rank \
    waits for the next startup message from its master. This waiting \
    time is real idle time where nothing is done. \
  </li> \
</p>" )

outFile.write( "<h2 id=\"runtime-rank-overview\">Links to individual reports</h2>" )
for i in range(1,int(sys.argv[2])):
  outFile.write( "<a href=\"#runtime-rank-" + str(i) + "\">Profile of rank " + str(i) + "</a> - " )


outFile.write( "<h2>Overview</h2>" )
outFile.write( "<img src=\"" + inputFilename + ".runtimes.calendar.png\" />" )
outFile.write( "<img src=\"" + inputFilename + ".runtimes.cpu.png\" />" )
outFile.write( "<img src=\"" + inputFilename + ".runtimes.log.calendar.png\" />" )
outFile.write( "<img src=\"" + inputFilename + ".runtimes.log.cpu.png\" />" )
outFile.write( "<h2>Global master node</h2>" )
outFile.write( "<img src=\"" + inputFilename + ".runtimes.master.calendar.png\" />" )
outFile.write( "<img src=\"" + inputFilename + ".runtimes.master.cpu.png\" />" )
outFile.write( "<img src=\"" + inputFilename + ".runtimes.master.log.calendar.png\" />" )
outFile.write( "<img src=\"" + inputFilename + ".runtimes.master.log.cpu.png\" />" )

outFile.write( "<p>The diagrams below give a history of the times where each individual node spent " )
outFile.write( "its time. Nodes that remain idle throughout the computation are not enlisted.</p>" )

for i in range(1,int(sys.argv[2])):
  outFile.write( "<h2 id=\"runtime-rank-" + str(i) + "\">Rank " + str(i) + "</h2>" )
  outFile.write( "<img src=\"" + inputFilename + ".runtimes.rank-" + str(i) + ".calendar.png\" />" )
  outFile.write( "<img src=\"" + inputFilename + ".runtimes.rank-" + str(i) + ".cpu.png\" />" )
  outFile.write( "<img src=\"" + inputFilename + ".runtimes.log.rank-" + str(i) + ".calendar.png\" />" )
  outFile.write( "<img src=\"" + inputFilename + ".runtimes.log.rank-" + str(i) + ".cpu.png\" />" )
  outFile.write( "<br /><br /><a href=\"#runtime-rank-overview\">To overview</a>" )

outFile.write( "</body>" )
outFile.write( "</html>" )

