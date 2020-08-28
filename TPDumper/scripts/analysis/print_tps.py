import ROOT as R 
import sys  
import numpy as np
from itertools import islice
from operator import itemgetter
from pprint import pprint

file = R.TFile(sys.argv[1], "READ")
tree = file.Get("tree")

debug=False

tps = []

for iEv, ev in enumerate(islice(tree,0,24)):
    for iTow, (tower, tp) in enumerate(zip( ev.xtalsDigis, ev.eRec )):
        tps.append(tp)
        for iXtl, digis in enumerate(tower):
            if debug: print "Event: ", iEv, "  TP: ", tp, " Tower: ", iTow," xtal: ", iXtl, "  digis: ", list(digis)
            

c = sorted([ (t, tps.count(t)) for t in set(tps)], key=itemgetter(1))
pprint(c)
