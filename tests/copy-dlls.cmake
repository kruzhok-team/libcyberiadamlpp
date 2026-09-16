# Copy the DLLs sitting next to the built test executable into the test
# directory. $<TARGET_RUNTIME_DLLS> lists only the targets CMake knows the
# location of: everything linked through an import library is invisible to it,
# and the vcpkg deployment puts its DLLs next to the original executable only.

file(GLOB dlls "${SRC}/*.dll")
if(dlls)
  file(COPY ${dlls} DESTINATION "${DST}")
endif()
