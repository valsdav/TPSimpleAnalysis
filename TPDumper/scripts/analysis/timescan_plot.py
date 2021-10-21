import ROOT as R 
import sys

f = R.TFile(sys.argv[1], "Read")

c = R.TCanvas()
mg = R.TMultiGraph() 

leg = R.TLegend(0.2,0.3,0.6,0.5)

g_nom = f.Get("nominal")
g_other = f.Get("plusweights")
#g_other_peak = f.Get("plusweights_peak")
#g_minus_peak = f.Get("minus_peak")


mg.Add(g_nom       )
mg.Add(g_other     )
leg.AddEntry(g_nom,        "Nominal weights, no peak finder")
leg.AddEntry(g_other,      "+12ns weight, no peak finder"   )
#leg.AddEntry(g_other_peak, "+12ns weight, peak finder"      )

#g_nom.SetMarkerColor(R.kRed)
g_other.SetMarkerColor(R.kRed)
#g_other_peak.SetMarkerColor(R.kBlue)

mg.SetTitle("TP with pulse t0 scan;time shift (ns);TP ET")
mg.Draw("AP")
leg.Draw("same")
c.Draw()
