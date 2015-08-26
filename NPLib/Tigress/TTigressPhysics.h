#ifndef TTIGRESSPHYSICS_H
#define TTIGRESSPHYSICS_H
/*****************************************************************************
 * Copyright (C) 2009-2014    this file is part of the NPTool Project        *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: Adrien MATTA  contact address: a.matta@surrey.ac.uk      *
 *                  Peter Bender  contact address: bender@triumf.ca          *
 * Creation Date  : November 2012                                            *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription:                                                               *
 *  This class hold Tigress treated data                                     *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *****************************************************************************/
#include <vector>
#include <map>
#include <string>
using namespace std;

// NPL
#include "TTigressData.h"
#include "NPCalibrationManager.h"
#include "NPVDetector.h"

// ROOT
#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"

class TTigressPhysics :  public TObject, public NPL::VDetector{

  public:
    TTigressPhysics();
    ~TTigressPhysics()  { };

    void Clear();
    void Clear(const Option_t*){Clear();}

  public: // inherited from VDetector
    //   Read stream at ConfigFile to pick-up parameters of detector (Position,...) using Token
    void ReadConfiguration(string);

    //   Add Parameter to the CalibrationManger
    void AddParameterToCalibrationManager();      

    //   Activated associated Branches and link it to the private member DetectorData address
    //   In this method mother Branches (Detector) AND daughter leaf (fDetector_parameter) have to be activated
    void InitializeRootInputRaw() ;

    //   Activated associated Branches and link it to the private member DetectorPhysics address
    //   In this method mother Branches (Detector) AND daughter leaf (parameter) have to be activated
    void InitializeRootInputPhysics() {};

    //   Create associated branches and associated private member DetectorPhysics address
    void InitializeRootOutput() ;

    //   This method is called at each event read from the Input Tree. Aime is to build treat Raw dat in order to extract physical parameter. 
    void BuildPhysicalEvent() ;

    //   Same as above, but only the simplest event and/or simple method are used (low multiplicity, faster algorythm but less efficient ...).
    //   This method aimed to be used for analysis performed during experiment, when speed is requiered.
    //   NB: This method can eventually be the same as BuildPhysicalEvent.
    void BuildSimplePhysicalEvent(){BuildPhysicalEvent();} ;

    //   Clear the Event Physics
    void ClearEventPhysics() {Clear();}      
    void ClearEventData() ;

  public:
    void PreTreat();

  private:   //   Root Input and Output tree classes

    TTigressData*         m_EventData;//!
    TTigressData*         m_PreTreatedData;//!
    TTigressPhysics*      m_EventPhysics;//!

  public: // Data Member
    vector<double> Gamma_Energy;
    vector<int> Crystal_Number;
    vector<int> Clover_Number;
    vector<bool>   BGO;
    vector<double>   Gamma_Time;
    vector<double> AddBack_E;   
    vector<double> AddBack_DC;   
  private: // use for anlysis
    map<int,double> m_map_E; //!
    TLorentzVector m_GammaLV; //!
  public:
    TVector3 GetPositionOfInteraction(int i) ;


  public: // Static constructor to be passed to the Detector Factory
    static NPL::VDetector* Construct();
    ClassDef(TTigressPhysics,1)  // TigressPhysics structure
};



#endif
