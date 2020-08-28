import ROOT as R 
import sys  
import numpy as np
from itertools import islice
from pprint import pprint

file = R.TFile(sys.argv[1], "READ")
tree = file.Get("tree")

selTow = 0 
selXtal = 20
debug = True

map_tp = {}
pulses = {}

shifts = []

# for i in range(0,24):
#     shifts.append(i)
for sign in [+2,-2]:
    for i in range(0,13):
        shifts.append(i*sign)
print(shifts)

debug_info = []

for iEv, ev in enumerate(islice(tree,0,30)):
    for iTow, (tower, tp, rawtp) in enumerate(zip( ev.xtalsDigis, ev.eRec, ev.rawTPData )):
        for iXtl, digis in enumerate(tower):
            iRef = digis[0]
            #if iRef > 25: continue
            #print(iRef)
            #

            if iTow==selTow and iXtl==selXtal:
                if debug: 
                    debug_info.append((iEv, str(iTow)+"-"+str(iXtl), shifts[iRef],bin(rawtp&0x1FFF), tp,
                                rawtp>>12 & 0x1, rawtp>>8 &0x1, list(digis)))
                   
                map_tp[iRef]=tp 
                pulses[iRef] = list(digis)

for event in sorted(debug_info, key=lambda k: k[2]):
    print "Event: {:>4}, Tower-xtal: {:>4}, Time shift: {:>3}, RawTP: {:>14}, "\
                          "TP Et: {:>4}, sFGVB: {}, infobit2: {}, digis: {}".format(
                                 *event)

# # pprint(map_tp)
# # pprint(pulses)



# c = R.TCanvas()
# mg = R.TMultiGraph()
# leg = R.TLegend(0.33,0.1,0.9,0.43)
# leg.SetNColumns(3)
# cache= []
# for ip, p in pulses.items():
#     g = R.TGraph(10, np.arange(0,250,25, dtype=float), np.array(p, dtype=float))
#     g.SetName("graph_"+str(ip))
#     mg.Add(g)
#     leg.AddEntry(g, "shift {}ns".format(shifts[ip]))
#     g.SetLineWidth(2)
#     cache.append(g)

# mg.SetTitle("Pulses;ns;ADC")
# mg.Draw("APL PLC PMC")
# leg.Draw("same")

# c.Draw()
# c.SaveAs("pulses_time_shifted.png")
# c.SaveAs("pulses_time_shifted.C")


c = R.TCanvas()
gg = R.TGraph(len(map_tp))
# for i in range(1,len(map_tp)+1):
#     mg.SetPoint(i, shifts[i], map_tp[i])
i = 0
for iS, tp in map_tp.items():
    #print(iS, tp)
    gg.SetPoint(i, shifts[iS], tp )
    i+=1

gg.SetMarkerStyle(8)
gg.SetTitle("Time shifted pulse analysis;time shift (ns);TP ET")
gg.Draw("AP")
c.Draw()


