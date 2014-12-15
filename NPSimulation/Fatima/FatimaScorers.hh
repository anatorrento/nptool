/*****************************************************************************
 * Copyright (C) 2009   this file is part of the NPTool Project              *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: M. Labiche  contact address: marc.labiche@stfc.ac.uk     *
 *                                                                           *
 * Creation Date  : 04/01/13                                                 *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription: This class holds all the scorers needed by the                *
 *             GaspardTracker*** objects.                                    *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#ifndef FATIMAScorer_h
#define FATIMAScorer_h 1

#include "G4VPrimitiveScorer.hh"
#include "G4THitsMap.hh"

namespace FATIMASCORERS
{


class FATIMAScorerLaBr3StageDetectorNumber : public G4VPrimitiveScorer
{
public: // with description
   FATIMAScorerLaBr3StageDetectorNumber(G4String name, G4String volumeName, G4int depth = 0);
   virtual ~FATIMAScorerLaBr3StageDetectorNumber();

protected: // with description
   virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);

public:
   virtual void Initialize(G4HCofThisEvent*);
   virtual void EndOfEvent(G4HCofThisEvent*);
   virtual void Clear();
   virtual void DrawAll();
   virtual void PrintAll();

private:
   G4String m_VolumeName;
   G4int HCID;
   G4THitsMap<G4int>* EvtMap;
};



class FATIMAScorerLaBr3StageEnergy : public G4VPrimitiveScorer
{
public: // with description
   FATIMAScorerLaBr3StageEnergy(G4String name, G4String volumeName, G4int depth = 0);
   virtual ~FATIMAScorerLaBr3StageEnergy();

protected: // with description
   virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);

public:
   virtual void Initialize(G4HCofThisEvent*);
   virtual void EndOfEvent(G4HCofThisEvent*);
   virtual void Clear();
   virtual void DrawAll();
   virtual void PrintAll();

private:
   G4String m_VolumeName;
   G4int HCID;
   G4THitsMap<G4double>* EvtMap;
};



class FATIMAScorerLaBr3StageCrystal : public G4VPrimitiveScorer
{
public: // with description
   FATIMAScorerLaBr3StageCrystal(G4String name, G4String volumeName, G4int depth = 0);
   virtual ~FATIMAScorerLaBr3StageCrystal();

protected: // with description
   virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);

public:
   virtual void Initialize(G4HCofThisEvent*);
   virtual void EndOfEvent(G4HCofThisEvent*);
   virtual void Clear();
   virtual void DrawAll();
   virtual void PrintAll();

private:
   G4String m_VolumeName;
   G4int HCID;
   G4THitsMap<G4int>* EvtMap;
};


class FATIMAScorerLaBr3StageTOF : public G4VPrimitiveScorer
{
public: // with description
   FATIMAScorerLaBr3StageTOF(G4String name, G4String volumeName, G4int depth = 0);
   virtual ~FATIMAScorerLaBr3StageTOF();

protected: // with description
   virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);

public:
   virtual void Initialize(G4HCofThisEvent*);
   virtual void EndOfEvent(G4HCofThisEvent*);
   virtual void Clear();
   virtual void DrawAll();
   virtual void PrintAll();

private:
   G4String m_VolumeName;
   G4int HCID;
   G4THitsMap<G4double>* EvtMap;
};



class FATIMAScorerCsIStageDetectorNumber : public G4VPrimitiveScorer
{
public: // with description
   FATIMAScorerCsIStageDetectorNumber(G4String name, G4String volumeName, G4int depth = 0);
   virtual ~FATIMAScorerCsIStageDetectorNumber();

protected: // with description
   virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);

public:
   virtual void Initialize(G4HCofThisEvent*);
   virtual void EndOfEvent(G4HCofThisEvent*);
   virtual void Clear();
   virtual void DrawAll();
   virtual void PrintAll();

private:
   G4String m_VolumeName;
   G4int HCID;
   G4THitsMap<G4int>* EvtMap;
};

// Added by Anna
class FATIMAScorerCsIStageCrystalNumber : public G4VPrimitiveScorer
{
public: // with description
   FATIMAScorerCsIStageCrystalNumber(G4String name, G4String volumeName, G4int depth = 0);
   virtual ~FATIMAScorerCsIStageCrystalNumber();

protected: // with description
   virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);

public:
   virtual void Initialize(G4HCofThisEvent*);
   virtual void EndOfEvent(G4HCofThisEvent*);
   virtual void Clear();
   virtual void DrawAll();
   virtual void PrintAll();

private:
   G4String m_VolumeName;
   G4int HCID;
   G4THitsMap<G4int>* EvtMap;
};


class FATIMAScorerCsIStageEnergy : public G4VPrimitiveScorer
{
public: // with description
   FATIMAScorerCsIStageEnergy(G4String name, G4String volumeName, G4int depth = 0);
   virtual ~FATIMAScorerCsIStageEnergy();

protected: // with description
   virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);

public:
   virtual void Initialize(G4HCofThisEvent*);
   virtual void EndOfEvent(G4HCofThisEvent*);
   virtual void Clear();
   virtual void DrawAll();
   virtual void PrintAll();

private:
   G4String m_VolumeName;
   G4int HCID;
   G4THitsMap<G4double>* EvtMap;
};

}

using namespace FATIMASCORERS;
#endif