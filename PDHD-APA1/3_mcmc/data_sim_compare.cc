
TCanvas *c1 = new TCanvas("c1", "c1", 1);
TString fname_out = "../data/compare_check.pdf";
TString simu_folder="f_1.00_1.00";
TString filename_simu_all[8];
double chi2_total;
void SetDataFileName(TString name_out)
{
  fname_out = name_out;
}
void SetSimFolder(TString folder){
      simu_folder = folder;
      filename_simu_all[0] = "../data/"+simu_folder+"/protodunehd-sim-check_evt0.root";
      filename_simu_all[1] = "../data/"+simu_folder+"/protodunehd-sim-check_evt1.root";
      filename_simu_all[2] = "../data/"+simu_folder+"/protodunehd-sim-check_evt1.root";
      filename_simu_all[3] = "../data/"+simu_folder+"/protodunehd-sim-check_evt2.root";
      filename_simu_all[4] = "../data/"+simu_folder+"/protodunehd-sim-check_evt2.root";
      filename_simu_all[5] = "../data/"+simu_folder+"/protodunehd-sim-check_evt3.root";
      filename_simu_all[6] = "../data/"+simu_folder+"/protodunehd-sim-check_evt3.root";
      filename_simu_all[7] = "../data/"+simu_folder+"/protodunehd-sim-check_evt3.root";
}
double par_w_all[8][9] = {
    {2107, 2261, 4072, 3891, 180, 2105.081300813008, 4178.373205741627, 2260.7723577235774, 4000.765550239235},
    {2082, 2449, 4267, 4007, 150, 2113.8907175773534, 4314.921223354958, 2454.9045424621463, 4088.0444856348468},
    {2112, 2334, 3850, 2850, 300, 2099.4075049374587, 4073.21594068582, 2270.901909150757, 3278.405931417979},
    {2277, 2384, 3875, 3591, 200, 2353.595166163142, 3770.547945205479, 2392.26586102719, 3657.5342465753424},
    {2453, 2520, 3000, 2000, 300, 2453.897280966767, 3205.4794520547944, 2523.7462235649546, 2116.4383561643835},
    //{2115, 2300, 2330, 2675, 200, 2111.7482517482517, 2416.0237388724036, 2295.244755244755, 2736.4985163204747},
    //{2388, 2551, 1834, 2777, 200, 2388.407163053723, 1936.3057324840765, 2551.272384542884, 2866.2420382165606},
    {2115, 2300, 2330, 2675, 200, 2137.4310480693457, 2469.6517412935323, 2299.0543735224587, 2744.278606965174},
    {2388, 2551, 1834, 2777, 200, 2380.9669992325403, 1883.1999999999998,2522.947045280123, 2728},
    {2288, 2370, 5095, 3872, 250, 2288.8784165881243, 5184.713375796178, 2370.3110273327047, 3987.261146496815}
    };

double par_v_all[8][5] = {
    {830, 983, 3735, 3991, 180},
    {1075, 1202, 4257, 4050, 150},
    {823, 1129, 3320, 3940, 300},
    {904, 1016, 3903, 3681, 200},
    {820, 1009, 2360, 2710, 300},
    {1030, 1261, 2335, 2697, 200},
    {854, 1030, 1714, 2729, 200},
    {931, 992, 5168, 4184, 250}
    };

TString filename_data_all[] = {
    "../data/tr_no_cnr_0.root",
    "../data/tr_no_cnr_10.root",
    "../data/tr_no_cnr_1.root",
    "../data/tr_no_cnr_2.root",
    "../data/tr_no_cnr_3.root",
    "../data/tr_no_cnr_4.root",
    "../data/tr_no_cnr_5.root",
    "../data/tr_no_cnr_6.root"
    };



struct Information
{
  // TString filename_data;
  // TString filename_simu;
  // double *par_w;
  // double *par_v;
  TH1F *h_signal_v;
  TH1F *h_signal_w;
  TH1F *h_signal_v_spec;
  TH1F *h_signal_w_spec;
  TH1F *h_sim_v;
  TH1F *h_sim_w;
  TH1F *h_sim_v_spec;
  TH1F *h_sim_w_spec;
  double chi2_wf_w;
  double chi2_wf_v;
  double chi2_pd_w;
  double chi2_pd_v;
  TString outname;
};

map<int, Information> track_Information;

TH1F *get_power_density(TH2F *hraw, std::vector<int> channels,
                        TString name,
                        int tick_offset = 1700, int tick_offset2 = 1700, int nticks = 200)
{

  TH1F *hfft = new TH1F(name, "", nticks, 0, 1. / 0.512); // MHz, 512ns period
  hfft->GetXaxis()->SetTitle("Frequency (MHz)");
  hfft->GetYaxis()->SetTitle("|C(#omega)^{2}|");
  double k = double((tick_offset2 - tick_offset)) / double(channels.size());
  // cout<<"k = "<<(tick_offset2-tick_offset)<<"  "<<channels.size()<<"  "<<k<<endl;
  for (auto channel : channels)
  {

    int start_bin = k * (channel - channels[0]) + tick_offset;
    // cout<<"start_bin  "<<start_bin<<endl;
    TH1F *h2 = new TH1F(Form("Channel%d", channel), Form("Channel %d", channel), nticks, 0, nticks);
    for (int i = 0; i != nticks; i++)
    {
      int ibin = hraw->GetXaxis()->FindBin(channel);
      h2->SetBinContent(i + 1, hraw->GetBinContent(ibin, i + 1 + start_bin));
    }

    TH1F *hmag = (TH1F *)h2->FFT(0, "MAG");
    for (int i = 0; i < nticks; i++)
    {
      hfft->AddBinContent(i + 1, hmag->GetBinContent(i + 1));
    }
    hmag->Delete();
  }
  hfft->Scale(1. / channels.size());
  hfft->Scale(1400.0 / (4096 * 4)); // convert ADC to mV

  for (int i = 0; i < nticks; i++)
  {
    hfft->SetBinContent(i + 1, std::pow(hfft->GetBinContent(i + 1), 2));
  }

  return hfft;
}

TH1F *align(TH2F *hraw, std::vector<int> channels, TString name,
            int tick_offset = 1700, int tick_offset2 = 1700, int nticks = 140)
{
  TH1F *h_align = new TH1F(name, "", 2 * nticks, 200 - nticks, 200 + nticks);
  double k = double((tick_offset2 - tick_offset)) / double(channels.size());
  for (auto channel : channels)
  {
    int start_bin = k * (channel - channels[0]) + tick_offset;
    TH1F *h2 = new TH1F(Form("Channel%d", channel), Form("Channel %d", channel), nticks, 0, nticks);
    for (int i = 0; i != nticks; i++)
    {
      int ibin = hraw->GetXaxis()->FindBin(channel);
      h2->SetBinContent(i + 1, hraw->GetBinContent(ibin, i + 1 + start_bin));
    }
    int maxbin = h2->GetMaximumBin();
    int start_bin_2 = nticks - maxbin;
    // h2->Draw();
    // TLine *l1 = new TLine(h2->GetBinCenter(maxbin),0,h2->GetBinCenter(maxbin),2000);
    // l1->Draw("same");
    // c1->SaveAs(outname);
    for (int i = 0; i < nticks; i++)
    {
      double value = h2->GetBinContent(i + 1);
      h_align->AddBinContent(nticks - maxbin + i, value);
    }
    delete h2;
  }
  h_align->Scale(1. / channels.size());
  return h_align;
}

TH1F *align2(TH2F *hraw, std::vector<int> channels, TString name,
             int tick_offset = 1700, int tick_offset2 = 1700, int nticks = 140, double x1 = 0, double x2 = 0, double y1 = 0, double y2 = 0)
{
  TH1F *h_align = new TH1F(name, "", 2 * nticks, 200 - nticks, 200 + nticks);
  double k = double((tick_offset2 - tick_offset)) / double(channels.size());
  double kk = (y2 - y1) / (x2 - x1);
  for (auto channel : channels)
  {
    int start_bin = k * (channel - channels[0]) + tick_offset;
    TH1F *h2 = new TH1F(Form("Channel%d", channel), Form("Channel %d", channel), nticks, 0, nticks);
    for (int i = 0; i != nticks; i++)
    {
      int ibin = hraw->GetXaxis()->FindBin(channel);
      h2->SetBinContent(i + 1, hraw->GetBinContent(ibin, i + 1 + start_bin));
    }

    int maxbin = kk * (channel - x1) + y1 - start_bin;

    int start_bin_2 = nticks - maxbin;

    for (int i = 0; i < nticks; i++)
    {
      double value = h2->GetBinContent(i + 1);
      h_align->AddBinContent(nticks - maxbin + i, value);
    }
    delete h2;
  }
  h_align->Scale(1. / channels.size());
  return h_align;
}

void fill_range(vector<int> &v, int start, int end)
{
  v.resize(end - start + 1);
  std::iota(v.begin(), v.end(), start);
}

void Get_wf_density_1(TH2F *h_w, TH1F *&h_signal_w, TH1F *&h_signal_w_spec, double *par_w, TString name)
{
  int w_chnl_s = par_w[0];
  int w_chnl_e = par_w[1];
  double w_t_s = par_w[2];
  double w_t_e = par_w[3];
  int w_nticks = par_w[4];
  double w_x1 = par_w[5];
  double w_y1 = par_w[6];
  double w_x2 = par_w[7];
  double w_y2 = par_w[8];

  std::vector<int> splusn_chans_w;

  fill_range(splusn_chans_w, w_chnl_s, w_chnl_e);
  h_signal_w = (TH1F *)align2(h_w, splusn_chans_w, name + TString("_wf"), w_t_s, w_t_e, w_nticks, w_x1, w_x2, w_y1, w_y2);
  h_signal_w_spec = (TH1F *)get_power_density(h_w, splusn_chans_w, name + TString("_pd"), w_t_s, w_t_e, w_nticks);
}

void Get_wf_density_2(TH2F *h_w, TH1F *&h_signal_w, TH1F *&h_signal_w_spec, double *par_w, TString name)
{
  int w_chnl_s = par_w[0];
  int w_chnl_e = par_w[1];
  double w_t_s = par_w[2];
  double w_t_e = par_w[3];
  int w_nticks = par_w[4];
  std::vector<int> splusn_chans_w;

  fill_range(splusn_chans_w, w_chnl_s, w_chnl_e);
  h_signal_w = (TH1F *)align(h_w, splusn_chans_w, name + TString("_wf"), w_t_s, w_t_e, w_nticks);
  h_signal_w_spec = (TH1F *)get_power_density(h_w, splusn_chans_w, name + TString("_pd"), w_t_s, w_t_e, w_nticks);
}

void range_check(TH2F *h_w, double *par_w)
{
  gStyle->SetPalette(kLightTemperature);
  int w_chnl_s = par_w[0];
  int w_chnl_e = par_w[1];
  double w_t_s = par_w[2];
  double w_t_e = par_w[3];
  int w_nticks = par_w[4];
  double w_x1 = par_w[5];
  double w_y1 = par_w[6];
  double w_x2 = par_w[7];
  double w_y2 = par_w[8];
  TLine *l1 = new TLine(w_chnl_s, w_t_s, w_chnl_e, w_t_e);
  TLine *l2 = new TLine(w_x1, w_y1, w_x2, w_y2);
  TLine *l3 = new TLine(w_chnl_s, w_t_s + w_nticks, w_chnl_e, w_t_e + w_nticks);
  l1->SetLineStyle(2);
  l2->SetLineStyle(2);
  l3->SetLineStyle(2);
  // h_w->GetXaxis()->SetRangeUser(w_chnl_s-100,w_chnl_e+100);
  // h_w->GetYaxis()->SetRangeUser(w_t_s-500,w_t_e+500);
  // h_w->GetXaxis()->SetRangeUser(2200,2400);
  // h_w->GetYaxis()->SetRangeUser(1000,5000);
  // h_w->GetXaxis()->SetRangeUser(w_chnl_s-100,w_chnl_e+100);
  // h_w->GetYaxis()->SetRangeUser(w_t_s-100,w_t_e+200+200);
  double y_range2 = w_t_e + 200 + 200;
  double y_range1 = w_t_s - 200;
  if (w_t_e < w_t_s)
  {
    y_range2 = w_t_s + 200 + 200;
    y_range1 = w_t_e - 200;
  }
  h_w->GetXaxis()->SetRangeUser(w_chnl_s - 100, w_chnl_e + 100);
  h_w->GetYaxis()->SetRangeUser(y_range1, y_range2);
  h_w->GetZaxis()->SetRangeUser(-50, 50);
  h_w->Draw("colz");
  l1->Draw("same");
  l2->Draw("same");
  l3->Draw("same");
  c1->SaveAs(fname_out);
}

void range_check_2(TH2F *h_w, double *par_w)
{
  gStyle->SetPalette(kLightTemperature);
  h_w->Draw("colz");
  int w_chnl_s = par_w[0];
  int w_chnl_e = par_w[1];
  double w_t_s = par_w[2];
  double w_t_e = par_w[3];
  int w_nticks = par_w[4];

  TLine *l1 = new TLine(w_chnl_s, w_t_s, w_chnl_e, w_t_e);
  TLine *l3 = new TLine(w_chnl_s, w_t_s + w_nticks, w_chnl_e, w_t_e + w_nticks);
  l1->SetLineStyle(2);
  l3->SetLineStyle(2);
  l1->Draw("same");
  l3->Draw("same");
  // h_w->GetXaxis()->SetRangeUser(2000,2400);

  double y_range2 = w_t_e + 200 + 200;
  double y_range1 = w_t_s - 200;
  if (w_t_e < w_t_s)
  {
    y_range2 = w_t_s + 200 + 200;
    y_range1 = w_t_e - 200;
  }
  h_w->GetXaxis()->SetRangeUser(w_chnl_s - 100, w_chnl_e + 100);
  h_w->GetYaxis()->SetRangeUser(y_range1, y_range2);
  // h_w->GetXaxis()->SetRangeUser(2200,2400);
  // h_w->GetYaxis()->SetRangeUser(1000,5000);
  h_w->GetZaxis()->SetRangeUser(-50, 50);
  c1->SaveAs(fname_out);
}

void set_track_par(int is_range_check=0)
{
  for (int i = 0; i < 8; i++)
  {
    TFile *fin_data = new TFile(filename_data_all[i], "read");
    track_Information[i].h_signal_w = (TH1F *)fin_data->Get("w_wf");
    track_Information[i].h_signal_w->SetDirectory(0);
    track_Information[i].h_signal_v = (TH1F *)fin_data->Get("v_wf");
    track_Information[i].h_signal_v->SetDirectory(0);
    track_Information[i].h_signal_w_spec = (TH1F *)fin_data->Get("w_pd");
    track_Information[i].h_signal_w_spec->SetDirectory(0);
    track_Information[i].h_signal_v_spec = (TH1F *)fin_data->Get("v_pd");
    track_Information[i].h_signal_v_spec->SetDirectory(0);
    fin_data->Close();


    TFile *fin_sim = new TFile(filename_simu_all[i], "read");
    cout<<filename_simu_all[i]<<endl;
    TH2F *hraw_sim_w = (TH2F *)fin_sim->Get("hw_raw0");
    TH2F *hraw_sim_v = (TH2F *)fin_sim->Get("hv_raw0");
    // hraw_sim_v->Scale(0.5); 
    Get_wf_density_1(hraw_sim_w, track_Information[i].h_sim_w, track_Information[i].h_sim_w_spec, par_w_all[i], "w");
    Get_wf_density_2(hraw_sim_v, track_Information[i].h_sim_v, track_Information[i].h_sim_v_spec, par_v_all[i], "v");

    track_Information[i].h_sim_w->SetDirectory(0);
    track_Information[i].h_sim_v->SetDirectory(0);
    track_Information[i].h_sim_w_spec->SetDirectory(0);
    track_Information[i].h_sim_v_spec->SetDirectory(0);

    // cout<<track_Information[i].h_signal_v_spec->Integral()<<endl;
    // cout<<track_Information[i].h_signal_v->Integral()<<endl;
    // cout<<track_Information[i].h_sim_v_spec->Integral()<<endl;
    // cout<<track_Information[i].h_sim_v->Integral()<<endl;

    if(is_range_check==1){
      range_check(hraw_sim_w,par_w_all[i]);
      range_check_2(hraw_sim_v,par_v_all[i]);
    }

    delete hraw_sim_w;
    delete hraw_sim_v;

    fin_sim->Close();

    track_Information[i].outname = Form("../data/track_%d_compare.pdf", i);
  }
}

// align peak again
TH1F *align_peak(TH1F *h1, TH1F *h2){
  int maxbin1 = h1->GetMaximumBin();
  int maxbin2 = h2->GetMaximumBin();
  int difference = maxbin2-maxbin1;
  int nbins = h2->GetNbinsX();
  TH1F *h_2 = (TH1F*)h2->Clone();
  h_2->SetDirectory(0);
  h_2->Reset();
  if(difference>0){
    for (int i=1;i<=nbins-difference;i++){
        h_2->SetBinContent(i,h2->GetBinContent(difference+i));
    } 
  }else{
    for (int i=-difference+1;i<=nbins;i++){
        h_2->SetBinContent(i,h2->GetBinContent(i+difference));
    } 
  }
  return h_2;
}

void normalize(int is_save_fig=0)
{
  for (int i = 0; i < 8; i++)
  {
    cout<<"*******Integral check*********"<<endl;
    cout<<track_Information[i].h_signal_v->Integral()<<endl;
    cout<<track_Information[i].h_sim_v->Integral()<<endl;
    cout<<i<<" ratio = "<<track_Information[i].h_signal_v->Integral()/track_Information[i].h_sim_v->Integral()<<endl;

    //double normal_factor = 1;
    double normal_factor = track_Information[i].h_signal_v->Integral()/track_Information[i].h_sim_v->Integral();
    cout<<"normal_factor = "<<normal_factor<<endl;
    track_Information[i].h_sim_v->Scale(normal_factor);
    track_Information[i].h_sim_w->Scale(normal_factor);
    track_Information[i].h_sim_v_spec->Scale(normal_factor * normal_factor);
    track_Information[i].h_sim_w_spec->Scale(normal_factor * normal_factor);
    

    track_Information[i].h_sim_w = align_peak(track_Information[i].h_signal_w,track_Information[i].h_sim_w); 
    // cout<<track_Information[i].h_signal_v_spec->GetBinContent(1)<<endl;
    // cout<<track_Information[i].h_sim_v_spec->GetBinContent(1)<<endl;
    // cout<<track_Information[i].h_sim_v->Integral()<<endl;

//    for (int bin = 1; bin <= track_Information[i].h_sim_v->GetNbinsX(); ++bin){
//            track_Information[i].h_sim_v->SetBinError(bin,sqrt(abs(track_Information[i].h_sim_v->GetBinContent(bin))));
//            track_Information[i].h_signal_v->SetBinError(bin,sqrt(abs(track_Information[i].h_signal_v->GetBinContent(bin))));
//
//        if (track_Information[i].h_sim_v->GetBinContent(bin) == 0 && track_Information[i].h_signal_v->GetBinContent(bin) == 0 ) {
//            track_Information[i].h_signal_v->SetBinError(bin,100);
//        }
//    }
//    for (int bin = 1; bin <= track_Information[i].h_sim_w->GetNbinsX(); ++bin){
//            track_Information[i].h_sim_w->SetBinError(bin,sqrt(abs(track_Information[i].h_sim_w->GetBinContent(bin))));
//            track_Information[i].h_signal_w->SetBinError(bin,sqrt(abs(track_Information[i].h_signal_w->GetBinContent(bin))));
//
//        if (track_Information[i].h_sim_w->GetBinContent(bin) == 0 && track_Information[i].h_signal_w->GetBinContent(bin) == 0 ) {
//            track_Information[i].h_signal_w->SetBinError(bin,100);
//        }
//    }
//    for (int bin = 1; bin <= track_Information[i].h_sim_v_spec->GetNbinsX(); ++bin){
//            track_Information[i].h_sim_v_spec->SetBinError(bin,sqrt(abs(track_Information[i].h_sim_v_spec->GetBinContent(bin))));
//            track_Information[i].h_signal_v_spec->SetBinError(bin,sqrt(abs(track_Information[i].h_signal_v_spec->GetBinContent(bin))));
//
//        if (track_Information[i].h_sim_v_spec->GetBinContent(bin) == 0 && track_Information[i].h_signal_v_spec->GetBinContent(bin) == 0 ) {
//            track_Information[i].h_signal_v_spec->SetBinError(bin,100);
//        }
//    }
//for (int bin = 1; bin <= track_Information[i].h_sim_w_spec->GetNbinsX(); ++bin){
//            track_Information[i].h_sim_w_spec->SetBinError(bin,sqrt(abs(track_Information[i].h_sim_w_spec->GetBinContent(bin))));
//            track_Information[i].h_signal_w_spec->SetBinError(bin,sqrt(abs(track_Information[i].h_signal_w_spec->GetBinContent(bin))));
//
//        if (track_Information[i].h_sim_w_spec->GetBinContent(bin) == 0 && track_Information[i].h_signal_w_spec->GetBinContent(bin) == 0 ) {
//            track_Information[i].h_signal_w_spec->SetBinError(bin,100);
//        }
//    }
//



    


    if(is_save_fig==1){

      c1->cd(1);
      TLegend *leg = new TLegend(0.65, 0.75, 0.87, 0.87);  
      leg->SetBorderSize(0);
      leg->AddEntry(track_Information[i].h_sim_v, " simulation", "l");
      leg->AddEntry(track_Information[i].h_signal_v, "data", "l");
      track_Information[i].h_signal_v->Draw("hist");
      track_Information[i].h_sim_v->SetLineColor(1); 
      track_Information[i].h_sim_v->Draw("hist same");
      track_Information[i].h_signal_v->SetLineColor(2); 
      leg->Draw();
      c1->cd(2);
      track_Information[i].h_signal_w->Draw("hist");
      //track_Information[i].h_signal_w->GetYaxis()->SetRangeUser(-15,30);
      track_Information[i].h_sim_w->SetLineColor(1); 
      track_Information[i].h_sim_w->Draw("hist same");
      track_Information[i].h_signal_w->SetLineColor(2); 
      leg->Draw();
      c1->cd(3);
      gPad->SetLogy();
      track_Information[i].h_signal_v_spec->Draw("hist");
      track_Information[i].h_sim_v_spec->SetLineColor(1); 
      track_Information[i].h_sim_v_spec->Draw("hist same");
      track_Information[i].h_signal_v_spec->SetLineColor(2); 
      track_Information[i].h_sim_v_spec->GetXaxis()->SetRangeUser(0, 1);
      leg->Draw();
      c1->cd(4);
      gPad->SetLogy();
      track_Information[i].h_signal_w_spec->Draw("hist");
      track_Information[i].h_sim_w_spec->SetLineColor(1); 
      track_Information[i].h_sim_w_spec->Draw("hist same");
      track_Information[i].h_signal_w_spec->SetLineColor(2); 
      track_Information[i].h_sim_w_spec->GetXaxis()->SetRangeUser(0, 1);
      track_Information[i].h_sim_w_spec->GetYaxis()->SetRangeUser(1, 1e3);
      leg->Draw();
      c1->SaveAs(fname_out);
    }
  }
  cout<<"end normalize"<<endl;
}

double chi2(TH1F *pdf, TH1F *data)
{
 //  cout<<"in chi2 ***************"<<endl;
  double value = 0;
  int nbins = pdf->GetNbinsX();
  // cout<<"nbins = "<<nbins<<endl;
  for (int i = 1; i <= nbins; i++)
  {
    double npdf = pdf->GetBinContent(i);
    double ndata = data->GetBinContent(i);
    if(ndata!=0){
    double value1 = (npdf - ndata) * (npdf - ndata)/abs(ndata);
     //cout<<"npdf = "<<npdf<<endl;
     //cout<<"ndata = "<<ndata<<endl;
    // cout<<"value1 = "<<value1<<endl;
    value += value1;
    }

  }
  // cout<<"value = "<<value<<endl;
  return value / nbins;
}

void compare_wf_density()
{
  normalize(1);  
    chi2_total =1;
  for (int i = 0; i < 7; i++)
  {
    track_Information[i].chi2_wf_w = chi2(track_Information[i].h_sim_w, track_Information[i].h_signal_w);
    track_Information[i].chi2_wf_v = chi2(track_Information[i].h_sim_v, track_Information[i].h_signal_v);
    track_Information[i].chi2_pd_w = chi2(track_Information[i].h_sim_w_spec, track_Information[i].h_signal_w_spec);
    track_Information[i].chi2_pd_v = chi2(track_Information[i].h_sim_v_spec, track_Information[i].h_signal_v_spec);

    //track_Information[i].chi2_wf_w = track_Information[i].h_signal_w->Chi2Test(track_Information[i].h_sim_w,"WW P");

    //track_Information[i].chi2_wf_v = track_Information[i].h_signal_w->Chi2Test(track_Information[i].h_sim_v,"WW");
    //cout<<track_Information[i].h_signal_w_spec->GetBinContent(200)<<endl;
    //cout<<track_Information[i].h_signal_w_spec->GetBinError(200)<<endl;
    //track_Information[i].chi2_pd_w = track_Information[i].h_signal_w_spec->Chi2Test(track_Information[i].h_sim_w_spec,"WW P");

    //track_Information[i].chi2_pd_v = track_Information[i].h_signal_v_spec->Chi2Test(track_Information[i].h_sim_v_spec,"WW");

    cout << "chi2_wf_w = " << track_Information[i].chi2_wf_w << endl;
    cout << "chi2_wf_v = " << track_Information[i].chi2_wf_v << endl;
    cout << "chi2_pd_w = " << track_Information[i].chi2_pd_w << endl;
    cout << "chi2_pd_v = " << track_Information[i].chi2_pd_v << endl;
    chi2_total = chi2_total*track_Information[i].chi2_wf_w *track_Information[i].chi2_pd_w;
  }

}

void data_sim_compare_all()
{
  // double w1[] = {0.00,0.01,0.5,1,1.5,2}; 
  //double w1[] = {1.00,1.1,1.3,1.5,1.8,2.0}; 
  double w1[] = {0.00}; 
  //double w1[] = {0.01}; 
  // double w2[] = {0.00,0.01,0.5,1,1.5,2,3,5} ;
  double w2[] = {1.5} ;
  double w3[] = {1} ;
  double w4[] = {1} ;
  double st[] = {0} ;

  const int N_w1 = sizeof(w1)/sizeof(w1[0]);
  const int N_w2 = sizeof(w2)/sizeof(w2[0]);
  const int N_w3 = sizeof(w3)/sizeof(w3[0]);
  const int N_w4 = sizeof(w4)/sizeof(w4[0]);
  const int N_st = sizeof(st)/sizeof(st[0]);
  for(int i=0;i<N_w1;i++){
    for(int j=0;j<N_w2;j++){
     for(int k=0;k<N_w3;k++){
     for(int l=0;l<N_w4;l++){
    for(int m=0;m<N_st;m++){
//  for(int i=0;i<1;i++){
//    for(int j=0;j<1;j++){
//    for(int k=0;k<1;k++){
//    for(int l=0;l<1;l++){
//    for(int m=0;m<1;m++){
      SetDataFileName(Form("../data/compare_check_ncn_%.2f_%.2f_%.2f_%.2f_%.0f.pdf",w1[i],w2[j],w3[k],w4[l],st[m]));
      SetSimFolder(Form("f_%.2f_%.2f_%.2f_%.2f_%.0f",w1[i],w2[j],w3[k],w4[l],st[m]));
      //SetSimFolder(Form("g_%.2f_%.2f_%.2f_%.2f_%.0f",w1[i],w2[j],w3[k],w4[l],st[m]));
//      SetDataFileName(Form("../data/compare_check_test_%.2f_%.2f_%.2f_%.2f_%.0f.pdf",w1[i],w2[j],w3[k],w4[l],st[m]));
//      SetSimFolder(Form("g_%.2f_%.2f_%.2f_%.2f_%.0f",w1[i],w2[j],w3[k],w4[l],st[m]));
      //  SetDataFileName("../data/0V_test.pdf");
      //  SetSimFolder("../data/0V_test");
      //SetDataFileName("../data/m100V_test.pdf");
      //SetSimFolder("../data/m100V_test");
      //SetDataFileName("../data/bf_test_ncn.pdf");
      //SetSimFolder("../data/bf_test_ncn");
    //  SetDataFileName("../data/compare_check_m100.pdf");
//      SetSimFolder("f_0.01_2.00");
    cout<<filename_simu_all[0]<<endl;


      c1->SaveAs(fname_out + "[");
      set_track_par(0);
      // normalize(1);
        c1->Divide(2,2);
      compare_wf_density(); 
      c1->SaveAs(fname_out + "]");
    }
    }
    }

    }
  }
}

void data_sim_compare(TString out_pdf, TString in_folder)
{
      SetDataFileName(out_pdf);
      SetSimFolder(in_folder);
      cout<<filename_simu_all[0]<<endl;

      c1->SaveAs(fname_out + "[");
      set_track_par(0);
      c1->Divide(2,2);
      compare_wf_density(); 
      c1->SaveAs(fname_out + "]");
    TString prefix = out_pdf(0, out_pdf.Last('.'));

    TString new_pdf_name =TString("../data/")+prefix+Form("_chi2_%.5e.pdf",chi2_total);

    std::rename(out_pdf, new_pdf_name);
    cout<<new_pdf_name<<endl;
     std::ofstream outFile("chi2_output.txt");
    if (outFile.is_open()) {
        outFile << chi2_total << std::endl;
        outFile.close();
    } else {
        std::cerr << "Error opening output file!" << std::endl;
    }

 
}
