import sys
import argparse 

parser = argparse.ArgumentParser()
parser.add_argument("-o", "--output", type=str, help="Output file")
parser.add_argument("-p", "--pedestal", type=int, help="Pedestal value", default=200)
parser.add_argument("-ms", "--mask-strips", type=int, nargs="+", help="Mask strips")
parser.add_argument("-a", "--amplitude", type=int, help="Single amplitude - standard pulse", required=False, default=-1)
parser.add_argument("-t0", type=float, help="Peak time", required=False, default=125.)
parser.add_argument("--alpha", type=float, help="Alpha parameter", required=False, default=1.16037)
parser.add_argument("--beta",  type=float, help="Beta parameter", required=False, default=41.059)
#parser.add_argument("-al", "--amplitudes-list", nargs="+", type=int, help="Amplitudes list", required=False, default=[])
args = parser.parse_args()

import ROOT as R
R.gSystem.Load("libTPSimpleAnalysisTPDumper.so")

pulseFunctions = []
for sign in [+2,-2]:
    for i in range(0,13):
        pulseFunctions.append(R.Pulse(1.0, args.t0 + sign*i , args.alpha, args.beta ))

# for i in range(0,24):
#     pulseFunctions.append(R.Pulse(1.0, args.t0 + i , args.alpha, args.beta ))

#pulse = [0.,0.,0.,0.,0.795696,0.999837,0.882055,0.674526,0.477181,0.3214,0.2092877,0.132943]
pulses = [[pF.sample(i) for i in range(0, 10)] for pF in pulseFunctions]
print(pulses)

amplitudes = []
pedestals = [args.pedestal]*10
pedestals_hex = [hex(p|0x1000)[2:] for p in pedestals]

GeVtoADC = 25

if args.amplitude !=-1:
    for pulse in pulses:
        amplitudes.append(  [ hex( int( pulse[i]*args.amplitude*GeVtoADC + pedestals[i]) | 0x1000)[2:]  for i in range(10)] )



print(amplitudes)

output = []

for iEv in range(1,24):
    event = amplitudes[iEv]
    # Change the first BX to associated the number of the event
    event[0] = hex( iEv | 0x1000 )[2:]

    for iTower in range(1,6):
        for iStrip in range(1,6):
            if args.mask_strips and iStrip in args.mask_strips:
                for iXtal in range(1,6):
                    output.append("Event {} Id 1 fed 611 Tower {} strip {} xtal {} {}".format(iEv, iTower,iStrip,iXtal," ".join(pedestals_hex)))
            else:
                for iXtal in range(1,6):
                    output.append("Event {} Id 1 fed 611 Tower {} strip {} xtal {} {}".format(iEv, iTower,iStrip,iXtal," ".join(event)))

with open(args.output, "w") as of:
    of.write("\n".join(output))