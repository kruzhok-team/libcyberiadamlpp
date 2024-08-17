cmake_minimum_required(VERSION 3.21)

project(cyberiadamlpp VERSION 1.0)

find_package(LibXml2 REQUIRED)
if(!LibXml2_FOUND)
  message(FATAL_ERROR "Cannot find libxml2 library")
endif()

set(cyberiadaml_DIR /usr/lib/cmake)
find_package(cyberiadaml REQUIRED)
if(!cyberiadaml_FOUND)
  message(FATAL_ERROR "Cannot find libcyberiadaml library")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D__DEBUG__")

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

file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/tests/")
file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")
foreach(source_path ${files})
  get_filename_component(target_name "${source_path}" NAME_WE)
  string(CONCAT target_name "${target_name}" ".test")
  add_executable("${target_name}" "${source_path}")
  target_include_directories("${target_name}" PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
  target_link_directories("${target_name}" PUBLIC "${PROJECT_BINARY_DIR}")
  target_link_libraries("${target_name}" PUBLIC cyberiadamlpp)
  if (WIN32)
    add_custom_command(TARGET "${target_name}"
      POST_BUILD
      COMMAND copy "${PROJECT_BINARY_DIR}/${target_name}" "${PROJECT_BINARY_DIR}/tests/"
      )
  else()
    add_custom_command(TARGET "${target_name}"
      POST_BUILD
      COMMAND ln -sf "${PROJECT_BINARY_DIR}/${target_name}" "${PROJECT_BINARY_DIR}/tests/"
      )
  endif()
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

install(TARGETS cyberiadamlpp DESTINATION lib EXPORT cyberiadamlpp)
install(FILES cyberiadamlpp.h
  ${CMAKE_CURRENT_SOURCE_DIR}/cyberiadamlpp.h
  DESTINATION include/cyberiada)
install(EXPORT cyberiadamlpp DESTINATION lib/cmake)

configure_file(
  ${PROJECT_SOURCE_DIR}/cyberiadamlpp-config.cmake.in
  ${PROJECT_BINARY_DIR}/cyberiadamlpp-config.cmake @ONLY)

install(FILES
  ${PROJECT_BINARY_DIR}/cyberiadamlpp-config.cmake
  DESTINATION lib/cmake)
