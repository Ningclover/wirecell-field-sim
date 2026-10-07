#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLine.h"
#include "TStyle.h"
#include "TROOT.h"
#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>

// ---- Track definitions from check_waveform_withdata_2.cc ----
// par_w: {ch_start, ch_end, tick_start, tick_end, nticks, x1, y1, x2, y2}
// par_v: {ch_start, ch_end, tick_start, tick_end, nticks}

struct TrackDef {
    int track_id;
    int event;
    double par_w[9];
    double par_v[5];
    const char *label;
};

TrackDef tracks[] = {
    // track 0: 456798 beam
    {0, 456798,
     {2107, 2261, 4072, 3891, 180,
      2104.951856946355, 4156.626506024097, 2264.511691884457, 3983.132530120482},
     {830, 983, 3735, 3991, 180},
     "track0_456798_beam"},

    // track 10: 439442 beam
    {10, 439442,
     {2082, 2449, 4267, 4007, 150,
      2083.44, 4332.10, 2449.80, 4081.63},
     {1075, 1202, 4257, 4050, 150},
     "track10_439442_beam"},

    // track 1: 439442 cosmic
    {1, 439442,
     {2112, 2334, 3850, 2850, 300,
      2112.250332889481, 3996.1941008563267, 2334.8868175765647, 2987.630827783063},
     {823, 1129, 3320, 3940, 200},
     "track1_439442_cosmic"},

    // track 2: 456790
    {2, 456790,
     {2237, 2310, 4006, 3828, 200,
      2237.535014005602, 4088.5416666666674, 2371.2885154061623, 3715.2777777777787},
     {822, 919, 4083, 3825, 150},
     "track2_456790"},

    // track 3: 456790
    {3, 456790,
     {2453, 2520, 2964, 1967, 300,
      2455.8303886925796, 3040.201005025125, 2525.323910482921, 2060.301507537688},
     {820, 1009, 2238, 2612, 200},
     "track3_456790"},

    // track 4: 456798
    {4, 456798,
     {2115, 2300, 2330, 2675, 200,
      2111.7482517482517, 2416.0237388724036, 2295.244755244755, 2736.4985163204747},
     {1030, 1261, 2335, 2697, 200},
     "track4_456798"},

    // track 5: 456798
    {5, 456798,
     {2388, 2551, 1834, 2777, 200,
      2388.407163053723, 1936.3057324840765, 2551.272384542884, 2866.2420382165606},
     {854, 1030, 1714, 2729, 200},
     "track5_456798"},

    // track 6: 456798
    {6, 456798,
     {2288, 2370, 5095, 3872, 250,
      2288.8784165881243, 5184.713375796178, 2370.3110273327047, 3987.261146496815},
     {931, 992, 5168, 4184, 200},
     "track6_456798"},
};
const int NTRACKS = 8;

// ---- Helper functions (from check_waveform_withdata_2.cc) ----

void fill_range(std::vector<int> &v, int start, int end) {
    v.resize(end - start + 1);
    std::iota(v.begin(), v.end(), start);
}

TH1F *get_power_density(TH2F *hraw, std::vector<int> channels,
                        TString name,
                        int tick_offset, int tick_offset2, int nticks) {
    TH1F *hfft = new TH1F(name, "", nticks, 0, 1. / 0.512);
    hfft->GetXaxis()->SetTitle("Frequency (MHz)");
    hfft->GetYaxis()->SetTitle("|C(#omega)^{2}|");
    double k = double(tick_offset2 - tick_offset) / double(channels.size());
    for (auto channel : channels) {
        int start_bin = k * (channel - channels[0]) + tick_offset;
        TH1F *h2 = new TH1F(Form("pd_ch%d_%s", channel, name.Data()), "", nticks, 0, nticks);
        for (int i = 0; i < nticks; i++) {
            int ibin = hraw->GetXaxis()->FindBin(channel);
            h2->SetBinContent(i + 1, hraw->GetBinContent(ibin, i + 1 + start_bin));
        }
        TH1F *hmag = (TH1F *)h2->FFT(0, "MAG");
        for (int i = 0; i < nticks; i++) {
            hfft->AddBinContent(i + 1, hmag->GetBinContent(i + 1));
        }
        delete hmag;
        delete h2;
    }
    hfft->Scale(1. / channels.size());
    hfft->Scale(1400.0 / (4096 * 4));
    for (int i = 0; i < nticks; i++) {
        hfft->SetBinContent(i + 1, std::pow(hfft->GetBinContent(i + 1), 2));
    }
    return hfft;
}

// align with slope correction (W plane)
TH1F *align2(TH2F *hraw, std::vector<int> channels, TString name,
             int tick_offset, int tick_offset2, int nticks,
             double x1, double x2, double y1, double y2) {
    TH1F *h_align = new TH1F(name, "", 2 * nticks, 200 - nticks, 200 + nticks);
    double k = double(tick_offset2 - tick_offset) / double(channels.size());
    double kk = (y2 - y1) / (x2 - x1);
    for (auto channel : channels) {
        int start_bin = k * (channel - channels[0]) + tick_offset;
        TH1F *h2 = new TH1F(Form("al2_ch%d_%s", channel, name.Data()), "", nticks, 0, nticks);
        for (int i = 0; i < nticks; i++) {
            int ibin = hraw->GetXaxis()->FindBin(channel);
            h2->SetBinContent(i + 1, hraw->GetBinContent(ibin, i + 1 + start_bin));
        }
        int maxbin = kk * (channel - x1) + y1 - start_bin;
        for (int i = 0; i < nticks; i++) {
            h_align->AddBinContent(nticks - maxbin + i, h2->GetBinContent(i + 1));
        }
        delete h2;
    }
    h_align->Scale(1. / channels.size());
    return h_align;
}

// align by peak (V plane)
TH1F *align(TH2F *hraw, std::vector<int> channels, TString name,
            int tick_offset, int tick_offset2, int nticks) {
    TH1F *h_align = new TH1F(name, "", 2 * nticks, 200 - nticks, 200 + nticks);
    double k = double(tick_offset2 - tick_offset) / double(channels.size());
    for (auto channel : channels) {
        int start_bin = k * (channel - channels[0]) + tick_offset;
        TH1F *h2 = new TH1F(Form("al_ch%d_%s", channel, name.Data()), "", nticks, 0, nticks);
        for (int i = 0; i < nticks; i++) {
            int ibin = hraw->GetXaxis()->FindBin(channel);
            h2->SetBinContent(i + 1, hraw->GetBinContent(ibin, i + 1 + start_bin));
        }
        int maxbin = h2->GetMaximumBin();
        for (int i = 0; i < nticks; i++) {
            h_align->AddBinContent(nticks - maxbin + i, h2->GetBinContent(i + 1));
        }
        delete h2;
    }
    h_align->Scale(1. / channels.size());
    return h_align;
}

void make_tr_noCNR(TString outdir = ".") {
    gStyle->SetOptStat(0);

    TCanvas *c1 = new TCanvas("c1", "c1", 1200, 900);
    TString pdf_name = outdir + "/noCNR_tracks.pdf";
    c1->SaveAs(pdf_name + "[");

    for (int t = 0; t < NTRACKS; t++) {
        TrackDef &tr = tracks[t];
        TString rawfile = Form("%s/raw_noCNR_%d.root", outdir.Data(), tr.event);

        std::cout << "=== Track " << tr.track_id << " (" << tr.label
                  << ") from " << rawfile << " ===" << std::endl;

        TFile *fin = TFile::Open(rawfile);
        if (!fin || fin->IsZombie()) {
            std::cout << "ERROR: cannot open " << rawfile << std::endl;
            continue;
        }

        TH2F *h_w = (TH2F *)fin->Get("raw_wf_ANF_1_w");
        TH2F *h_v = (TH2F *)fin->Get("raw_wf_ANF_1_v");
        if (!h_w || !h_v) {
            std::cout << "ERROR: missing histograms in " << rawfile << std::endl;
            fin->Close();
            continue;
        }

        // --- Extract W plane (align2 with slope correction) ---
        int w_ch_s  = tr.par_w[0];
        int w_ch_e  = tr.par_w[1];
        double w_ts = tr.par_w[2];
        double w_te = tr.par_w[3];
        int w_nt    = tr.par_w[4];
        double w_x1 = tr.par_w[5];
        double w_y1 = tr.par_w[6];
        double w_x2 = tr.par_w[7];
        double w_y2 = tr.par_w[8];

        std::vector<int> chans_w;
        fill_range(chans_w, w_ch_s, w_ch_e);
        TString wf_tag = Form("tr%d", tr.track_id);
        TH1F *h_wf_w = align2(h_w, chans_w, wf_tag + "_w_wf",
                               w_ts, w_te, w_nt, w_x1, w_x2, w_y1, w_y2);
        TH1F *h_pd_w = get_power_density(h_w, chans_w, wf_tag + "_w_pd",
                                          w_ts, w_te, w_nt);

        // --- Extract V plane (align by peak) ---
        int v_ch_s  = tr.par_v[0];
        int v_ch_e  = tr.par_v[1];
        double v_ts = tr.par_v[2];
        double v_te = tr.par_v[3];
        int v_nt    = tr.par_v[4];

        std::vector<int> chans_v;
        fill_range(chans_v, v_ch_s, v_ch_e);
        TH1F *h_wf_v = align(h_v, chans_v, wf_tag + "_v_wf",
                              v_ts, v_te, v_nt);
        TH1F *h_pd_v = get_power_density(h_v, chans_v, wf_tag + "_v_pd",
                                          v_ts, v_te, v_nt);

        // --- Save tr_noCNR file ---
        TString trfile = Form("%s/tr_noCNR_%d.root", outdir.Data(), tr.track_id);
        TFile *fout = new TFile(trfile, "recreate");
        h_wf_v->SetName("v_wf"); h_wf_v->Write();
        h_wf_w->SetName("w_wf"); h_wf_w->Write();
        h_pd_v->SetName("v_pd"); h_pd_v->Write();
        h_pd_w->SetName("w_pd"); h_pd_w->Write();
        fout->Close();
        std::cout << "Saved: " << trfile << std::endl;

        // --- Plot: 2x2 layout ---
        c1->Clear();
        c1->Divide(2, 2);

        TLegend *leg = new TLegend(0.55, 0.75, 0.88, 0.88);
        leg->SetBorderSize(0);
        leg->SetTextSize(0.04);

        // top-left: V waveform
        c1->cd(1);
        h_wf_v->SetTitle(Form("V plane wf - %s", tr.label));
        h_wf_v->SetLineColor(kBlue);
        h_wf_v->GetXaxis()->SetTitle("Aligned tick");
        h_wf_v->GetYaxis()->SetTitle("ADC");
        h_wf_v->Draw("hist");

        // top-right: W waveform
        c1->cd(2);
        h_wf_w->SetTitle(Form("W plane wf - %s", tr.label));
        h_wf_w->SetLineColor(kRed);
        h_wf_w->GetXaxis()->SetTitle("Aligned tick");
        h_wf_w->GetYaxis()->SetTitle("ADC");
        h_wf_w->Draw("hist");

        // bottom-left: V power density
        c1->cd(3);
        gPad->SetLogy();
        h_pd_v->SetTitle(Form("V plane PD - %s", tr.label));
        h_pd_v->SetLineColor(kBlue);
        h_pd_v->GetXaxis()->SetRangeUser(0, 1);
        h_pd_v->Draw("hist");

        // bottom-right: W power density
        c1->cd(4);
        gPad->SetLogy();
        h_pd_w->SetTitle(Form("W plane PD - %s", tr.label));
        h_pd_w->SetLineColor(kRed);
        h_pd_w->GetXaxis()->SetRangeUser(0, 1);
        h_pd_w->Draw("hist");

        c1->SaveAs(pdf_name);

        // cleanup
        delete h_wf_v; delete h_wf_w;
        delete h_pd_v; delete h_pd_w;
        delete fout;
        fin->Close();
        delete fin;
    }

    c1->SaveAs(pdf_name + "]");
    std::cout << "PDF saved: " << pdf_name << std::endl;
}
