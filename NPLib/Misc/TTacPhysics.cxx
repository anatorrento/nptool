/*****************************************************************************
 * Copyright (C) 2009-2010   this file is part of the NPTool Project         *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: L. Lefebvre    contact address: lefebvrl@ipno.in2p3.fr   *
 *                                                                           *
 * Creation Date  : October 2011                                             *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription:                                                               *
 *  This class hold the Tac Physics                                          *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

//   NPL
#include "TTacPhysics.h"
#include "../include/RootOutput.h"
#include "../include/RootInput.h"

//   STL
#include <iostream>
#include <sstream>
#include <fstream>
#include <limits>
#include <stdlib.h>
using namespace std;

//   ROOT
#include "TChain.h"

//   tranform an integer to a string
string itoa(int value)
{
   char buffer [33];
   sprintf(buffer,"%d",value);
   return buffer;
}

ClassImp(TTacPhysics)
///////////////////////////////////////////////////////////////////////////
TTacPhysics::TTacPhysics()
   {      
      EventData = new TTacData ;
      EventPhysics = this ;
   }
   
///////////////////////////////////////////////////////////////////////////
TTacPhysics::~TTacPhysics()
   {}
   
///////////////////////////////////////////////////////////////////////////
void TTacPhysics::Clear()
   {
      TacNumber.clear() ;
      Time_TAC_1 =0;
      Time_TAC_2 =0;
      Time_TAC_3 =0;
      Time_TAC_4 =0;
      Time_TAC_5 =0;
      Time_TAC_6 =0;
      Time_TAC_7 =0;
      Time_TAC_8 =0;
   }
   
///////////////////////////////////////////////////////////////////////////
void TTacPhysics::ReadConfiguration(string Path) 
   {
   }

///////////////////////////////////////////////////////////////////////////
void TTacPhysics::AddParameterToCalibrationManager()
   {
      CalibrationManager* Cal = CalibrationManager::getInstance();
      
      for(int i = 0 ; i < 8 ; i++)
         {
            Cal->AddParameter("TAC", "_T"+itoa(i+1),"TAC_T"+itoa(i+1))   ;
         }
   }
   
///////////////////////////////////////////////////////////////////////////
void TTacPhysics::InitializeRootInputRaw() 
   {
      TChain* inputChain = RootInput::getInstance()->GetChain()     ;
      inputChain->SetBranchStatus ( "TAC"       , true )        ;
      inputChain->SetBranchStatus ( "fTAC_MM*"    , true )        ;
      inputChain->SetBranchAddress( "TAC"       , &EventData )  ;
   }
///////////////////////////////////////////////////////////////////////////
void TTacPhysics::InitializeRootInputPhysics()
   {
      TChain* inputChain = RootInput::getInstance()->GetChain();
      inputChain->SetBranchStatus ( "Tac", true );
      inputChain->SetBranchStatus ( "TacNumber", true );
      inputChain->SetBranchStatus ( "Time_TAC_1", true );
      inputChain->SetBranchStatus ( "Time_TAC_2", true );
      inputChain->SetBranchStatus ( "Time_TAC_3", true );
      inputChain->SetBranchStatus ( "Time_TAC_4", true );
      inputChain->SetBranchStatus ( "Time_TAC_5", true );
      inputChain->SetBranchStatus ( "Time_TAC_6", true );
      inputChain->SetBranchStatus ( "Time_TAC_7", true );
      inputChain->SetBranchStatus ( "Time_TAC_8", true );
      inputChain->SetBranchAddress( "Tac", &EventPhysics );
   }
///////////////////////////////////////////////////////////////////////////
void TTacPhysics::InitializeRootOutput()
   {
      TTree* outputTree = RootOutput::getInstance()->GetTree()            ;
      outputTree->Branch( "Tac" , "TTacPhysics" , &EventPhysics ) ;
   }

///////////////////////////////////////////////////////////////////////////
void TTacPhysics::BuildPhysicalEvent()
   {
      BuildSimplePhysicalEvent()   ;
   }

///////////////////////////////////////////////////////////////////////////
void TTacPhysics::BuildSimplePhysicalEvent()
   {
	for(int i=0;i<8;i++)
	{
	   	TacNumber.push_back(EventData->GetTAC_MM_HF_DetectorNbr(i));
	}
		Time_TAC_1=CalibrationManager::getInstance()->ApplyCalibration("TAC/_T" + itoa( EventData->GetTAC_MM_HF_DetectorNbr(0) ),EventData->GetTAC_MM1_HF() );
		Time_TAC_2=CalibrationManager::getInstance()->ApplyCalibration("TAC/_T" + itoa( EventData->GetTAC_MM_HF_DetectorNbr(1) ),EventData->GetTAC_MM2_HF() );
		Time_TAC_3=CalibrationManager::getInstance()->ApplyCalibration("TAC/_T" + itoa( EventData->GetTAC_MM_HF_DetectorNbr(2) ),EventData->GetTAC_MM3_HF() );
		Time_TAC_4=CalibrationManager::getInstance()->ApplyCalibration("TAC/_T" + itoa( EventData->GetTAC_MM_HF_DetectorNbr(3) ),EventData->GetTAC_MM4_HF() );
		Time_TAC_5=CalibrationManager::getInstance()->ApplyCalibration("TAC/_T" + itoa( EventData->GetTAC_MM_HF_DetectorNbr(4) ),EventData->GetTAC_MM5_HF() );
		Time_TAC_6=CalibrationManager::getInstance()->ApplyCalibration("TAC/_T" + itoa( EventData->GetTAC_MM_HF_DetectorNbr(5) ),EventData->GetTAC_MM6_HF() );
		Time_TAC_7=CalibrationManager::getInstance()->ApplyCalibration("TAC/_T" + itoa( EventData->GetTAC_MM_HF_DetectorNbr(6) ),EventData->GetTAC_MM7_HF() );
		Time_TAC_8=CalibrationManager::getInstance()->ApplyCalibration("TAC/_T" + itoa( EventData->GetTAC_MM_HF_DetectorNbr(7) ),EventData->GetTAC_MM8_HF() );
   }
