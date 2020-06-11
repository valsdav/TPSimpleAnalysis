import sys
import argparse 

parser = argparse.ArgumentParser()
parser.add_argument("-o", "--output", type=str, help="Output file")
parser.add_argument("-p", "--pedestal", type=int, help="Pedestal value", default=200)
parser.add_argument("-ms", "--mask-strips", type=int, nargs="+", help="Mask strips")
parser.add_argument("-a", "--amplitude", type=int, help="Single amplitude - standard pulse", required=False, default=-1)
parser.add_argument("-t0", type=int, help="Peak time", required=False, default=125.)
parser.add_argument("--alpha", type=int, help="Alpha parameter", required=False, default=1.16037)
parser.add_argument("--beta",  type=int, help="Beta parameter", required=False, default=41.059)
parser.add_argument("-al", "--amplitudes-list", nargs="+", type=int, help="Amplitudes list", required=False, default=[])
args = parser.parse_args()

import ROOT as R
R.gSystem.Load("libTPSimpleAnalysisTPDumper.so")

GeVtoADC = 25

pulseFunction = R.Pulse(1.0, args.t0, args.alpha, args.beta )

#pulse = [0.,0.,0.,0.,0.795696,0.999837,0.882055,0.674526,0.477181,0.3214,0.2092877,0.132943]
pulse = [pulseFunction.sample(i) for i in range(0, 10)]

amplitudes = []
pedestals = [args.pedestal]*10
pedestals_hex = [hex(p|0x1000)[2:] for p in pedestals]

if args.amplitude !=-1:
    for i in range(10):
        amplitudes.append(   hex( int(pulse[i]*args.amplitude*GeVtoADC + pedestals[i]) | 0x1000)[2:] )
elif args.amplitudes_list:
    for a in args.amplitudes_list:
        amplitudes.append( hex(a + pedestals[i] | 0x1000)[2:])

print(amplitudes)

output = []

for iEv in range(1,24):
    for iTower in range(1,6):
        for iStrip in range(1,6):
            if args.mask_strips and iStrip in args.mask_strips:
                for iXtal in range(1,6):
                    output.append("Event {} Id 1 fed 611 Tower {} strip {} xtal {} {}".format(iEv, iTower,iStrip,iXtal," ".join(pedestals_hex)))
            else:
                for iXtal in range(1,6):
                    output.append("Event {} Id 1 fed 611 Tower {} strip {} xtal {} {}".format(iEv, iTower,iStrip,iXtal," ".join(amplitudes)))

with open(args.output, "w") as of:
    of.write("\n".join(output))