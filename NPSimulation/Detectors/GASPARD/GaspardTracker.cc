/*****************************************************************************
 * Copyright (C) 2009-2013   this file is part of the NPTool Project         *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: N. de Sereville  contact address: deserevi@ipno.in2p3.fr *
 *                                                                           *
 * Creation Date  : 10/06/09                                                 *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription: This class manages different shapes of module for the Gaspard *
 *             tracker. It allows to have Gaspard geometries with an         *
 *             heterogeneous set of modules                                  *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

// C++ headers
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

// NPTool headers
#include "GaspardTracker.hh"
#include "GaspardTrackerSquare.hh"
#include "GaspardTrackerRectangle.hh"
#include "GaspardTrackerTrapezoid.hh"
#include "GaspardTrackerAnnular.hh"
#include "GaspardTrackerDummyShape.hh"
#include "NPSDetectorFactory.hh"
#include "MaterialManager.hh"

// G4 
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh" 
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4LogicalVolume.hh"

using namespace std;


GaspardTracker::GaspardTracker()
{
}



GaspardTracker::~GaspardTracker()
{
}



// Read stream at Configfile to pick-up parameters of detector (Position,...)
// Called in DetecorConstruction::ReadDetextorConfiguration Method
void GaspardTracker::ReadConfiguration(string Path)
{
  // open configuration file
  ifstream ConfigFile;
  ConfigFile.open(Path.c_str());

  bool GPDTrkSquare     = false;
  bool GPDTrkRectangle  = false;
  bool GPDTrkTrapezoid  = false;
  bool GPDTrkAnnular    = false;
  bool GPDTrkDummyShape = false;
  bool GPDChamber       = false;
  string LineBuffer;
  while (!ConfigFile.eof()) {
    getline(ConfigFile, LineBuffer);
    if (LineBuffer.compare(0, 11, "GPDChamber=") == 0  &&  GPDChamber== false) {
      GPDChamber= true;
      m_Chamber = "MUGAST";
      cout << "Chamber found : " << m_Chamber << endl; 
      // read part of the configuration file corresponding to square elements
    }

    else if (LineBuffer.compare(0, 9, "GPDSquare") == 0  &&  GPDTrkSquare == false) {
      GPDTrkSquare = true;

      // instantiate a new "detector" corresponding to the Square elements
      GaspardTrackerModule* myDetector = new GaspardTrackerSquare();

      // read part of the configuration file corresponding to square elements
      ConfigFile.close();
      myDetector->ReadConfiguration(Path);
      ConfigFile.open(Path.c_str());

      // ms_InterCoord comes from NPS::VDetector
      myDetector->SetInterCoordPointer(ms_InterCoord);

      // store GaspardTrackerSquare "detector"
      m_Modules.push_back(myDetector);
    }
    else if (LineBuffer.compare(0, 12, "GPDRectangle") == 0  &&  GPDTrkRectangle == false) {
      GPDTrkRectangle = true;

      // instantiate a new "detector" corresponding to the Square elements
      GaspardTrackerModule* myDetector = new GaspardTrackerRectangle();

      // read part of the configuration file corresponding to square elements
      ConfigFile.close();
      myDetector->ReadConfiguration(Path);
      ConfigFile.open(Path.c_str());

      // ms_InterCoord comes from NPS::VDetector
      myDetector->SetInterCoordPointer(ms_InterCoord);

      // store GaspardTrackerSquare "detector"
      m_Modules.push_back(myDetector);
    }
    else if (LineBuffer.compare(0, 12, "GPDTrapezoid") == 0  &&  GPDTrkTrapezoid == false) {
      GPDTrkTrapezoid = true;

      // instantiate a new "detector" corresponding to the Trapezoid elements
      GaspardTrackerModule* myDetector = new GaspardTrackerTrapezoid();

      // read part of the configuration file corresponding to trapezoid elements
      ConfigFile.close();
      myDetector->ReadConfiguration(Path);
      ConfigFile.open(Path.c_str());

      // ms_InterCoord comes from NPS::VDetector
      myDetector->SetInterCoordPointer(ms_InterCoord);

      // store GaspardTrackerTrapezoid "detector"
      m_Modules.push_back(myDetector);
    }
    else if (LineBuffer.compare(0, 10, "GPDAnnular") == 0  &&  GPDTrkAnnular == false) {
      GPDTrkAnnular = true;

      // instantiate a new "detector" corresponding to the Trapezoid elements
      GaspardTrackerModule* myDetector = new GaspardTrackerAnnular();

      // read part of the configuration file corresponding to trapezoid elements
      ConfigFile.close();
      myDetector->ReadConfiguration(Path);
      ConfigFile.open(Path.c_str());

      // ms_InterCoord comes from NPS::VDetector
      myDetector->SetInterCoordPointer(ms_InterCoord);

      // store GaspardTrackerTrapezoid "detector"
      m_Modules.push_back(myDetector);
    }
    else if (LineBuffer.compare(0, 13, "GPDDummyShape") == 0  &&  GPDTrkDummyShape == false) {
      GPDTrkDummyShape = true;

      // instantiate a new "detector" corresponding to the Shape elements
      // The GaspardTrackerSquare class should be replaced by the
      // GaspardTrackerShape class you need to define
      GaspardTrackerModule* myDetector = new GaspardTrackerDummyShape();

      // read part of the configuration file corresponding to shape elements
      ConfigFile.close();
      myDetector->ReadConfiguration(Path);
      ConfigFile.open(Path.c_str());

      // ms_InterCoord comes from NPS::VDetector
      myDetector->SetInterCoordPointer(ms_InterCoord);

      // store GaspardTrackerShape "detector"
      m_Modules.push_back(myDetector);
    }
  }
}



// Construct detector and initialize sensitive part.
// Called After DetecorConstruction::AddDetector Method
void GaspardTracker::ConstructDetector(G4LogicalVolume* world)
{
  // loop on sub-detectors belonging to GaspardTracker
  int nbDetectors = m_Modules.size();
  for (int i = 0; i < nbDetectors; i++) m_Modules[i]->ConstructDetector(world);

  // Chambers
  if(m_Chamber=="MUGAST"){

    G4AssemblyVolume* Chamber = BuildMugastChamber();
    G4ThreeVector Pos(0,0,0);
    G4RotationMatrix* R = new G4RotationMatrix();
    Chamber->MakeImprint(world,Pos,R,0);
  }
}



// Connect the GaspardTrackingData class to the output TTree
// of the simulation
void GaspardTracker::InitializeRootOutput()
{
  // loop on sub-detectors belonging to GaspardTracker
  int nbDetectors = m_Modules.size();
  for (int i = 0; i < nbDetectors; i++) m_Modules[i]->InitializeRootOutput();
}



// Initialize all scorers necessary for each detector
void GaspardTracker::InitializeScorers()
{
  // loop on sub-detectors belonging to GaspardTracker
  int nbDetectors = m_Modules.size();
  for (int i = 0; i < nbDetectors; i++) m_Modules[i]->InitializeScorers();
}



// Read sensitive part and fill the Root tree.
// Called at in the EventAction::EndOfEventAction
void GaspardTracker::ReadSensitive(const G4Event* event)
{
  // Before looping on each sub-detector, clear the static variable
  // ms_InterCoord
  // This is done on the first element of the m_Modules vector.
  // This should be done here since this variable (of type TIneractionCoordinates)
  // deals with multiplicity of events > 1.
  m_Modules[0]->GetInterCoordPointer()->Clear();

  // We do the same for the static variable ms_Event
  m_Modules[0]->GetEventPointer()->Clear();

  // loop on sub-detectors belonging to GaspardTracker
  int nbDetectors = m_Modules.size();
  for (int i = 0; i < nbDetectors; i++) m_Modules[i]->ReadSensitive(event);
}


// Build the Mugast Chamber
G4AssemblyVolume* GaspardTracker::BuildMugastChamber(){

  G4AssemblyVolume* Chamber = new G4AssemblyVolume();
  G4Material* Alu = MaterialManager::getInstance()->GetMaterialFromLibrary("Al");

  G4Sphere* Sphere = new G4Sphere("Mugast_sphere",228*mm,230*mm,0,360*deg,115*deg,60*deg);
  G4Sphere* Sphere2 = new G4Sphere("Mugast_sphere2",250*mm,252*mm,0,360*deg,55*deg,20*deg);

  G4UnionSolid* Rear= new G4UnionSolid("MugastChamber_RearSol" , Sphere, Sphere2,0,G4ThreeVector(0,0,-228));
  G4Tubs*   Pipe = new G4Tubs("MugastChamber_PipeSol",15*mm,17*mm,125*mm,0,360*deg);
  G4Tubs*   Middle = new G4Tubs("MugastChamber_MiddleSol",250*mm,252*mm,180*mm,0,360*deg);

  G4LogicalVolume* RearLV = new G4LogicalVolume(Rear,Alu,"MugastChamber_Rear");
  G4LogicalVolume* PipeLV = new G4LogicalVolume(Pipe,Alu,"MugastChamber_Pipe");
  G4LogicalVolume* MiddleLV = new G4LogicalVolume(Middle,Alu,"MugastChamber_Middle");

  // Visual Attribute
  G4VisAttributes* ChamberVisAtt= new G4VisAttributes(G4Colour(0.0,0.4,0.5,0.2));
  RearLV->SetVisAttributes(ChamberVisAtt);
  PipeLV->SetVisAttributes(ChamberVisAtt);
  MiddleLV->SetVisAttributes(ChamberVisAtt);   

  G4ThreeVector PosRear(0,0,0);
  Chamber->AddPlacedVolume(RearLV,PosRear,0); 

  G4ThreeVector PosPipe(0,0,-228*mm-125*mm);
  Chamber->AddPlacedVolume(PipeLV,PosPipe,0); 
  G4ThreeVector PosMiddle(0,0,34.5);
  Chamber->AddPlacedVolume(MiddleLV,PosMiddle,0); 

  return Chamber;
}



////////////////////////////////////////////////////////////////////////////////
//            Construct Method to be pass to the DetectorFactory              //
////////////////////////////////////////////////////////////////////////////////
NPS::VDetector* GaspardTracker::Construct(){
  return  (NPS::VDetector*) new GaspardTracker();
}

////////////////////////////////////////////////////////////////////////////////
//            Registering the construct method to the factory                 //
////////////////////////////////////////////////////////////////////////////////
extern"C" {
  class proxy_nps_gaspard{
    public:
      proxy_nps_gaspard(){
        NPS::DetectorFactory::getInstance()->AddToken("GaspardTracker","GaspardTracker");
        NPS::DetectorFactory::getInstance()->AddDetector("GaspardTracker",GaspardTracker::Construct);
      }
  };

  proxy_nps_gaspard p_nps_gaspard;
}
