
TCanvas *c1 = new TCanvas("c1","c1",1);
TString fname = "../data/test_APA1w_28548_456798.root";
TString fname_out = "../data/wf_28548_456798_1.root";
void SetDataFileName(TString name,TString name_out){
  fname=name;
  fname_out=name_out;
}
TH1F* get_power_density(TH2F* hraw, std::vector<int> channels,
                       TString name,
		       int tick_offset=1700,int tick_offset2=1700, int nticks=200){

  TH1F* hfft= new TH1F(name,"", nticks,0,1./0.512); // MHz, 512ns period
  hfft->GetXaxis()->SetTitle("Frequency (MHz)");
  hfft->GetYaxis()->SetTitle("|C(#omega)^{2}|");
    double k = double((tick_offset2-tick_offset))/double(channels.size());
    // cout<<"k = "<<(tick_offset2-tick_offset)<<"  "<<channels.size()<<"  "<<k<<endl;
  for(auto channel: channels){

    int start_bin = k*(channel-channels[0])+tick_offset;
    // cout<<"start_bin  "<<start_bin<<endl;
    TH1F *h2 = new TH1F(Form("Channel%d", channel),Form("Channel %d", channel),nticks,0,nticks);
    for (int i=0; i!=nticks; i++){
      int ibin = hraw->GetXaxis()->FindBin(channel);
      h2->SetBinContent(i+1,hraw->GetBinContent(ibin,i+1 + start_bin));
    }

    TH1F* hmag = (TH1F*)h2->FFT(0, "MAG");
    for(int i=1; i<nticks; i++){
      hfft->AddBinContent(i+1, hmag->GetBinContent(i+1) );
    }
    hmag->Delete();
  }
  hfft->Scale(1./channels.size());
  hfft->Scale(1400.0/(4096*4)); // convert ADC to mV

  for(int i=0; i<nticks; i++) {
    hfft->SetBinContent(i+1, std::pow(hfft->GetBinContent(i+1),2) );
  }

  return hfft;
}

double chi2(TH1F * pdf, TH1F *data){
    double value=0;
    int nbins = pdf->GetNbinsX();
    for(int i=1;i<=nbins;i++){
        double npdf = pdf->GetBinContent(i);
        double ndata = data->GetBinContent(i);
        double value1 = (npdf-ndata)*(npdf-ndata);
        value += value1 ;
    }
    return value/nbins;
}

TH1F* align(TH2F* hraw, std::vector<int> channels,TString name,
	int tick_offset=1700,int tick_offset2=1700, int nticks=140){
TH1F *h_align = new TH1F(name,"",2*nticks,200-nticks,200+nticks);
double k = double((tick_offset2-tick_offset))/double(channels.size());
for(auto channel: channels){
    int start_bin = k*(channel-channels[0])+tick_offset;
    TH1F *h2 = new TH1F(Form("Channel%d", channel),Form("Channel %d", channel),nticks,0,nticks);
    for (int i=0; i!=nticks; i++){
        int ibin = hraw->GetXaxis()->FindBin(channel);
        h2->SetBinContent(i+1,hraw->GetBinContent(ibin,i+1 + start_bin));
    }
    int maxbin = h2->GetMaximumBin();
    int start_bin_2 = nticks-maxbin;
    // h2->Draw();
    // TLine *l1 = new TLine(h2->GetBinCenter(maxbin),0,h2->GetBinCenter(maxbin),2000);
    // l1->Draw("same");
    // c1->SaveAs(outname);
    for (int i=0; i<nticks; i++){
        double value =h2->GetBinContent(i+1); 
        h_align->AddBinContent(nticks-maxbin+i,value);
    }
    delete h2;
}
h_align->Scale(1./channels.size());
return h_align;
}

TH1F* align2(TH2F* hraw, std::vector<int> channels,TString name,
	int tick_offset=1700,int tick_offset2=1700, int nticks=140,double x1=0,double x2=0,double y1=0,double y2=0){
TH1F *h_align = new TH1F(name,"",2*nticks,200-nticks,200+nticks);
double k = double((tick_offset2-tick_offset))/double(channels.size());
    double kk = (y2-y1)/(x2-x1);
for(auto channel: channels){
    int start_bin = k*(channel-channels[0])+tick_offset;
    TH1F *h2 = new TH1F(Form("Channel%d", channel),Form("Channel %d", channel),nticks,0,nticks);
    for (int i=0; i!=nticks; i++){
        int ibin = hraw->GetXaxis()->FindBin(channel);
        h2->SetBinContent(i+1,hraw->GetBinContent(ibin,i+1 + start_bin));
    }

    int maxbin = kk*(channel-x1)+y1-start_bin;

    int start_bin_2 = nticks-maxbin;

    for (int i=0; i<nticks; i++){
        double value =h2->GetBinContent(i+1); 
        h_align->AddBinContent(nticks-maxbin+i,value);
    }
    delete h2;
}
h_align->Scale(1./channels.size());
return h_align;
}

void fill_range(vector<int>& v, int start, int end){
  v.resize(end-start+1);
  std::iota(v.begin(), v.end(), start);
}

void Get_wf_density_1(TH2F* h_w, TH1F *& h_signal_w,TH1F *& h_signal_w_spec, double *par_w,TString name){
  int w_chnl_s=par_w[0];
  int w_chnl_e=par_w[1];
  double w_t_s=par_w[2];
  double w_t_e=par_w[3];
  int w_nticks=par_w[4];
  double w_x1=par_w[5];
  double w_y1=par_w[6];
  double w_x2=par_w[7];
  double w_y2=par_w[8];


  std::vector<int> splusn_chans_w;

  fill_range(splusn_chans_w, w_chnl_s, w_chnl_e); 
  h_signal_w = (TH1F*)align2(h_w,splusn_chans_w,name+TString("_wf"),w_t_s,w_t_e,w_nticks,w_x1,w_x2,w_y1,w_y2); 
  h_signal_w_spec = (TH1F*)get_power_density(h_w,splusn_chans_w,name+TString("_pd"),w_t_s,w_t_e,w_nticks); 

  
}

void Get_wf_density_2(TH2F* h_w, TH1F* &h_signal_w,TH1F* &h_signal_w_spec, double *par_w,TString name){
  int w_chnl_s=par_w[0];
  int w_chnl_e=par_w[1];
  double w_t_s=par_w[2];
  double w_t_e=par_w[3];
  int w_nticks=par_w[4];
  std::vector<int> splusn_chans_w;

  fill_range(splusn_chans_w, w_chnl_s, w_chnl_e); 
  h_signal_w = (TH1F*)align(h_w,splusn_chans_w,name+TString("_wf"),w_t_s,w_t_e,w_nticks);
  h_signal_w_spec = (TH1F*)get_power_density(h_w,splusn_chans_w,name+TString("_pd"),w_t_s,w_t_e,w_nticks); 

  
}

void range_check(TH2F* h_w,double *par_w){
gStyle->SetPalette(kLightTemperature);
  int w_chnl_s=par_w[0];
  int w_chnl_e=par_w[1];
  double w_t_s=par_w[2];
  double w_t_e=par_w[3];
  int w_nticks=par_w[4];
  double w_x1=par_w[5];
  double w_y1=par_w[6];
  double w_x2=par_w[7];
  double w_y2=par_w[8];
  TLine *l1 = new TLine(w_chnl_s,w_t_s,w_chnl_e,w_t_e); 
  TLine *l2 = new TLine(w_x1,w_y1,w_x2,w_y2); 
  TLine *l3 = new TLine(w_chnl_s,w_t_s+w_nticks,w_chnl_e,w_t_e+w_nticks);
  l1->SetLineStyle(2); 
  l2->SetLineStyle(2); 
  l3->SetLineStyle(2); 
  // h_w->GetXaxis()->SetRangeUser(w_chnl_s-100,w_chnl_e+100); 
  // h_w->GetYaxis()->SetRangeUser(w_t_s-500,w_t_e+500);
double y_range2=w_t_e+200+200;
  double y_range1=w_t_s-200;
  if(w_t_e<w_t_s) 
  {
    y_range2=w_t_s+200+200;
    y_range1=w_t_e-200;
  }
  h_w->GetXaxis()->SetRangeUser(w_chnl_s-100,w_chnl_e+100); 
  h_w->GetYaxis()->SetRangeUser(y_range1,y_range2);
  h_w->GetZaxis()->SetRangeUser(-50, 50);
  h_w->Draw("colz");
  l1->Draw("same"); 
  l2->Draw("same"); 
  l3->Draw("same"); 
  c1->SaveAs(fname_out+".pdf");
}

void range_check_2(TH2F* h_w,double *par_w){
gStyle->SetPalette(kLightTemperature);
  h_w->Draw("colz");
  int w_chnl_s=par_w[0];
  int w_chnl_e=par_w[1];
  double w_t_s=par_w[2];
  double w_t_e=par_w[3];
  int w_nticks=par_w[4];

  TLine *l1 = new TLine(w_chnl_s,w_t_s,w_chnl_e,w_t_e); 
  TLine *l3 = new TLine(w_chnl_s,w_t_s+w_nticks,w_chnl_e,w_t_e+w_nticks);
  l1->SetLineStyle(2); 
  l3->SetLineStyle(2); 
  l1->Draw("same"); 
  l3->Draw("same"); 
 double y_range2=w_t_e+200+200;
  double y_range1=w_t_s-200;
  if(w_t_e<w_t_s) 
  {
    y_range2=w_t_s+200+200;
    y_range1=w_t_e-200;
  }
  h_w->GetXaxis()->SetRangeUser(w_chnl_s-100,w_chnl_e+100); 
  h_w->GetYaxis()->SetRangeUser(y_range1,y_range2);
  h_w->GetZaxis()->SetRangeUser(-50, 50);
  c1->SaveAs(fname_out+".pdf");
}

void check_waveform_withdata_2(){

// SetDataFileName("../data/test_APA1w_28548_456798.root","../data/wf_28548_456798_1.root");
// SetDataFileName("../data/test_APA1w_28548_456798.root","../data/tr_0.root");
// double par_w[]={2107,2261,4072,3891,180,2104.951856946355,4156.626506024097,2264.511691884457,3983.132530120482}; //beam; track_0 
// double par_v[]={830, 983,3735,3991,180}; 

// SetDataFileName("../data/test_APA1w_28548_439442.root","../data/tr_10.root");
// double par_w[]={2082, 2449,4267,4007,150,2083.44,4332.10,2449.80,4081.63}; // track_1
// double par_v[]={1075, 1202,4257,4050,150}; 

// SetDataFileName("../data/test_APA1w_28548_439442.root","../data/tr_1.root");
// double par_w[]={2112, 2334,3850,2850,300,2112.250332889481, 3996.1941008563267 ,2334.8868175765647, 2987.630827783063}; // track_1
// double par_v[]={823, 1129,3320,3940,200}; 

// SetDataFileName("../data/test_APA1w_28548_456790.root","../data/tr_2.root");
// double par_w[]={2237, 2310,4006,3828,200,2237.535014005602,4088.5416666666674,2371.2885154061623,3715.2777777777787}; // track_2
// double par_v[]={822, 919,4083,3825,150}; 

// SetDataFileName("../data/test_APA1w_28548_456790.root","../data/tr_3.root");
// double par_w[]={2453, 2520,2964,1967,300,2455.8303886925796,3040.201005025125,2525.323910482921,2060.301507537688}; //track_3
// double par_v[]={820, 1009,2238,2612,200}; 


// SetDataFileName("../data/test_APA1w_28548_456798.root","../data/tr_4.root");
// double par_w[]={2115, 2300,2330,2675,200,2111.7482517482517, 2416.0237388724036,2295.244755244755, 2736.4985163204747}; //track_4
// double par_v[]={1030, 1261,2335,2697,200};

// SetDataFileName("../data/test_APA1w_28548_456798.root","../data/tr_5.root");
// double par_w[]={2388, 2551,1834,2777,200,2388.407163053723, 1936.3057324840765,2551.272384542884, 2866.2420382165606};  //track_5
// double par_v[]={854, 1030,1714,2729,200};

SetDataFileName("../data/test_APA1w_28548_456798.root","../data/tr_6.root");
double par_w[]={2288, 2370,5095,3872,250,2288.8784165881243, 5184.713375796178,2370.3110273327047, 3987.261146496815}; //track_6
double par_v[]={931, 992,5168,4184,200};

TFile *file_data = TFile::Open(fname);
// TFile *file_data = TFile::Open("/exp/dune/data/users/xning/proto-dune-hd/SignalProcessing/sigproc/wire-cell-cfg/pgrapher/experiment/pdhd/test_APA1w_28548_439442.root");
TH2F *hraw_data_w = (TH2F*)file_data->Get("raw_wf_ANF_1_w");
TH2F *hraw_data_v = (TH2F*)file_data->Get("raw_wf_ANF_1_v");


c1->SaveAs(fname_out+".pdf[");
TH1F *h_signal_v;
TH1F *h_signal_w ;
TH1F *h_signal_v_spec;
TH1F *h_signal_w_spec;

range_check(hraw_data_w,par_w);
Get_wf_density_1(hraw_data_w, h_signal_w, h_signal_w_spec, par_w,"w");
range_check_2(hraw_data_v,par_v);
Get_wf_density_2(hraw_data_v, h_signal_v, h_signal_v_spec, par_v,"v");

// cout<<h_signal_v->GetName()<<endl;

// TH1F *h_signal_v = (TH1F*)align(hraw_data_v,splusn_chans_v,"signal_v",3255,3917,200); //439442_v_cosmic
// TH1F *h_signal_w = (TH1F*)align(hraw_data_w,splusn_chans_w,"signal_w",3941,2865,200); //439442_w_cosmic
// TH1F *h_signal_v_spec = (TH1F*)get_power_density(hraw_data_v,splusn_chans_v,"pd_v",3255,3917,200); //439442_v_cosmic
// TH1F *h_signal_w_spec = (TH1F*)get_power_density(hraw_data_w,splusn_chans_w,"pd_w",3941,2865,200); //439442_w_cosmic

    // double x1=2083.44;
    // double x2=2449.80;
    // double y1=4332.10;
    // double y2=4081.63;
// TH1F *h_signal_v = (TH1F*)align(hraw_data_v,splusn_chans_v,"signal_v",4257,4050,150); //439442_v_beam
// TH1F *h_signal_w = (TH1F*)align2(hraw_data_w,splusn_chans_w,"signal_w",4267,4007,150,2083.44,2449.80,4332.10,4081.63); //439442_w_beam
// TH1F *h_signal_v_spec = (TH1F*)get_power_density(hraw_data_v,splusn_chans_v,"pd_v",4257,4050,150); //439442_v_beam
// TH1F *h_signal_w_spec = (TH1F*)get_power_density(hraw_data_w,splusn_chans_w,"pd_w",4267,4007,150); //439442_w_beam


//TFile *f1 = new TFile("protodunehd-sim-check-cosmic-wm100.root","read");
//TFile *f2 = new TFile("protodunehd-sim-check-cosmic-wm70.root","read");
//TFile *f3 = new TFile("protodunehd-sim-check-cosmic-wm50.root","read");
//TFile *f4 = new TFile("protodunehd-sim-check-cosmic-wm30.root","read");
// TFile *f1 = new TFile("/exp/dune/data/users/xning/proto-dune-hd/SignalProcessing/sigproc/wire-cell-cfg/pgrapher/experiment/pdhd/protodunehd-sim-check.root","read");
// TH2F *hraw_sim_w = (TH2F*)f1->Get("hw_raw0");
// TH2F *hraw_sim_v = (TH2F*)f1->Get("hv_raw0");


// TH1F *h_sim_v = (TH1F*)align(hraw_sim_v,splusn_chans_v,"sim_v",3850,4100,180); //456798_v_beam
// TH1F *h_sim_w = (TH1F*)align2(hraw_sim_w,splusn_chans_w,"sim_w",4072,3780,180,2081.55,2551.54,4230.12,3695.47); //456798_w_beam
// TH1F *h_sim_v_spec = (TH1F*)get_power_density(hraw_sim_v,splusn_chans_v,"pd_sim_v",3850,4100,180); //456798_v_beam
// TH1F *h_sim_w_spec = (TH1F*)get_power_density(hraw_sim_w,splusn_chans_w,"pd_sim_w",4072,3780,180); //456798_w_beam


// TH1F *h_sim_v = (TH1F*)align(hraw_sim_v,splusn_chans_v,"sim_v",3000,3600,200); //439442_v_cosmic
// TH1F *h_sim_w = (TH1F*)align(hraw_sim_w,splusn_chans_w,"sim_w",3625,2650,200); //439442_w_cosmic
// TH1F *h_sim_v_spec = (TH1F*)get_power_density(hraw_sim_v,splusn_chans_v,"pd_sim_v",3000,3600,200); //439442_v_cosmic
// TH1F *h_sim_w_spec = (TH1F*)get_power_density(hraw_sim_w,splusn_chans_w,"pd_sim_w",3625,2650,200); //439442_w_cosmic


// cout<<"ratio v = "<<h_sim_v->Integral()/h_signal_v->Integral()<<endl;

// double ratio =h_sim_v->Integral()/h_signal_v->Integral();
// // h_sim_v->Scale(1./ratio); 
// // h_sim_w->Scale(1./ratio); 

h_signal_v->Draw();
c1->SaveAs(fname_out+".pdf");
h_signal_w->Draw("");
c1->SaveAs(fname_out+".pdf");
// h_sim_v->Draw("same");
// h_sim_v->SetLineColor(2);
// TCanvas *c2 = new TCanvas("c2","c2",1);
// h_signal_w->Draw();
// h_signal_w->GetYaxis()->SetRangeUser(-20,30);;
// h_sim_w->Draw("same");
// h_sim_w->SetLineColor(2);
// TCanvas *c3 = new TCanvas("c3","c3",1);
h_signal_w_spec->Draw();
h_signal_w_spec->GetXaxis()->SetRangeUser(0,1);;
c1->SaveAs(fname_out+".pdf");
h_signal_v_spec->Draw("");
h_signal_v_spec->GetXaxis()->SetRangeUser(0,1);;
c1->SaveAs(fname_out+".pdf");
c1->SaveAs(fname_out+".pdf]");
// h_sim_w_spec->Draw("same");
// h_sim_w_spec->SetLineColor(2);
// TCanvas *c4 = new TCanvas("c4","c4",1);
// h_signal_v_spec->Draw();
// h_signal_v_spec->GetXaxis()->SetRangeUser(0,1);;
// h_sim_v_spec->Draw("same");
// h_sim_v_spec->SetLineColor(2);

// cout<<"track1:llh = "<<chi2(h_sim_w,h_signal_w)<<endl;
// cout<<"pow1:llh = "<<chi2(h_sim_w_spec,h_signal_w_spec)<<endl;


TFile *fout = new TFile(fname_out,"recreate");
h_signal_v->Write();
h_signal_w->Write();
h_signal_v_spec->Write();
h_signal_w_spec->Write();
fout->Close();

}
