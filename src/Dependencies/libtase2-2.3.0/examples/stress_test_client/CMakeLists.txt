include_directories(
   .
)

set(EXAMPLE_SRC
   stress_test_client.c
)

IF(WIN32)
set_source_files_properties(${EXAMPLE_SRC} PROPERTIES LANGUAGE CXX)
ENDIF(WIN32)

add_executable(stress_test_client
  ${EXAMPLE_SRC}
)

target_link_libraries(stress_test_client
    tase2
)
