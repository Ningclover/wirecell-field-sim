# ProtoDUNE-HD APA1 field response (W plane at 0 V, MCMC-tuned)

This folder holds everything needed to rebuild
`np04hd-garfield-6paths-mcmc-bestfit.json.bz2`: the field response used
for ProtoDUNE-HD **APA1** (WCT anode 0). On this APA the collection (W)
wire plane was simulated with **V_w = 0 V**. With W at the same
potential as V, electrons are collected on V, and W sees only small
bipolar induction signals. This matches what data show on this APA
(run 28548).

There are three steps:

1. **Garfield**: drift electrons along 6 paths per half pitch with
   V_w = 0 V. This gives `dune_0_rightv.tar.gz`.
2. **Tune** the two outermost W-plane paths (impact 1.884 mm and
   2.355 mm). Each gets its own amplitude scale and tail time-stretch,
   both starting at a common sample. This is the `-w` option of
   `wirecell-sigproc convert-garfield`.
3. **MCMC fit**: fit those 5 parameters to noise-filtered, no-CNR
   data from run 28548, comparing WCT simulation to data.

## Final result: best-fit parameters

| w1 | w2 | w3 | w4 | st |
|---|---|---|---|---|
| **-0.04** | **2.10** | **1.00** | **2.50** | **650** |

```bash
wirecell-sigproc convert-garfield -s "1.565*mm/us" -d 37 -n 0.49549801 \
    -w -0.04 2.10 1.00 2.50 650 dune_0_rightv.tar.gz np04hd-garfield-6paths-mcmc-bestfit.json.bz2
```

These values are final.

```
1_garfield/    dune_0_rightv.tar.gz          Garfield output (input to step 2)
2_convert/     convert_garfield.sh           step 2: Garfield tarball -> WCT FR (best fit by default)
               compare_fr.py                 compare two FR files path by path
               avg_fr_json2root.py           FR json.bz2 -> ROOT TH2F, for looking at it
               wire-cell-python-weight.patch adds the -w / --norm-plane options to wire-cell-python
               garfield.py.fnal-orig         original FNAL fork of wirecell/sigproc/garfield.py (provenance)
3_mcmc/        mcmc.py, run_sim_formcmc_dev.sh, data_sim_compare.cc, ...   step 3 (see Portability below)
               cfg/                          WCT jsonnet for the per-event track simulation
               data_extraction/              how the data files were made from art-root (provenance only)
data/          raw_noCNR_{439442,456790,456798}.root   2D noCNR waveforms of the 3 events (not in the repo, ~63 MB each)
               tr_noCNR_<track>.root         1D data waveforms + power spectra for the 8 tracks (fit input)
               noCNR_tracks.pdf              plots of those 8 tracks
field_response/np04hd-garfield-6paths-mcmc-bestfit.json.bz2   final result (same as toolkit/cfg)
```

## Quick reproduction

Needs a wire-cell-python with `wire-cell-python-weight.patch` applied.
Set `WCPY` (path to that wire-cell-python) and `PYTHON` (an interpreter
with numpy and click) if the defaults in the script do not apply.

```bash
cd /some/workdir
WCPY=/path/to/wire-cell-python PYTHON=python3 \
    <PDHD-APA1>/2_convert/convert_garfield.sh
#  -> garfield_scan_-0.04_2.10_1.00_2.50_650.json.bz2
python3 <PDHD-APA1>/2_convert/compare_fr.py \
    garfield_scan_-0.04_2.10_1.00_2.50_650.json.bz2 \
    <PDHD-APA1>/field_response/np04hd-garfield-6paths-mcmc-bestfit.json.bz2
#  metadata same
#  378 paths, 0 differ, worst relative difference 4.34e-17
```

The rebuilt file matches the toolkit file to floating-point rounding.
After decompression, 66 numbers differ in their last digit, and the
`.bz2` bytes differ.

---

## Step 1: Garfield, W plane at 0 V

The scripts are in the parent directory (`wirecell-field-sim/`):
`run_garfield.sh`, `generate_cell.sh`, `generate_signal.sh`,
`gas.gar` and `gas_file/table_87.7k.txt`.

```bash
cd wirecell-field-sim
# container: wget --no-check-certificate https://www.phy.bnl.gov/~bviren/garfield/garfield-sl7.sif
GARFIELD_SIF=/path/to/garfield-sl7.sif ./run_garfield.sh
#  -> garfield_test/ and garfield_test.tar.gz
```

Cell (`generate_cell.sh`, run with `w_v=0`), coordinates in cm:

| Plane | Position y | Bias | Garfield rows | Output file |
|---|---|---|---|---|
| cathode-side plane | 20.4 | -10151 V | p1 | |
| G (grid) | 1.413 | -665 V | g1 k g2 | |
| U | 0.942 | -370 V | u1 **s1** u2 | `*_U.dat` |
| V | 0.471 | 0 V | v1 **l1** v2 | `*_V.dat` |
| W (Y) | 0.0 | **0 V** (nominal +820 V) | y1 **d1** y2 | `*_Y.dat` |
| ground plane | -0.471 | 0 V | p2 | |

- Pitch is 4.71 mm. Each plane has 21 readout wires (rows d1/l1/s1)
  plus 40 dummy wires on each side.
- Liquid argon at 87.7 K, using the drift-velocity table from the
  global fit.
- One electron per path starts at y = 10 cm. Impact positions are
  x = 0, 0.471, 0.942, 1.413, 1.884 and 2.355 mm, i.e. 6 paths per
  half pitch.
- The window is 0-100 µs with 1000 samples (0.1 µs per sample).
- Diffusion is on, attachment and avalanche are off.

**Check.** The `.dat` files from a fresh run (`garfield_test.tar.gz`)
match `1_garfield/dune_0_rightv.tar.gz` exactly, except for the
`% Created` timestamp lines. Only the folder name inside the tarball
differs, and `convert-garfield` ignores it. Either tarball can be used
as the step 2 input.

## Step 2: Convert and tune the outer W paths (`-w`)

Upstream `convert-garfield` has no `-w` option. The patch
`2_convert/wire-cell-python-weight.patch` (to
`wirecell/sigproc/garfield.py` and `wirecell/sigproc/__main__.py`)
adds:

- `-w w1 w2 w3 w4 st`: applied **only to W-plane paths** at impact
  1.884 mm and 2.355 mm. It changes the current those two paths
  induce on all 21 W wires (42 paths in total). All other paths are
  untouched.

  | Parameter | Meaning | Best fit |
  |---|---|---|
  | w1 | amplitude scale, impact 1.884 mm | -0.04 |
  | w2 | amplitude scale, impact 2.355 mm | 2.10 |
  | w3 | tail time-stretch, impact 1.884 mm | 1.00 (no stretch) |
  | w4 | tail time-stretch, impact 2.355 mm | 2.50 |
  | st | sample where the stretch starts (Garfield sample, 0.1 µs, before the `-d` delay) | 650 (65 µs) |

  `scaled_current()` keeps times `t[:st]`. It maps
  `t[st:] -> t[st] + (t - t[st])*stretch` and multiplies the current
  by the amplitude. It then re-interpolates onto the original grid,
  with the last value held constant beyond the end.

- `--norm-plane {u,v,w}` (default `w`, same as upstream). This picks
  the region-0 paths used as the charge reference when `-n < 0`. The
  original version of this patch hard-coded `v`, because V collects on
  this APA. It has no
  effect for `-n > 0`, so it does not change the best-fit file. Use
  `--norm-plane v -n -<electrons>` if you want charge normalization
  for this geometry.

Without `-w`, the patched code gives byte-identical output to
upstream.

Other convert options:

- `-s 1.565*mm/us`: nominal drift speed.
- `-d 37`: delays every response by 37 samples (3.7 µs).
- `-n 0.49549801`: a plain multiplicative scale. It is within 0.03%
  of the ratio of the W-plane central-wire charge in the earlier APA1
  response `np04hd-garfield-6paths.json.bz2` to that in raw
  `dune_0_rightv` (0.4954). In other words, it gives this response the
  same overall scale as that earlier file.

The convert input is the Garfield tarball `dune_0_rightv.tar.gz`, not
`np04hd-garfield-6paths.json.bz2`. That file is an earlier,
already-converted APA1 response from a different Garfield run. In it
V also collects and W does not, but it differs from the untuned
conversion of `dune_0_rightv` on every path. The nominal-bias PDHD
response, where W collects, is `dune-garfield-1d565.json.bz2`.

## Step 3: MCMC fit to data

**Data.** Run 28548, events 439442, 456790 and 456798. The data were
reconstructed with `wclsdatahdfilter`, i.e. noise filter with no
coherent-noise removal (`data_extraction/standard_reco_stage1_protodunehd_keepup.fcl`).
Eight tracks on APA1 are used (0, 10, 1, 2, 3, 4, 5, 6). For each
track, `make_tr_noCNR.C` aligns the W waveforms using the track slope
and the V waveforms by their peaks, and averages them. It reads `data/raw_noCNR_<event>.root` (2D
histograms `raw_wf_ANF_1_{u,v,w}`, not included in the repository
because of their size) and stores the
average waveform (`w_wf`, `v_wf`) and power spectrum (`w_pd`, `v_pd`)
in `data/tr_noCNR_<track>.root`.

| Track | Event | Type | W channels | W ticks | W nticks | V channels | V ticks | V nticks | Sim config | Charge scale |
|---|---|---|---|---|---|---|---|---|---|---|
| 0  | 456798 | beam   | 2107-2261 | 4072-3891 | 180 | 830-983   | 3735-3991 | 180 | evt0 | -500 × 1.07 |
| 10 | 439442 | beam   | 2082-2449 | 4267-4007 | 150 | 1075-1202 | 4257-4050 | 150 | evt1 | -500 × 1.24 |
| 1  | 439442 | cosmic | 2112-2334 | 3850-2850 | 300 | 823-1129  | 3320-3940 | 200 | evt1 | -500 × 1.28 |
| 2  | 456790 |        | 2237-2310 | 4006-3828 | 200 | 822-919   | 4083-3825 | 150 | evt2 | -500 × 1.30 |
| 3  | 456790 |        | 2453-2520 | 2964-1967 | 300 | 820-1009  | 2238-2612 | 200 | evt2 | -500 × 1.18 |
| 4  | 456798 |        | 2115-2300 | 2330-2675 | 200 | 1030-1261 | 2335-2697 | 200 | evt3 | -500 × 1.62 |
| 5  | 456798 |        | 2388-2551 | 1834-2777 | 200 | 854-1030  | 1714-2729 | 200 | evt3 | -500 × 1.20 |
| 6  | 456798 |        | 2288-2370 | 5095-3872 | 250 | 931-992   | 5168-4184 | 200 | evt3 | -500 × 1.87 |

These are the data windows from `make_tr_noCNR.C`. The simulation
windows for tracks 2 and 3 in `data_sim_compare.cc` (`par_w_all`,
`par_v_all`) differ slightly from these.

**One iteration** (`run_sim_formcmc_dev.sh w1 w2 w3 w4 st`):

1. `convert-garfield ... -w w1 w2 w3 w4 st` writes
   `garfield_scan_<w1>_<w2>_<w3>_<w4>_<st>.json.bz2`. It is cached
   and skipped if it already exists.
2. `sed` replaces line 167 of `cfg/params_base.jsonnet` (the anode-0
   entry of `fields`, `'bz2file'`) to produce `params.jsonnet`. Anodes
   1-3 keep `dune-garfield-1d565.json.bz2`.
3. `wire-cell wct-sim-check_evt{0..3}.jsonnet` simulates straight
   tracks matched to the data tracks. Settings: DL 6.2 cm²/s,
   DT 16.3 cm²/s, lifetime 50 ms, 7.8 mV/fC gain, 6000 ticks, with
   the per-track charge scales in the table above. Outputs are 2D
   histograms `hw_raw0` and `hv_raw0`.
4. `root data_sim_compare.cc` extracts the same track windows from
   the simulation. It normalizes charge with V, then for each track
   computes `chi2 = Σ (sim-data)²/|data| / nbins` for the W waveform
   and the W power spectrum. It writes
   `chi2_total = Π_tracks chi2_wf_w * chi2_pd_w` to `chi2_output.txt`.
   Only the **W plane** enters, and only the **first 7** tracks (loop
   `i<7`, so track 6 is left out).

**MCMC** (`mcmc.py`): Metropolis-Hastings with fixed ±step proposals
and 50 iterations, starting at `[-0.04, 2.3, 1.2, 2.5, 640]`.

| Parameter | Step | Bounds | Best |
|---|---|---|---|
| w1 | 0.01 | [-0.05, 0.00] | -0.04 |
| w2 | 0.10 | [1.90, 2.50] | 2.10 |
| w3 | 0.10 | [1.00, 1.20] | 1.00 (lower bound) |
| w4 | 0.10 | [2.30, 2.50] | 2.50 (upper bound) |
| st | 10 | [600, 650] | 650 (upper bound) |

Earlier, coarser stages were `run_sim.sh` (grid scan) and
`run_sim_formcmc.sh` (used `-n -1`). The bounds were narrowed between
stages. Because chi2_total is a product of 14 normalized chi2 terms,
the acceptance `exp(-Δχ²/2)` is not a calibrated likelihood. Treat
the chain as a stochastic minimizer, not as posterior uncertainties.
Three of the five best-fit values sit on the bounds.

**Portability.** The files in `3_mcmc/` are the scripts as used for the
original fit at FNAL. They hard-code `/exp/dune/data/users/xning/...`,
`dunesw v09_91_03d00` and a wire-cell-python venv. To run them
elsewhere you need to:
- point `filename` at `1_garfield/dune_0_rightv.tar.gz`, and replace
  the venv activation with `2_convert/convert_garfield.sh`;
- put `cfg/*.jsonnet` where `pgrapher/experiment/pdhd/` is found on
  `WIRECELL_PATH`, or adjust the imports;
- provide the data under the names `data_sim_compare.cc` expects
  (`../data/tr_no_cnr_<n>.root` relative to `3_mcmc/`), e.g. as
  symlinks to `PDHD-APA1/data/tr_noCNR_<n>.root`;
- fix `cfg/wct-sim-check_evt0.jsonnet`, which was left in an SP/debug
  state with no raw output, so it will not write `hw_raw0`/`hv_raw0`
  as it is. evt1-3 are fine.
