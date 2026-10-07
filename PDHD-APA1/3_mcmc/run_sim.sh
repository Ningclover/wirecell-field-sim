#!/bin/sh

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunesw v09_91_03d00 -q e26:prof

export WIRECELL_PATH=/exp/dune/data/users/xning/proto-dune-hd/SignalProcessing/sigproc/wire-cell-cfg:$WIRECELL_PATH
export LD_LIBRARY_PATH=/exp/dune/data/users/xning/proto-dune-hd/SignalProcessing/sigproc/wire-cell-toolkit/install/lib64/:$LD_LIBRARY_PATH

declare -a w1=("-0.01" "-0.03")
declare -a w2=("2.00" "2.50" "3.00")
declare -a w3=("1.00")
declare -a w4=("1.75" "2.00" "2.25" "2.50" "3.00")
declare -a st=("600" "550" "600")


start=$(date +%s)

for i in {0..1}
do
    for j in {0..0}
    do
        for k in {0..0}
        do
        for l in {0..0}
        do
        for m in {0..0}
        do
cd /exp/dune/data/users/xning/proto-dune-hd/SignalProcessing/sigproc/wire-cell-cfg/pgrapher/experiment/pdhd/
    #sed '167s/.*/'\''garfield_scan_2_'${w1[$i]}'_'${w2[$j]}'_'${w3[$k]}'_'${w4[$l]}'_'${st[$m]}'.json.bz2'\'',/' params_base.jsonnet >params.jsonnet
    sed '167s/.*/'\''garfield_scan_'${w1[$i]}'_'${w2[$j]}'_'${w3[$k]}'_'${w4[$l]}'_'${st[$m]}'.json.bz2'\'',/' params_base.jsonnet >params.jsonnet
    #sed '167s/.*/'\''garfield_scan_'${w1[$i]}'_'${w2[$j]}'.json.bz2'\'',/' params_base.jsonnet >params.jsonnet


wire-cell -l stdout wct-sim-check_evt0.jsonnet
wire-cell -l stdout wct-sim-check_evt1.jsonnet
wire-cell -l stdout wct-sim-check_evt2.jsonnet
wire-cell -l stdout wct-sim-check_evt3.jsonnet

data_dir=/exp/dune/data/users/xning/proto-dune-hd/SignalProcessing/data/

outfolder=a_${w1[$i]}_${w2[$j]}_${w3[$k]}_${w4[$l]}_${st[$m]}
#outfolder=b_${w1[$i]}_${w2[$j]}_${w3[$k]}_${w4[$l]}_${st[$m]}

mkdir $data_dir$outfolder
echo $outfolder

mv protodunehd-sim-check_evt0.root  $data_dir$outfolder
mv protodunehd-sim-check_evt1.root  $data_dir$outfolder
mv protodunehd-sim-check_evt2.root  $data_dir$outfolder
mv protodunehd-sim-check_evt3.root  $data_dir$outfolder

cd /exp/dune/data/users/xning/proto-dune-hd/SignalProcessing/script/
root -l -b -q data_sim_compare.cc\(\"${outfolder}.pdf\",\"$outfolder\"\)

done
done
done
done
done

end=$(date +%s)

echo "Time taken: $((end - start)) seconds"

