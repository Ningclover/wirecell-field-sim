#!/bin/bash
# Run inside the container with dunesw setup
# Usage: source run_all.sh

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunesw v09_91_03d00 -q e26:prof

BASEDIR=/exp/dune/data/users/xning/proto-dune-hd/SignalProcessing
OUTDIR=$BASEDIR/noCNR_recovery
cd $BASEDIR

# Step 1: Extract raw_noCNR histograms from artroot files
# File 1831 contains event 439442
echo "=== Step 1a: Extracting from 1831 (event 439442) ==="
root -l -b -q "${OUTDIR}/extract_noCNR.C(\"run28548/np04hd_raw_run028548_1831_dataflow2_datawriter_0_20240804T191136_reco_stage1.root\",\"${OUTDIR}\",\"439442\")"

# File 1903 contains events 456790, 456798
echo "=== Step 1b: Extracting from 1903 (events 456790, 456798) ==="
root -l -b -q "${OUTDIR}/extract_noCNR.C(\"run28548/np04hd_raw_run028548_1903_dataflow6_datawriter_0_20240804T200716_reco_stage1.root\",\"${OUTDIR}\",\"456790,456798\")"

# Step 2: Create tr_noCNR files and plots
echo "=== Step 2: Creating tr_noCNR files and plots ==="
root -l -b -q "${OUTDIR}/make_tr_noCNR.C(\"${OUTDIR}\")"

echo "=== Done ==="
ls -lh ${OUTDIR}/raw_noCNR_*.root ${OUTDIR}/tr_noCNR_*.root ${OUTDIR}/*.pdf 2>/dev/null
