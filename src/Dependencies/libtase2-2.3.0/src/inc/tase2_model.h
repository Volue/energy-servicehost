/*
 * tase2_model.h
 *
 * This file is part of libtase2
 *
 * Copyright 2014-2019 MZ Automation GmbH
 *
 * All rights reserved.
 *
 */

#ifndef PRIVATE_TASE2_TASE2_SERVER_TASE2_MODEL_H_
#define PRIVATE_TASE2_TASE2_SERVER_TASE2_MODEL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "tase2_common.h"

typedef enum {
    TASE2_EDITION_1996_08 = 1,
    TASE2_EDITION_2000_08 = 2
} Tase2_Edition;

/**
 * \defgroup tase2_model Server side data model related API parts
 * @{
 */

typedef struct sTase2_DataModel* Tase2_DataModel;

typedef struct sTase2_Domain* Tase2_Domain;

/**
 * \brief Abstract type representing data points in the server data model
 *
 * Includes indication points \ref Tase2_IndicationPoint, protection equipment points \ref Tase2_ProtectionEquipment
 * and control points (devices) \ref Tase2_ControlPoint
 */
typedef struct sTase2_DataPoint* Tase2_DataPoint;

/**
 * \brief Representation of indication points in the server data model
 */
typedef struct sTase2_IndicationPoint* Tase2_IndicationPoint;

/**
 * \brief Representation of protection equipment in the server data model
 */
typedef struct sTase2_ProtectionEquipment* Tase2_ProtectionEquipment;

/**
 * \brief Represents a data set transfer set (DSTS) in the server
 */
typedef struct sTase2_DSTransferSet* Tase2_DSTransferSet;

struct sTase2_DataModel {
    Tase2_Domain vcc;
    Tase2_Domain domains;
    bool hideSpecialTSVariables;
    Tase2_Edition edition; /* 1 = 1996-08, 2 = 2000-08 */
};


typedef enum {
    TASE2_POINT_TYPE_INDICATION,
    TASE2_POINT_TYPE_CONTROL,
    TASE2_POINT_TYPE_PROTECTION_EVENT
} Tase2_DataPointType;

/**
 * \brief Representation of a data set in the server data model
 */
typedef struct sTase2_DataSet* Tase2_DataSet;

/**
 * \brief Representation of a data set entry in the server data model
 */
typedef struct sTase2_DataSetEntry* Tase2_DataSetEntry;

struct sTase2_DataSet {
    char* name;
    Tase2_DataSet sibling;
    Tase2_DataSetEntry entries;
    Tase2_DataSetEntry lastEntry;
};

struct sTase2_DataSetEntry {
    Tase2_Domain domain;
    char* variableName;
    Tase2_DataSetEntry sibling;
};

typedef struct sTase2_TransferSet* Tase2_TransferSet;

typedef struct sTase2_InformationMessage* Tase2_InformationMessage;

struct sTase2_InformationMessage {
    int maxSize;
    void* infoBuf;
    Tase2_InformationMessage sibling;
};


struct sTase2_Domain {
    char* name;
    char* bilateralTableId;

    Tase2_DataPoint dataPoints; /* list of data points */
    void* dataPointHashTree; /* list of data points for efficient name lookup */
    int dataPointCount; /* number of registered data points */
    Tase2_DataPoint lastDataPoint; /* last element of list of data points */
    Tase2_DataSet dataSets;
    Tase2_TransferSet transferSets;
    Tase2_InformationMessage informationMessages;
    Tase2_Domain sibling;
};

typedef enum {
    TASE2_TS_TYPE_DATA_SET = 0
} Tase2_TransferSetType;


typedef enum {
    TASE2_DEVICE_CLASS_SBO = 0,
    TASE2_DEVICE_CLASS_DIRECT = 1
} Tase2_DeviceClass;



typedef enum {
    TASE2_CONTROL_TYPE_COMMAND = 0,
    TASE2_CONTROL_TYPE_SETPOINT_REAL = 1,
    TASE2_CONTROL_TYPE_SETPOINT_DESCRETE = 2
} Tase2_ControlPointType;

typedef struct sTase2_ControlPoint* Tase2_ControlPoint;

/**
 * \brief Get the name of the data point
 *
 * The data point can be of type \ref Tase2_IndicationPoint, \ref Tase2_ProtectionEquipment,
 * or \ref Tase2_ControlPoint
 *
 * \param self the data point instance
 */
TASE2_API const char*
Tase2_DataPoint_getName(Tase2_DataPoint self);

/**
 * \brief Get the \ref Tase2_Domain of the data point
 *
 * The data point can be of type \ref Tase2_IndicationPoint, \ref Tase2_ProtectionEquipment,
 * or \ref Tase2_ControlPoint
 *
 * \param self the data point instance
 *
 * \return the domain of the data point
 */
TASE2_API Tase2_Domain
Tase2_DataPoint_getDomain(Tase2_DataPoint self);

/**
 * \brief Get the point value (\ref Tase2_PointValue) of this data point
 *
 * \param self the data point instance
 *
 * \return the point value instance of this data point
 */
TASE2_API Tase2_PointValue
Tase2_DataPoint_getPointValue(Tase2_DataPoint self);

/**
 * \brief Get the relative name of the indication point
 *
 * \return name (only valid during the lifetime of the \ref Tase2_IndicationPoint object)
 */
TASE2_API const char*
Tase2_IndicationPoint_getName(Tase2_IndicationPoint self);

/**
 * \brief Set the value of a data point of type TASE2_IND_POINT_TYPE_REAL
 *
 * \param value new value
 */
TASE2_API void
Tase2_IndicationPoint_setReal(Tase2_IndicationPoint self, float value);

/**
 * \brief Set the value, and quality of a data point of type TASE2_IND_POINT_TYPE_REAL
 *
 * \param value new value
 * \param flags the quality flags
 */
TASE2_API void
Tase2_IndicationPoint_setRealQ(Tase2_IndicationPoint self, float value, Tase2_DataFlags flags);

/**
 * \brief Set the value, quality, and timestamp of a data point of type TASE2_IND_POINT_TYPE_REAL
 *
 * \param value new value
 * \param flags the quality flags
 * \paran timestamp the time stamp in ms since Epoch
 */
TASE2_API void
Tase2_IndicationPoint_setRealQTimeStamp(Tase2_IndicationPoint self, float value, Tase2_DataFlags flags, uint64_t timestamp);

/**
 * \brief Set the value of a data point of type TASE2_IND_POINT_TYPE_DISCRETE
 *
 * \param value new value
 */
TASE2_API void
Tase2_IndicationPoint_setDiscrete(Tase2_IndicationPoint self, int32_t value);

/**
 * \brief Set the value, and quality of a data point of type TASE2_IND_POINT_TYPE_DISCRETE
 *
 * \param value new value
 * \param flags the quality flags
 */
TASE2_API void
Tase2_IndicationPoint_setDiscreteQ(Tase2_IndicationPoint self, int32_t value, Tase2_DataFlags flags);

/**
 * \brief Set the value, quality, and timestamp of a data point of type TASE2_IND_POINT_TYPE_DISCRETE
 *
 * \param value new value
 * \param flags the quality flags
 * \paran timestamp the time stamp in ms since Epoch
 */
TASE2_API void
Tase2_IndicationPoint_setDiscreteQTimeStamp(Tase2_IndicationPoint self, int32_t value, Tase2_DataFlags flags, uint64_t timestamp);

/**
 * \brief Set the state and quality of a TASE2_IND_POINT_TYPE_STATE type indication point
 *
 * NOTE: the \ref Tase2_DataState type represents the state and the quality.
 *
 * \param value the state (and quality) value
 */
TASE2_API void
Tase2_IndicationPoint_setState(Tase2_IndicationPoint self, Tase2_DataState value);

/**
 * \brief Set the state, quality, and timestamp of a TASE2_IND_POINT_TYPE_STATE type indication point
 *
 * \param value the state (and quality) value
 */
TASE2_API void
Tase2_IndicationPoint_setStateTimeStamp(Tase2_IndicationPoint self, Tase2_DataState value, uint64_t timestamp);

/**
 * \brief Set the value of a data point of type TASE2_IND_POINT_TYPE_STATE_SUPPLEMENTAL
 *
 * \param value new value
 */
TASE2_API void
Tase2_IndicationPoint_setStateSupplemental(Tase2_IndicationPoint self, Tase2_DataStateSupplemental value);

/**
 * \brief Set the value, and quality of a data point of type TASE2_IND_POINT_TYPE_STATE_SUPPLEMENTAL
 *
 * \param value new value
 * \param flags the quality flags
 */
TASE2_API void
Tase2_IndicationPoint_setStateSupplementalQ(Tase2_IndicationPoint self, Tase2_DataStateSupplemental value, Tase2_DataFlags flags);

/**
 * \brief Set the value, quality, and timestamp of a data point of type TASE2_IND_POINT_TYPE_STATE_SUPPLEMENTAL
 *
 * \param value new value
 * \param flags the quality flags
 * \paran timestamp the time stamp in ms since Epoch
 */
TASE2_API void
Tase2_IndicationPoint_setStateSupplementalQTimeStamp(Tase2_IndicationPoint self, Tase2_DataStateSupplemental value, Tase2_DataFlags flags, uint64_t timestamp);

/**
 * \brief Sets the quality flags of a data point
 *
 * NOTE: for STATE type indication points the quality can also be set with the \ref Tase2_IndicationPoint_setState
 * or \ref Tase2_IndicationPoint_setStateTimeStamp functions.
 *
 * \param flags the quality flags values
 */
TASE2_API void
Tase2_IndicationPoint_setQuality(Tase2_IndicationPoint self, Tase2_DataFlags flags);

/**
 * \brief Sets the timestamp of a data point
 *
 * NOTE: This has no effect when the data point type has no timestamp.
 *
 * \param timestamp the time stamp in ms since Epoch
 */
TASE2_API void
Tase2_IndicationPoint_setTimeStamp(Tase2_IndicationPoint self, uint64_t timestamp);

/**
 * \brief Set the COV (cointer of value changes) of the data point
 *
 * NOTE: This has no effect when the data point type has no COV.
 *
 * \param cov the COV value
 */
TASE2_API void
Tase2_IndicationPoint_setCOV(Tase2_IndicationPoint self, uint16_t cov);


/**
 * \brief Get the name of the protection equipment
 */
TASE2_API const char*
Tase2_ProtectionEquipment_getName(Tase2_ProtectionEquipment self);

/**
 * \brief Set the single event flags (value and quality)
 *
 * \param eventFlags the single event flags
 */
TASE2_API void
Tase2_ProtectionEquipment_setSingleEventFlags(Tase2_ProtectionEquipment self, Tase2_SingleEventFlags eventFlags);

/**
 * \brief Set the event flags of a packed event
 *
 * \param packedEvent the event flags
 */
TASE2_API void
Tase2_ProtectionEquipment_setPackedEvent(Tase2_ProtectionEquipment self, Tase2_EventFlags packedEvent);

/**
 * \brief Set the quality flags of a packed event
 *
 * \param flags the quality flags
 */
TASE2_API void
Tase2_ProtectionEquipment_setPackedEventFlags(Tase2_ProtectionEquipment self, Tase2_PackedEventFlags flags);

/**
 * \brief Set the operating time (duration) of a protection equipment event
 *
 * \param operatingTime the operating time in ms
 */
TASE2_API void
Tase2_ProtectionEquipment_setOperatingTime(Tase2_ProtectionEquipment self, int32_t operatingTime);

/**
 * \brief Set the event time (timestamp when the event started) of a protection equipment event
 *
 * \param eventTime the time stamp in ms since Epoch
 */
TASE2_API void
Tase2_ProtectionEquipment_setEventTime(Tase2_ProtectionEquipment self, uint64_t eventTime);

/**
 * \brief Get the name of the control point
 *
 * NOTE: the control point instance is the owner of the returned string.
 *
 * \param self Tase2_ControlPoint instance
 *
 * \return name of the control point
 */
TASE2_API const char*
Tase2_ControlPoint_getName(Tase2_ControlPoint self);

/**
 * \brief Get the domain of the control point
 *
 * \param self Tase2_ControlPoint instance
 *
 * \return domain of the control point
 */
TASE2_API Tase2_Domain
Tase2_ControlPoint_getDomain(Tase2_ControlPoint self);

/**
 * \brief Get the type of the control point
 *
 * \param self Tase2_ControlPoint instance
 *
 * \return the type of the control point
 */
TASE2_API Tase2_ControlPointType
Tase2_ControlPoint_getType(Tase2_ControlPoint self);

/**
 * \brief Get the tag value and additional tag information
 *
 * NOTES:
 * - The control point needs the tag attribute. Otherwise TAG_INVALID is returned.
 * - When returned the reason string is newly allocated and owned by the caller
 * - Function can only be called after the \ref Tase2_Server instance was created
 *
 * \param self Tase2_ControlPoint instance
 * \param reason pointer to store the reason message, or NULL if the reason message is not required
 * \param tagOwner a pointer to store the Tase2_BilateralTable that is representing the owner, can be NULL if owner information is not required.
 *
 * \return the value of the tag, if the control point has not tag, TASE2_TAG_INVALID is returned.
 */
TASE2_API Tase2_TagValue
Tase2_ControlPoint_getTag(Tase2_ControlPoint self, char** reason, Tase2_BilateralTable* tagOwner);

/**
 * \brief Set the tag value and additional tag information locally
 *
 *  \param self Tase2_ControlPoint instance
 *  \param value the \ref Tase2_TagValue value
 *  \param reason the reason as a human readable message
 *  \param tagOwner should be NULL to indicate that the tag was set locally (cannot overwritten by remote client)
 */
TASE2_API void
Tase2_ControlPoint_setTag(Tase2_ControlPoint self, Tase2_TagValue value, const char* reason, Tase2_BilateralTable tagOwner);


/**
 * \brief Create a new data model instance
 *
 * NOTE: a data model can only be used by a single \ref Tase2_Server instance!
 */
TASE2_API Tase2_DataModel
Tase2_DataModel_create();

/**
 * \brief Set the TASE.2/ICCP edition to be used with the data model
 *
 * \param edition the TASE.2/ICCP standard edition to be used
 */
TASE2_API void
Tase2_DataModel_setEdition(Tase2_DataModel self, Tase2_Edition edition);

/**
 * \brief Release all resources and memory used by the data model
 */
TASE2_API void
Tase2_DataModel_destroy(Tase2_DataModel self);

/**
 * \brief Option to hide special transfer set variables in data model
 *
 * When enabled the special transfer set variables (like "Transfer_Set_Name") will
 * not appear in the get-name-list response of the server.
 *
 * \param value, when true the special variables are hidden, otherwise they are visible
 */
TASE2_API void
Tase2_DataModel_hideSpecialTSVariables(Tase2_DataModel self, bool value);

/**
 * \brief Add a TASE.2 domain to the data model
 *
 * \param domainName the name of the domain (max. 32 characters)
 *
 * \return the new domain instance
 */
TASE2_API Tase2_Domain
Tase2_DataModel_addDomain(Tase2_DataModel self, const char* domainName);

/**
 * \brief Get the domain instance with a specific name
 *
 * \param domainName the domain name
 *
 * \return the domain with the specified name
 */
TASE2_API Tase2_Domain
Tase2_DataModel_getDomain(Tase2_DataModel self, const char* domainName);

/**
 * \brief Get the VCC pseudo domain that is representing the VCC scope data model items.
 *
 * \return domain object the is representing the VCC scope
 */
TASE2_API Tase2_Domain
Tase2_DataModel_getVCC(Tase2_DataModel self);

/**
 * \brief Get the name of the domain
 *
 * \return the name of the domain as C string (only valid as long as the domain object exists)
 */
TASE2_API const char*
Tase2_Domain_getName(Tase2_Domain self);

/**
 * \brief Set the bilateral table (BLT) id of the domain
 *
 * NOTE: a domain is related to a specific bilateral table that is an agreement between client
 * and server on what data is accessible by the client. Usually a (server) domain is dedicated to a specific
 * other control center (client).
 *
 * \param id the BLT ID
 */
TASE2_API void
Tase2_Domain_setBilateralTableId(Tase2_Domain self, const char* id);

/**
 * \brief Create a new data point of type indication point
 *
 * NOTE: An indication point represents measurement or status data. There are some combinations that
 * don't result in valid types. The type StateQ is not required (because State already supports quality
 * information) and should not be used (also it is not accepted by some third-party implementations).
 *
 * \param name the name of the data point
 * \param type the type of the indication point
 * \param qualityClass defines if a quality flag is available
 * \param timeStampClass defines if and what kind of time stamp is available
 * \param hasCOV has a COV counter
 * \param readOnly true when indication point is read-only, false when read-write.
 *
 * \return the new indication point instance
 *
 * Allowed parameter combinations are:
 *
 * Resulting type (specification) | type | qualityClass | timeStampClass | hasCOV
 * --------------------------|------|--------------|----------------|--------
 * Data_State | STATE | NO_QUALITY | NO_TIMESTAMP | false
 * Data_Discrete | DISCRETE  | NO_QUALITY | NO_TIMESTAMP | false
 * Data_Real | REAL | NO_QUALITY | NO_TIMESTAMP | false
 * Data_StateSupplemental | STATE_SUPPLEMENTAL | NO_QUALITY | NO_TIMESTAMP | false
 * Data_RealQ | REAL | QUALITY | NO_TIMESTAMP | false
 * Data_DiscreteQ | DISCRETE | QUALITY | NO_TIMESTAMP | false
 * Data_StateSupplementalQ | STATE_SUPPLEMENTAL | QUALITY | NO_TIMESTAMP | false
 * Data_RealQTimeTag | REAL | QUALITY | TIMESTAMP | false
 * Data_StateQTimeTag | STATE | QUALITY | TIMESTAMP | false
 * Data_DiscreteQTimeTag | DISCRETE | QUALITY | TIMESTAMP | false
 * Data_StateSupplementalQTimeTag | STATE_SUPPLEMENTAL | QUALITY | TIMESTAMP | false
 * Data_RealExtended | REAL | QUALITY | TIMESTAMP | true
 * Data_StateExtended | STATE | QUALITY | TIMESTAMP | true
 * Data_DiscreteExtended | STATE | QUALITY | TIMESTAMP | true
 * Data_StateSupplementalExtended | STATE_SUPPLEMENTAL | QUALITY | TIMESTAMP | true
 * Data_RealQTimeTagExtended | REAL | QUALITY | TIMESTAMP_EXTENDED | false
 * Data_StateQTimeTagExtended | STATE | QUALITY | TIMESTAMP_EXTENDED | false
 * Data_DiscreteQTimeTagExtended | DISCRETE | QUALITY | TIMESTAMP_EXTENDED | false
 * Data_StateSupplementalQTimeTagExtended | STATE_SUPPLEMENTAL | QUALITY | TIMESTAMP_EXTENDED | false
 *
 */
TASE2_API Tase2_IndicationPoint
Tase2_Domain_addIndicationPoint(Tase2_Domain self, const char* name, Tase2_IndicationPointType type,
        Tase2_QualityClass qualityClass, Tase2_TimeStampClass timeStampClass, bool hasCOV, bool readOnly);

/**
 * \brief Create a new data point of type control point (device)
 *
 * NOTE: A control point (device) is used to control outputs or set points.
 *
 * \param deviceName name of the control point
 * \param pointType type of the control point (one of COMMAND, SETPOINT_REAL, SETPOINT_DISCRETE)
 * \param deviceClass the device class (one of TASE2_DEVICE_CLASS_SBO or TASE2_DEVICE_CLASS_DIRECT)
 * \param hasTag define if the device has a tag (a tag is a restriction that prohibits specific usage of the device)
 * \param checkBackId Set the check back ID for SBO.
 *
 * \return the new control point instance
 */
TASE2_API Tase2_ControlPoint
Tase2_Domain_addControlPoint(Tase2_Domain self, const char* deviceName, Tase2_ControlPointType pointType, Tase2_DeviceClass deviceClass, bool hasTag, int16_t checkBackId);

/**
 * \brief Create a new data point of type protection equipment
 *
 * \param equipmentName the name of the data point
 * \param isPacked specify if packed type is used
 *
 * \return the new protection equipment instance
 */
TASE2_API Tase2_ProtectionEquipment
Tase2_Domain_addProtectionEquipment(Tase2_Domain self, const char* equipmentName, bool isPacked);

/**
 * \brief Add a new data set transfer set (DSTS) to a domain
 *
 * \note Cannot be used with VCC pseudo-domain because transfer set are not allowed in VCC scope
 *
 * \param self the domain instance
 * \param name the name of the DSTS
 *
 * \return the new DSTS instance
 */
TASE2_API Tase2_DSTransferSet
Tase2_Domain_addDSTransferSet(Tase2_Domain self, const char* name);

/**
 * \brief Add a new data set to the domain or VCC pseudo-domain.
 *
 * A data set is a group of VCC or ICC scope variables.
 *
 * \param self the domain instance or VCC pseudo domain (see \ref Tase2_DataModel_getVCC)
 * \param name the name of the new data set
 *
 * \return the new data set instance
 */
TASE2_API Tase2_DataSet
Tase2_Domain_addDataSet(Tase2_Domain self, const char* name);

/**
 * \brief Get the data point (indication point, device, or protection equipment) with the given name.
 *
 * \param self the domain instance or VCC pseudo domain (see \ref Tase2_DataModel_getVCC)
 * \param name the name of the data point
 *
 * \return the data point instance or NULL if the data point doesn't exist.
 */
TASE2_API Tase2_DataPoint
Tase2_Domain_getDataPoint(Tase2_Domain self, const char* pointName);

/**
 * \brief Add an information message object to a domain of VCC scope
 *
 * \param self the domain instance or VCC pseudo domain (see \ref Tase2_DataModel_getVCC)
 * \param maxSize maximum size of the information message buffer
 */
TASE2_API void
Tase2_Domain_addInformationMessageObject(Tase2_Domain self, int maxSize);

/**
 * \brief Add a data set entry to a static data set
 *
 * \param domain the domain of the entry variable
 * \param varibaleName the name of the entry variable
 *
 * \return the new data set entry instance
 */
TASE2_API Tase2_DataSetEntry
Tase2_DataSet_addEntry(Tase2_DataSet self, Tase2_Domain domain, const char* variableName);

/**
 * \brief Get the name of the transfer set
 *
 * \return the name string (only valid as long the the transfer set object exists)
 */
TASE2_API const char*
Tase2_TransferSet_getName(Tase2_TransferSet self);

/**
 * \brief Get the TASE.2 domain of the transfer set
 *
 * \return the domain object of the transfer set
 */
TASE2_API Tase2_Domain
Tase2_TransferSet_getDomain(Tase2_TransferSet self);

/**
 * \brief Get the data set name of the transfer set
 *
 * The data set name is a string with the format:
 *
 * domain_name/data_set_name for domain scope data sets
 *
 * or
 *
 * data_set_name for VCC scope data sets
 *
 * NOTE: The returned value has to be released by the caller!
 *
 * \return a copy of the data set name or NULL
 */
TASE2_API char*
Tase2_DSTransferSet_getDataSetName(Tase2_DSTransferSet self);

/**
 * \brief Get the start time for condition monitoring of the transfer set
 *
 * \return start time in seconds sonce epoch (GMTBasedS)
 */
TASE2_API int32_t
Tase2_DSTransferSet_getStartTime(Tase2_DSTransferSet self);

/**
 * \brief Get the interval value (time interval between server reports)
 *
 * \return the interval value in milliseconds (TimeIntervalL32)
 */
TASE2_API int32_t
Tase2_DSTransferSet_getInterval(Tase2_DSTransferSet self);

/**
 * \brief Get the TLE (time limit for execution) value
 *
 * \return the TLE value in seconds (TimeIntervalS)
 */
TASE2_API int32_t
Tase2_DSTransferSet_getTLE(Tase2_DSTransferSet self);

/**
 * \brief Get the buffer time (wait time before report is sent)
 *
 * \return the buffer time in seconds (TimeIntervalS)
 */
TASE2_API int32_t
Tase2_DSTransferSet_getBufferTime(Tase2_DSTransferSet self);

/**
 * \brief Get the time interval for integrity checks (Integrity Check)
 *
 * When the integrity check condition is used an integrity report
 * is sent after the timeout elapsed containing the whole data set
 *
 * \return the integrity check value in milliseconds (TimeIntervalL32)
 */
TASE2_API int32_t
Tase2_DSTransferSet_getIntegrityCheck(Tase2_DSTransferSet self);

/**
 * \brief Get the block data flag
 *
 * \return when block data is enabled, false otherwise
 */
TASE2_API bool
Tase2_DSTransferSet_getBlockData(Tase2_DSTransferSet self);

/**
 * \brief Get the critical flag (report acknowledge by client required when set)
 *
 * \return when critical reports are enabled, false otherwise
 */
TASE2_API bool
Tase2_DSTransferSet_getCritical(Tase2_DSTransferSet self);

/**
 * \brief Get the RBE (report by exception) flag
 *
 * \return when RBE is enabled, false otherwise
 */
TASE2_API bool
Tase2_DSTransferSet_getRBE(Tase2_DSTransferSet self);

/**
 * \brief Get the All Changes Reported flag
 *
 * \return when all changes reported is enabled, false otherwise
 */
TASE2_API bool
Tase2_DSTransferSet_getAllChangesReported(Tase2_DSTransferSet self);

/**
 * \brief Get the status of the DS transfer set
 *
 * \return true, when the transfer set is enabled, false otherwise
 */
TASE2_API bool
Tase2_DSTransferSet_getStatus(Tase2_DSTransferSet self);

/**
 * \brief Get the event code requested value
 *
 * \return event code requested value
 */
TASE2_API int16_t
Tase2_DSTransferSet_getEventCodeRequested(Tase2_DSTransferSet self);

/** @} */

#ifdef __cplusplus
}
#endif


#endif /* PRIVATE_TASE2_TASE2_SERVER_TASE2_MODEL_H_ */
