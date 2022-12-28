if(C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS_CODECOV "-O3 -fprofile-arcs -fprofile-abs-path -ftest-coverage " CACHE STRING "Flags used by the C compiler in coverage builds." FORCE)
    set(CMAKE_CXX_FLAGS_CODECOV "-O3 -fprofile-arcs -fprofile-abs-path -ftest-coverage" CACHE STRING "Flags used by the C++ compiler in coverage builds." FORCE)
    set(CMAKE_EXE_LINKER_FLAGS_CODECOV "-fgcov" CACHE STRING "Flags used for linking binaries in coverage builds." FORCE)
    set(CMAKE_SHARED_LINKER_FLAGS_CODECOV "-fgcov" CACHE STRING "Flags used by the shared libraries linker in coverage builds." FORCE)
elseif(C_COMPILER_ID STREQUAL "Clang" OR C_COMPILER_ID STREQUAL "AppleClang")
    set(CMAKE_C_FLAGS_CODECOV "${CMAKE_C_FLAGS_RELEASE} -fprofile-instr-generate" CACHE STRING "Flags used by the C compiler during CodeCov builds." FORCE)
elseif(C_COMPILER_ID STREQUAL "Intel")
    set(CMAKE_C_FLAGS_CODECOV "${CMAKE_C_FLAGS_RELEASE} -prof-gen" CACHE STRING "Flags used by the C compiler during CodeCov builds." FORCE)
elseif(C_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_C_FLAGS_CODECOV "${CMAKE_C_FLAGS_RELEASE} /Wall /Wabi /fsanitize=coverage" CACHE STRING "Flags used by the C compiler during CodeCov builds." FORCE)
endif()
mark_as_advanced(
  CMAKE_CXX_FLAGS_CODECOV
  CMAKE_C_FLAGS_CODECOV
  CMAKE_EXE_LINKER_FLAGS_CODECOV
  CMAKE_SHARED_LINKER_FLAGS_CODECOV)