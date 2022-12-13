include_directories(
   .
)

set(EXAMPLE_SRC
   iccp_server1.c
)

IF(WIN32)
set_source_files_properties(${EXAMPLE_SRC} PROPERTIES LANGUAGE CXX)
ENDIF(WIN32)

add_executable(iccp_server
  ${EXAMPLE_SRC}
)

target_link_libraries(iccp_server
    tase2
)
