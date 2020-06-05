#ifndef Pulse_h
#define Pulse_h

#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TF1.h>
#include <TString.h>
#include <TFormula.h>

#include <iostream>

// #include "PulseParameters.h"

class Pulse{

    std::vector <double> _samples;
    double _A;
    double _t0;
    double _alpha;
    double _beta;
    double _dt = 25.;
    double _xmin= 0.; 
    double _xmax = 750.;
    int _NSamples;
    TF1* _pulseShape;
    void calculateSamples();
    void InitNoise();
    // noise vectors
    std::vector <double> _mC;
    std::vector < std::vector< double > > _mL;

public:
    Pulse(double A, double t_0, double alpha, double beta);
    ~Pulse();
    
    double sample(int i);
    std::vector<double> GetSamples(){ return _samples; };

    void setSamplingInterval(double dt);
    void setRange(double xmin, double xmax);

    void setAmplitude(double A);

    int GetNSamples(){ return _NSamples; }
    double GetAlpha(){ return _alpha; }
    double GetBeta(){ return _beta; }
    double GetT0(){ return _t0; }

    // Noise Correlation matrix
    void SetNoiseCorrelationZero();
    double noise_corr_matrix(int i, int j);


};


#endif
