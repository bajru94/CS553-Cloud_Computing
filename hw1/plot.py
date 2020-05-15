from matplotlib 
import pyplot as set

generation_time = [0.38,41,4528]
sort_time = [0.008,0.008,1.37]

set.plot(generation_time,sort_time, linestyle='-', marker='.', color='b') 
set.xlabel('Generation time (s)')
set.ylabel('Sort time(s)')

set.savefig('Graph.png')
