/*****************************************************************************
 * Copyright (C) 2009-2014    this file is part of the NPTool Project        *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: Adrien MATTA  contact address: a.matta@surrey.ac.uk      *
 *                                                                           *
 * Creation Date  : march 2025                                               *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription:                                                               *
 * Class describing the property of an Analysis object                       *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/
#include<iostream>
using namespace std;

#include "Analysis.h"
#include "NPFunction.h"
#include "NPAnalysisFactory.h"
#include "NPDetectorManager.h"
#include "NPOptionManager.h"
////////////////////////////////////////////////////////////////////////////////
Analysis::Analysis(){
}
////////////////////////////////////////////////////////////////////////////////
Analysis::~Analysis(){
}

////////////////////////////////////////////////////////////////////////////////
void Analysis::Init() {
  // initialize input and output branches
  InitOutputBranch();
  InitInputBranch();

  // get MUST2 and Gaspard objects
  M2 = (TMust2Physics*)  m_DetectorManager -> GetDetector("MUST2Array");
  GD = (GaspardTracker*) m_DetectorManager -> GetDetector("GaspardTracker");

  // get reaction information
  myReaction = new NPL::Reaction();
  myReaction->ReadConfigurationFile(NPOptionManager::getInstance()->GetReactionFile());
  OriginalBeamEnergy = myReaction->GetBeamEnergy();

  // target thickness
  TargetThickness = m_DetectorManager->GetTargetThickness()*micrometer;
  string TargetMaterial = m_DetectorManager->GetTargetMaterial();
  // Cryo target case
  WindowsThickness = m_DetectorManager->GetWindowsThickness()*micrometer; 
  string WindowsMaterial = m_DetectorManager->GetWindowsMaterial();
  cout << WindowsThickness << " c " <<  WindowsMaterial << " d " << endl; 
  // energy losses
  string light=NPL::ChangeNameToG4Standard(myReaction->GetNucleus3()->GetName());
  string beam=NPL::ChangeNameToG4Standard(myReaction->GetNucleus1()->GetName());

  LightCD2 = EnergyLoss(light+"_"+TargetMaterial+".G4table","G4Table",100 );
  LightAl = EnergyLoss(light+"_Al.G4table","G4Table",100);
  LightSi = EnergyLoss(light+"_Si.G4table","G4Table",100);
  BeamCD2 = EnergyLoss(beam+"_"+TargetMaterial+".G4table","G4Table",100);

  if(WindowsThickness){
    BeamWindow= new EnergyLoss(beam+"_"+WindowsMaterial+".G4table","G4Table",100); 
    LightWindow=  new EnergyLoss(light+"_"+WindowsMaterial+".G4table","G4Table",100);  
  }

  else{
    BeamWindow= NULL;
    LightWindow=NULL;
  }

  // initialize various parameters
  Rand = TRandom3();
  DetectorNumber = 0;
  ThetaNormalTarget = 0;
  ThetaM2Surface = 0;
  Si_E_M2 = 0;
  CsI_E_M2 = 0;
  Energy = 0;
  E_M2 = 0;

  ThetaGDSurface = 0;
  X_GD = 0;
  Y_GD = 0;
  Z_GD = 0;
  Si_E_GD = 0;
  E_GD = 0;
  Si_X_GD = 0;
  Si_Y_GD = 0;
}

////////////////////////////////////////////////////////////////////////////////
void Analysis::TreatEvent() {
  // Reinitiate calculated variable
  ReInitValue();

  // assume beam is centered and parallel to z axis
  double XTarget = 0;
  double YTarget = 0;
  TVector3 BeamDirection = TVector3(0,0,1);

  // determine beam energy for a randomized interaction point in target
  //  double BeamEnergy = BeamCD2.Slow(OriginalBeamEnergy, Rand.Uniform(0,TargetThickness), 0);
  double BeamEnergy = BeamCD2.Slow(OriginalBeamEnergy, TargetThickness/2., 0);
  
  if(BeamWindow)
    BeamEnergy = BeamWindow->Slow(BeamEnergy,WindowsThickness,0);
  
  myReaction->SetBeamEnergy(BeamEnergy);

  //////////////////////////// LOOP on MUST2 //////////////////
  for(unsigned int countMust2 = 0 ; countMust2 < M2->Si_E.size() ; countMust2++){
    /************************************************/
    //Part 0 : Get the usefull Data
    // MUST2
    int TelescopeNumber = M2->TelescopeNumber[countMust2];

    /************************************************/
    // Part 1 : Impact Angle
    ThetaM2Surface = 0;
    ThetaNormalTarget = 0;
    TVector3 HitDirection = M2 -> GetPositionOfInteraction(countMust2) - BeamImpact ;
    ThetaLab = HitDirection.Angle( BeamDirection );

    ThetaM2Surface = HitDirection.Angle(- M2 -> GetTelescopeNormal(countMust2) );
    ThetaNormalTarget = HitDirection.Angle( TVector3(0,0,1) ) ;

    /************************************************/

    /************************************************/
    // Part 2 : Impact Energy
    Energy = ELab = 0;
    Si_E_M2 = M2->Si_E[countMust2];
    CsI_E_M2= M2->CsI_E[countMust2];

    // if CsI
    if(CsI_E_M2>0 ){
      // The energy in CsI is calculate form dE/dx Table because
      Energy = CsI_E_M2;
      Energy = LightAl.EvaluateInitialEnergy( Energy ,0.4*micrometer , ThetaM2Surface);
      Energy+=Si_E_M2;
    }

    else
      Energy = Si_E_M2;

    // Evaluate energy using the thickness
    ELab = LightAl.EvaluateInitialEnergy( Energy ,0.4*micrometer , ThetaM2Surface);
    // Target Correction
    ELab   = LightCD2.EvaluateInitialEnergy( ELab ,TargetThickness/2., ThetaNormalTarget);

    if(LightWindow)
      ELab = LightWindow->EvaluateInitialEnergy( ELab ,WindowsThickness, ThetaNormalTarget);
    /************************************************/

    /************************************************/
    // Part 3 : Excitation Energy Calculation
    Ex = myReaction -> ReconstructRelativistic( ELab , ThetaLab );
    ThetaLab=ThetaLab/deg;

    /************************************************/

    /************************************************/
    // Part 4 : Theta CM Calculation
    ThetaCM  = myReaction -> EnergyLabToThetaCM( ELab , ThetaLab)/deg;
    /************************************************/
  }//end loop MUST2

  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////
  //////////////////////////// LOOP on GASPARD //////////////////
  if(GD->GetEnergyDeposit()>0){
    /************************************************/
    // Part 1 : Impact Angle
    ThetaGDSurface = 0;
    ThetaNormalTarget = 0;
    if(XTarget>-1000 && YTarget>-1000){
      TVector3 HitDirection = GD -> GetPositionOfInteraction() - BeamImpact ;
      ThetaLab = HitDirection.Angle( BeamDirection );

      ThetaGDSurface = HitDirection.Angle( TVector3(0,0,1) ) ;
      ThetaNormalTarget = HitDirection.Angle( TVector3(0,0,1) ) ;
    }

    else{
      BeamDirection = TVector3(-1000,-1000,-1000);
      ThetaGDSurface    = -1000  ;
      ThetaNormalTarget = -1000  ;
    }

    /************************************************/

    /************************************************/
    // Part 2 : Impact Energy
    Energy = ELab = 0;
    Energy = GD->GetEnergyDeposit();
    // Target Correction

    ELab   = LightCD2.EvaluateInitialEnergy( Energy ,TargetThickness/2., ThetaNormalTarget);
    /************************************************/

    /************************************************/
    // Part 3 : Excitation Energy Calculation
    Ex = myReaction -> ReconstructRelativistic( ELab , ThetaLab );

    /************************************************/

    /************************************************/
    // Part 4 : Theta CM Calculation
    ThetaCM  = myReaction -> EnergyLabToThetaCM( ELab , ThetaLab)/deg;
    ThetaLab=ThetaLab/deg;

    /************************************************/
  }//end loop GASPARD

}

////////////////////////////////////////////////////////////////////////////////
void Analysis::End(){
}
////////////////////////////////////////////////////////////////////////////////
void Analysis::InitOutputBranch() {
  RootOutput::getInstance()->GetTree()->Branch("Ex",&Ex,"Ex/D");
  RootOutput::getInstance()->GetTree()->Branch("ELab",&ELab,"ELab/D");
  RootOutput::getInstance()->GetTree()->Branch("ThetaLab",&ThetaLab,"ThetaLab/D");
  RootOutput::getInstance()->GetTree()->Branch("ThetaCM",&ThetaCM,"ThetaCM/D");
  RootOutput::getInstance()->GetTree()->Branch("Run",&Run,"Run/I");
}


////////////////////////////////////////////////////////////////////////////////
void Analysis::InitInputBranch(){
  RootInput::getInstance()->GetChain()->SetBranchAddress("Run",&Run);
}
////////////////////////////////////////////////////////////////////////////////
void Analysis::ReInitValue(){
  Ex = -1000 ;
  ELab = -1000;
  ThetaLab = -1000;
  ThetaCM = -1000;
}


////////////////////////////////////////////////////////////////////////////////
//            Construct Method to be pass to the AnalysisFactory              //
////////////////////////////////////////////////////////////////////////////////
NPL::VAnalysis* Analysis::Construct(){
  return (NPL::VAnalysis*) new Analysis();
}

////////////////////////////////////////////////////////////////////////////////
//            Registering the construct method to the factory                 //
////////////////////////////////////////////////////////////////////////////////
extern "C"{
class proxy{
  public:
    proxy(){
      NPL::AnalysisFactory::getInstance()->SetConstructor(Analysis::Construct);
    }
};

proxy p;
}

