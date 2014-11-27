#ifndef TCRYSTALHIT_H
#define TCRYSTALHIT_H

#include <cstdio>
#include <vector>

#include <TGRSIDetectorHit.h>


class TCrystalHit : public TGRSIDetectorHit	{

	public:
		TCrystalHit();
		virtual ~TCrystalHit();

	private: 
		int segment;		         //
		int charge;		            //

		double energy;	            //
		double time;		         //
		double cfd;		            //

		std::vector<Short_t> wave;	//

	public:
		
		virtual void Clear(Option_t *opt = "");		//!
		virtual void Print(Option_t *opt = "");		//!

		inline int    GetSegment()  {   return segment;}       //!
		inline int    GetCharge()	{	return charge; }	//!		
		inline double GetEnergy()	{	return energy;	}	//!
		inline double GetTime()		{	return time;	}	//!
		inline double GetCfd()		{	return cfd;	}	//!

		inline void SetSegmentNumber(const int &seg) { segment = seg;   }       //!	
		inline void SetCharge(const int &chg)	{	charge = chg;	}	//!
		inline void SetEnergy(const double &e)	{	energy = e;	}	//!
		inline void SetTime(const double &t)	{	time = t;	}	//!
		inline void SetCfd(const double &c)	{	cfd = c;	}	//!

		inline void SetWave(const std::vector<Short_t> &w)	{	wave = w;	} //!
		inline std::vector<Short_t> *GetWave()	{	return &wave;	}	  //!


	ClassDef(TCrystalHit,2)
};

#endif
