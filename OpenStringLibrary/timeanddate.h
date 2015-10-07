/***************************************************************
 * (C) Copyright 1998-2011 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/
#ifndef __TIME_AND_DATE__H
#define __TIME_AND_DATE__H

struct sDay {
	   int oftheweek;   //0-6
	   int ofthemonth;  //0-30
	   int oftheyear; //0-365(6)
};


//translates from a string to the numeric month
class cString;
class cTimeAndDate {     //contains information about a particular day
	protected:
		int thisyear;     //2000 would be 2000 and so on
		int thismonth;  //numerical month between 0-11
		sDay thisday;
		bool verbose, debug;
		int timeofday[24];
		int hour,minute,second;
		bool isUTC;
		cString timezone;

	public:
		long unsigned int SecondsSince1970(); // return # of seconds since January 1, 1970

		int GetYear() const      { return thisyear;  }
		int GetMonthInt() const { return thismonth; }  //returns 1 thru 12
		cString GetMonthString() const;  //returns January thru December
  

		//almost an external function, doesn't change the class at all,
		//simply returns the corresponding int value, also used internally.
		int GetMonth(const cString& s) const;

		//returns the day structure
		const sDay GetDay() const  { return thisday;   }
		cString GetDayOfWeek() const;


		//set any part of the date manually
		void SetYear(const int pYear) { thisyear=pYear; }
  		void SetMonth(const int pMonth) { *this=cTimeAndDate(pMonth, thisday.ofthemonth, thisyear); }
    		void SetMonth(const cString &m) { *this=cTimeAndDate(GetMonth(m),thisday.ofthemonth,thisyear); }
    		void SetDay(const int pDay) { *this=cTimeAndDate(thismonth,pDay,thisyear); }
		void SetFromDate();
		bool SetFromString(cString s);

		cString ToString();
		
		//default constructors
		cTimeAndDate(int m, int d, int y);
                cTimeAndDate(const cTimeAndDate& d);
		cTimeAndDate(time_t *t);
		cTimeAndDate();
		const cTimeAndDate & operator=(const cTimeAndDate &d); // assignment operator
		const cTimeAndDate & operator=(time_t *t); // assignment operator


	      //displays the current date in a compact form
	      //example:   	7/16/2000
		const cString TodaysDateCompact() const { return cString(thismonth)+"/"+cString(thisday.ofthemonth)+"/"+cString(thisyear); }

	      //displays the current date in a nice form
	      //example:  "July 16, 2000"
		const cString TodaysDate() const { return GetMonthString()+" "+cString(thisday.ofthemonth)+", "+cString(thisyear); }

		//returns the length of any month, ie limit of July would 31
		int LengthOfMonth(int month) const;
                int LengthOfMonth() const { return LengthOfMonth(thismonth); }

  		int LengthOfYear() const {
    			return int(IsLeapYear())+365; //returns 366 for leapyear, 365 otherwise
                }


		//this function is valid iff IsValiDate returns true
    		bool IsLeapYear() const { return (thisyear%4==0 && thisyear%100!=0) || thisyear%400==0; }
		bool IsValiDate() const;

		//several common operators		
		bool operator< (const cTimeAndDate &d) const;
    		bool operator==(const cTimeAndDate &d) const { return thisyear==d.thisyear && thismonth==d.thismonth && thisday.ofthemonth==d.thisday.ofthemonth; }
  		bool operator<=(const cTimeAndDate &d) const { return *this<d || *this==d; }
  
		bool operator!=(const cTimeAndDate &d) const { return !(*this==d); }
		bool operator> (const cTimeAndDate &d) const { return !(*this<=d); }
        	bool operator>=(const cTimeAndDate &d) const { return !(*this<d); } 
		
		bool Debug() { return debug; }
		void SetDebug(bool _debug) { debug=_debug; }
		bool Verbose() { return verbose; }
		void SetVerbose(bool _verbose) { verbose=_verbose; }
		

		//miltary to real time
		static int Military2RealTime( int h, bool &ispm) {
			if(h==0) {
				ispm=false;
				return 12;
			}
			if(h<=12) {
				ispm=false;
				return h;
			}
			ispm=true;
			return h-12;
		}
		//real time to military time
		static int RealTime2MilitaryTime(int h,bool ispm) {
			if(!ispm && h==12) return 0; //midnight
			if(!ispm || (ispm && h==12)) return h; //am reduces to same time in mil time
			return h+12; //for 1pm on, hours range from 13-23
		}
		//converts a number into a string depicting a day
		static cString Number2Day(int i,int st);
		//same as previous function, but shorter day strings
		static cString Number2ShortDay(int i,int st);
		//Converts a string day into it's number
		static int Day2Number(cString day,int st);
		
		//set the time and date using the external "date" command for now
		static bool SetTimeAndDate(int _m, int _d, int _hour, int _min, int _y);
		static  unsigned long int Years2Days(unsigned long int n) { return n*365; }
		static unsigned long int Days2Hours(unsigned long int n) { return n*24; }
		static unsigned long int Hours2Min(unsigned long int n)     {return n*60; }
		static unsigned long int Min2Sec(unsigned long int n)   { return n*60; }
		static unsigned long int Years2Sec(unsigned long int n) { return Min2Sec(Hours2Min(Days2Hours(Years2Days(n)))); }
		static unsigned long int Days2Years(unsigned long int n)  { return n/365; }
		static  unsigned long int Hours2Days(unsigned long int n)  { return n/24; }
		static unsigned long int Min2Hours(unsigned long int n)   { return n/60; }
		static  unsigned long int Sec2Min(unsigned long int n) { return n/60; }
		static  unsigned long int Sec2Years(unsigned long int n) { return Days2Years(Hours2Days(Min2Hours(Sec2Min(n)))); }
		
		void Refresh();
		cString CurTime();
		int GetHour() { return hour; }
		int GetMinute() { return minute; }
		cString Getampm() { if(hour>11) return "pm"; else return "am"; }
		
		bool IsPm() { return Getampm()=="pm"; }
		bool IsAm() { return !IsPm(); }
		bool IsUTC() { return isUTC; }
		void SetUTC(bool _isUTC) { isUTC=_isUTC; }
		void SetTimezone(cString _tz) { timezone=_tz; }
		bool ApplyTimeZone(std::ostream & fout) {
			fout<<"ZONE=\""<<timezone<<"\""<<endl;
			fout<<"UTC="<<cString::BoolToString(isUTC)<<endl;
			fout<<"ARC=false"<<endl;
			return true;
		}
		static bool SetTimeZone(cString tz, bool isUTC, bool tostdout, cString external_exec="");
		
		
		static cString Ctime() {
			cString temp;
			time_t timer;			//time_t for time functions
			time_t tp;
			timer = time(&tp);
			temp=ctime(&timer);
			return temp;
		}
		
};

#endif

