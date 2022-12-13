include_directories(
   .
)

set(EXAMPLE_SRC
   endpoint2.c
)

IF(WIN32)
set_source_files_properties(${EXAMPLE_SRC} PROPERTIES LANGUAGE CXX)
ENDIF(WIN32)

add_executable(endpoint2
  ${EXAMPLE_SRC}
)

target_link_libraries(endpoint2
    tase2
)
