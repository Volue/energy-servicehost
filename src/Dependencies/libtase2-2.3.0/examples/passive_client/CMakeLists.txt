include_directories(
   .
)

set(EXAMPLE_SRC
   passive_client.c
)

IF(WIN32)
set_source_files_properties(${EXAMPLE_SRC} PROPERTIES LANGUAGE CXX)
ENDIF(WIN32)

add_executable(passive_client
  ${EXAMPLE_SRC}
)

target_link_libraries(passive_client
    tase2
)
