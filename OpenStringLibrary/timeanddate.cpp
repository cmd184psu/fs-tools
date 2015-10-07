/***************************************************************
 * (C) Copyright 1998-2011 Chris Delezenski
 * This software is released under the LGPL v2, see COPYING and LGPL
 ***************************************************************/
#include "internal.h"
#ifndef WIN32
cString daysofweek[7]={ "Sunday","Monday","Tuesday","Wednesday",
				"Thursday","Friday","Saturday" };

cString monthsofyear[12]={"January","February","March","April",
                          "May","June","July","August",
                          "September","October","November","December"};
/*****************************************************************************

	cTimeAndDate constructor.  This constructor constructs a date
 	class based on m, d and y.  Originally, I had a ofthemillienium
  	item in the day struct found in the header file.  I took it out
   	for a couple of reasons: it's not required for this particular project
    	and I'm not sure it's right.
     
	The rest of the constructor sets up the calculations for the day struct.
 *****************************************************************************/




cTimeAndDate::cTimeAndDate(int m, int d, int y) {
	thismonth=m;
	thisday.ofthemonth=d;
	thisday.oftheyear=0;
	thisyear=y;
	for(int i=1; i<thismonth; i++)
	   	thisday.oftheyear+=LengthOfMonth(i);
	thisday.oftheyear+=thisday.ofthemonth;

//	thisday.ofthemillenium
	int n=(y-2000)*365+((y-2000!=0)*1+(y-2000)/4);
//	thisday.ofthemillenium
	n+=thisday.oftheyear;
	thisday.oftheweek=(n+5)%7; //plus fudge factor
	
	Refresh();
	
	
}

/*****************************************************************************
	This is the default constructor.  It uses localtime() which is
 	Unix and dos friendly function to get a specific format for 
  	today's date.  That is then copied to dateandtime which is of the form
   		month dayofthemonth time year
     	By carefully stripping this string down by spaces, we can get today's
      date in m d y form, and pass it to the above constructor, which handles
      the correct calculations for the day struct.
      
******************************************************************************/

cTimeAndDate::cTimeAndDate() {
	time_t now;
	time(&now);
	char dateandtime[26];
	strcpy(dateandtime, asctime(localtime(&now)));


        strtok(dateandtime," \t"); // strip off 0th string
        thismonth=GetMonth(strtok(NULL," \t"));
	
        thisday.ofthemonth=cString(strtok(NULL," \t")).GetInt();

	strtok(NULL," \t"); //strip off more garbage so we can get the year
        thisyear=cString(strtok(NULL," \t")).GetInt();
	
	
	
	
	*this=cTimeAndDate(thismonth,thisday.ofthemonth,thisyear);
	
	timeofday[0]=12;
	for(int i=1; i<13; i++) {
		timeofday[i]=i;
		timeofday[i+12]=i;
	}
	Refresh();
	
}


//This is a copy constructor which basically just copies every
//data item, nothing special here.

cTimeAndDate::cTimeAndDate( const cTimeAndDate &d) { //copy constructor
	thisyear=d.thisyear;     //2000 would be 2000 and so on
	thismonth=d.thismonth;  //numerical month between 0-11
	thisday.oftheweek=d.thisday.oftheweek;
	thisday.ofthemonth=d.thisday.ofthemonth;
	thisday.oftheyear=d.thisday.oftheyear;
	hour=d.hour;
	minute=d.minute;
	second=d.second;
//	
//	thisday.ofthemillenium=d.thisday.ofthemillenium;
}

cTimeAndDate::cTimeAndDate(time_t *t) {// copy constructor
	struct tm mytm;
	
	localtime_r(t,&mytm);

	thisyear=mytm.tm_year;     //2000 would be 2000 and so on
	thismonth=mytm.tm_mon+1;  //numerical month between 0-11
	thisday.oftheweek=mytm.tm_wday;
	thisday.ofthemonth=mytm.tm_mday;
	thisday.oftheyear=mytm.tm_yday;
	hour=mytm.tm_hour;
	minute=mytm.tm_min;
	second=mytm.tm_sec;
	
}
//This is an assignment operator, same as above pretty much.
const cTimeAndDate & cTimeAndDate::operator=(const cTimeAndDate &d) {// assignment operator
	thisyear=d.thisyear;     //2000 would be 2000 and so on
	thismonth=d.thismonth;  //numerical month between 0-11
	thisday.oftheweek=d.thisday.oftheweek;
	thisday.ofthemonth=d.thisday.ofthemonth;
	thisday.oftheyear=d.thisday.oftheyear;
	hour=d.hour;
	minute=d.minute;
	second=d.second;
//	thisday.ofthemillenium=d.thisday.ofthemillenium;
	return *this;
}


const cTimeAndDate & cTimeAndDate::operator=(time_t *t) {// assignment operator
	cTimeAndDate d(t);

	thisyear=d.thisyear;     //2000 would be 2000 and so on
	thismonth=d.thismonth;  //numerical month between 0-11
	thisday.oftheweek=d.thisday.oftheweek;
	thisday.ofthemonth=d.thisday.ofthemonth;
	thisday.oftheyear=d.thisday.oftheyear;
	hour=d.hour;
	minute=d.minute;
	second=d.second;
//	thisday.ofthemillenium=d.thisday.ofthemillenium;
	return *this;
}



//This returns the current day of the week, 0th being Sunday, 6th being Saturday
//I could have made this inline, but then I would have to put the daysoftheweek array
//in the header file (not a good idea) or use a switch statement which would put
//this function back here anyway.
cString cTimeAndDate::GetDayOfWeek() const { return daysofweek[thisday.oftheweek]; }


//returns the length of any month, ie length of November would be 30
int cTimeAndDate::LengthOfMonth(int month) const {
        int ret=-1;
	switch(month) {
 		case 9: case 4: case 6: case 11: ret=30; break;
   		case 1: case 3: case 5: case 7: case 8: case 10: case 12: ret=31; break;
		case 2: if(IsLeapYear()) ret=29; else ret=28; break;
  		default: ret=-1; //invalid month
    	}
     	return ret;
}

//compares the current date with d, returns true 
//iff current date is comes before d
bool cTimeAndDate::operator< (const cTimeAndDate &d) const {
	bool ret=false; //assume it's not

	
	if(thisyear<d.thisyear) {
		ret=true;
		//if this year is less than d, then it's true
		//and we're done
	} // end if
	else if(thisyear==d.thisyear) { //that is thisyear !> d,otherwise it's false
 		if(thismonth<d.thismonth) {
 			ret=true;
			//if the month is less than d, then it's true
 		}
  		else if(thismonth==d.thismonth) {
			if(thisday.ofthemonth<d.thisday.ofthemonth) ret=true;
		} //end else month compare
	} //end else year compare
     	return ret;
}

//This function should probably not be in the class
//as it really has nothing to do with the class itself
//it takes on a month, shortens it to the 3 letter, upper case format
//then returns the corresponding number to it.
int cTimeAndDate::GetMonth(const cString &s) const {
	cString m="***";
	m[0]=toupper(s[0]);
	m[1]=toupper(s[1]);
	m[2]=toupper(s[2]);
        int ret=-1; //defaults to error

	if(m=="JAN") ret=1;
	else if(m=="FEB") ret=2;
	else if(m=="MAR") ret=3;
	else if(m=="APR") ret=4;
	else if(m=="MAY") ret=5;
	else if(m=="JUN") ret=6;
	else if(m=="JUL") ret=7;
	else if(m=="AUG") ret=8;
	else if(m=="SEP") ret=9;
	else if(m=="OCT") ret=10;
	else if(m=="NOV") ret=11;
	else if(m=="DEC") ret=12;
	else ret=-1;
 	return ret;
}

//Returns the current month in string form based on the array at the
//top of this page.
cString cTimeAndDate::GetMonthString() const{   //current month
	return monthsofyear[thismonth-1];
}


//This date class is designed to take on values which are considered invalid
//I want the date class to act not only as a date structure, but also
//as a way of finding differences of dates (not really fully implemented yet)
//So this function validates the date based on criteria for this specific project
//and guareentees that IsLeapYear() will work as it is expected to.
bool cTimeAndDate::IsValiDate() const { 

	bool ret=thisyear>1600;

	ret=ret && thismonth>=1 && thismonth<=12;
	ret=ret && GetDay().ofthemonth>=1 && GetDay().ofthemonth<=LengthOfMonth();
	return ret;
}

long unsigned int cTimeAndDate::SecondsSince1970() { // return # of seconds since January 1, 1970
	struct tm timestruct;
	
	timestruct.tm_sec=second;
	timestruct.tm_min=minute;
	timestruct.tm_hour=hour;
	timestruct.tm_mday=thisday.ofthemonth;
	timestruct.tm_mon=thismonth-1;
	timestruct.tm_year=thisyear-1900;
	
	timestruct.tm_wday=0; //thisday.oftheweek;
	timestruct.tm_yday=0; //thisday.oftheweek;
	timestruct.tm_isdst=0;
	time_t t=mktime(&timestruct);
	
	return t;
	
}

bool cTimeAndDate::SetFromString(cString s) {

	//date string must be of the form mmddyy
	//but it can be in mddyy if m<10
	if(s.Length()<5) return false;

	if(s.Length()!=6) s="0"+s;
	
	
	cString m="  ",d="  ",y="  ";
	
	m[0]=s[0];
	m[1]=s[1];
	d[0]=s[2];
	d[1]=s[3];
	y[0]=s[4];
	y[1]=s[5];
	
	cTimeAndDate tempdate(m.AtoI(),d.AtoI(), y.AtoI()+2000);
	*this=tempdate;

	return true;
}
	
cString cTimeAndDate::ToString() { 

	cString m,d,y;
	
	if(thismonth<10) m="0"+cString(thismonth);
	else m=thismonth;

	if(thisday.ofthemonth<10) d="0"+cString(thisday.ofthemonth);
	else d=thisday.ofthemonth;
	
	if(thisyear>=2000) y=thisyear-2000;
	else y=thisyear-1900;
	
	if(y.AtoI()<10) y="0"+y;
	
	return m+d+y;
}

void cTimeAndDate::SetFromDate() {

	cString thedate;
	thedate.FromPopen("/bin/date -I");
	
	int m,d,y;

	y=thedate.ChopAllRt('-').AtoI();
	m=thedate.ChopLf('-').ChopRt('-').AtoI();
	d=thedate.ChopAllLf('-').AtoI();
	(*this)=cTimeAndDate(m,d,y);
	
	thedate.FromPopen("/bin/date");
	
	hour=thedate.ChopLf(' ').ChopLf(' ').TrimLf().ChopLf(' ').TrimLf().ChopAllRt(':').AtoI();
	minute=thedate.ChopLf(':').ChopRt(':').AtoI();
	second=0;
	
	

}

//converts a number into a string depicting a day
cString cTimeAndDate::Number2Day(int i,int st) {
	i=abs(i)+st; //+attrib["STARTDAY"].GetInt();
	if(i>6) i%=7;
	switch (i) {
		case 0:
			return "Sunday";
		break;
		case 1:
			return "Monday";
		break;
		case 2:
			return "Tuesday";
		break;
		case 3:
 			return "Wednesday";
		break;
		case 4:
			return "Thursday";
 		break;
		case 5:
			return "Friday";
		break;
		case 6:
			return "Saturday";
		break;
	}
	return "";
}

//same as previous function, but shorter day strings
cString cTimeAndDate::Number2ShortDay(int i,int st) {
   i=abs(i)+st; //attrib["STARTDAY"].GetInt();
   if(i>6) i%=7;
	switch (i) {
      case 0:
         return "Sun";
      break;
   	case 1:
         return "Mon";
      break;
   	case 2:
         return "Tue";
      break;
   	case 3:
         return "Wed";
      break;
   	case 4:
         return "Thu";
      break;
   	case 5:
         return "Fri";
      break;
   	case 6:
         return "Sat";
      break;
	}
	return "";
}

//Converts a string day into it's number
int cTimeAndDate::Day2Number(cString day,int st) {
	day=day.ToUpper();
	if(day.Contains("SUN")) return ((0-st)+7)%7;
	if(day.Contains("MON")) return ((1-st)+7)%7;
	if(day.Contains("TUE")) return ((2-st)+7)%7;
	if(day.Contains("WED")) return ((3-st)+7)%7;
	if(day.Contains("THU")) return ((4-st)+7)%7;
	if(day.Contains("FRI")) return ((5-st)+7)%7;
	if(day.Contains("SAT")) return ((6-st)+7)%7;
	return -1;
}

void cTimeAndDate::Refresh() {
	timeval tp;
	struct timezone tz;
	gettimeofday(&tp,&tz);
	long int tseconds=tp.tv_sec-(60*60*24*365*30); //seconds since when?
	long int tminutes=tseconds/60; //minutes since when?
	long int thours=tminutes/60;  //hours since when?
	hour=thours%24;
	minute=tminutes%60;
	second=tseconds%60;
	
//	int tz_minuteswest=tz.tz_minuteswest; //from timezone struct
	//int tz_dsttime=tz.tz_dsttime; // from timezone struct
	
//	cerr<<"tz_minuteswest="<<tz_minuteswest<<endl;
//	cerr<<"tz_dsttime="<<tz_dsttime<<endl;
	
	thours-=tz.tz_minuteswest/60;
	
}

cString cTimeAndDate::CurTime() {
	cString h,m,s,e,z="0",c=":";
	h=timeofday[hour];
	if(hour>=12) e="pm"; else e="am";
	m=minute;
	if(minute<10) m=z+m;
//	s=second;
//	if(second<10) s=z+s;
	return h+c+m+e;
}

//set the time and date using the external "date" command for now
bool cTimeAndDate::SetTimeAndDate(int _m, int _d,int _hour, int _min, int _y) {
	cString m=_m,d=_d,y=_y,min=_min,hour=_hour;
	
	
	if(_m<10) m="0"+m;
	
	if(_d<10) d="0"+d;
	
	if(_min<10) min="0"+min;
	
	if(_hour<10) hour="0"+hour;
	
	cString cmdline="/bin/date";
	//if system clock is set to UTC
	//if(_isUTC) cmdline+=" --utc"; 
	cmdline+=" "+m+d+hour+min+y;
	cExec exec;
	return exec.ForkExecFG(cmdline);
}

bool cTimeAndDate::SetTimeZone(cString tz, bool isUTC, bool tostdout, cString external_exec) {
	cTimeAndDate *now;
	cExec exec;
	if(geteuid()==0) {
		now=new cTimeAndDate;
		now->SetUTC(isUTC);
		now->SetTimezone(tz);
		ofstream fout;
		if(tostdout) {
			if(!now->ApplyTimeZone(cout)) return false;
		} else {
			if(!cExec::CopyFile("/usr/share/zoneinfo/"+tz,"/etc/localtime")) {
				return false;
			}
			fout.open("/etc/sysconfig/clock");
			if(!now->ApplyTimeZone(fout)) {
				return false;
			}
			fout.close();
		}
		delete now;
	} else if(external_exec!="") {
		if(!external_exec.Contains(tz)) external_exec+=" -timezone "+tz;
		if(isUTC || external_exec.Contains("isUTC")) external_exec+=" -isUTC";
		if(!exec.ForkExecFG(external_exec)) {
			cerr<<"failed to exec: "<<external_exec<<endl;
			return false;
		}
	} else {
		cerr<<"not root, but no external_exec defined either"<<endl;
		return false;
	}
	return true;
}
#endif
