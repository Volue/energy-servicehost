// OTL.NET.h

#pragma once

#include "otl.h"

using namespace System;
using namespace System::Collections;
using namespace System::Runtime::InteropServices;
using namespace System::Threading;
using namespace Powel::Icc::Services::Time;

// #define SUPRESS_LOG4CXX_IN_OTLUTIL

#include "MarshalUtils.h"
#include <list>
#include "winerror.h"

using namespace clix;

/** The OTLNET module provides a .NET assembly for the OTL/OCI Oracle library,
 *  - eliminates the need for ODP.NET and::
 *  This package solves problem with multiple-sessions due to different technology
 *  (.NET/ODP  not needed )
 *  and it provides deterministic cleanup of resources and memory related to
 *  database operation (possible Xtreme high volume of objects).
 * First experimental version by SiH.  
 **/
namespace OTLNET {
	public enum class ExceptionCategory {
			OTL_EXCEPTION,
			CALENDARMISSING_EXCEPTION,
			CONNECTFAIL_EXCEPTION
		};

	/** Exception needs to be handled and transformed into .NET
	*   exceptions.*/
	public ref class dbexception:public System::Exception {
		!dbexception() {
		}
	public:
		ExceptionCategory category;
		dbexception() {}
		dbexception(otl_exception *ex):Exception(gcnew String(ex?((char*)ex->msg):"")),category(ExceptionCategory::OTL_EXCEPTION) {
			if(ex) {
				Source = "OTLNET/Oracle";
				HResult = MAKE_HRESULT(SEVERITY_ERROR,FACILITY_NULL,ex->code);
				if(ex->stm_text) Data["OTL.Statement"] = gcnew String((char*)ex->stm_text);
				if(ex->code != 0) Data["OTL.ErrorNumber"]= gcnew String(int(ex->code).ToString());
				if(ex->sqlstate[0]) Data["OTL.SqlState"]= gcnew String(ex->sqlstate);
				if(ex->error_offset) Data["OTL.ErrorOffset"]= gcnew String(int(ex->error_offset).ToString());
			} else {
				;
			}
		}
		dbexception(const char *internalMsg,ExceptionCategory categoryEx)
			: Exception(gcnew String(internalMsg?internalMsg:"<nullmsg>")),category(categoryEx) {
			Source = "OTLNET/Oracle";
			HResult = MAKE_HRESULT(SEVERITY_ERROR,FACILITY_NULL,1);//hmm. 
			if(category== ExceptionCategory::CALENDARMISSING_EXCEPTION)
				Data["OTL.Statement"] = gcnew String("Generic: Attempt to read timestamp/date from db without using calendar");
			else if(category==ExceptionCategory::CONNECTFAIL_EXCEPTION) {
				Data["OTL.Statement"] = gcnew String("Generic: Failed to get connection from pool within timelimit");
			}
		}
		~dbexception() {
			this->!dbexception();
		}
	};


	/** Experimental generic (not finished) ala otl_value<> */
	generic < class T >
	public ref struct dbvalue {
		dbvalue( T x):v(x),dbnull(false) {
		}
		dbvalue() {
			dbnull=true;
		}
		T v;
		bool dbnull;
		bool is_null() { return dbnull;}
		void set_null() { dbnull=true;}
		void set_null(bool x) { dbnull=x;}
	};
	generic <typename T>
	public ref class dbvector {
	   public:
		   dbvector(int size,bool setNull) {
			  v=gcnew array<T>(0);
			  isNull=gcnew   array<bool>(0);
			  set_len(size,setNull);
		   }
		array<T>^ v;
		array<bool>^ isNull;
		void set_len(int l,bool setNull) {
			array<T>^ nwV= gcnew array<T>(l);
			array<bool>^ nwIsNull= gcnew array<bool>(l);
			for each (bool e in nwIsNull) {
				e=setNull;
			}
			for(int i=0;!(v == nullptr) && i<l && i < v->Length;i++ ){
				nwV[i]=v[i];
			}
			delete v;
			delete isNull;
			v=nwV;
			isNull=nwIsNull;
		}
		int len() {
			return v==nullptr?0:v->Length;

		}
		property T default[int] {
			T get(int i) { return v[i];}
			void set(int i,T x) { v[i]=x;}
		}
		//T% operator[](int i) { return v[i];}
		bool is_null(int i) { return isNull[i];}
		void set_null(int i) { isNull[i]=true;}
		void set_non_null(int i) {isNull[i]=false;}
	    
	};

	/** dbconnect class alias otl_connect */
#define DBTRY   try {
#define DBCATCH	} catch (otl_exception& e) { throw gcnew dbexception(&e);}
	ref class dbpool; // fwd decl.
	public ref class dbconnect{
	private:
		otl_connect *db;
		dbpool^ pool;// optionally the dbpool that owns db.
		otl_connect *clone(otl_connect *x) {
			if(!x) return x;
			otl_connect *r= new otl_connect();
			return r;
		}
		dbconnect% operator=(dbconnect% x) { // managed =
			if(db) delete db;
			db=clone(x.db);
			return *this;
		}	
		dbconnect(dbconnect const % x) {// managed cp ct.
			db=clone(x.db);
		}
	protected:
		!dbconnect();
	protected public:// needed by dbpool.
		dbconnect(dbpool^ pool,otl_connect *dbx);
		otl_connect * detach() { otl_connect *x=db; db=0;return x;}
	public:
		/* create a connection from pool, maxWaitMs to wait, if timeout, then exception */
		dbconnect(dbpool^ poolA,int maxWaitMs);
		/* create a connection with no pool assoc */
		dbconnect():pool(nullptr) {
			db= new otl_connect();
		}

		~dbconnect() {
			this->!dbconnect();
		}
		otl_connect& getDb() {return *db;}
		/** function needed to support same access in .NET and native c++ */
		__int64 getDbHandle() { return (__int64)db;}
		bool server_attach(String^ tnsName)  {
			std::string tnsNameX= marshalString<E_ANSI>(tnsName);//(char*)Marshal::StringToHGlobalAnsi(tnsName).ToPointer();
			DBTRY;
				db->server_attach(tnsNameX.c_str());
			DBCATCH;
			return true;
		}
		void session_begin(String^ user,String^ passwd,bool autoCommit,int sessionMode ) {
			DBTRY;
				db->session_begin(marshalString<E_ANSI>(user).c_str(),marshalString<E_ANSI>(passwd).c_str());
			DBCATCH;
		}
		void session_begin(String^ user,String^ passwd,bool autoCommit) {
			session_begin(user,passwd,autoCommit,OCI_DEFAULT);
		}
		void session_begin(String^ user,String^ passwd) {
			session_begin(user,passwd,false,OCI_DEFAULT);
		}
		void session_reopen() {
			DBTRY;
			db->session_reopen(0);// never use auto commit
			DBCATCH;
		}
		void commit() {
			DBTRY;
			db->commit();
			DBCATCH;
		}
		void commit_nowait() {
			DBTRY;
			db->commit_nowait();
			DBCATCH;
		}
		void rollback() {
			DBTRY;
			db->rollback();
			DBCATCH;
		}
		bool session_end() {
			bool res=true;
			DBTRY;
				db->session_end();
			DBCATCH;
			return res;
		}
		bool server_detach() {
			DBTRY;
			db->server_detach();
			DBCATCH;
			return true;
		}
		/**Set the maximum buffer size for operations with
		 * Large Objects: varchar_long, raw_long, clob and blob. 
		 * This function needs to be called in order to increase 
		 * the buffer size (default size is 32767).  */
		void set_max_long_size(int amax_size) {
			DBTRY;
			db->set_max_long_size(amax_size);
			DBCATCH;
		}
		int direct_exec(String^ sqlstmX,int ignore_error) {
			DBTRY;
			return db->direct_exec(marshalString<E_ANSI>(sqlstmX).c_str(),ignore_error);
			DBCATCH;
		}
		int direct_exec(String^ sqlstmX) {
			DBTRY;
			return direct_exec(sqlstmX,otl_exception::enabled);
			DBCATCH;
		}
		void cancel() {
			DBTRY;
			db->cancel();
			DBCATCH;
		}
		bool connected() {
			DBTRY;
			return db->connected?true:false;
			DBCATCH;
		}
	};
	public ref class dbpool {
		std::list<otl_connect*> *connectionPool;
		int minN;
		int maxN;
		Semaphore^ resourcePool;
		String^ tnsName;
	protected:
		/** detatch any remaining connections, release memory, and zero out connectionPool*/
		void clearPool() {
			for(std::list<otl_connect*>::iterator i= connectionPool->begin();i != connectionPool->end(); ++i) {
				if( (*i) ) {
					try {
					(*i)->server_detach();
					} catch(otl_exception) {
						//todo: what to do here..
						// in debug: throw exeception
						// in release: just ignore ?
					}
					delete (*i);
				}
			}
			delete connectionPool;connectionPool=0;
		}
		!dbpool() {
			clearPool();
		}
	public:
		dbpool(int minN_,int maxN_,String^tnsName_):minN(minN_),maxN(maxN_) {
			connectionPool= new std::list<otl_connect*>();
			tnsName= tnsName_;
			for(int i=0;i<minN;i++) {
				otl_connect * db=new otl_connect();
				try {
					db->server_attach(marshalString<E_ANSI>(tnsName).c_str());
					connectionPool->push_back(db);
				} catch (otl_exception) {
					// questionable: should we rethrow here
					// or should we wait until user really tries to get a connect?
				}
			}
			resourcePool = gcnew Semaphore(maxN,maxN);
		}
		~dbpool() {
			this->!dbpool();
		}
	internal:
		otl_connect *get(int maxWait_ms) {
			otl_connect *r=0;
			if( resourcePool->WaitOne(TimeSpan(0,0,0,0,maxWait_ms),true) ) {
				if(connectionPool->empty()) {
					r= new otl_connect();
					r->server_attach(marshalString<E_ANSI>(tnsName).c_str());
				} else {
					if( Monitor::TryEnter(this,maxWait_ms) ){
						// protect list
						r= connectionPool->front();
						connectionPool->pop_front();
						Monitor::Exit(this);
					} else {
						resourcePool->Release();
					}
				}
			}
			return r;
		}
	public:
		bool  get([Runtime::InteropServices::Out]dbconnect^% db,int maxWait_ms) {
			otl_connect *r  = get(maxWait_ms);
			if(r) {
				db= gcnew dbconnect(this,r);

				return true;
			} else {
				db=nullptr;
				return false;
			}
		}
	internal:
		void  put(dbconnect^ db) {
			otl_connect *x=db->detach();
			if(x) {
				Monitor::Enter(this);
				if((int) connectionPool->size() < maxN) {
					connectionPool->push_back(x);
					Monitor::Exit(this);
					resourcePool->Release();
				} else {//excessive list of dbconnections. waste this one
					Monitor::Exit(this);
					// release db.
					try {
						x->server_detach();
					} catch( otl_exception&) {
						//--hmm.
						// debug: throw
						// release: silently ignore.
					}
					delete x;
				}
			}
		}
	};
	/** internal only, called from dbpool when creating connections */
	dbconnect::dbconnect(dbpool^ poolA,otl_connect *r):pool(poolA),db(r) {}

	/** public, called when create connection from pool */
	dbconnect::dbconnect(dbpool^ poolA,int maxWaitMs):pool(poolA) {
		if(pool) {
			db= pool->get(maxWaitMs);
			if(!db) {
				throw gcnew dbexception("Failed to get a connection from pool within timelimit",ExceptionCategory::CONNECTFAIL_EXCEPTION);
			}
		} else { // pool is a null ptr, then just create a connection? or maybe throw exception ?
			db= new otl_connect();
		}
	}
	dbconnect::!dbconnect() {
		if(pool) {
			pool->put(this);// the pool calls back using detatch, and takes care of the db.
		} else {
			delete db;
		}
		db=0;
	}

	/** dbstream class encapsulates the otl_stream class,
	 *  except for the << and >> operators which is named
	 *  put() and get() due to limitations in CLR and 
	 *  operator overloading */
		public ref class dbstream {
		private:
			/** ref. to otl_stream, never null */
			otl_stream *s;
			/** copy ct/assignment not supported */
			dbstream% operator=(dbstream%){ return *this;}  
			dbstream(const dbstream % x ):s(0) {}
			RegionalCalendar^ dbc;
			RegionalCalendar^ dbCalendar() {
				if(dbc==nullptr)  {
					throw gcnew dbexception("Attempt to read UtcTime from dbstream without supplying Calendar",ExceptionCategory::CALENDARMISSING_EXCEPTION);
					//dbc= RegionalCalendar::UtcCalendar;
				}
				return dbc;
			}
		public:
			dbstream(int buffSz,String^ sql,dbconnect^ db) {
				DBTRY;
				s= new otl_nocommit_stream(buffSz,marshalString<E_ANSI>(sql).c_str(),db->getDb());
				DBCATCH;
			}
			dbstream(int buffSz,String^ sql,dbconnect^ db, RegionalCalendar^ cal) {
				DBTRY;
				dbc=cal;
				s= new otl_nocommit_stream(buffSz,marshalString<E_ANSI>(sql).c_str(),db->getDb());
				DBCATCH;
			}
			dbstream(int buffSz,String^ sql,dbconnect^ db, String^ ref_cur_placeholder) {
				DBTRY;
				s= new otl_nocommit_stream(buffSz,marshalString<E_ANSI>(sql).c_str(),
						db->getDb(),
						marshalString<E_ANSI>(ref_cur_placeholder).c_str()
						);
				DBCATCH;
			}
			dbstream(int buffSz,String^ sql,dbconnect^ db, String^ ref_cur_placeholder,RegionalCalendar^cal) {
				DBTRY;
				dbc=cal;
				s= new otl_nocommit_stream(buffSz,marshalString<E_ANSI>(sql).c_str(),
						db->getDb(),
						marshalString<E_ANSI>(ref_cur_placeholder).c_str()
						);
				DBCATCH;
			}
			dbstream(int buffSz,String^ sql,dbconnect^ db, String^ ref_cur_placeholder,String^ sqlstm_label) {
				DBTRY;
				s= new otl_stream(buffSz,marshalString<E_ANSI>(sql).c_str(),
					db->getDb(),
					marshalString<E_ANSI>(ref_cur_placeholder).c_str(),
					marshalString<E_ANSI>(sqlstm_label).c_str()
					);
				s->set_commit(0);// turn off commit
				DBCATCH;
			}
			dbstream(int buffSz,String^ sql,dbconnect^ db, String^ ref_cur_placeholder,String^ sqlstm_label, RegionalCalendar^cal) {
				DBTRY;
				dbc=cal;
				s= new otl_stream(buffSz,marshalString<E_ANSI>(sql).c_str(),
					db->getDb(),
					marshalString<E_ANSI>(ref_cur_placeholder).c_str(),
					marshalString<E_ANSI>(sqlstm_label).c_str()
					);
				s->set_commit(0);// turn off commit
				DBCATCH;
			}

			void flush() {
				DBTRY;
				s->flush();
				DBCATCH;
			}
			void flush(int row_offset) {
				DBTRY;
				s->flush(row_offset,false);
				DBCATCH;
			}
			void flush(int row_offset,bool force_flush) {
				DBTRY;
				s->flush(row_offset,force_flush);
				DBCATCH;
			}
			int get_stream_type() {
				DBTRY;
				return s->get_stream_type();
				DBCATCH;
			}
			void clean() {
				DBTRY;
				s->clean();
				DBCATCH;
			}
			void clean(int clean_up_error_flag) {
				DBTRY;
				s->clean(clean_up_error_flag);
				DBCATCH;
			}
			void rewind() {
				DBTRY;
				s->rewind();
				DBCATCH;
			}
			int get_rpc() {
				DBTRY;
				return s->get_rpc();
				DBCATCH;
			}
			/** Set the stream auto-commit flag. 
			 * When the output buffer is flushed, the current 
			 * transaction is automatically commited, if the flag
			 * is set. By default, the flag is NOT set. In order to 
			 * enable "auto-committing",
			 * set the flag using this function. The stream 
			 * auto-commit flag has nothing to do with the database 
			 * auto-commit mode. The auto-commit is specific to 
			 * the otl_stream class.
			 * It is more convenient to have the stream 
			 * "auto-commit off" by default, so the otl_nocommit_stream 
			 * is  used. otl_nocommit_stream is a class derived directly from otl_stream with
			 * auto-commit turned off by default, so it does not commit transactions. 
			 */
			void set_commit(bool autocommit) {
				DBTRY;
				s->set_commit(autocommit?1:0);
				DBCATCH;
			}
			void set_flush(bool auto_flush) {
				DBTRY;
				s->set_flush(auto_flush);
				DBCATCH;
			}
			void close() {
				DBTRY;
				s->close();
				DBCATCH;
			}
			bool good() {
				DBTRY;
				return s->good()?true:false;
				DBCATCH;
			}
			~dbstream() {
				this->!dbstream();
			}
			!dbstream() {
				if(s) delete s;
				s=0;
			}

			dbstream^ put(int x) {
				DBTRY;
				(*s)<<x;
				DBCATCH;
				return this;
			}
			dbstream^ put(double x) {
				DBTRY;
				(*s)<<x;
				DBCATCH;
				return this;
			}
			dbstream^ put(UtcTime x) {
				return put(x,dbCalendar());
			}
			dbstream^ put(UtcTime x,RegionalCalendar^ cal) {
				DBTRY;
				otl_value<otl_datetime> X;
				if(x != UtcTime::Null) {
					int Y,M,D,h,m,sec,ms;
					cal->asYMDhms_ms(x,Y,M,D,h,m,sec,ms);
					X.v= otl_datetime(Y,M,D,h,m,sec);				
					if(ms) {// consider how to deal with this.
						X.v.frac_precision=3;
						X.v.fraction=ms;
					}
					X.set_non_null();
				} else {
					X.set_null();
				}
				(*s)<<X;
				DBCATCH;
				return this;
			}
			dbstream^ put(String^ x) {
				DBTRY;
					std::string xs;
                    if(String::IsNullOrEmpty(x)){
						xs="";
					} else
						xs=marshalString<E_ANSI>(x);
					(*s)<<xs;
				DBCATCH;
				return this;
			}
			bool eof() {
				DBTRY;
				return s?(s->eof()?true:false):true;
				DBCATCH;
				return true;
			}

			/** Test if NULL was fetched from the stream  */
			bool is_null() {return s?(s->is_null()?true:false):true;}

			/** Section with overloaded get() alias << operations,
			 *   each basic type has following overloads: 
			 *      get(out int% x)
			 *      get(out dbvalue<int>% dbx) -- provides .is_null() info
			 *      get(out vector<int>% xv);
			 *      get(out vector<dbvalue<int>> % dbxv) provide dbxv.is_null(i) info
			 */

			dbstream^ get([Runtime::InteropServices::Out] int% x) {
				DBTRY;
				pin_ptr<int> px=&x;
				(*s)>> (*px);
				DBCATCH;
				return this;
			}
			dbstream^ get([Runtime::InteropServices::Out] double% x) {
				DBTRY;
				pin_ptr<double> px=&x;
				(*s)>>(*px);
				DBCATCH;
				return this;
			}
			dbstream^ get ([Runtime::InteropServices::Out] UtcTime% x,RegionalCalendar^ cal) {
				DBTRY;
				otl_value<otl_datetime> X;
				(*s)>> X;
				int ms=0;//todo calculate ms 
				x= cal->ToUtcTime(X.v.year,X.v.month,X.v.day,X.v.hour,X.v.minute,X.v.second,ms);
				DBCATCH;
				return this;
			}
			dbstream^ get ([Runtime::InteropServices::Out] UtcTime% x) {
				return get( x,dbCalendar());
			}
			dbstream^ get([Runtime::InteropServices::Out] String^% x) {
				DBTRY;
				otl_value<std::string> xx;
				(*s)>>xx;
				if(xx.is_null())
					x="";
				else
					x=marshalString<E_ANSI>(xx.v);
				DBCATCH;
				return this;
			}
			dbstream^ get([Runtime::InteropServices::Out] dbvalue< String^ >% x) {
				DBTRY;
				get(x.v);
				x.set_null((*s).is_null()?true:false);
				DBCATCH;
				return this;
			}
#define    OTLDB_GET(T,NT) \
			dbstream^ get([Runtime::InteropServices::Out] dbvalue< T >% x) {\
			DBTRY;pin_ptr<NT> px=&x.v;(*s)>>(*px);x.dbnull=(*s).is_null()?true:false;DBCATCH;\
				return this;\
			}
			OTLDB_GET(int,int);
			OTLDB_GET(double,double);

			dbstream^ get([Runtime::InteropServices::Out] dbvector<int>^% dbv) {
				otl_int_vec iv;
				DBTRY;
				 (*s)>>iv;
				 if(!s->is_null()) {
					 dbv= gcnew dbvector<int>(iv.len(),false);
					 pin_ptr<int> pi=&dbv->v[0];
					 memcpy(&(*pi),&iv.v[0],iv.len()*sizeof(int));
					 for(int i=0;i< iv.len();i++) {
						 if(iv.is_null(i))
							dbv->set_null(i);
					 }
				 }
				DBCATCH;
				return this;
			}
			dbstream^ get([Runtime::InteropServices::Out] dbvector<double>^% dbv) {
				otl_double_vec iv;
				DBTRY;
				(*s)>>iv;
				if(!s->is_null()) {
					dbv= gcnew dbvector<double>(iv.len(),false);
					pin_ptr<double> pi=&dbv->v[0];
					memcpy(&(*pi),&iv.v[0],iv.len()*sizeof(double));
					for(int i=0;i< iv.len();i++) {
						if(iv.is_null(i))
							dbv->set_null(i);
					}
				}
				DBCATCH;
				return this;
			}
			dbstream^ get([Runtime::InteropServices::Out] dbvector<UtcTime>^% dbv, RegionalCalendar^ cal) {
				otl_datetime_vec iv;
				DBTRY;
				(*s)>>iv;
				if(!s->is_null()) {
					dbv= gcnew dbvector<UtcTime>(iv.len(),false);
					int i=0;
					for each ( UtcTime% Tx in dbv->v) {
						if(iv.is_null(i)) {
							//-- default is null
							dbv->set_null(i);
						} else { 
							otl_datetime tx=iv[i];
							int ms=0;// todo ms.
							Tx=cal->ToUtcTime(tx.year,tx.month,tx.day,tx.hour,tx.minute,tx.second,ms);
						}
						i++;
					}
				}
				DBCATCH;
				return this;
			}
			dbstream^ get([Runtime::InteropServices::Out] dbvector<UtcTime>^% dbv) {
				return get(dbv,dbCalendar());
			}
			dbstream^ get([Runtime::InteropServices::Out] dbvector<String^>^% dbv) {
				DBTRY;
				otl_value<std::string> xx;
				otl_string_vec sv;
				(*s)>>sv;
				if(!s->is_null()) {
					int i=0;
					dbv= gcnew dbvector<String^>(sv.len(),false);
					for each (String^% x in dbv->v ) {
						if(sv.is_null(i))
							x="";
						else
							x=marshalString<E_ANSI>(sv[i]);
						i++;
					}
				}
				DBCATCH;
				return this;
			}

			dbstream^ put([Runtime::InteropServices::In] dbvector<int>^ dbv) {
				DBTRY;
				
					otl_int_vec iv;
					iv.set_len(dbv->len(),false);
					if(iv.len()) {
						pin_ptr<int> pi=&dbv->v[0];
						memcpy(&iv.v[0],&(*pi),iv.len()*sizeof(int));
						int i=0;
						for each (bool x in dbv->isNull) {
							if(x) iv.set_null(i);
							i++;
						}
					}
					(*s)<<iv;
				
				DBCATCH;
				return this;
			}
			dbstream^ put([Runtime::InteropServices::In] dbvector<double>^ dbv) {
				DBTRY;

				otl_double_vec iv;
				iv.set_len(dbv->len(),false);
				if(iv.len()) {
					pin_ptr<double> pi=&dbv->v[0];
					memcpy(&iv.v[0],&(*pi),iv.len()*sizeof(double));
					int i=0;
					for each (bool x in dbv->isNull) {
						if(x) iv.set_null(i);
						i++;
					}
				}
				(*s)<<iv;

				DBCATCH;
				return this;
			}
			dbstream^ put([Runtime::InteropServices::In] dbvector<UtcTime>^ dbv, RegionalCalendar^ cal) {
				DBTRY;

				otl_datetime_vec iv;
				iv.set_len(dbv->len(),false);
				if(iv.len()) {
					int i=0;
					for each (UtcTime x in dbv->v) {
						if(dbv->is_null(i)) {
							iv.set_null(i);
						} else {
							int Y,M,D,h,m,sec,ms;
							cal->asYMDhms_ms(x,Y,M,D,h,m,sec,ms);
							iv[i]= otl_datetime(Y,M,D,h,m,sec,ms);
						}
						i++;
					}
				}
				(*s)<<iv;

				DBCATCH;
				return this;
			}
			dbstream^ put([Runtime::InteropServices::In] dbvector<UtcTime>^ dbv) {
				return put(dbv,dbCalendar());
			}

			dbstream^ put([Runtime::InteropServices::In] dbvector<String^>^ dbv) {
				DBTRY;

				otl_string_vec iv;
				iv.set_len(dbv->len(),false);
				if(iv.len()) {
					int i=0;
					for each (String^ x in dbv->v) {
						if(dbv->is_null(i)) {
							iv.set_null(i);
						} else {
							iv[i]= marshalString<E_ANSI>(x);
						}
						i++;
					}
				}
				(*s)<<iv;

				DBCATCH;
				return this;
			}

		};
}
