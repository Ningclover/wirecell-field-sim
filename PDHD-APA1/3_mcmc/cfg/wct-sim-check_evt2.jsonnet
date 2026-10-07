// usage: wire-cell -l stdout wct-sim-check.jsonnet
/*

APA0
wire dir: (0 0.812013 0.58364) pitch dir: (0 -0.58364 0.812013)
wire dir: (0 0.812013 -0.58364) pitch dir: (0 0.58364 0.812013)
wire dir: (0 1 0) pitch dir: (0 0 1)

APA1
- wire dir: (0 0.812013 -0.58364) pitch dir: (0 0.58364 0.812013)
- wire dir: (0 0.812013 0.58364) pitch dir: (0 -0.58364 0.812013)
- wire dir: (0 1 0) pitch dir: (0 0 1)

*/


local g = import 'pgraph.jsonnet';
local f = import 'pgrapher/common/funcs.jsonnet';
local wc = import 'wirecell.jsonnet';

local io = import 'pgrapher/common/fileio.jsonnet';
local tools_maker = import 'pgrapher/common/tools.jsonnet';
local base = import 'pgrapher/experiment/pdhd/simparams.jsonnet';
local params = base {
  lar: super.lar {
    // Longitudinal diffusion constant
    DL: 6.2 * wc.cm2 / wc.s,
    // Transverse diffusion constant
    DT: 16.3 * wc.cm2 / wc.s,
    lifetime: 50 * wc.ms,
    drift_speed: 1.565 * wc.mm / wc.us,
  },
};


local tools = tools_maker(params);

local sim_maker = import 'pgrapher/experiment/pdhd/sim.jsonnet';
local sim = sim_maker(params, tools);

local beam_dir = [-0.178177, -0.196387, 0.959408];
local beam_center = [-27.173, 421.445, 0];


local track0 = {  //beam
  // tail: wc.point(-260,300, 50,wc.cm),
  // head: wc.point(-260,300,200,wc.cm),
  //tail: wc.point(260, 300, 50, wc.cm),  //APA2
  //head: wc.point(260, 300, 200, wc.cm),
  // tail: wc.point(260, 300, 50, wc.cm),  //APA2 u
  // head: wc.point(260, 300+(0.58364*150), 50+(0.812013*150), wc.cm),
  // tail: wc.point(-260, 300, 50, wc.cm),  //APA1 u
  // head: wc.point(-260, 300+(-0.58364*200), 50+(0.812013*200), wc.cm),
  // tail: wc.point(-260, 300, 50, wc.cm),  //APA1 v
  // head: wc.point(-260, 300+(0.58364*150), 50+(0.812013*150), wc.cm),
  // tail: wc.point(260, 300, 50, wc.cm),  //APA2 v
  // head: wc.point(260, 300-(0.58364*150), 50+(0.812013*150), wc.cm),
  head: wc.point(beam_center[0], beam_center[1], beam_center[2], wc.cm),
  tail: wc.point(beam_center[0] + 1000*beam_dir[0], beam_center[1] + 1000*beam_dir[1], beam_center[2] + 1000*beam_dir[2], wc.cm),
  // head: wc.point(-52.4, 347.8, 16.7, wc.cm),
  // tail: wc.point(-70.4, 14.4, 182.4, wc.cm),
};

local track1 = {   
  head: wc.point(-52.4, 347.8, 16.7, wc.cm),
  tail: wc.point(-70.4, 14.4, 182.4, wc.cm),
};


local track_2= {
head: wc.point(-80.6, 390.7, 95.1, wc.cm),
tail: wc.point(-106.9, 186.9, 150.0, wc.cm),
};

local track_3= {
head: wc.point(-277.5, 589.7, 228.0, wc.cm),
tail: wc.point(-153.3, 19.3, 178.9, wc.cm),
};

local track_4= {
head: wc.point(-200.1, 354.3, 27.0, wc.cm),
tail: wc.point(-172.4, 23.6, 127.1, wc.cm),
};

local track_5= {
head: wc.point(-254.1, 368.4, 138.2, wc.cm),
tail: wc.point(-166.8, 91.3, 228.2, wc.cm),
};

// local track_6= {
// head: wc.point(-35.2, 294.0, 121.8, wc.cm),
// tail: wc.point(-104.0, 213.2, 150.0, wc.cm),
// };
local dir_6= [0.626561,0.735844,-0.256817];
// local center_6 = [-35.2,294,121.8];
local center_6 = [0,335.34,107.372];
local track_6= {
head: wc.point(center_6[0], center_6[1], center_6[2], wc.cm),
tail: wc.point(center_6[0] - 150*dir_6[0], center_6[1] - 150*dir_6[1], center_6[2] - 150*dir_6[2], wc.cm),
};

local track_7= {
head: wc.point(-147.3, 576.7, 183.8, wc.cm),
tail: wc.point(-239.9, 383.6, 122.0, wc.cm),
};

local track_8= {
head: wc.point(-375.2, 594.6, 48.6, wc.cm),
tail: wc.point(-339.7, 491.9, 34.0, wc.cm),
};

local track_9= {
head: wc.point(-51.7, 527.2, 164.8, wc.cm),
tail: wc.point(1.2-10, 418.3, 108.3, wc.cm),
};

local track_10= {
head: wc.point(-155.0, 577.2, 118.0, wc.cm),
tail: wc.point(-72.5, 322.8, 9.4, wc.cm),
};

local tracklist = [

  // {
  //   time: (-250-17) * wc.us,
  //   charge: -5000,  // negative means # electrons per step (see below configuration)
  //   ray: track0,  // params.det.bounds,
  // },

  // {
  //   time: (-250+199.49) * wc.us,
  //   charge: -5000,  // negative means # electrons per step (see below configuration)
  //   ray: track1,  // params.det.bounds,
  // },

  // {
  //   time: (-250+207.36) * wc.us,
  //   charge: -5000,  // negative means # electrons per step (see below configuration)
  //   ray: track_10,  // params.det.bounds,
  // },

    {
    //time: 0 * wc.us,
    time: (-250+219.52) * wc.us,
    charge: -500*1.30,  // negative means # electrons per step (see below configuration)
    ray: track_2,  // params.det.bounds,
  },
      {
    //time: 0 * wc.us,
    time: (-250+296.485) * wc.us,
    charge: -500*1.18,  // negative means # electrons per step (see below configuration)
    ray: track_3,  // params.det.bounds,
  },
  //     {
  //   //time: 0 * wc.us,
  //   time: (-250+222.485) * wc.us,
  //   charge: -5000,  // negative means # electrons per step (see below configuration)
  //   ray: track_4,  // params.det.bounds,
  // },
  //     {
  //   //time: 0 * wc.us,
  //   time: (-250+221.899) * wc.us,
  //   charge: -5000,  // negative means # electrons per step (see below configuration)
  //   ray: track_5,  // params.det.bounds,
  // },
  //     {
  //   //time: 0 * wc.us,
  //   time: (-250+208.255) * wc.us,
  //   charge: -5000,  // negative means # electrons per step (see below configuration)
  //   ray: track_6,  // params.det.bounds,
  // },
  //     {
  //   //time: 0 * wc.us,
  //   time: -250 * wc.us,
  //   charge: -500,  // negative means # electrons per step (see below configuration)
  //   ray: track_7,  // params.det.bounds,
  // },
  //     {
  //   //time: 0 * wc.us,
  //   time: -250 * wc.us,
  //   charge: -500,  // negative means # electrons per step (see below configuration)
  //   ray: track_8,  // params.det.bounds,
  // },
  //     {
  //   //time: 0 * wc.us,
  //   time: -250 * wc.us,
  //   charge: -500,  // negative means # electrons per step (see below configuration)
  //   ray: track_9,  // params.det.bounds,
  // },

];

local depos = sim.tracks(tracklist, step=0.1 * wc.mm);  // MIP <=> 5000e/mm

local nanodes = std.length(tools.anodes);
local anode_iota = std.range(0, nanodes - 1);
local anode_idents = [anode.data.ident for anode in tools.anodes];

// local output = 'wct-sim-ideal-sig.npz';
// local deposio = io.numpy.depos(output);
local drifter = sim.drifter;
local bagger = sim.make_bagger();
// signal plus noise pipelines
local sn_pipes = sim.splusn_pipelines;
// local sn_pipes = sim.signal_pipelines; //signal only
// local analog_pipes = sim.analog_pipelines;

local perfect = import 'pgrapher/experiment/pdhd/chndb-base.jsonnet';
local chndb = [{
  type: 'OmniChannelNoiseDB',
  name: 'ocndbperfect%d' % n,
  data: perfect(params, tools.anodes[n], tools.field, n) { dft: wc.tn(tools.dft) },
  uses: [tools.anodes[n], tools.field, tools.dft],
} for n in anode_iota];

local nf_maker = import 'pgrapher/experiment/pdhd/nf.jsonnet';
local nf_pipes = [nf_maker(params, tools.anodes[n], chndb[n], n, name='nf%d' % n) for n in std.range(0, std.length(tools.anodes) - 1)];

local sp_override = {
  sparse: true,
  use_roi_debug_mode: false,
  use_multi_plane_protection: true,
  process_planes: [0, 1, 2],
};

local sp_maker = import 'pgrapher/experiment/pdhd/sp.jsonnet';
local sp = sp_maker(params, tools, sp_override);
local sp_pipes = [sp.make_sigproc(a) for a in tools.anodes];

local magoutput = 'protodunehd-sim-check_evt2.root';
local magnify = import 'pgrapher/experiment/pdhd/magnify-sinks.jsonnet';
local magnifyio = magnify(tools, magoutput);

local parallel_pipes = [
  g.pipeline([
               sn_pipes[n],
               magnifyio.orig_pipe[n],
               nf_pipes[n],
               magnifyio.raw_pipe[n],
              //  sp_pipes[n],
               // // magnifyio.debug_pipe[n],
              //  magnifyio.decon_pipe[n],
             ],
             'parallel_pipe_%d' % n)
  for n in std.range(0, std.length(tools.anodes) - 1)
];
local outtags = ['raw%d' % n for n in std.range(0, std.length(tools.anodes) - 1)];
local parallel_graph = f.fanpipe('DepoSetFanout', parallel_pipes, 'FrameFanin', 'sn_mag_nf', outtags);

//local frameio = io.numpy.frames(output);
local sink = sim.frame_sink;
local graph = g.pipeline([depos, drifter, bagger, parallel_graph, sink]);


local app = {
  type: 'Pgrapher',
  data: {
    edges: g.edges(graph),
  },
};

local cmdline = {
  type: 'wire-cell',
  data: {
    plugins: ['WireCellGen', 'WireCellPgraph', 'WireCellSio', 'WireCellSigProc', 'WireCellRoot'],
    apps: ['Pgrapher'],
  },
};


// Finally, the configuration sequence which is emitted.

[cmdline] + g.uses(graph) + [app]
