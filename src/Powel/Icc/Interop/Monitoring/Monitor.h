#pragma once
//#include "otl.h"
//#include "Monitoring.h"
//#include "MonitorItem.h"
////#using <mscorlib.dll>
//using namespace System;
//using namespace System::Collections;
//using namespace System::Collections::Generic;
//using namespace Powel::Icc::Services::Time;
//using namespace OTLNET;
//
//namespace Powel{namespace Icc{namespace Interop	{namespace Monitoring{
//				public enum class Monitoring
//				{
//					ObjectKeyNull = -9999999
//				};
//
//				/* A monitor type.
//				 - SetBased monitors a set of client-specified objects of a given type
//				 - TypeBased monitors all objects of a given type
//				 */
//				public enum class MonitoringType
//				{
//					SetBased = 1,
//					TypeBased = 2
//				};
//
//				/*Monitor is a generic class that monitors database changes to objects of a given type that implements IMonitorable.
//				 The type is also required to have a parameterless constructor.
//				*/
//				generic <typename T>
//				where T : IMonitorable, gcnew()
//				public ref class Monitor
//				{
//				public: 
//					Monitor(MonitoringType monitoringType) {
//						monitor = NULL;
//						this->excludeAppKey = -1;
//						this->monitoringType = monitoringType;
//					}
//					Monitor(MonitoringType monitoringType, int excludeAppKey){
//						monitor = NULL;
//						this->monitoringType = monitoringType;
//						this->excludeAppKey = excludeAppKey;
//					};
//					void Add(T item){
//						if(!this->items)
//							this->items = gcnew ArrayList;
//
//						if(this->monitoringType == MonitoringType::SetBased)
//							this->items->Add(item);
//						else
//							throw gcnew ApplicationException("Add() is not allowed to call on a type based monitor");
//					};
//					void Add(array<T>^ items){
//						if(!this->items)
//							this->items = gcnew ArrayList;
//
//						if(this->monitoringType == MonitoringType::SetBased)
//							this->items->AddRange(items);
//						else
//							throw gcnew ApplicationException("Add() is not allowed to call on a type based monitor");
//					};
//					void Start(dbconnect^ db){
//						RegionalCalendar^ cal = RegionalCalendar::UtcCalendar;
//						std::vector<CppMonitoring::MonitorItem> cppItems;
//						if( this->monitoringType == MonitoringType::SetBased)
//						{
//							monitor = new CppMonitoring::Monitor(CppMonitoring::MONITORING_TICKETTYPE_DISTINCT, this->excludeAppKey);
//							for each(T t in items){
//								TimePeriod^ p = t->MonitorPeriod;	
//								cppItems.push_back(CppMonitoring::MonitorItem(t->MonitorKey1, t->MonitorKey2, t->ObjectType, p->From.ToPdtime(), p->Until.ToPdtime()));
//							}
//						}
//						else
//						{
//							monitor = new CppMonitoring::Monitor(CppMonitoring::MONITORING_TICKETTYPE_ALL, this->excludeAppKey);
//							T t = gcnew T;
//							PDTime time = time.now();
//							cppItems.push_back(CppMonitoring::MonitorItem(0, 0, t->ObjectType, MINPDTIME, MAXPDTIME));
//						}
//
//						CppMonitoring::MonitorSetInterface_ ms;
//						ms = new CppMonitoring::MonitorSet<std::vector<CppMonitoring::MonitorItem>>(cppItems);
//						try
//						{	
//							otl_connect *dbo =(otl_connect*)db->getDbHandle();
//							monitor->addObjects(ms, *dbo);	
//						if(this->items){
//							this->items->Clear();
//						}
//						} 
//						catch (otl_exception& e) {
//							throw gcnew ApplicationException(gcnew String((char *)e.msg));
//						}
//						catch (std::exception& e) {
//							throw gcnew ApplicationException(gcnew String(e.what()));
//						}
//					};
//					void End(dbconnect^ db){
//						if(!monitor)
//							throw gcnew ApplicationException("Monitor must be started before calling End()");
//						try	{
//							otl_connect *dbo =(otl_connect*)db->getDbHandle();
//							monitor->end(*dbo);
//						} catch (otl_exception& e) {
//							throw gcnew ApplicationException(gcnew String((char *)e.msg));
//						}
//						catch (std::exception& e) {
//							throw gcnew ApplicationException(gcnew String(e.what()));
//						}
//					};
//					array<T>^ GetChanges(dbconnect^ db){
//						if( !monitor )
//							throw gcnew ApplicationException("Monitor must be started before calling GetChanges()");
//
//						CppMonitoring::MonitorSetInterface_ changedItems;
//						try {
//							otl_connect *dbo =(otl_connect*)db->getDbHandle();
//							changedItems = monitor->getChanges(*dbo);
//						} catch (otl_exception& e) {
//							throw gcnew ApplicationException(gcnew String((char *)e.msg));
//						}
//						catch (std::exception& e) {
//							throw gcnew ApplicationException(gcnew String(e.what()));
//						}
//						array<T>^ items = gcnew array<T>(changedItems->size());
//						CppMonitoring::MonitorItem item;
//						T t = gcnew T();
//						int i = 0;
//						for(changedItems->reset(); changedItems->valid(); changedItems->next()){
//							changedItems->getItem(item.oKey, item.oKey2, item.oType, item.Tfrom, item.Tend);
//							t->MonitorKey1 = item.oKey;
//							t->MonitorKey2 = item.oKey2;
//							t->MonitorPeriod = gcnew TimePeriod(UtcTime::CreateFromPdtime(item.Tfrom), UtcTime::CreateFromPdtime(item.Tend));
//							items[i] = t;
//							i++;
//						}
//						return items;
//					};
//					array<MonitorItem^>^ GetChangesAsMonitorItems(dbconnect^ db){
//						if( !monitor )
//							throw gcnew ApplicationException("Monitor must be started before calling GetChanges()");
//
//						CppMonitoring::MonitorSetInterface_ changedItems;
//						try {
//							otl_connect *dbo =(otl_connect*)db->getDbHandle();
//							changedItems = monitor->getChanges(*dbo);
//						} catch (otl_exception& e) {
//							throw gcnew ApplicationException(gcnew String((char *)e.msg));
//						}
//						catch (std::exception& e) {
//							throw gcnew ApplicationException(gcnew String(e.what()));
//						}
//						array<MonitorItem^>^ items = gcnew array<MonitorItem^>(changedItems->size());
//						CppMonitoring::MonitorItem item;
//						MonitorItem^ t;
//						int i = 0;
//						for(changedItems->reset(); changedItems->valid(); changedItems->next()){
//							changedItems->getItem(item.oKey, item.oKey2, item.oType, item.Tfrom, item.Tend);
//							t = gcnew MonitorItem(item.oKey, item.oKey2, (ObjectTypes)item.oType, 
//								gcnew TimePeriod(UtcTime::CreateFromPdtime(item.Tfrom), UtcTime::CreateFromPdtime(item.Tend)));
//							items[i] = t;
//							i++;
//						}
//						return items;
//					};
//				private:
//					ArrayList^ items;	
//					MonitoringType monitoringType;
//					int excludeAppKey;
//					CppMonitoring::Monitor* monitor;
//					~Monitor()
//					{
//						this->!Monitor();
//					}
//					!Monitor()
//					{
//						delete monitor;
//					}
//				};
//			}
//		}
//	}
//}