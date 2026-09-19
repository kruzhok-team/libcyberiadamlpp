# The CyberiadaML++ test driver: run a test binary and compare its output
# with the pattern files.
#
# Usage: cmake -DBINARY=<test binary> [-DCAPTURE=1] [-DPATTERN_TXT=<file>]
#        [-DPATTERN_GRAPHML=<file>] [-DMEMCHECK=<command>] [-DEMULATOR=<wrapper>]
#        -P runtest.cmake

if(NOT BINARY)
  message(FATAL_ERROR "BINARY is not set")
endif()

set(test_command)
if(MEMCHECK)
  separate_arguments(test_command UNIX_COMMAND "${MEMCHECK}")
endif()
# EMULATOR carries CMAKE_CROSSCOMPILING_EMULATOR (e.g. wine64) so a cross-built
# exe runs under it; empty on a native build
if(EMULATOR)
  list(APPEND test_command ${EMULATOR})
endif()
list(APPEND test_command "${BINARY}")

if(CAPTURE)
  execute_process(COMMAND ${test_command}
    OUTPUT_FILE "${BINARY}.txt"
    RESULT_VARIABLE status)
else()
  execute_process(COMMAND ${test_command}
    RESULT_VARIABLE status)
endif()
if(NOT status EQUAL 0)
  message(FATAL_ERROR "test run failed: ${status}")
endif()

function(compare_with_pattern output pattern)
  if(NOT EXISTS "${output}")
    message(FATAL_ERROR "test output ${output} is missing")
  endif()
  file(READ "${output}" output_content)
  file(READ "${pattern}" pattern_content)
  # the tests derive their data file from argv[0], which Wine reports as an
  # absolute Windows path (e.g. Z:\...\tests\NAME.graphml); normalize it back to
  # the platform-independent tests/NAME form the golden uses (a no-op natively)
  string(REGEX REPLACE "[A-Za-z]:[\\\\/][^']*[\\\\/]tests[\\\\/]" "tests/"
         output_content "${output_content}")
  if(NOT output_content STREQUAL pattern_content)
    find_program(diff_tool diff)
    if(diff_tool)
      execute_process(COMMAND "${diff_tool}" -u "${pattern}" "${output}")
    endif()
    message(FATAL_ERROR "output ${output} didn't match the pattern ${pattern}")
  endif()
endfunction()

if(PATTERN_TXT)
  compare_with_pattern("${BINARY}.txt" "${PATTERN_TXT}")
endif()
if(PATTERN_GRAPHML)
  compare_with_pattern("${BINARY}.graphml" "${PATTERN_GRAPHML}")
endif()
