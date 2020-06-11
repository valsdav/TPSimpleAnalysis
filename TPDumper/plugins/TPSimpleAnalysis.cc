// -*- C++ -*-
//
// Package:    TPSimpleAnalysis/TPSimpleAnalysis
// Class:      TPSimpleAnalysis
//
/**\class TPSimpleAnalysis TPSimpleAnalysis.cc TPSimpleAnalysis/TPSimpleAnalysis/plugins/TPSimpleAnalysis.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Davide Valsecchi
//         Created:  Tue, 19 May 2020 13:18:16 GMT
//
//


// system include files
#include <vector>
#include <string>
#include <TFile.h>
#include <array>
#include <TTree.h>
#include <memory>
#include <iostream>
#include <TMath.h>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"


#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalDigi/interface/EcalTriggerPrimitiveDigi.h"

#include "Geometry/CaloTopology/interface/EcalTrigTowerConstituentsMap.h"
// #include "Geometry/CaloGeometry/interface/CaloGeometry.h"
// #include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
// #include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
// #include "Geometry/Records/interface/CaloGeometryRecord.h"
//
// class declaration
//
using namespace std;

// Auxiliary class
class towerEner {   
 public:
  // raw word with flags in it
  int rawTPData_; 
  // 8-bit compressed energy
  int compressedEt_;
  // energy in GeV
  float eRec_ ;
  int sFGVB_;
  int iphi_, ieta_, nbXtal_ ;
  vector<array<int, 10>> digis_;

  towerEner()
    : rawTPData_(0), compressedEt_(0),eRec_(0), sFGVB_(0),  
      iphi_(-999), ieta_(-999), nbXtal_(0){}
};


// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.

class TPSimpleAnalysis : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit TPSimpleAnalysis(const edm::ParameterSet&);
      ~TPSimpleAnalysis();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
      edm::EDGetTokenT<EcalTrigPrimDigiCollection> tpCollection_ ;
      edm::EDGetTokenT<EBDigiCollection> digiCollectionEB_ ;
      edm::EDGetTokenT<EEDigiCollection> digiCollectionEE_ ;
      bool print_;

      struct EcalTPGVariables
      {
         // event variables
         unsigned int runNb ;
         unsigned int evtNb ;
         unsigned int bxNb ;
         unsigned int orbitNb ;
         unsigned int nbOfActiveTriggers ;

         // tower variables
         unsigned int nbOfTowers ; //max 4032 EB+EE
         vector<int> ieta;
         vector<int> iphi ;
         vector<int> nbOfXtals;
         vector<int> rawTPData ;
         vector<int> compressedTPEt;
         vector<float> eRec;
         vector<int> sFGVB;
         vector<vector<array<int, 10>>> xtalsDigis;

      } ;
      
      TFile * file_;
      TTree * tree_ ;
      EcalTPGVariables treeVariables_ ;

   
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
TPSimpleAnalysis::TPSimpleAnalysis(const edm::ParameterSet& iConfig)
 :
  tpCollection_(consumes<EcalTrigPrimDigiCollection>(iConfig.getParameter<edm::InputTag>("TPcollection"))),
  digiCollectionEB_(consumes<EBDigiCollection>(iConfig.getParameter<edm::InputTag>("DigiCollectionEB"))),
  digiCollectionEE_(consumes<EEDigiCollection>(iConfig.getParameter<edm::InputTag>("DigiCollectionEE"))),
  print_(iConfig.getParameter<bool>("Print"))
{
   // file
   file_ = new TFile(iConfig.getParameter<std::string>("OutputFileName").c_str(),"RECREATE");
   file_->cd() ;

   // tree
   tree_ = new TTree( "tree","EcalTPGAnalysis" );

   tree_->Branch("runNb",&treeVariables_.runNb,"runNb/i"); //
   tree_->Branch("evtNb",&treeVariables_.evtNb,"evtNb/i"); //
   tree_->Branch("bxNb",&treeVariables_.bxNb,"bxNb/i"); //
   tree_->Branch("orbitNb",&treeVariables_.orbitNb,"orbitNb/i"); //
   // tree_->Branch("nbOfActiveTriggers",&treeVariables_.nbOfActiveTriggers,"nbOfActiveTriggers/i"); //
   // tree_->Branch("activeTriggers",treeVariables_.activeTriggers,"activeTriggers[nbOfActiveTriggers]/I"); //

   tree_->Branch("nbOfTowers",&treeVariables_.nbOfTowers,"nbOfTowers/I"); //
   tree_->Branch("ieta", "vector<int>", &treeVariables_.ieta);//
   tree_->Branch("iphi", "vector<int>", &treeVariables_.iphi);
   tree_->Branch("nbOfXtals", "vector<int>", &treeVariables_.nbOfXtals);
   tree_->Branch("rawTPData", "vector<int>", &treeVariables_.rawTPData);
   tree_->Branch("compressedTPEt", "vector<int>", &treeVariables_.compressedTPEt);
   tree_->Branch("eRec", "vector<float>",&treeVariables_.eRec);//
   tree_->Branch("sFGVB", "vector<int>", &treeVariables_.sFGVB);//
   tree_->Branch("xtalsDigis", "std::vector<std::vector<std::array<int, 10>>>", &treeVariables_.xtalsDigis);
}


TPSimpleAnalysis::~TPSimpleAnalysis()
{
  file_->cd();
  tree_->Write();
  file_->Close();
}


//
// member functions
//

// ------------ method called for each event  ------------
void
TPSimpleAnalysis::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace std;

  
      
   if (print_) std::cout<<"==========="<<iEvent.id()<<std::endl ;


   map<EcalTrigTowerDetId, towerEner> mapTower ;
   map<EcalTrigTowerDetId, towerEner>::iterator itTT ;

   ///////////////////////////
   // get Evts info
   ///////////////////////////

   treeVariables_.runNb = iEvent.id().run() ;
   treeVariables_.evtNb = iEvent.id().event() ;
   treeVariables_.bxNb = iEvent.bunchCrossing() ;
   treeVariables_.orbitNb = iEvent.orbitNumber() ;
   
   /////////////////////////// 
   // Get TP data  
   ///////////////////////////

   Handle<EcalTrigPrimDigiCollection> tp;
   iEvent.getByToken(tpCollection_, tp);
   if (print_) std::cout<<"TP collection size="<<tp.product()->size()<<std::endl ;

   for (unsigned int i=0;i<tp.product()->size();i++) {
      EcalTriggerPrimitiveDigi d = (*(tp.product()))[i];
      const EcalTrigTowerDetId TPtowid= d.id();
      towerEner tE ;
      tE.iphi_ = TPtowid.iphi() ;
      tE.ieta_ = TPtowid.ieta() ;
      tE.rawTPData_ = d[0].raw() ;
      tE.compressedEt_ = d.compressedEt();
      // 0.5 GeV precision 
      tE.eRec_ = d.compressedEt() / 2.;
      tE.sFGVB_ = d.sFGVB();
      mapTower[TPtowid] = tE ;
      cout << TPtowid << " TP:" <<  tE.compressedEt_  <<  endl;
   }


   ///////////////////////////
   // Get nb of crystals read out
   ///////////////////////////

   // Get EB xtal digi inputs
   edm::Handle<EBDigiCollection> digiEB;
   iEvent.getByToken(digiCollectionEB_, digiEB);

   for (unsigned int i=0;i<digiEB.product()->size();i++) {
      const EBDataFrame & df = (*(digiEB.product()))[i];    
      const EBDetId & id = df.id();
      const EcalTrigTowerDetId towid = id.tower();
      itTT = mapTower.find(towid) ;
      if (itTT != mapTower.end()) (itTT->second).nbXtal_++ ;
      array<int,10> digis;
      cout << "Digis ";
      for (int i= 0; i<10; i++) {
         digis[i] = df[i] & 0xFFF;
         cout << digis[i]<<" ";
      }
      cout <<endl;
      itTT->second.digis_.push_back(digis);
   }

  
   // Get EE xtal digi inputs
   // edm::Handle<EEDigiCollection> digiEE;
   // iEvent.getByToken(digiCollectionEE_, digiEE);
   // for (unsigned int i=0;i<digiEE.product()->size();i++) {
   //    const EEDataFrame & df = (*(digiEE.product()))[i];
   //    const EEDetId & id = df.id();
   //    const EcalTrigTowerDetId towid = (*eTTmap_).towerOf(id);
   //    itTT = mapTower.find(towid) ;
   //    if (itTT != mapTower.end()) (itTT->second).nbXtal_++ ;
   // }
   

   ///////////////////////////  
   // Get rechits
   ///////////////////////////

   //... to be completed



   ///////////////////////////  
   // fill tree
   ///////////////////////////  

   treeVariables_.nbOfTowers = mapTower.size() ;
   int towerNb = 0 ;
   treeVariables_.ieta.clear();
   treeVariables_.ieta.resize(treeVariables_.nbOfTowers);
   treeVariables_.iphi.clear();
   treeVariables_.iphi.resize(treeVariables_.nbOfTowers);
   treeVariables_.nbOfXtals.clear();
   treeVariables_.nbOfXtals.resize(treeVariables_.nbOfTowers);
   treeVariables_.rawTPData.clear();
   treeVariables_.rawTPData.resize(treeVariables_.nbOfTowers);
   treeVariables_.compressedTPEt.clear();
   treeVariables_.compressedTPEt.resize(treeVariables_.nbOfTowers);
   treeVariables_.eRec.clear();
   treeVariables_.eRec.resize(treeVariables_.nbOfTowers);
   treeVariables_.sFGVB.clear();
   treeVariables_.sFGVB.resize(treeVariables_.nbOfTowers);
   treeVariables_.xtalsDigis.clear();
   treeVariables_.xtalsDigis.resize(treeVariables_.nbOfTowers);

   for (itTT = mapTower.begin() ; itTT != mapTower.end() ; ++itTT) {
      treeVariables_.ieta[towerNb] = (itTT->second).ieta_ ;
      treeVariables_.iphi[towerNb] = (itTT->second).iphi_ ;
      treeVariables_.nbOfXtals[towerNb] = (itTT->second).nbXtal_ ;
      treeVariables_.rawTPData[towerNb] = (itTT->second).rawTPData_ ;
      treeVariables_.compressedTPEt[towerNb] = (itTT->second).compressedEt_;
      treeVariables_.eRec[towerNb] = (itTT->second).eRec_ ;
      treeVariables_.sFGVB[towerNb] = (itTT->second).sFGVB_;
      treeVariables_.xtalsDigis[towerNb] =  (itTT->second).digis_;
      towerNb++ ;
   }

   tree_->Fill() ;
}


// ------------ method called once each job just before starting event loop  ------------
void
TPSimpleAnalysis::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
TPSimpleAnalysis::endJob()
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TPSimpleAnalysis::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  desc.add<edm::InputTag>("TPcollection",edm::InputTag("ecalEBunpacker","EcalTriggerPrimitives"));
  desc.add<edm::InputTag>("DigiCollectionEB",edm::InputTag("ecalEBunpacker","ebDigis"));
  desc.add<edm::InputTag>("DigiCollectionEE",edm::InputTag("ecalEBunpacker","eeDigis"));
  desc.add<std::string>("OutputFileName", "EcalTPGtree.root");
  desc.add<bool>("Print", false);
  
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TPSimpleAnalysis);
