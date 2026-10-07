#!/bin/sh

source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunesw v09_91_03d00 -q e26:prof

export WIRECELL_PATH=/exp/dune/data/users/xning/proto-dune-hd/SignalProcessing/sigproc/wire-cell-cfg:$WIRECELL_PATH
export LD_LIBRARY_PATH=/exp/dune/data/users/xning/proto-dune-hd/SignalProcessing/sigproc/wire-cell-toolkit/install/lib64/:$LD_LIBRARY_PATH

w1=`printf "%.2f" $1`
w2=`printf "%.2f" $2`
w3=`printf "%.2f" $3`
w4=`printf "%.2f" $4`
st=`printf "%.0f" $5`
start=$(date +%s)

garfield_name=garfield_scan_${w1}_${w2}_${w3}_${w4}_${st}.json.bz2
cfgPATH=/exp/dune/data/users/xning/proto-dune-hd/SignalProcessing/sigproc/wire-cell-cfg/
filename=/exp/dune/data/users/xning/git2/wire-cell-python/data/dune_0_rightv.tar.gz


o_filename=${cfgPATH}${garfield_name}
echo $o_filename

if [ -f "$o_filename" ]; then
    echo "field response exist"
else
    echo "generate field response"

    cd /exp/dune/data/users/xning/git2/wire-cell-python/
    source ../venv/bin/activate

    #wirecell-sigproc convert-garfield -s "1.565*mm/us" -d 37 -n -1 -w ${w1} ${w2} ${w3} ${w4} ${st} ${filename} ${o_filename}
    wirecell-sigproc convert-garfield -s "1.565*mm/us" -d 37 -n 0.49549801 -w ${w1} ${w2} ${w3} ${w4} ${st} ${filename} ${o_filename}

    deactivate

fi

cd /exp/dune/data/users/xning/proto-dune-hd/SignalProcessing/sigproc/wire-cell-cfg/pgrapher/experiment/pdhd/
    sed '167s/.*/'\'${garfield_name}\'',/' params_base.jsonnet >params.jsonnet


wire-cell -l stdout wct-sim-check_evt0.jsonnet
wire-cell -l stdout wct-sim-check_evt1.jsonnet
wire-cell -l stdout wct-sim-check_evt2.jsonnet
wire-cell -l stdout wct-sim-check_evt3.jsonnet

data_dir=/exp/dune/data/users/xning/proto-dune-hd/SignalProcessing/data/

outfolder=a_${w1}_${w2}_${w3}_${w4}_${st}

folder_tmp=a_tmp

mkdir -p $data_dir$folder_tmp
echo $outfolder
echo $data_dir$folder_tmp

mv protodunehd-sim-check_evt0.root  $data_dir$folder_tmp
mv protodunehd-sim-check_evt1.root  $data_dir$folder_tmp
mv protodunehd-sim-check_evt2.root  $data_dir$folder_tmp
mv protodunehd-sim-check_evt3.root  $data_dir$folder_tmp

cd /exp/dune/data/users/xning/proto-dune-hd/SignalProcessing/script/
root -l -b -q data_sim_compare.cc\(\"${outfolder}.pdf\",\"${folder_tmp}\"\)


end=$(date +%s)

echo "Time taken: $((end - start)) seconds"

