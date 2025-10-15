#!/bin/sh

out_folder=$2
x_position=$1
xx=$(echo "$x_position * 10" | bc)
tag=`printf "%.3f" ${xx}`
filenameY=${out_folder}/${tag}_Y.dat
filenameV=${out_folder}/${tag}_V.dat
filenameU=${out_folder}/${tag}_U.dat
filename_plot=${out_folder}/${tag}_track.ps


echo	"&SIGNAL"
echo	"opt nocluster-print nocluster-plot						"
echo	"avalanche fixed 1                                                              "
echo	"int-par int-acc 1e-9 max-step 0.1                                              "
echo    "                                                                          "
echo	"* cm                                                                           "
echo	"area -3. -.2 3. 10.2                                                           "
echo	"select d                                                                       "
echo	"                                                                               "
echo	"*for 0 on the center, use 0.0001 instead                                       "
echo	"track ${x_position} 10 ${x_position} 10 fixed-number lines 1                   "
echo	"                                                                               "
echo	"*window set from 0 to xxx milisecond MXLIST is 1000 determined at compilation  "
echo	"window 0. 0.1 1000                                                             "
echo	"signal noavalanche diffusion noatta noion-tail electron-pulse new              "
echo	"                                                                               "
echo	"*plot-signals time-window 0. 100.  cross-induced-signals                       "
echo	"                                                                               "
echo	"write-signal dataset \"${filenameY}\" format spice                               "
echo	"                                                                               "
echo	"select l                                                                       "
echo	"signal noavalanche diffusion noatta noion-tail electron-pulse new              "
echo	"                                                                               "
echo	"*plot-signals time-window 0. 100.  cross-induced-signals                       "
echo	"                                                                               "
echo	"write-signal dataset \"${filenameV}\" format spice                               "
echo	"                                                                               "
echo	"                                                                               "
echo	"select s                                                                       "
echo	"signal noavalanche diffusion noatta noion-tail electron-pulse new              "
echo	"                                                                               "
echo	"*plot-signals time-window 0. 100.  cross-induced-signals                       "
echo	"                                                                               "
echo	"write-signal dataset \"${filenameU}\" format spice                               "
echo	"                                                                               "
echo	"*select k                                                                      "
echo	"*signal noavalanche nodiffusion noatta noion-tail electron-pulse new           "
echo	"                                                                               "
echo	"*plot-signals time-window 0. 100.  cross-induced-signals                       "
echo	"                                                                               "
echo	"*write-signal dataset "./testzone/trans_shift_26_G.dat" format spice           "
echo	"                                                                               "
echo	"!add meta type PostScript file-name \"${filename_plot}\"                         "
echo	"!open meta                                                                     "
echo	"!act meta                                                                      "
echo	"                                                                               "
echo	"&drift                                                                         "
echo	"drift track                                                                    "
echo	"                                                                               "
echo	"!deact meta                                                                    "
echo	"!close meta                                                                    "
echo	"!del meta                                                                      "
echo	"                                                                               "
echo	"&stop                                                                          "
	
