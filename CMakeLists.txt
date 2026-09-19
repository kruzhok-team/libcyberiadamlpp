cmake_minimum_required(VERSION 3.16)

project(cyberiadamlpp VERSION 1.0.6)

find_package(LibXml2 REQUIRED)
if(NOT LibXml2_FOUND)
  message(FATAL_ERROR "Cannot find libxml2 library")
endif()

find_package(cyberiadaml REQUIRED)
if(NOT cyberiadaml_FOUND)
  message(FATAL_ERROR "Cannot find libcyberiadaml library")
endif()

# cyberiadamlpp.cpp calls the htgeom API (htree_new_*) directly, so it must link
# htgeom itself: MinGW/Windows resolves nothing transitively at link time
find_package(HTGeom REQUIRED)
if(NOT HTGeom_FOUND)
  message(FATAL_ERROR "Cannot find libhtgeom library")
endif()

# Under a cross-build the tests run via the emulator with the whole prefix on its
# DLL path (WINEPATH), so this bulk copy of every prefix DLL is unnecessary — and,
# run per test target into a shared dir under parallel make, it races. The fresh
# cyberiada DLLs still reach the tests via $<TARGET_RUNTIME_DLLS> below. Keep the
# copy only for a native Windows build.
if(WIN32 AND NOT CMAKE_CROSSCOMPILING)
  # the library DLLs live in the install prefix: libcyberiadaml puts its own
  # into bin, libhtreegeom puts htgeom.dll beside its import library in lib.
  # $<TARGET_RUNTIME_DLLS> cannot report htgeom.dll, which reaches the link
  # through an import library path rather than a target
  get_filename_component(CYB_PREFIX_LIB "${cyberiadaml_LIBRARY}" ABSOLUTE)
  file(GLOB CYB_DEPENDENCY_DLLS "${CYB_PREFIX_LIB}/*.dll" "${CYB_PREFIX_LIB}/../bin/*.dll")
  # an installed copy of this library must never overwrite the fresh build
  # output: the executables are linked against the new import library
  string(TOLOWER "${PROJECT_NAME}.dll" CYB_OWN_DLL)
  set(CYB_PREFIX_DLLS)
  foreach(dll IN LISTS CYB_DEPENDENCY_DLLS)
    get_filename_component(dll_name "${dll}" NAME)
    string(TOLOWER "${dll_name}" dll_name)
    if(NOT "${dll_name}" STREQUAL "${CYB_OWN_DLL}")
      list(APPEND CYB_PREFIX_DLLS "${dll}")
    endif()
  endforeach()
  set(CYB_DEPENDENCY_DLLS ${CYB_PREFIX_DLLS})
  if(NOT CYB_DEPENDENCY_DLLS)
    message(WARNING "No library DLLs found near ${CYB_PREFIX_LIB}; "
                    "copy cyberiadaml.dll and htgeom.dll next to the executables manually")
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
# the cyberiadaml include/lib dirs are absolute build-prefix paths from its config,
# so they are build-only: baking them into the export makes the installed config
# non-relocatable. The installed headers share include/cyberiada (INSTALL_INTERFACE)
# and libcyberiadaml.so resolves from the install location at link/run time
target_include_directories(cyberiadamlpp PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/cyberiada>
  $<BUILD_INTERFACE:${cyberiadaml_INCLUDE_DIRS}>)

target_link_directories(cyberiadamlpp PUBLIC $<BUILD_INTERFACE:${cyberiadaml_LIBRARY}>)
# htgeom is linked by its absolute build-prefix path (build-only, kept off the
# export for relocatability); it resolves the htree_* symbols at link time
target_link_libraries(cyberiadamlpp PUBLIC "${cyberiadaml_LIBRARIES}" $<BUILD_INTERFACE:${HTGeom_LIBRARY}>)

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
  if(WIN32 AND CMAKE_VERSION VERSION_GREATER_EQUAL 3.21)
    add_custom_command(TARGET "${target_name}"
      POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
              "$<TARGET_RUNTIME_DLLS:${target_name}>"
              "${PROJECT_BINARY_DIR}/tests/"
      COMMAND_EXPAND_LISTS
      )
    # next to the executable the build produces, and next to the copy the
    # test driver actually runs
    if(CYB_DEPENDENCY_DLLS)
      add_custom_command(TARGET "${target_name}"
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                ${CYB_DEPENDENCY_DLLS}
                "$<TARGET_FILE_DIR:${target_name}>"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                ${CYB_DEPENDENCY_DLLS}
                "${PROJECT_BINARY_DIR}/tests/"
        COMMAND_EXPAND_LISTS
        )
    endif()
    add_custom_command(TARGET "${target_name}"
      POST_BUILD
      COMMAND "${CMAKE_COMMAND}"
              -DSRC=$<TARGET_FILE_DIR:${target_name}>
              -DDST=${PROJECT_BINARY_DIR}/tests
              -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/copy-dlls.cmake"
      )
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
  # run a cross-built exe under CMAKE_CROSSCOMPILING_EMULATOR (e.g. wine64); empty natively
  if(CMAKE_CROSSCOMPILING_EMULATOR)
    list(APPEND test_args "-DEMULATOR=${CMAKE_CROSSCOMPILING_EMULATOR}")
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
  file(COPY "${f}" DESTINATION "${PROJECT_BINARY_DIR}/tests/")
endforeach()

file(COPY "${PROJECT_SOURCE_DIR}/run-tests.sh" DESTINATION "${PROJECT_BINARY_DIR}")

file(COPY "${PROJECT_SOURCE_DIR}/run-mem-tests.sh" DESTINATION "${PROJECT_BINARY_DIR}")

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
