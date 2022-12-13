//#pragma once
//
//#include "..\Enums.h"
//
//#using <mscorlib.dll>
//using namespace System;
//using namespace Powel::Icc::Services::Time;
//
//namespace Powel
//{
//	namespace Icc
//	{
//		namespace Interop
//		{
//			namespace Monitoring
//			{
//				/* Defines a set of properties that a type implements to be able to monitor database changes to objects of this type.
//				*/
//				public interface class IMonitorable
//				{
//					property int MonitorKey1 {int get(); void set(int);}
//					property int MonitorKey2 {int get(); void set(int);}
//					property int ObjectType {int get();}//property ObjectTypes ObjectType { ObjectTypes get();}
//					property TimePeriod^ MonitorPeriod { TimePeriod^ get(); void set(TimePeriod^);} 
//				};
//
//				/* A simple and generic class to store db change (monitor) information for an object
//				*/
//				public ref class MonitorItem : IMonitorable
//				{
//				public: 
//					MonitorItem(void){}
//					MonitorItem(int objectKey1,int objectKey2, ObjectTypes objectType,TimePeriod^ monitorPeriod):
//					objectKey1(objectKey1), objectKey2(objectKey2), objectType(objectType), monitorPeriod(monitorPeriod)
//					{
//					}
//					/*MonitorItem(int objectKey1,int objectKey2, ObjectTypes objectType,UtcTime from,UtcTime until):
//						objectKey1(objectKey1), objectKey2(objectKey2), objectType(objectType), from(from), until(until)
//					{		
//					}*/
//					virtual property int MonitorKey1 { int get(){return objectKey1;} void set(int value){ objectKey1 = value;}}
//					virtual property int MonitorKey2 { int get(){return objectKey2;} void set(int value){ objectKey2 = value;}}
//					virtual property int ObjectType { int get(){return (int)objectType;} void set(int value){ objectType = (ObjectTypes)value;} }
//					virtual property TimePeriod^ MonitorPeriod { TimePeriod^ get(){return gcnew TimePeriod(monitorPeriod->From, monitorPeriod->Until);} void set(TimePeriod^ value){ monitorPeriod = value;} }
//					/*property UtcTime From { UtcTime get(){return from;} }
//					property UtcTime Until { UtcTime get(){return until;} }*/
//				private:
//					int objectKey1;
//					int objectKey2;
//					ObjectTypes objectType;
//					TimePeriod^ monitorPeriod;
//					/*UtcTime from;
//					UtcTime until;*/
//
//				};
//			}
//		}
//	}
//}