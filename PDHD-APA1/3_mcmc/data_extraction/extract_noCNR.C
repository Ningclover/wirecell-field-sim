#include "canvas/Utilities/InputTag.h"
#include "gallery/Event.h"
#include "lardataobj/RawData/RawDigit.h"
#include <iostream>
#include <string>
#include <vector>
#include <set>

#include "TFile.h"
#include "TH2F.h"

// APA1 channel ranges
// u: 0 - 800, v: 800 - 1600, w: 1600 - 2560
const int nticks = 5859;
const int channels[] = {0, 800, 1600, 2560};
const char *plane_name[] = {"u", "v", "w"};

void extract_noCNR(std::string const &filename,
                   TString outdir = ".",
                   TString evt_list_str = "439442,456790,456798")
{
    using namespace art;

    // Parse target event list
    std::set<int> target_events;
    TString tok;
    Ssiz_t from = 0;
    while (evt_list_str.Tokenize(tok, from, ",")) {
        target_events.insert(tok.Atoi());
    }

    std::cout << "Target events: ";
    for (auto e : target_events) std::cout << e << " ";
    std::cout << std::endl;

    InputTag raw_tag{"tpcrawdecoder:daq"};
    InputTag nf_tag{"wclsdatahdfilter:raw"};

    std::vector<std::string> filenames(1, filename);

    for (gallery::Event ev(filenames); !ev.atEnd(); ev.next())
    {
        auto aux = ev.eventAuxiliary();
        int run = aux.run();
        int evt = aux.event();

        if (target_events.find(evt) == target_events.end()) continue;

        std::cout << "Processing run=" << run << " event=" << evt << std::endl;

        // Create APA1 histograms for v and w planes (indices 1,2)
        TH2F *h_ANf[3];
        TH2F *h_orig[3];
        for (int j = 0; j < 3; j++) {
            int ch_start = channels[j];
            int ch_end = channels[j+1];
            int nbins = ch_end - ch_start;
            h_ANf[j] = new TH2F(Form("raw_wf_ANF_1_%s", plane_name[j]), "",
                                nbins, ch_start - 0.5, ch_end - 0.5,
                                nticks, 0, nticks);
            h_orig[j] = new TH2F(Form("raw_wf_1_%s", plane_name[j]), "",
                                 nbins, ch_start - 0.5, ch_end - 0.5,
                                 nticks, 0, nticks);
        }

        // Fill original raw digits
        auto const &rawdigits = *ev.getValidHandle<std::vector<raw::RawDigit>>(raw_tag);
        for (auto &rd : rawdigits) {
            int channel = rd.Channel();
            for (int j = 0; j < 3; j++) {
                if (channel > channels[j] && channel < channels[j+1]) {
                    int nSamples = rd.Samples();
                    for (int t = 0; t < nSamples; t++) {
                        h_orig[j]->SetBinContent(channel + 1 - channels[j],
                                                 t + 1,
                                                 rd.ADC(t) - rd.GetPedestal());
                    }
                }
            }
        }

        // Fill noise-filtered digits (noCNR)
        auto const &nfdigits = *ev.getValidHandle<std::vector<raw::RawDigit>>(nf_tag);
        for (auto &rd : nfdigits) {
            int channel = rd.Channel();
            for (int j = 0; j < 3; j++) {
                if (channel > channels[j] && channel < channels[j+1]) {
                    int nSamples = rd.Samples();
                    for (int t = 0; t < nSamples; t++) {
                        h_ANf[j]->SetBinContent(channel + 1 - channels[j],
                                                t + 1,
                                                rd.ADC(t) - rd.GetPedestal());
                    }
                }
            }
        }

        // Save
        TString outname = Form("%s/raw_noCNR_%d.root", outdir.Data(), evt);
        TFile *fout = new TFile(outname, "recreate");
        for (int j = 0; j < 3; j++) {
            h_ANf[j]->Write();
            h_orig[j]->Write();
        }
        fout->Close();
        std::cout << "Saved: " << outname << std::endl;

        for (int j = 0; j < 3; j++) {
            delete h_ANf[j];
            delete h_orig[j];
        }
        delete fout;

        target_events.erase(evt);
        if (target_events.empty()) break;
    }
}
