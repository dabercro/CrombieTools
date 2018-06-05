#! /usr/bin/env python

import sys

sys.argv.append('-b')

from ROOT import TFile

if __name__ == '__main__':
    infile = TFile(sys.argv[1])
    inhist = infile.Get(sys.argv[2])
    varname = sys.argv[3]

    outstr = ''

    nbin = inhist.GetNbinsX()
    for bin in range(nbin):
        if bin + 1 == nbin:
            outstr += '+({var}>{down})*{val}'.format(
                var=varname,
                down=inhist.GetXaxis().GetBinLowEdge(bin + 1),
                val=inhist.GetBinContent(bin + 1)
                )
        elif bin:
            outstr += '+({var}>{down}&&{var}<{up})*{val}'.format(
                var=varname,
                up=inhist.GetXaxis().GetBinUpEdge(bin + 1),
                down=inhist.GetXaxis().GetBinLowEdge(bin + 1),
                val=inhist.GetBinContent(bin + 1)
                )
        else:
            outstr += '({var}<{up})*{val}'.format(
                var=varname,
                up=inhist.GetXaxis().GetBinUpEdge(bin + 1),
                val=inhist.GetBinContent(bin + 1)
                )

    print '(%s)' % outstr
