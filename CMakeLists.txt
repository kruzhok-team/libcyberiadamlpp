cmake_minimum_required(VERSION 3.21)

project(cyberiadamlpp VERSION 1.0.6)

find_package(LibXml2 REQUIRED)
if(NOT LibXml2_FOUND)
  message(FATAL_ERROR "Cannot find libxml2 library")
endif()

find_package(cyberiadaml REQUIRED)
if(NOT cyberiadaml_FOUND)
  message(FATAL_ERROR "Cannot find libcyberiadaml library")
endif()

if(WIN32)
  # htgeom is linked through its import library, which does not tell where the
  # DLL is, so the tests cannot get it from $<TARGET_RUNTIME_DLLS>
  find_file(HTGeom_RUNTIME_LIBRARY NAMES htgeom.dll
            HINTS "${cyberiadaml_LIBRARY}" "${cyberiadaml_LIBRARY}/.." "${HTGeom_DIR}"
            PATH_SUFFIXES bin lib)
  if(NOT HTGeom_RUNTIME_LIBRARY)
    message(WARNING "Cannot find htgeom.dll; copy it next to the executables manually")
  endif()
endif()

add_compile_definitions($<$<CONFIG:Debug>:__DEBUG__>)

if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall)
  add_compile_options(-Wextra)
  add_compile_options(-Wpedantic)
  add_compile_options(-Wformat-security)
  # /W4 reports the hidden members, -Wall -Wextra do not
  add_compile_options(-Wshadow)
endif()

if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(cyberiadamlpp SHARED cyberiadamlpp.cpp)
target_include_directories(cyberiadamlpp PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/cyberiada>
  "${cyberiadaml_INCLUDE_DIRS}")
			 
target_link_directories(cyberiadamlpp PUBLIC "${cyberiadaml_LIBRARY}")
target_link_libraries(cyberiadamlpp PUBLIC "${cyberiadaml_LIBRARIES}")

add_executable(cyberiadapp main.cpp)
target_include_directories(cyberiadapp PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/cyberiada>)
target_link_directories(cyberiadapp PUBLIC "${PROJECT_BINARY_DIR}")
target_link_libraries(cyberiadapp PUBLIC cyberiadamlpp)

enable_testing()

option(MEMCHECK "Run the tests under valgrind memcheck" OFF)
if(MEMCHECK)
  find_program(VALGRIND valgrind REQUIRED)
  set(MEMCHECK_COMMAND "${VALGRIND} --tool=memcheck -s --leak-check=full --error-exitcode=1")
endif()

file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/tests/")
file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")
foreach(source_path ${files})
  get_filename_component(test_name "${source_path}" NAME_WE)
  string(CONCAT target_name "${test_name}" ".test")
  add_executable("${target_name}" "${source_path}")
  target_include_directories("${target_name}" PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
  target_link_directories("${target_name}" PUBLIC "${PROJECT_BINARY_DIR}")
  target_link_libraries("${target_name}" PUBLIC cyberiadamlpp)
  # the tests derive their data file names from argv[0], so the copy keeps
  # the name without the platform executable suffix
  add_custom_command(TARGET "${target_name}"
    POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "$<TARGET_FILE:${target_name}>"
            "${PROJECT_BINARY_DIR}/tests/${target_name}"
    )
  if(WIN32)
    add_custom_command(TARGET "${target_name}"
      POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
              "$<TARGET_RUNTIME_DLLS:${target_name}>"
              "${PROJECT_BINARY_DIR}/tests/"
      COMMAND_EXPAND_LISTS
      )
    if(HTGeom_RUNTIME_LIBRARY)
      add_custom_command(TARGET "${target_name}"
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${HTGeom_RUNTIME_LIBRARY}"
                "${PROJECT_BINARY_DIR}/tests/"
        )
    endif()
  endif()
  # the binary path is relative to the build directory: the tests
  # resolve their input/output files and print the paths via argv[0]
  string(SUBSTRING "${test_name}" 0 2 test_num)
  set(test_args "-DBINARY=tests/${target_name}")
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/${target_name}-input.graphml" OR
      EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/${test_num}-output.txt")
    list(APPEND test_args "-DCAPTURE=1")
  endif()
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/${test_num}-output.txt")
    list(APPEND test_args "-DPATTERN_TXT=${CMAKE_CURRENT_SOURCE_DIR}/tests/${test_num}-output.txt")
  endif()
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/${test_num}-output.graphml")
    list(APPEND test_args "-DPATTERN_GRAPHML=${CMAKE_CURRENT_SOURCE_DIR}/tests/${test_num}-output.graphml")
  endif()
  if(MEMCHECK)
    list(APPEND test_args "-DMEMCHECK=${MEMCHECK_COMMAND}")
  endif()
  add_test(NAME "${test_name}"
    COMMAND ${CMAKE_COMMAND} ${test_args} -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/runtest.cmake")
  set_tests_properties("${test_name}" PROPERTIES WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")
endforeach()

file(GLOB test_files
  "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.graphml"
  "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.txt"
  )
foreach(f ${test_files})
  get_filename_component(fname "${f}" NAME)
  file(COPY_FILE
    "${f}"
    "${PROJECT_BINARY_DIR}/tests/${fname}"
    ONLY_IF_DIFFERENT)  
endforeach()

file(COPY_FILE
  ${PROJECT_SOURCE_DIR}/run-tests.sh
  ${PROJECT_BINARY_DIR}/run-tests.sh ONLY_IF_DIFFERENT)

file(COPY_FILE
  ${PROJECT_SOURCE_DIR}/run-mem-tests.sh
  ${PROJECT_BINARY_DIR}/run-mem-tests.sh ONLY_IF_DIFFERENT)

install(TARGETS cyberiadamlpp EXPORT cyberiadamlpp
  RUNTIME DESTINATION bin COMPONENT runtime
  LIBRARY DESTINATION lib COMPONENT runtime
  ARCHIVE DESTINATION lib COMPONENT dev)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cyberiadamlpp.h
  DESTINATION include/cyberiada
  COMPONENT dev)
install(EXPORT cyberiadamlpp DESTINATION lib/cmake/cyberiadamlpp
  COMPONENT dev)

configure_file(
  ${PROJECT_SOURCE_DIR}/cyberiadamlpp-config.cmake.in
  ${PROJECT_BINARY_DIR}/cyberiadamlpp-config.cmake @ONLY)

install(FILES
  ${PROJECT_BINARY_DIR}/cyberiadamlpp-config.cmake
  DESTINATION lib/cmake/cyberiadamlpp
  COMPONENT dev)

configure_file ("${PROJECT_SOURCE_DIR}/CyberiadaMLPackOptions.cmake.in"
                "${PROJECT_BINARY_DIR}/CyberiadaMLPackOptions.cmake"
                @ONLY)
set (CPACK_PROJECT_CONFIG_FILE
     "${PROJECT_BINARY_DIR}/CyberiadaMLPackOptions.cmake")

include(CPack)
