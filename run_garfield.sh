#!/bin/bash

folder=garfield_test
dir=$folder
w_v=-100
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

cd /lbne/u/xning/garfield_work/
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
	apptainer run garfield-sl7.sif garfield-9 -batch <${track_file}
done

mv ${dir}/0.000_U.dat ${dir}/0.0_U.dat
mv ${dir}/0.000_V.dat ${dir}/0.0_V.dat
mv ${dir}/0.000_Y.dat ${dir}/0.0_Y.dat
tar -czf ${folder}.tar.gz --exclude='*.gar' --exclude='*.ps' ${dir}


