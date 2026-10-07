#!/bin/bash
# Convert the PDHD APA1 (V_w = 0 V) Garfield tarball into a WCT field
# response with the MCMC-tuned outer W-plane paths.
#
# Usage: ./convert_garfield.sh [w1 w2 w3 w4 st] [input.tar.gz] [output.json.bz2]
#   default parameters = MCMC best fit: -0.04 2.10 1.00 2.50 650
#
# Needs the wire-cell-python with the -w/--weight option of convert-garfield
# (wire-cell-python-weight.patch).  Override the interpreter / package with
#   PYTHON=...  WCPY=...  ./convert_garfield.sh
set -e
here="$(dirname "$(readlink -f "$0")")"
WCPY=${WCPY:-/nfs/data/1/xning/wirecell-working/python}
PYTHON=${PYTHON:-/nfs/data/1/xning/wirecell-working/.direnv/python-3.11.9/bin/python}

if [ $# -ge 5 ]; then
    w1=$1 w2=$2 w3=$3 w4=$4 st=$5; shift 5
else
    w1=-0.04 w2=2.10 w3=1.00 w4=2.50 st=650
fi
input=${1:-$here/../1_garfield/dune_0_rightv.tar.gz}
output=${2:-garfield_scan_${w1}_${w2}_${w3}_${w4}_${st}.json.bz2}

PYTHONPATH=$WCPY${PYTHONPATH:+:$PYTHONPATH} $PYTHON -m wirecell.sigproc convert-garfield \
    -s "1.565*mm/us" -d 37 -n 0.49549801 \
    -w $w1 $w2 $w3 $w4 $st \
    "$input" "$output"
echo "wrote $output"
