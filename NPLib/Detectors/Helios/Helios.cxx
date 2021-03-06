/*****************************************************************************
 * Copyright (C) 2009-2014   this file is part of the NPTool Project         *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: Marc Labiche  contact address: marc.labiche@stfc.ac.uk   *
 *                                                                           *
 * Creation Date  : 30/01/12                                                 *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription: This class is mainly an interface to the                      *
 *             THeliosPhysics class and it deals with the geometrical        *
 *             correspondance between strip number and absolute coordinates  *
 *             (X, Y, Z) of interaction.                                     *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "Helios.h"

// C++ headers
#include <iostream>	
#include <fstream>
#include <string>
#include <cmath>
#include <stdlib.h>

// NPL headers
#include "RootInput.h"
#include "RootOutput.h"

// ROOT headers
#include "TChain.h"

using namespace std ;	
	
//	Default Constructor

Helios::Helios()
{
   m_NumberOfModule = 0;
   m_EventData    = new THeliosData();
   m_EventPhysics = new THeliosPhysics();
}	



Helios::~Helios()
{
   m_NumberOfModule = 0;
   delete m_EventData;
   delete m_EventPhysics;
}	



// Read stream at ConfigFile to pick-up parameters of detector (Position,...) using Token
void Helios::ReadConfiguration(string Path) 	
{
   ifstream ConfigFile           	;
   ConfigFile.open(Path.c_str()) 	;
   string LineBuffer          		;
   string DataBuffer          		;	

   // A:X1_Y1     --> X:1    Y:1
   // B:X128_Y1   --> X:128  Y:1
   // C:X1_Y128   --> X:1    Y:128
   // D:X128_Y128 --> X:128  Y:128

   double   Ax, Bx, Cx, Dx, Ay, By, Cy, Dy, Az, Bz, Cz, Dz;
   TVector3 A, B, C, D; 
   double   Theta = 0, Phi = 0, R = 0, beta_u = 0 , beta_v = 0 , beta_w = 0;

   bool check_A = false;
   bool check_C = false;
   bool check_B = false;
   bool check_D = false;

   bool check_Theta = false;
   bool check_Phi   = false;
   bool check_R     = false;
   bool check_beta  = false;
   
   bool ReadingStatus = false;

   bool isSquare     = false;
   bool isDummyShape = false;
   bool isTrapezoid  = false;
   bool isAnnular    = false;

   while (!ConfigFile.eof()) {
      getline(ConfigFile, LineBuffer);

      // If line is a GaspardXXX bloc, reading toggle to true
      // and toggle to true flags indicating which shape is treated.
      //if (LineBuffer.compare(0, 12, "HeliosSquare")     == 0  ||
      //    LineBuffer.compare(0, 15, "HeliosTrapezoid")  == 0  ||
      //    LineBuffer.compare(0, 13, "HeliosAnnular")    == 0  ||
      //    LineBuffer.compare(0, 16, "HeliosDummyShape") == 0) {
      if (LineBuffer.compare(0, 16, "HeliosDummyShape") == 0) {
         cout << "///////////////////////" << endl;
         cout << "Module found:" << endl;        

	 // if (LineBuffer.compare(0,  12, "HeliosSquare")     == 0) isSquare     = true;
	 if (LineBuffer.compare(0, 16, "HeliosDummyShape") == 0) isDummyShape = true;
	 // if (LineBuffer.compare(0, 15, "HeliosTrapezoid")  == 0) isTrapezoid  = true;
	 // if (LineBuffer.compare(0, 13, "HeliosAnnular")    == 0) isAnnular    = true;
         ReadingStatus = true;
      }
      // Else don't toggle to Reading Block Status
      else ReadingStatus = false;

      // Reading Block
      while (ReadingStatus) {	 
         if (isSquare) { 	// square shape
            ConfigFile >> DataBuffer ;
            // Comment Line 
            if (DataBuffer.compare(0, 1, "%") == 0) {
               ConfigFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n' );
            }
            // Finding another telescope (safety), toggle out
            else if (DataBuffer.compare(0, 12, "HeliosSquare") == 0) {
               cout << "WARNING: Another Module is find before standard sequence of Token, Error may occured in Telecope definition" << endl;
               ReadingStatus = false;
            }

            // Position method
            else if (DataBuffer.compare(0, 6, "X1_Y1=") == 0) {
               check_A = true;
               ConfigFile >> DataBuffer ;
               Ax = atof(DataBuffer.c_str()) ;
               Ax = Ax  ;
               ConfigFile >> DataBuffer ;
               Ay = atof(DataBuffer.c_str()) ;
               Ay = Ay  ;
               ConfigFile >> DataBuffer ;
               Az = atof(DataBuffer.c_str()) ;
               Az = Az  ;

               A = TVector3(Ax, Ay, Az);
               cout << "X1 Y1 corner position : (" << A.X() << ";" << A.Y() << ";" << A.Z() << ")" << endl;
            }
            else if (DataBuffer.compare(0, 8, "X128_Y1=") == 0) {
               check_B = true;
               ConfigFile >> DataBuffer ;
               Bx = atof(DataBuffer.c_str()) ;
               Bx = Bx  ;
               ConfigFile >> DataBuffer ;
               By = atof(DataBuffer.c_str()) ;
               By = By  ;
               ConfigFile >> DataBuffer ;
               Bz = atof(DataBuffer.c_str()) ;
               Bz = Bz  ;

               B = TVector3(Bx, By, Bz);
               cout << "X128 Y1 corner position : (" << B.X() << ";" << B.Y() << ";" << B.Z() << ")" << endl;
            }
            else if (DataBuffer.compare(0, 8, "X1_Y128=") == 0) {
               check_C = true;
               ConfigFile >> DataBuffer ;
               Cx = atof(DataBuffer.c_str()) ;
               Cx = Cx  ;
               ConfigFile >> DataBuffer ;
               Cy = atof(DataBuffer.c_str()) ;
               Cy = Cy  ;
               ConfigFile >> DataBuffer ;
               Cz = atof(DataBuffer.c_str()) ;
               Cz = Cz  ;

               C = TVector3(Cx, Cy, Cz);
               cout << "X1 Y128 corner position : (" << C.X() << ";" << C.Y() << ";" << C.Z() << ")" << endl;
            }
            else if (DataBuffer.compare(0, 10, "X128_Y128=") == 0) {
               check_D = true;
               ConfigFile >> DataBuffer ;
               Dx = atof(DataBuffer.c_str()) ;
               Dx = Dx  ;
               ConfigFile >> DataBuffer ;
               Dy = atof(DataBuffer.c_str()) ;
               Dy = Dy  ;
               ConfigFile >> DataBuffer ;
               Dz = atof(DataBuffer.c_str()) ;
               Dz = Dz  ;

               D = TVector3(Dx, Dy, Dz);
               cout << "X128 Y128 corner position : (" << D.X() << ";" << D.Y() << ";" << D.Z() << ")" << endl;
            } // End Position Method

            // Angle method
            else if (DataBuffer.compare(0, 6, "THETA=") == 0) {
               check_Theta = true;
               ConfigFile >> DataBuffer ;
               Theta = atof(DataBuffer.c_str()) ;
               Theta = Theta ;
               cout << "Theta:  " << Theta << endl;
            }
            else if (DataBuffer.compare(0, 4, "PHI=") == 0) {
               check_Phi = true;
               ConfigFile >> DataBuffer ;
               Phi = atof(DataBuffer.c_str()) ;
               Phi = Phi ;
               cout << "Phi:  " << Phi << endl;
            }
            else if (DataBuffer.compare(0, 2, "R=") == 0) {
               check_R = true;
               ConfigFile >> DataBuffer ;
               R = atof(DataBuffer.c_str()) ;
               R = R ;
               cout << "R:  " << R << endl;
            }
            else if (DataBuffer.compare(0, 5, "BETA=") == 0) {
               check_beta = true;
               ConfigFile >> DataBuffer ;
               beta_u = atof(DataBuffer.c_str()) ;
               beta_u = beta_u    ;
               ConfigFile >> DataBuffer ;
               beta_v = atof(DataBuffer.c_str()) ;
               beta_v = beta_v    ;
               ConfigFile >> DataBuffer ;
               beta_w = atof(DataBuffer.c_str()) ;
               beta_w = beta_w    ;
               cout << "Beta:  " << beta_u << " " << beta_v << " " << beta_w << endl  ;
            }

            /////////////////////////////////////////////////
            // If All necessary information there, toggle out
            if ( (check_A && check_B && check_C && check_D) || (check_Theta && check_Phi && check_R && check_beta)  ) {
               ReadingStatus = false; 

               // Add The previously define telescope
               // With position method
               if ( check_A && check_B && check_C && check_D ) {
                  AddModuleSquare(A   ,
                                  B   ,
                                  C   ,
                                  D   ) ;
               }

               // with angle method
               else if ( check_Theta && check_Phi && check_R && check_beta ) {
                  AddModuleSquare(Theta   ,
                                  Phi     ,
                                  R       ,
                                  beta_u  ,
                                  beta_v  ,
                                  beta_w  );
               }

               // reset boolean flag for point positioning
               check_A = false;
               check_B = false;
               check_C = false;
               check_D = false;

               // reset boolean flag for angle positioning
               check_Theta = false;
               check_Phi   = false;
               check_R     = false;
               check_beta  = false;

               // reset boolean flag for shape determination
               isSquare     = false;
               isDummyShape = false;
               isTrapezoid  = false;
               isAnnular    = false;
            } // end test for adding a module
         } // end test for Square shape

        else if (isDummyShape) { 	// dummyshape shape
            ConfigFile >> DataBuffer ;
            // Comment Line 
            if (DataBuffer.compare(0, 1, "%") == 0) {
               ConfigFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n' );
            }
            // Finding another telescope (safety), toggle out
            else if (DataBuffer.compare(0, 16, "HeliosDummyShape") == 0) {
               cout << "WARNING: Another Module is find before standard sequence of Token, Error may occured in Telecope definition" << endl;
               ReadingStatus = false;
            }

            // Position method
            else if (DataBuffer.compare(0, 6, "X1_Y1=") == 0) {
               check_A = true;
               ConfigFile >> DataBuffer ;
               Ax = atof(DataBuffer.c_str()) ;
               Ax = Ax  ;
               ConfigFile >> DataBuffer ;
               Ay = atof(DataBuffer.c_str()) ;
               Ay = Ay  ;
               ConfigFile >> DataBuffer ;
               Az = atof(DataBuffer.c_str()) ;
               Az = Az  ;

               A = TVector3(Ax, Ay, Az);
               cout << "X1 Y1 corner position : (" << A.X() << ";" << A.Y() << ";" << A.Z() << ")" << endl;
            }
            else if (DataBuffer.compare(0, 8, "X128_Y1=") == 0) {
               check_B = true;
               ConfigFile >> DataBuffer ;
               Bx = atof(DataBuffer.c_str()) ;
               Bx = Bx  ;
               ConfigFile >> DataBuffer ;
               By = atof(DataBuffer.c_str()) ;
               By = By  ;
               ConfigFile >> DataBuffer ;
               Bz = atof(DataBuffer.c_str()) ;
               Bz = Bz  ;

               B = TVector3(Bx, By, Bz);
               cout << "X128 Y1 corner position : (" << B.X() << ";" << B.Y() << ";" << B.Z() << ")" << endl;
            }
            else if (DataBuffer.compare(0, 8, "X1_Y128=") == 0) {
               check_C = true;
               ConfigFile >> DataBuffer ;
               Cx = atof(DataBuffer.c_str()) ;
               Cx = Cx  ;
               ConfigFile >> DataBuffer ;
               Cy = atof(DataBuffer.c_str()) ;
               Cy = Cy  ;
               ConfigFile >> DataBuffer ;
               Cz = atof(DataBuffer.c_str()) ;
               Cz = Cz  ;

               C = TVector3(Cx, Cy, Cz);
               cout << "X1 Y128 corner position : (" << C.X() << ";" << C.Y() << ";" << C.Z() << ")" << endl;
            }
            else if (DataBuffer.compare(0, 10, "X128_Y128=") == 0) {
               check_D = true;
               ConfigFile >> DataBuffer ;
               Dx = atof(DataBuffer.c_str()) ;
               Dx = Dx  ;
               ConfigFile >> DataBuffer ;
               Dy = atof(DataBuffer.c_str()) ;
               Dy = Dy  ;
               ConfigFile >> DataBuffer ;
               Dz = atof(DataBuffer.c_str()) ;
               Dz = Dz  ;

               D = TVector3(Dx, Dy, Dz);
               cout << "X128 Y128 corner position : (" << D.X() << ";" << D.Y() << ";" << D.Z() << ")" << endl;
            } // End Position Method

            // Angle method
            else if (DataBuffer.compare(0, 6, "THETA=") == 0) {
               check_Theta = true;
               ConfigFile >> DataBuffer ;
               Theta = atof(DataBuffer.c_str()) ;
               Theta = Theta ;
               cout << "Theta:  " << Theta << endl;
            }
            else if (DataBuffer.compare(0, 4, "PHI=") == 0) {
               check_Phi = true;
               ConfigFile >> DataBuffer ;
               Phi = atof(DataBuffer.c_str()) ;
               Phi = Phi ;
               cout << "Phi:  " << Phi << endl;
            }
            else if (DataBuffer.compare(0, 2, "R=") == 0) {
               check_R = true;
               ConfigFile >> DataBuffer ;
               R = atof(DataBuffer.c_str()) ;
               R = R ;
               cout << "R:  " << R << endl;
            }
            else if (DataBuffer.compare(0, 5, "BETA=") == 0) {
               check_beta = true;
               ConfigFile >> DataBuffer ;
               beta_u = atof(DataBuffer.c_str()) ;
               beta_u = beta_u    ;
               ConfigFile >> DataBuffer ;
               beta_v = atof(DataBuffer.c_str()) ;
               beta_v = beta_v    ;
               ConfigFile >> DataBuffer ;
               beta_w = atof(DataBuffer.c_str()) ;
               beta_w = beta_w    ;
               cout << "Beta:  " << beta_u << " " << beta_v << " " << beta_w << endl  ;
            }

            /////////////////////////////////////////////////
            // If All necessary information there, toggle out
            if ( (check_A && check_B && check_C && check_D) || (check_Theta && check_Phi && check_R && check_beta)  ) {
               ReadingStatus = false; 

               // Add The previously define telescope
               // With position method
               if ( check_A && check_B && check_C && check_D ) {
                  AddModuleDummyShape(A   ,
                                      B   ,
                                      C   ,
                                      D   ) ;
               }

               // with angle method
               else if ( check_Theta && check_Phi && check_R && check_beta ) {
                  AddModuleDummyShape(Theta,
                                      Phi,
                                      R,
                                      beta_u,
                                      beta_v,
                                      beta_w);
               }

               // reset boolean flag for point positioning
               check_A = false;
               check_B = false;
               check_C = false;
               check_D = false;

               // reset boolean flag for angle positioning
               check_Theta = false;
               check_Phi   = false;
               check_R     = false;
               check_beta  = false;

               // reset boolean flag for shape determination
               isSquare     = false;
               isDummyShape = false;
               isTrapezoid  = false;
               isAnnular    = false;
            } // end test for adding a module
         } // end test for DummyShape shape

         else if (isTrapezoid) {	// trapezoid shape
            cout << "Trapezoid treatment not implemented yet!" << endl;
         } // end test for Trapezoid shape

         else if (isAnnular) {		// annular shape
            cout << "Annular treatment not implemented yet!" << endl;
         } // end test for Annular shape

      } // end while for reading block
   } // end while for reading file

   cout << endl << "/////////////////////////////" << endl<<endl;
}


// Read stream at Path and pick-up calibration parameter using Token
// If argument is "Simulation" no change calibration is loaded
void Helios::ReadCalibrationFile(string Path)	
{
   // Order of Polynom function used for calibration
   int Calibration_Si_E_Order;      Calibration_Si_E_Order   = 0; Calibration_Si_E_Order *= 1;
   int Calibration_Si_T_Order;      Calibration_Si_T_Order   = 0; Calibration_Si_T_Order *= 1;
   int Calibration_SiLi_E_Order;    Calibration_SiLi_E_Order = 0; Calibration_SiLi_E_Order *= 1;
   int Calibration_CsI_E_Order;     Calibration_CsI_E_Order  = 0; Calibration_CsI_E_Order *= 1;

   // Calibration_Si_X_E[DetectorNumber][StripNumber][Order of Coeff]
   vector< vector< vector< double > > >	Calibration_Si_X_E	;
   vector< vector< vector< double > > >	Calibration_Si_X_T	;
   vector< vector< vector< double > > >	Calibration_Si_Y_E	;
   vector< vector< vector< double > > >	Calibration_Si_Y_T	;

   // Calibration_SiLi_E[DetectorNumber][PadNumber][Order of Coeff]
   vector< vector< vector< double > > >	Calibration_SiLi_E	;

   // Calibration_SiLi_E[DetectorNumber][CrystalNumber][Order of Coeff]
   vector< vector< vector< double > > >	Calibration_CsI_E	;

   if (Path == "Simulation") {   // Simulation case: data already calibrated
      Calibration_Si_E_Order   = 1;
      Calibration_Si_T_Order   = 1;
      Calibration_SiLi_E_Order = 1;
      Calibration_CsI_E_Order  = 1;

      vector<double> Coef;
      // Order 0				Order 1
      Coef.push_back(0) ; Coef.push_back(1) 	;

      vector< vector<double> > StripLine 		;
      StripLine.resize( 128 , Coef)			;

      Calibration_Si_X_E.resize( m_NumberOfModule , StripLine)	;
      Calibration_Si_X_T.resize( m_NumberOfModule , StripLine)	;
      Calibration_Si_Y_E.resize( m_NumberOfModule , StripLine)	;
      Calibration_Si_Y_T.resize( m_NumberOfModule , StripLine)	;
				
      Calibration_SiLi_E.resize( m_NumberOfModule , StripLine)	;
      Calibration_CsI_E .resize( m_NumberOfModule , StripLine)	;
   }
   else {
   }
}		

	

// Activated associated Branches and link it to the private member DetectorData address
// In this method mother Branches (Detector) AND daughter leaf (fDetector_parameter) have to be activated
void Helios::InitializeRootInput() 		
{
   TChain* inputChain = RootInput::getInstance()->GetChain();
   inputChain->SetBranchStatus("HELIOS", true);
   inputChain->SetBranchStatus("fHelios*", true);
   inputChain->SetBranchAddress("HELIOS", &m_EventData);
}



// Create associated branches and associated private member DetectorPhysics address
void Helios::InitializeRootOutput() 	
{
   TTree* outputTree = RootOutput::getInstance()->GetTree();
   outputTree->Branch("HELIOS", "THeliosPhysics", &m_EventPhysics);
}



// This method is called at each event read from the Input Tree. Aime is to build treat Raw dat in order to extract physical parameter. 
void Helios::BuildPhysicalEvent()		
{
   m_EventPhysics -> BuildPhysicalEvent(m_EventData);
}



// Same as above, but only the simplest event and/or simple method are used (low multiplicity, faster algorythm but less efficient ...).
// This method aimed to be used for analysis performed during experiment, when speed is requiered.
// NB: This method can eventually be the same as BuildPhysicalEvent.
void Helios::BuildSimplePhysicalEvent()	
{
   m_EventPhysics -> BuildSimplePhysicalEvent(m_EventData);
}



void Helios::AddModuleSquare(TVector3 C_X1_Y1,
                                     TVector3 C_X128_Y1,
                                     TVector3 C_X1_Y128,
                                     TVector3 C_X128_Y128)
{
   m_NumberOfModule++;

   // remove warning using C_X128_Y128
   C_X128_Y128.Unit();

   // Vector U on Module Face (paralelle to Y Strip) (NB: remember that Y strip are allong X axis)
   TVector3 U = C_X128_Y1 - C_X1_Y1;
   U = U.Unit();

   // Vector V on Module Face (parallele to X Strip)
   TVector3 V = C_X1_Y128 - C_X1_Y1;
   V = V.Unit();

   // Position Vector of Strip Center
   TVector3 StripCenter = TVector3(0,0,0);
   // Position Vector of X=1 Y=1 Strip 
   TVector3 Strip_1_1;

   // Geometry Parameter
   double Face = 98;		// mm
   double NumberOfStrip = 128;
   double StripPitch = Face/NumberOfStrip; // mm

   // Buffer object to fill Position Array
   vector<double> lineX; 
   vector<double> lineY; 
   vector<double> lineZ;

   vector< vector< double > >	OneModuleStripPositionX;
   vector< vector< double > >	OneModuleStripPositionY;
   vector< vector< double > >	OneModuleStripPositionZ;

   // Moving StripCenter to 1.1 corner:
   Strip_1_1 = C_X1_Y1 + (U+V) * (StripPitch/2.);

   for (int i = 0; i < NumberOfStrip; i++) {
      lineX.clear();
      lineY.clear();
      lineZ.clear();

      for (int j = 0; j < NumberOfStrip; j++) {
         StripCenter  = Strip_1_1 + StripPitch*( i*U + j*V  );
//         StripCenter += -TargetPosition;

         lineX.push_back( StripCenter.X() );
         lineY.push_back( StripCenter.Y() );
         lineZ.push_back( StripCenter.Z() );	
      }

      OneModuleStripPositionX.push_back(lineX);
      OneModuleStripPositionY.push_back(lineY);
      OneModuleStripPositionZ.push_back(lineZ);
   }

   m_StripPositionX.push_back( OneModuleStripPositionX );
   m_StripPositionY.push_back( OneModuleStripPositionY );
   m_StripPositionZ.push_back( OneModuleStripPositionZ );
}



void Helios::AddModuleSquare(double theta,
                                     double phi,
                                     double distance,
                                     double beta_u,
                                     double beta_v,
                                     double beta_w)
{
   m_NumberOfModule++;

   // convert from degree to radian:
   double Pi = 3.141592654;
   theta = theta * Pi/180. ; 
   phi   = phi   * Pi/180. ;

   // Vector U on Module Face (paralelle to Y Strip) (NB: remember that Y strip are allong X axis)
   TVector3 U ;	
   // Vector V on Module Face (parallele to X Strip)
   TVector3 V ;
   // Vector W normal to Module Face (pointing CsI)
   TVector3 W ;
   // Vector position of Module Face center
   TVector3 C ;

   C = TVector3(distance * sin(theta) * cos(phi),
                distance * sin(theta) * sin(phi),
                distance * cos(theta));

  TVector3 YperpC = TVector3( cos(theta) * cos(phi),
                              cos(theta) * sin(phi),
                             -sin(theta));

   W = C.Unit();
   U = W.Cross(YperpC);
   V = W.Cross(U);

   U = U.Unit();
   V = V.Unit();

   U.Rotate( beta_u * Pi/180. , U ) ;
   V.Rotate( beta_u * Pi/180. , U ) ;

   U.Rotate( beta_v * Pi/180. , V ) ;
   V.Rotate( beta_v * Pi/180. , V ) ;

   U.Rotate( beta_w * Pi/180. , W ) ;
   V.Rotate( beta_w * Pi/180. , W ) ;

   double Face = 98; // mm
   double NumberOfStrip = 128;
   double StripPitch = Face/NumberOfStrip; // mm

   vector<double> lineX;
   vector<double> lineY;
   vector<double> lineZ;

   vector< vector< double > >	OneModuleStripPositionX;
   vector< vector< double > >	OneModuleStripPositionY;
   vector< vector< double > >	OneModuleStripPositionZ;
		
   double X, Y, Z;

   // Moving C to the 1.1 corner:
   C.SetX( C.X() - ( Face/2 - StripPitch/2 ) * ( V.X() + U.X() ) )  ; 
   C.SetY( C.Y() - ( Face/2 - StripPitch/2 ) * ( V.Y() + U.Y() ) )  ; 
   C.SetZ( C.Z() - ( Face/2 - StripPitch/2 ) * ( V.Z() + U.Z() ) )  ; 

   for (int i = 0; i < NumberOfStrip; i++) {
      lineX.clear();
      lineY.clear();
      lineZ.clear();

      for (int j = 0; j < NumberOfStrip; j++) {
         X = C.X() + StripPitch * ( U.X()*i + V.X()*j );
         Y = C.Y() + StripPitch * ( U.Y()*i + V.Y()*j );
         Z = C.Z() + StripPitch * ( U.Z()*i + V.Z()*j );

         lineX.push_back(X);
         lineY.push_back(Y);
         lineZ.push_back(Z);
      }

      OneModuleStripPositionX.push_back(lineX);
      OneModuleStripPositionY.push_back(lineY);
      OneModuleStripPositionZ.push_back(lineZ);
   }

   m_StripPositionX.push_back( OneModuleStripPositionX );
   m_StripPositionY.push_back( OneModuleStripPositionY );
   m_StripPositionZ.push_back( OneModuleStripPositionZ );
}



void Helios::AddModuleDummyShape(TVector3 C_X1_Y1,
                                         TVector3 C_X128_Y1,
                                         TVector3 C_X1_Y128,
                                         TVector3 C_X128_Y128)
{
   m_NumberOfModule++;

   // remove warning using C_X128_Y128
   C_X128_Y128.Unit();

   // Vector U on Module Face (paralelle to Y Strip) (NB: remember that Y strip are allong X axis)
   TVector3 U = C_X128_Y1 - C_X1_Y1;
   U = U.Unit();

   // Vector V on Module Face (parallele to X Strip)
   TVector3 V = C_X1_Y128 - C_X1_Y1;
   V = V.Unit();

   // Position Vector of Strip Center
   TVector3 StripCenter = TVector3(0,0,0);
   // Position Vector of X=1 Y=1 Strip 
   TVector3 Strip_1_1;

   // Geometry Parameter
   //Commented by Adrien Matta (produce a warning)
  //  double Face = 50;		// mm
   double NumberOfStrip = 128;
   double StripPitch = 0.98; //Face/NumberOfStrip; // mm

   // Buffer object to fill Position Array
   vector<double> lineX; 
   vector<double> lineY; 
   vector<double> lineZ;

   vector< vector< double > >	OneModuleStripPositionX;
   vector< vector< double > >	OneModuleStripPositionY;
   vector< vector< double > >	OneModuleStripPositionZ;

   // Moving StripCenter to 1.1 corner:
   Strip_1_1 = C_X1_Y1 + (U+V) * (StripPitch/2.);

   for (int i = 0; i < NumberOfStrip; i++) {
      lineX.clear();
      lineY.clear();
      lineZ.clear();

      for (int j = 0; j < NumberOfStrip; j++) {
         StripCenter  = Strip_1_1 + StripPitch*( i*U + j*V  );
//         StripCenter += -TargetPosition;

         lineX.push_back( StripCenter.X() );
         lineY.push_back( StripCenter.Y() );
         lineZ.push_back( StripCenter.Z() );	
      }

      OneModuleStripPositionX.push_back(lineX);
      OneModuleStripPositionY.push_back(lineY);
      OneModuleStripPositionZ.push_back(lineZ);
   }

   m_StripPositionX.push_back( OneModuleStripPositionX );
   m_StripPositionY.push_back( OneModuleStripPositionY );
   m_StripPositionZ.push_back( OneModuleStripPositionZ );
}



void Helios::AddModuleDummyShape(double theta,
                                         double phi,
                                         double distance,
                                         double beta_u,
                                         double beta_v,
                                         double beta_w)
{
   m_NumberOfModule++;

   // convert from degree to radian:
   double Pi = 3.141592654;
   theta = theta * Pi/180. ; 
   phi   = phi   * Pi/180. ;

   // Vector U on Module Face (paralelle to Y Strip) (NB: remember that Y strip are allong X axis)
   TVector3 U ;	
   // Vector V on Module Face (parallele to X Strip)
   TVector3 V ;
   // Vector W normal to Module Face (pointing CsI)
   TVector3 W ;
   // Vector position of Module Face center
   TVector3 C ;

   C = TVector3(distance * sin(theta) * cos(phi),
                distance * sin(theta) * sin(phi),
                distance * cos(theta));

  TVector3 YperpW = TVector3( cos(theta) * cos(phi),
                              cos(theta) * sin(phi),
                             -sin(theta));

   W = C.Unit();
   U = W.Cross(YperpW);
   V = W.Cross(U);

   U = U.Unit();
   V = V.Unit();

   U.Rotate( beta_u * Pi/180. , U ) ;
   V.Rotate( beta_u * Pi/180. , U ) ;

   U.Rotate( beta_v * Pi/180. , V ) ;
   V.Rotate( beta_v * Pi/180. , V ) ;

   U.Rotate( beta_w * Pi/180. , W ) ;
   V.Rotate( beta_w * Pi/180. , W ) ;

   double Face = 1; // mm
   double NumberOfStrip = 1;
   //double NumberOfStrip = 500;
   double StripPitch = 9.8;  // Face/NumberOfStrip; // mm

   vector<double> lineX;
   vector<double> lineY;
   vector<double> lineZ;

   vector< vector< double > >	OneModuleStripPositionX;
   vector< vector< double > >	OneModuleStripPositionY;
   vector< vector< double > >	OneModuleStripPositionZ;
		
   double X, Y, Z;

   // Moving C to the 1.1 corner:
   C.SetX( C.X() - ( Face/2 - StripPitch/2 ) * ( V.X() + U.X() ) )  ; 
   C.SetY( C.Y() - ( Face/2 - StripPitch/2 ) * ( V.Y() + U.Y() ) )  ; 
   C.SetZ( C.Z() - ( Face/2 - StripPitch/2 ) * ( V.Z() + U.Z() ) )  ; 

   for (int i = 0; i < NumberOfStrip; i++) {
      lineX.clear();
      lineY.clear();
      lineZ.clear();

      for (int j = 0; j < NumberOfStrip; j++) {
         X = C.X() + StripPitch * ( U.X()*i + V.X()*j );
         Y = C.Y() + StripPitch * ( U.Y()*i + V.Y()*j );
         Z = C.Z() + StripPitch * ( U.Z()*i + V.Z()*j );

         lineX.push_back(X);
         lineY.push_back(Y);
         lineZ.push_back(Z);
      }

      OneModuleStripPositionX.push_back(lineX);
      OneModuleStripPositionY.push_back(lineY);
      OneModuleStripPositionZ.push_back(lineZ);
   }

   m_StripPositionX.push_back( OneModuleStripPositionX );
   m_StripPositionY.push_back( OneModuleStripPositionY );
   m_StripPositionZ.push_back( OneModuleStripPositionZ );
}



double Helios::GetEnergyDeposit()		
{ 
   if (m_EventPhysics->TotalEnergy.size() > 0)
      return m_EventPhysics->TotalEnergy[0]; 
   else 
      return -1000;
}

double Helios::GetTimeDetected()		
{ 
   if (m_EventPhysics->TotalTime.size() > 0)
      return m_EventPhysics->TotalTime[0]; 
   else 
      return -1000;
}



TVector3 Helios::GetPositionOfInteraction()
{
   TVector3 Position = TVector3(-1000,-1000,-1000);

   if (m_EventPhysics->ModuleNumber.size() == 1) {
      Position = TVector3(GetStripPositionX(m_EventPhysics->ModuleNumber[0], m_EventPhysics->FirstStage_X[0], m_EventPhysics->FirstStage_Y[0]),
                          GetStripPositionY(m_EventPhysics->ModuleNumber[0], m_EventPhysics->FirstStage_X[0], m_EventPhysics->FirstStage_Y[0]),
                          GetStripPositionZ(m_EventPhysics->ModuleNumber[0], m_EventPhysics->FirstStage_X[0], m_EventPhysics->FirstStage_Y[0])); 
   }

   return(Position);
}



void Helios::Print()
{
   cout << "Number of Modules: " << m_NumberOfModule << endl;

   for (int f = 0; f < m_NumberOfModule; f++) {
      cout << "Module " << f+1 << endl;

      for (int i = 0; i < 128; i++) {
         for (int j = 0; j < 128; j++) {
            cout << i+1 << "  "<< j+1 << "  " 
                 << m_StripPositionX[f][i][j]   << "  " 
                 << m_StripPositionY[f][i][j]   << "  "
                 << m_StripPositionZ[f][i][j]   << "  " 
                 << endl ;
         }
      }
   }
}
