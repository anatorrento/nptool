#ifndef __PLASTICDATA__
#define __PLASTICDATA__

#include <vector>

#include "TObject.h"



class TPlasticData : public TObject {
 private:
   // ADC
   vector<double>	  fPlastic_Energy	;	
   vector<double>	  fPlastic_Time		;
   vector<short>      fPlastic_Number	;
   
 public:
   TPlasticData();
   virtual ~TPlasticData();

   void	Clear();
   void	Dump();

   /////////////////////           GETTERS           ////////////////////////
   // (E)
   double	GetEnergy(int i)			{return fPlastic_Energy[i];}
   // (T)
   double	GetTime(int i)				{return fPlastic_Time[i];}
   // (N)
   int      GetPlasticNumber(int i)		{return fPlastic_Number[i];}
   /////////////////////           SETTERS           ////////////////////////
   // (E)
   void	SetEnergy(double E)			{fPlastic_Energy.push_back(E);}
   void	SetTime(double T)			{fPlastic_Time.push_back(T);}
   void	SetPlasticNumber(int N)		{fPlastic_Number.push_back(N);}
   
   //
   ClassDef(TPlasticData,1)  // PlasticData structure
};

#endif