#!/bin/bash
set -e

# Run from the directory containing this script so all relative paths resolve.
cd "$(dirname "$(readlink -f "$0")")"

# Path to the Garfield apptainer image; override with GARFIELD_SIF=... ./run_garfield.sh
GARFIELD_SIF=${GARFIELD_SIF:-/nfs/data/1/xning/garfield/garfield-sl7.sif}
if [ ! -f "$GARFIELD_SIF" ]; then
	echo "ERROR: Garfield container not found at $GARFIELD_SIF" >&2
	echo "Download it with: wget --no-check-certificate 'https://www.phy.bnl.gov/~bviren/garfield/garfield-sl7.sif'" >&2
	exit 1
fi

folder=garfield_test
dir=$folder
w_v=0
declare -a x_position=("0.00001" "0.0471" "0.0942" "0.1413" "0.1884" "0.2355")


	if [ ! -d "${dir}" ];then
	mkdir -p $dir
	else
	echo "Warning: ${dir} already exist"
	fi

track_file=${dir}/track_protodune.gar
gas_file=gas.gar
cell_file=${dir}/cell_protodune.gar
signal_file=${dir}/signal_protodune.gar

for xx in {0..5}
do
	#generate .gar file
	#echo ${x_position[$xx]}
	./generate_signal.sh ${x_position[$xx]} ${dir} >$signal_file
	./generate_cell.sh $w_v  >$cell_file
	echo "<$gas_file" >$track_file
	echo "<$cell_file" >>$track_file
	echo "<$signal_file" >>$track_file
	echo "&stop" >>$track_file
    #cat $signal_file
	#run garfield
#	echo /lbne/u/yichen/garfield-build/garfield-9 -batch \<$track_file
#	/lbne/u/yichen/garfield-build/garfield-9 -batch <${track_file}
	apptainer run "$GARFIELD_SIF" garfield-9 -batch <${track_file}
done

mv ${dir}/0.000_U.dat ${dir}/0.0_U.dat
mv ${dir}/0.000_V.dat ${dir}/0.0_V.dat
mv ${dir}/0.000_Y.dat ${dir}/0.0_Y.dat
tar -czf ${folder}.tar.gz --exclude='*.gar' --exclude='*.ps' ${dir}


