cmake_minimum_required(VERSION 3.0.0)
project(libtase2 C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(USE_CCACHE "Enable CCache Support" ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

set(TASE2_LIB_VERSION_MAJOR "2")
set(TASE2_LIB_VERSION_MINOR "3")
set(TASE2_LIB_VERSION_PATCH "0")

set(LIB_VERSION ${TASE2_LIB_VERSION_MAJOR}.${TASE2_LIB_VERSION_MINOR}.${TASE2_LIB_VERSION_PATCH})

##### Define Macros #####
macro(ADD_C_FLAGS flags)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flags}")
endmacro()

macro(ADD_CXX_FLAGS flags)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flags}")
endmacro()

##### Git Version Info ####
set(TASE2_LIBRARY_GIT_DIR "${CMAKE_CURRENT_LIST_DIR}/.git")


##### Options ######

option(BUILD_EXAMPLES "Build the examples" ON)
option(BUILD_TESTS "Build the tests" ON)
option(DEBUG_LOG "Print debug messages to console" ON)
option(DEBUG_SYMBOLS "Compile with debug symbols" ON)

##### OLD VISUAL STUDO FIX ########

if(MSVC AND MSVC_VERSION LESS 1800)
    include_directories(
        ${CMAKE_CURRENT_LIST_DIR}/dependencies/libmms/src/vs
    )
endif(MSVC AND MSVC_VERSION LESS 1800)

###### feature checks #########################
include(CheckLibraryExists)
check_library_exists(rt clock_gettime "time.h" CONFIG_SYSTEM_HAS_CLOCK_GETTIME)


##################################################


if(DEBUG_SYMBOLS)
set(CMAKE_BUILD_TYPE Debug)
endif(DEBUG_SYMBOLS)

if(DEBUG_LOG)
add_definitions(-DTASE2_DEBUG_OUTPUT)
endif(DEBUG_LOG)

set(TASE2_LIB_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/src")

set(TASE2_LIB_SRC
	"${TASE2_LIB_SRC_DIR}/tase2_common/tase2_object_values.c"
	"${TASE2_LIB_SRC_DIR}/tase2_common/tase2_point_value.c"
	"${TASE2_LIB_SRC_DIR}/tase2_common/tase2_log.c"
	"${TASE2_LIB_SRC_DIR}/tase2_common/tase2_common.c"
	"${TASE2_LIB_SRC_DIR}/tase2_common/tase2_endpoint.c"
	"${TASE2_LIB_SRC_DIR}/tase2_server/tase2_model.c"
	"${TASE2_LIB_SRC_DIR}/tase2_server/tase2_blt.c"
	"${TASE2_LIB_SRC_DIR}/tase2_server/tase2_server.c"
	"${TASE2_LIB_SRC_DIR}/tase2_server/ds_transfer_set.c"
	"${TASE2_LIB_SRC_DIR}/tase2_server/im_transfer_set.c"
	"${TASE2_LIB_SRC_DIR}/tase2_client/tase2_client.c"
)

if(MSVC AND MSVC_VERSION LESS 1800)
set_source_files_properties(${TASE2_LIB_SRC}
                                       PROPERTIES LANGUAGE CXX)
endif(MSVC AND MSVC_VERSION LESS 1800)

set(MMS_LIB_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/dependencies/libmms/src")
set(HAL_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/dependencies/libmms/")

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/config-mms
    ${MMS_LIB_SRC_DIR}/common/inc
    ${HAL_SRC_DIR}/hal/inc
    ${HAL_SRC_DIR}/hal/tls/mbedtls
    ${MMS_LIB_SRC_DIR}/mms/inc
    ${MMS_LIB_SRC_DIR}/mms/inc_private
    ${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c
    ${MMS_LIB_SRC_DIR}/logging
)

set (MMS_LIB_SRC
	${MMS_LIB_SRC_DIR}/common/linked_list.c
	${MMS_LIB_SRC_DIR}/common/byte_buffer.c
	${MMS_LIB_SRC_DIR}/common/lib_memory.c
	${MMS_LIB_SRC_DIR}/common/string_utilities.c
	${MMS_LIB_SRC_DIR}/common/buffer_chain.c
	${MMS_LIB_SRC_DIR}/common/conversions.c
	${MMS_LIB_SRC_DIR}/common/mem_alloc_linked_list.c
	${MMS_LIB_SRC_DIR}/common/simple_allocator.c
	${MMS_LIB_SRC_DIR}/common/hash_tree.c
	${MMS_LIB_SRC_DIR}/common/avl_tree.c
	${MMS_LIB_SRC_DIR}/mms/iso_server/iso_connection.c
	${MMS_LIB_SRC_DIR}/mms/iso_server/iso_server.c
	${MMS_LIB_SRC_DIR}/mms/iso_acse/acse.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/common/mms_type_spec.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/common/mms_value.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/common/mms_common_msg.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/endpoint/mms_endpoint.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/endpoint/mms_association_service.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/endpoint/mms_identify_service.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/endpoint/mms_information_report.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/endpoint/mms_var_access_specification.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/client/mms_client_initiate.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/client/mms_client_write.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/client/mms_client_identify.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/client/mms_client_status.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/client/mms_client_named_variable_list.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/client/mms_client.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/client/mms_client_files.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/client/mms_client_get_namelist.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/client/mms_client_get_var_access.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/client/mms_client_common.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/client/mms_client_read.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/client/mms_client_journals.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/server/mms_read_service.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/server/mms_file_service.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/server/mms_status_service.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/server/mms_named_variable_list_service.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/server/mms_value_cache.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/server/mms_get_namelist_service.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/server/mms_access_result.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/server/mms_server.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/server/mms_server_common.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/server/mms_named_variable_list.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/server/mms_domain.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/server/mms_device.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/server/mms_journal.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/server/mms_journal_service.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/server/mms_write_service.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/server/mms_get_var_access_service.c
	${MMS_LIB_SRC_DIR}/mms/iso_cotp/cotp.c
	${MMS_LIB_SRC_DIR}/mms/iso_presentation/iso_presentation.c
	${MMS_LIB_SRC_DIR}/mms/asn1/ber_decode.c
	${MMS_LIB_SRC_DIR}/mms/asn1/asn1_ber_primitive_value.c
	${MMS_LIB_SRC_DIR}/mms/asn1/ber_encoder.c
	${MMS_LIB_SRC_DIR}/mms/asn1/ber_integer.c
	${MMS_LIB_SRC_DIR}/mms/iso_client/iso_client_connection.c
	${MMS_LIB_SRC_DIR}/mms/iso_common/iso_connection_parameters.c
	${MMS_LIB_SRC_DIR}/mms/iso_session/iso_session.c
)

set (MMS_TLS_SRC
	${HAL_SRC_DIR}/hal/tls/mbedtls/tls_mbedtls.c
)

set (MMS_ASN1C_SRC
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/DataAccessError.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/DeleteNamedVariableListRequest.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/constr_SET_OF.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/MmsPdu.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/GetNamedVariableListAttributesResponse.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/BIT_STRING.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/ber_tlv_tag.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/constr_SEQUENCE_OF.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/asn_SET_OF.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/ReadResponse.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/InformationReport.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/ConfirmedServiceRequest.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/DeleteNamedVariableListResponse.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/asn_SEQUENCE_OF.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/VariableAccessSpecification.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/GetVariableAccessAttributesRequest.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/xer_support.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/ObjectName.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/NativeEnumerated.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/per_encoder.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/constr_SEQUENCE.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/GetNameListResponse.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/MMSString.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/InitiateErrorPdu.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/IndexRangeSeq.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/ConfirmedErrorPDU.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/UnconfirmedService.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/UTF8String.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/ServiceError.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/TimeOfDay.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/GetNameListRequest.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/asn_codecs_prim.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/Data.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/ScatteredAccessDescription.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/ReadRequest.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/per_decoder.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/Identifier.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/ServiceSupportOptions.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/Integer8.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/ConfirmedServiceResponse.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/ParameterSupportOptions.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/Integer16.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/ber_tlv_length.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/OCTET_STRING.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/DefineNamedVariableListRequest.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/FloatingPoint.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/xer_encoder.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/Unsigned8.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/BOOLEAN.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/INTEGER.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/UnconfirmedPDU.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/DataSequence.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/constraints.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/der_encoder.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/VisibleString.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/InitiateResponsePdu.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/StructComponent.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/Address.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/Unsigned16.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/ber_decoder.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/per_support.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/WriteResponse.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/InitRequestDetail.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/InitiateRequestPdu.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/DefineNamedVariableListResponse.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/NULL.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/ListOfVariableSeq.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/UtcTime.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/ConcludeResponsePDU.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/AccessResult.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/Integer32.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/GetNamedVariableListAttributesRequest.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/VariableSpecification.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/Unsigned32.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/constr_CHOICE.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/AlternateAccess.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/ObjectClass.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/InitResponseDetail.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/ConfirmedResponsePdu.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/GetVariableAccessAttributesResponse.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/NativeInteger.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/xer_decoder.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/AlternateAccessSelection.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/ConfirmedRequestPdu.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/ConcludeRequestPDU.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/WriteRequest.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/RejectPDU.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/TypeSpecification.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/constr_TYPE.c
	${MMS_LIB_SRC_DIR}/mms/iso_mms/asn1c/GeneralizedTime.c
)

set (HAL_LINUX_SRC
	${HAL_SRC_DIR}/hal/socket/linux/socket_linux.c
	${HAL_SRC_DIR}/hal/thread/linux/thread_linux.c
	${HAL_SRC_DIR}/hal/filesystem/linux/file_provider_linux.c
	${HAL_SRC_DIR}/hal/time/unix/time.c
)

set (HAL_WINDOWS_SRC
	${HAL_SRC_DIR}/hal/socket/win32/socket_win32.c
	${HAL_SRC_DIR}/hal/thread/win32/thread_win32.c
	${HAL_SRC_DIR}/hal/filesystem/win32/file_provider_win32.c
	${HAL_SRC_DIR}/hal/time/win32/time.c
)

set (HAL_BSD_SRC
	${HAL_SRC_DIR}/hal/socket/bsd/socket_bsd.c
	${HAL_SRC_DIR}/hal/thread/bsd/thread_bsd.c
	${HAL_SRC_DIR}/hal/filesystem/linux/file_provider_linux.c
	${HAL_SRC_DIR}/hal/time/unix/time.c
)

set (HAL_MACOS_SRC
	${HAL_SRC_DIR}/hal/socket/bsd/socket_bsd.c
	${HAL_SRC_DIR}/hal/thread/macos/thread_macos.c
        ${HAL_SRC_DIR}/hal/filesystem/linux/file_provider_linux.c
	${HAL_SRC_DIR}/hal/time/unix/time.c
)	

IF(WIN32)

set_source_files_properties(${MMS_ASN1C_SRC}
                                       PROPERTIES LANGUAGE C)
                                       
if(MSVC AND MSVC_VERSION LESS 1800)

set_source_files_properties(${MMS_LIB_SRC} ${HAL_WINDOWS_SRC}
                                       PROPERTIES LANGUAGE CXX)
endif(MSVC AND MSVC_VERSION LESS 1800)

set (MMS_SRC
	${MMS_LIB_SRC}
	${MMS_ASN1C_SRC}
	${HAL_WINDOWS_SRC}
)

ELSEIF(UNIX)
IF(APPLE)
set (MMS_SRC
	${MMS_LIB_SRC}
	${MMS_ASN1C_SRC}
	${HAL_MACOS_SRC}
)
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
set (MMS_SRC
	${MMS_LIB_SRC}
	${MMS_ASN1C_SRC}
	${HAL_BSD_SRC}
)
ELSE()
set (MMS_SRC
	${MMS_LIB_SRC}
	${MMS_ASN1C_SRC}
	${HAL_LINUX_SRC}
)
ENDIF(APPLE)
ENDIF(WIN32)



set (MBEDTLS_BASE_DIR
	"${CMAKE_CURRENT_LIST_DIR}/dependencies/libmms/third_party/mbedtls/mbedtls-2.16.12"
)

set (MBEDTLS_SRC_DIR
	${MBEDTLS_BASE_DIR}/library
)

file (GLOB MBEDTLS_SRC
	"${MBEDTLS_SRC_DIR}/*.c"
)

set (MMS_SRC
	${MMS_SRC}
	${MMS_TLS_SRC}
	${MBEDTLS_SRC}
)

include_directories("${MBEDTLS_BASE_DIR}/include")

add_definitions(-DCONFIG_MMS_SUPPORT_TLS=1)
add_definitions(-DMBEDTLS_CONFIG_FILE="mbedtls_config.h")

include_directories("${TASE2_LIB_SRC_DIR}/inc")
include_directories("${TASE2_LIB_SRC_DIR}/inc_internal")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/src/inc_internal")

add_library (tase2-shared SHARED ${TASE2_LIB_SRC} ${MMS_SRC} )

set_target_properties(tase2-shared PROPERTIES
           OUTPUT_NAME tase2
           SOVERSION "${TASE2_LIB_VERSION_MAJOR}.${TASE2_LIB_VERSION_MINOR}.${TASE2_LIB_VERSION_PATCH}"
           DEFINE_SYMBOL "EXPORT_FUNCTIONS_FOR_DLL"
)

configure_file(
    ${TASE2_LIB_SRC_DIR}/inc_internal/tase2_version.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/src/inc_internal/tase2_version.h
)

####################################

add_library (tase2 STATIC ${TASE2_LIB_SRC} ${MMS_SRC} )

IF(UNIX)
  IF (CONFIG_SYSTEM_HAS_CLOCK_GETTIME)
     target_link_libraries (tase2
         -lpthread
         -lm
         -lrt
     )
  ELSE ()
     target_link_libraries (tase2
         -lpthread
         -lm
     )
  ENDIF (CONFIG_SYSTEM_HAS_CLOCK_GETTIME)
ENDIF(UNIX)

if(BUILD_EXAMPLES)
	add_subdirectory(examples)
endif(BUILD_EXAMPLES)

if(BUILD_TESTS)
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/tests")
	add_subdirectory(tests)
endif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/tests")
endif(BUILD_TESTS)

find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(doxygen/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMMENT "Generating API documentation with Doxygen" VERBATIM)

  configure_file(doxygen/Doxyfile.NET.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.NET @ONLY)
  configure_file(doxygen/mainpage_net.md ${CMAKE_CURRENT_BINARY_DIR}/mainpage_net.md COPYONLY)
  add_custom_target(doc-net ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.NET WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMMENT "Generating .NET API documentation with Doxygen" VERBATIM)
  
  configure_file(doxygen/Doxyfile.Java.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.Java @ONLY)
  add_custom_target(doc-java ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.Java WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMMENT "Generating Java API documentation with Doxygen" VERBATIM)
  
endif(DOXYGEN_FOUND)

set(API_HEADERS
    dependencies/libmms/hal/inc/hal_base.h
    dependencies/libmms/hal/inc/hal_time.h
    dependencies/libmms/hal/inc/hal_thread.h
    dependencies/libmms/hal/inc/hal_socket.h
    dependencies/libmms/hal/inc/tls_config.h
    dependencies/libmms/src/common/inc/linked_list.h
    dependencies/libmms/src/common/inc/libmms_common_api.h
    dependencies/libmms/src/mms/inc/iso_connection_parameters.h
    src/inc/tase2_api_exports.h
    src/inc/tase2_endpoint.h 
    src/inc/tase2_client.h
    src/inc/tase2_common.h
    src/inc/tase2_model.h
    src/inc/tase2_server.h
)


IF(MINGW)
target_link_libraries (tase2
   -lwsock32
   -lws2_32
)   


target_link_libraries (tase2-shared
   -lwsock32
   -lws2_32
)
ENDIF(MINGW)                 


set(LIBDIR "lib")

install(FILES ${API_HEADERS} DESTINATION include/libtase2 COMPONENT Development)

install (TARGETS tase2 tase2-shared
	ARCHIVE DESTINATION ${LIBDIR} COMPONENT Libraries
    LIBRARY DESTINATION ${LIBDIR} COMPONENT Libraries
)

# add_subdirectory(dependencies/libmms/examples/mms_utility)

