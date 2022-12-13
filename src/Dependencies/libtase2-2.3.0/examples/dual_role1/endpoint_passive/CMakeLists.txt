include_directories(
   .
)

set(EXAMPLE_SRC
   endpoint_passive.c
)

IF(WIN32)
set_source_files_properties(${EXAMPLE_SRC} PROPERTIES LANGUAGE CXX)
ENDIF(WIN32)

add_executable(endpoint_passive
  ${EXAMPLE_SRC}
)

target_link_libraries(endpoint_passive
    tase2
)
