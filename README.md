# wirecell-field-sim

This repository is used to generate field response functions for signal processing in LArTPC experiments.

## Overview

The field response describes how drifting ionization electrons induce signals on wire planes in a TPC. This repository uses Garfield simulations to calculate these responses for various detector geometries and configurations. (will include pochoir later)

## Dependencies

### Garfield Simulation

Garfield, a toolkit for detailed simulation of particle detectors. Since Garfield can be difficult to compile, a container version is provided.

**To obtain the Garfield container:**
```bash
wget --no-check-certificate 'https://www.phy.bnl.gov/~bviren/garfield/garfield-sl7.sif'
```

**To run Garfield:**
```bash
apptainer run garfield-sl7.sif garfield-9
```

## Current Configuration

The repository currently contains an example configuration for the **ProtoDUNE-HD APA1** detector with a wire plane voltage of **V_w = 0V**.

Future updates will include configuration files for other liquid argon experiments.

## Usage

### Running the Simulation

1. Download the Garfield container:
```bash
wget --no-check-certificate 'https://www.phy.bnl.gov/~bviren/garfield/garfield-sl7.sif'
```

2. Execute the simulation:
```bash
./run_garfield.sh
```

This will generate field response data at multiple drift positions and package the results into `garfield_test.tar.gz`.

### Converting Output for Wire-Cell

The output tar file can be converted to the Wire-Cell format using [`wire-cell-python`](https://github.com/WireCell/wire-cell-python)::

```bash
wirecell-sigproc convert-garfield \
  -s "1.565*mm/us" \
  -d 37 \
  -n 0.49549801 \
  garfield_test.tar.gz \
  garfield_test.json.bz2
```

**Parameters:**
- `-s`: Drift velocity (1.565 mm/µs for LAr at 87.7 K)
- `-d`: patch number
- `-n`: Normalization factor

The resulting `garfield_test.json.bz2` file contains the field response in a format suitable for Wire-Cell signal processing.
