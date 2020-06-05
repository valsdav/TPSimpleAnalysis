#include "TPSimpleAnalysis/TPDumper/interface/Pulse.h"
#include <math.h>
#include <iostream>
#include <TF1.h>

Pulse::Pulse(double A, double t_0, double alpha, double beta):
    _A(A),_t0(t_0),_alpha(alpha),_beta(beta)
{
    // alpha-beta function
    _pulseShape = new TF1("pulse", 
        "( (1 + (x-[1]) / ([2]*[3]) ) > 0.0)*([0] * pow( 1 + (x-[1]) / ([2]*[3]) , [2] ) * exp ( - (x-[1]) / [3])) + ((1 + (x-[1]) / ([2]*[3]) <= 0.0 ))*(0)",
        _xmin, _xmax);
    _pulseShape->SetParameter(0, _A);
    _pulseShape->SetParameter(1, _t0);
    _pulseShape->SetParameter(2, _alpha);
    _pulseShape->SetParameter(3, _beta);

    calculateSamples();
    InitNoise();
}

Pulse::~Pulse(){
    _samples.clear();
}


// Sample the pulseShape at fixed intervals
void Pulse::calculateSamples()
{   
    _samples.clear();
    int s = 0;
    for(double i= 0.; i<= _xmax; i+=_dt){
        if (i<=(_t0 - _alpha*_beta)) 
            _samples.push_back(0);
        else 
            _samples.push_back(_pulseShape->Eval(i));
        s+=1;
    }
    _NSamples = _samples.size();
}

double Pulse::sample(int i){
    return _samples.at(i);
}

void Pulse::setSamplingInterval(double dt)
{
    _dt = dt;
    calculateSamples();
}

// If the range is changed the shape formula has to be redefined
void Pulse::setRange(double xmin, double xmax)
{
    _xmin = xmin;
    _xmax = xmax;
    delete _pulseShape;
    _pulseShape = new TF1("pulse", 
        "( (1 + (x-[1]) / ([2]*[3]) ) > 0.0)*([0] * pow( 1 + (x-[1]) / ([2]*[3]) , [2] ) * exp ( - (x-[1]) / [3])) + ((1 + (x-[1]) / ([2]*[3]) <= 0.0 ))*(0)",
        _xmin, _xmax);
    _pulseShape->SetParameter(0, _A);
    _pulseShape->SetParameter(1, _t0);
    _pulseShape->SetParameter(2, _alpha);
    _pulseShape->SetParameter(3, _beta);

    calculateSamples();
    InitNoise();
}

void Pulse::setAmplitude(double A)
{
    _A = A;
    _pulseShape->SetParameter(0, _A);
    calculateSamples();
}


void Pulse::InitNoise(){
    _mC.clear();
    for(int i=0; i<_NSamples; i++){
        // the tau of the falling tail if  _beta with alpha-beta formula
        double y = 1. - exp( -double(_dt * i) / (sqrt(2.) * _beta));
        _mC.push_back( 1. - y * y);
    }

    _mL.clear();
      // initialize
    for(int i=0; i<_NSamples; ++i){
        std::vector<double> temp_mL;
        for(int j=0; j<_NSamples; ++j){
        temp_mL.push_back(0);
        }
        _mL.push_back(temp_mL);
    }
    
    _mL.at(0).at(0) = sqrt(_mC.at(0));
    for( int col=1; col<_NSamples; col++){
        _mL.at(0).at(col) = 0;
    }
    for( int row=1; row<_NSamples; row++){
        for( int col=0; col<row; col++ ){
        double sum1 = 0;
        int m=abs(row-col);
        for( int k=0; k<col; ++k) sum1 += _mL.at(row).at(k)*_mL.at(col).at(k);
        _mL.at(row).at(col) = (_mC.at(m) - sum1)/_mL.at(col).at(col);
        }
        double sum2 = 0;
        for( int k=0; k<row; ++k) sum2 += _mL.at(row).at(k)*_mL.at(row).at(k);
        //     std::cout << "  ----> _mC.at(0) - sum2 = " << _mC.at(0) << " - " << sum2 << " = " << _mC.at(0) - sum2 << std::endl;
        
        _mL.at(row).at(row) = sqrt( _mC.at(0) - sum2 );
        for( int col=row+1; col<_NSamples; col++ ) _mL.at(row).at(col) = 0;
    }
}


void Pulse::SetNoiseCorrelationZero() {
  _mC.clear();
  _mC.push_back(1.0);
  for(int i=1; i<_NSamples; i++){
    _mC.push_back(0.0);
  }

  _mL.clear();
  // initialize
  for(int i=0; i<_NSamples; ++i){
    std::vector<double> temp_mL;
    for(int j=0; j<_NSamples; ++j){
      temp_mL.push_back(0);
    }
    _mL.push_back(temp_mL);
  }
  for(int i=0; i<_NSamples; i++){
    _mL.at(i).at(i) = 1;
  }
}


double Pulse::noise_corr_matrix( int i, int j ) {
  return _mL.at(i).at(j);
}



