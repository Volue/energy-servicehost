include_directories(
   .
)

set(EXAMPLE_SRC
   iccp_client1.c
)

IF(WIN32)
set_source_files_properties(${EXAMPLE_SRC} PROPERTIES LANGUAGE CXX)
ENDIF(WIN32)

add_executable(iccp_client
  ${EXAMPLE_SRC}
)

target_link_libraries(iccp_client
    tase2
)
