
#include "PMNS_Fast.h"
#include "PMNS_TaylorExp.h"
#include "PremModel.h"

// Some functions to make nice plots
#include "SetNiceStyle.C"

// Check the accuracy of the
// oscillation averaging function
void testAvgProb(){

  // Set nice overall style
  SetNiceStyle();

  // Get a PMNS object
  OscProb::PMNS_Fast p;
  OscProb::PMNS_TaylorExp taylor;

  // Set the baseline through the earth
  double L = 2*6368 + 18;
  p.SetLength(L);
  taylor.SetLength(L);

  // PREM Model
  OscProb::PremModel prem;    

  double cosT = -0.7;

  // Fill path for cosT
  prem.FillPath(cosT);

  // Give path to calculator
  p.SetPath(prem.GetNuPath());
  taylor.SetPath(prem.GetNuPath());

  


  // Define some fine and coarse binnings
  int navg = 20;
  int nbins = navg * 100;

  // Lots of histograms
  TH1D* h1 = new TH1D("","",nbins,0,3);
  TH1D* h2 = new TH1D("","",navg,0,3);
  TH1D* h3 = new TH1D("","",navg,0,3);
  TH1D* h4 = new TH1D("","",navg,0,3);
  TH1D* h5 = new TH1D("","",navg,0,3);

  // Do some fine binning and uniform sampling
  for(int i=1; i<=nbins; i++){

    double minE  = pow(10, h1->GetBinLowEdge(i));
    double maxE  = pow(10, h1->GetBinLowEdge(i+1));

    double E = 0.5 * (minE + maxE);
    double dE = (maxE - minE);

    int abin = h3->FindFixBin(log10(E));

    double prob = p.Prob(1,1, E);

    h1->SetBinContent(i, prob);
    h3->AddBinContent(abin, prob * dE);

  }

  // Do the AvgProb sampling and bin center sampling
  for(int i=1; i<=navg; i++){

    double minE  = pow(10, h2->GetBinLowEdge(i));
    double maxE  = pow(10, h2->GetBinLowEdge(i+1));

    double E  = (maxE + minE) / 2;
    double dE = (maxE - minE);

    h2->SetBinContent(i, taylor.avgProbTaylor(1,1, E, dE));
    //h2->SetBinContent(i, p.AvgProb(1,1, E, dE));

    h3->SetBinContent(i, h3->GetBinContent(i) / dE);
    h4->SetBinContent(i, p.Prob(1,1, E));

    // Store the number of samples per bin
    if(minE < 0.1*E) minE = 0.1 * E;

    double LoE  = (L/minE + L/maxE)/2;
    double dLoE = (L/minE - L/maxE);

    int nsamples = (p.GetSamplePoints(LoE, dLoE)).size();

    h5->SetBinContent(i, nsamples);

  }

  // Print the sampling statistics
  int sumsample = h5->Integral();
  cout << "Total samples needed: " << sumsample << endl;
  cout << "Mean samples per bin: " << double(sumsample) / navg << endl;

  // Make a long canvas
  MakeLongCanvas();

  // Set some nice histograms
  SetHist(h1, kBlack);
  SetHist(h2, kBlue);
  SetHist(h3, kGreen);
  SetHist(h4, kRed);

  // Change line styles
  h3->SetLineStyle(7);

  h2->SetLineWidth(3);
  h3->SetLineWidth(3);
  h4->SetLineWidth(5);

  // The axis titles
  h1->SetTitle(";Log10[Neutrino Energy (GeV)];P(#nu_{#mu}#rightarrow#nu_{#mu})");

  // Draw different samplings
  h1->DrawCopy("curv");
  h4->DrawCopy("hist same ][");
  h2->DrawCopy("hist same ][");
  h3->DrawCopy("hist same ][");

  MiscText(0.8, 0.86, 0.04, TString::Format("nbrBinAvg = %0.1d", navg) );
  MiscText(0.8, 0.82, 0.04, TString::Format("cos#theta_{z} = %0.1f", cosT) );

  TLegend* leg = new TLegend(0.7,0.6,0.9,0.8);

  leg->AddEntry(h2, "P(#nu_{#mu}#rightarrow#nu_{#mu}) - avg taylor", "l");
  leg->AddEntry(h3, "P(#nu_{#mu}#rightarrow#nu_{#mu}) - avg ", "l");
  leg->AddEntry(h4, "P(#nu_{#mu}#rightarrow#nu_{#mu}) - centre bin", "l");

  SetLeg(leg);

  leg->Draw();

  /*
  // Make a new canvas
  gPad->DrawClone();

  // Set a nice histogram
  SetHist(h5, kBlue);

  // The axis titles
  h5->SetTitle(";Log10[Neutrino Energy (GeV)];# of samples needed");

  // Set the plot scale
  double smax = 1.2 * h5->GetMaximum();
  h5->GetYaxis()->SetRangeUser(0, smax);
  h1->Scale(smax);

  // Draw the number of sample points
  h5->DrawCopy("hist ][");

  // Draw the scaled oscillation
  // probability behind it
  h1->DrawCopy("hist ][ same");
  h5->DrawCopy("hist ][ same");

  */
}
