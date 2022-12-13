/**** File_Header *************************************************************
 *
 *  $Header$
 *
 *  Author    : Jan-Otto Bakkland    EFI-20     DATE: 920903
 *  Copyright : Norwegian Electric Power Research Institute
 *  Compiler  : gcc
 *
 *  Keywords  : 
 *  Contents  : 
 *  This file contains the function definitions for the time
 *  library routines.
 *
 *  Includes and defines:
 */
#ifndef	_TIME_API_H     /* once only test to avoid multiple inclusions       */
#define	_TIME_API_H

#include <time.h>
#include "icc.h"
/*	Externals and statics:
 */
typedef char TIME_c[18];
typedef const char* TIME_cp;
typedef int TIME_i[7];
typedef int TIME_w[3];
enum Season { Winter, Spring, Summer, Autumn};

/*	Function prototypes
 */
#ifdef __cplusplus
extern "C" {
#endif

void   TIME_formdate(const char *date_string, const char *form, char *return_date_string);
double TIME_timediff(const char *starttime, const char *endtime, int unit_code);
void   TIME_timeadd(const char *starttime, char *endtime, int unit_code, long add);
void   TIME_dtimeadd(char *starttime, char *endtime, int unit_code, double add);
void   TIME_today(char *date_string, int unit_code );
int    TIME_datetoken(const char *in_datestring,  char *out_datestring);
int    TIME_form_token(char *date_string, char *form, char *return_date_string);
int    TIME_weekday_to_num(char *weekday);
int    TIME_weekname_to_num(char *weekname);
int    TIME_monthname_to_num(char *monthname);
int    TIME_string_digit(char *string);
void   TIME_string_to_lower(char *string);
int    TIME_num_day_year(int Year);
int    TIME_num_day_month( int Year, int Month);
int    TIME_check_date(const char *date_string);
int    TIME_check_date_u(char *date_string);
int    TIME_timetoken(char *in_timestring,  char *out_datestring);
void   TIME_find_date_order(char **date_array, int unit_code, long *offset_array);
int    TIME_datestr_to_datestruct(char *date_string, struct tm *date_struct);
int    TIME_datestruct_to_datestr(char *date_string, struct tm *date_struct);
int    TIME_num_week_in_year(int year);
int    TIME_day_week1(int year);
int    TIME_first_day_in_week(char *indate, char *outdate);
int    TIME_check_date_values(int Year,int Month,int Day,int Hour,int Minute,int Second,int Millisec);
void   TIME_houradd(            /* Adds "HourAdd" to date 1 and calculate new date 2.*/
	int  Year1,                   /* [1900..2100]		     */
	int  Month1,                  /* [1..12]		     */
	int  Day1,                    /* [1..31]		     */
	int  Hour1,                   /* [1..24]		     */
	int *Year2,
	int *Month2,
	int *Day2,
	int *Hour2,
	long HourAdd                  /* [-N..N] No. of. hours to be added/sub. date 1*/
	);
	
int   TIME_hourdiff(            /* Calculate no.of. hours between dat/time 1 and 2*/
	int Year1,                    /* [1900..2100]		     */
	int Month1,                   /* [1..12]		     */
	int Day1,                     /* [1..31]		     */
	int Hour1,                    /* [0..24]		     */
	int Year2,                    /*			     */
	int Month2,                   /*			     */
	int Day2,                     /*			     */
	int Hour2,                    /*			     */
	long *HourDiff                /* hour difference, date/time 2 minus date/time 1*/
	);
void  TIME_daydiff(             /* Calculate no.of. days between to dates            */
	int Year1,                    /* [1900..2100]		     */
	int Month1,                   /* [1..12]		     */
	int Day1,                     /* [1..31]		     */
	int Year2,                    /* [1900..2100]		     */
	int Month2,                   /* [1..12]		     */
	int Day2,                     /* [1..31]		     */
	long *DayDiff                 /* [-N..N] No.of days between date 1 and 2*/ 
	);
void  TIME_dayno(               /* Calc. weekday,dayno and days since monday 1/1-1900*/
	int  Year,                    /* [1900..2100]		     */
	int  Month,                   /* [1..12]		     */
	int  Day,                     /* [1..31]		     */
	int *WeekDay,                 /* [1..7], 1 = monday	     */
	int *DayNo,                   /* [1..366] from 1.jan. in Year*/
	long *SerialDayNo             /* [0..n] No.of. days since monday 1.jan.1900*/
  );
void  TIME_ydate(               /* Make day and month from the dayno. in the Year    */
	int  DayNo,                   /* [1..366] from 1.jan. in Year*/
	int  Year,                    /* [1900..2100]		     */
	int *Month,                   /* [1..12]		     */
	int *Day                      /* [1..31]		     */
	);
void  TIME_sdate(               /* Make year, day and month from serialdayno*/
	long  SerialDayNo,            /* [1..n] no.of. days since monday 1.jan.1900*/
	int  *Year,                   /* [1900..2100]		     */
	int  *Month,                  /* [1..12]		     */
	int  *Day                     /* [1..31]		     */
	);
void  TIME_wdate(               /* Make date from a given WeekDay, WeekNo and Year*/
	int  WeekDay,                 /* [1..7], 1 = monday	     */
	int  WeekNo,                  /* [1..53]		     */
	int  WeekYear,                /* [1900..2100] year of weekno.*/
	int *Month,                   /* [1..12]		     */
	int *Day,                     /* [1..31]		     */
	int *DateYear                 /* [1900..2100] year of date */
	);
	
void  TIME_easter(              /* Calculate the date of easter sunday*/
	int  Year,                    /* [1900..2100]		     */
	int *Month,                   /* [1..12]		     */
	int  *Day                     /* [1..31]		     */
	);
void  TIME_weekno(              /* Calculate weekno. and weekday of a given date*/
	int  Year,                    /* [1900..2100]		     */
	int  Month,                   /* [1..12]		     */
	int  Day,                     /* [1..31]		     */
	int *WeekDay,                 /* [1..7], 1 = monday	     */
	int *WeekNo,                  /* [1..53]		     */
	int  *YearHome                /* [0,1,-1] 0 = This year,   */
	);                            /*  1 = Next year, ex. 31.jan. is weekno 1           */
                                /* -1 = Previous year, ex. 1.jan is weekno 53        */
 
int  TIME_leap(	/* [0,1] return: 1 = leap year,0 =  no leap year*/
               int Year				/* [1900..2100]		     */
	);

TIME_cp TIME_c_wdate( int year,
                      int week,
                      int dayno);
long	TIME_i_hourdiff( TIME_i s_time
	                 , TIME_i e_time);
long TIME_c_hourdiff( const TIME_c s_time
  , const TIME_c e_time);
void	TIME_i_date	( int date_i[3]);
void	TIME_w_date	( TIME_w date_w);
void	TIME_i_clock	( int clock_i[4]);
void	TIME_i_now	( TIME_i time_i);
TIME_cp	TIME_c_now	();
void	TIME_i2w	( int date_i[3]
		  , TIME_w week_day_i);
void	TIME_c2i	( const TIME_c c_date
		  , TIME_i i_date);

void	TIME_c2w	( const TIME_c date_c
		  , TIME_w date_w);
TIME_cp	TIME_i2c	( TIME_i i_date
		  , TIME_c c_date);
TIME_cp	TIME_w2c	( TIME_w date_w);
void	TIME_w_weekadd	( TIME_w in_week
		  , long no_week
		  , TIME_w out_week);
int
TIME_week_start_and_end(int year, int week, int day,
                        char *startdate, char *enddate);
int
TIME_YEARWEEK_add(int yearweek, int n);

int
TIME_offset_add(char *date, long *offset);

int 
TIME_offset_from_env(long *offs, int *iucode, long *diffs);
int
TIME_find_offset(char *date1, char *date2, long *offset );

int
TIME_find_curr_date(char *date);
int
TIME_add_curr_diffs(char *date);
int
TIME_add_curr_offset(char *date);

int
TIME_week_desc(char *date, ...);

char *
TIME_dls_start(int year);

char *
TIME_dls_end(int year);
char *
TIME_dls_start_hour(int year);

char *
TIME_dls_end_hour(int year);

int
TIME_date_dls_status(const char *date);

double
TIME_timediff_local(const char *starttime, const char *endtime,  int unit_code);
void
TIME_timeadd_local(const char *starttime, char *endtime, int unit_code, long add);
const char *
TIME_local_to_gmt(const char *date);
const char *
TIME_gmt_to_local(const char *date);
const char *
TIME_normal_to_gmt(const char *date);
int
TIME_get_gmtoffset();
const char *
TIME_local_to_normal(const char *date); /* Check function header for INFO */
const char *
TIME_normal_to_local(const char *normalDate); /* Check function header for INFO */
const char *
TIME_local_to_normal_SourceIsLocal(const char *date); /* Check function header for INFO */
const char *
TIME_normal_to_local_SourceIsNormal(const char *normalDate); /* Check function header for INFO */
time_t
TIME_time_t_norm_loc(const char *normdate,int *found);
int
TIME_daylight_norm_offset(char *normfdate, char *normtodate, char *offset,int n);
int
TIME_get_norm_dls(const char *ndate);
int
TIME_find_next_dls_change(time_t tfrom,int maxnbhour,int *nhour,time_t *ret_t,int *dls);

long TIME_JUL_tojul(int year, int month, int day);
void TIME_JUL_fromjul(long jul, int*, int*, int*);
int  TIME_JUL_dayofweek(long jul);
char *
TIME_dayname(int day, int day_len);

int TIME_datecmp(const char date1[], const char date2[]);


/* The following from time_holiday.c: */
/*  Check if day[/time] is work hours or not. Hour = -1 -> check date only. */
int TIME_holiday(int year, int month, int day, int hour, int minute);
/*  Check if day[/time] is work hours or not. Hour = -1 -> check date only. */
int TIME_workhours(int year, int month, int day, int hour, int minute);
/*  Check of day is "unusual" day off (ie, not weekend). */
int TIME_nwholiday(int year, int month, int day);
/*  Check if day is holiday or day before and after is holiday */
int TIME_specholiday(int year, int month, int day, int *spec);
/* Find season corresponding to date. */
enum Season TIME_get_season(int year, int month, int day);
/* Load a specified holiday description */
int TIME_init_holidays( const char* holidays_file );
void TIME_get_workhours(int* from, int* to);

/* The following are from expand_year.c */
int TIME_expand_year(int shortyear);	/* expand year 2 -> 4 digits */
int TIME_set_limit(int lower_limit);	/* set lower limit (default=50) */

TIME_cp timemacro(int argc, char** argv, const int unattendedExecution); /* timeconv args are used */

#ifdef __cplusplus
}
#endif

#endif				/* end for include only once */

/*****************************************************************************/
