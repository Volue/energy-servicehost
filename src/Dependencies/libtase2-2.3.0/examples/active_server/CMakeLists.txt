include_directories(
   .
)

set(EXAMPLE_SRC
   active_server.c
)

IF(WIN32)
set_source_files_properties(${EXAMPLE_SRC} PROPERTIES LANGUAGE CXX)
ENDIF(WIN32)

add_executable(active_server
  ${EXAMPLE_SRC}
)

target_link_libraries(active_server
    tase2
)
