import ROOT as R 
import sys

f = R.TFile(sys.argv[1], "Read")
f2 = R.TFile(sys.argv[2], "Read")


c = R.TCanvas()
mg = R.TMultiGraph() 

leg = R.TLegend(0.2,0.3,0.6,0.5)

g_nom = f.Get("nominal")
g_nom_peak = f.Get("nominal_peak")
g_minus = f.Get("minus")
g_plus = f2.Get("plusweights_peak")
#g_minus_peak = f.Get("minus_peak")

mg.Add(g_nom_peak)
mg.Add(g_nom)

leg.AddEntry(g_nom,"Nominal weights, no peak finder")
leg.AddEntry(g_nom_peak, "Nominal weights, peak finder ")
leg.AddEntry(g_minus, "Weights for -1 BX, no peak finder")
leg.AddEntry(g_plus, "Weights +12 ns, no peak finder")
# mg.Add(g_minus_peak)
mg.Add(g_minus)
mg.Add(g_plus)


g_nom_peak.SetMarkerColor(R.kGreen)
g_minus.SetMarkerColor(R.kBlue)
g_plus.SetMarkerColor(R.kRed)

mg.SetTitle("TP with pulse t0 scan;time shift (ns);TP ET")
mg.Draw("AP")
leg.Draw("same")
c.Draw()
