include_directories(
   .
)

set(EXAMPLE_SRC
   browse_client.c
)

IF(WIN32)
set_source_files_properties(${EXAMPLE_SRC} PROPERTIES LANGUAGE CXX)
ENDIF(WIN32)

add_executable(browse_client
  ${EXAMPLE_SRC}
)

target_link_libraries(browse_client
    tase2
)
