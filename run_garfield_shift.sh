#!/bin/sh

folder=dunem30-garfield-6paths-shift
dir=/lbne/u/xning/garfield_work/$folder
w_v=-30
#declare -a x_position=("0.205" "0.21" "0.215" "0.22" "0.225" "0.23")
declare -a x_position=("0.00001" "0.0471" "0.0942" "0.1413" "0.1884" "0.2355")

	if [ ! -d "${dir}" ];then
	mkdir -p $dir
	else
	echo "Warning: ${dir} already exist"
	fi

track_file=${dir}/track_protodune.gar
gas_file=/lbne/u/xning/garfield_work/gas.gar
cell_file=${dir}/cell_protodune_shift.gar
signal_file=${dir}/signal_protodune.gar

cd /lbne/u/xning/garfield_work/
for xx in {0..5}
do
	#generate .gar file
	#echo ${x_position[$xx]}
	./generate_signal.sh ${x_position[$xx]} ${dir} >$signal_file
	./generate_cell_shift.sh $w_v  >$cell_file
	echo "<$gas_file" >$track_file
	echo "<$cell_file" >>$track_file
	echo "<$signal_file" >>$track_file
	echo "&stop" >>$track_file

	#run garfield
	echo /lbne/u/yichen/garfield-build/garfield-9 -batch \<$track_file
	/lbne/u/yichen/garfield-build/garfield-9 -batch <${track_file}

done

mv ${dir}/0.000_U.dat ${dir}/0.0_U.dat
mv ${dir}/0.000_V.dat ${dir}/0.0_V.dat
mv ${dir}/0.000_Y.dat ${dir}/0.0_Y.dat
#mv ${dir}/2.300_U.dat ${dir}/2.355_U.dat
#mv ${dir}/2.300_V.dat ${dir}/2.355_V.dat
#mv ${dir}/2.300_Y.dat ${dir}/2.355_Y.dat
tar -czf ${folder}.tar.gz --exclude='*.gar' --exclude='*.ps' ${dir}


