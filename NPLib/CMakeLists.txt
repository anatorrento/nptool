cmake_minimum_required (VERSION 2.8)
include(CheckCXXCompilerFlag)
project(NPLib CXX)
set(CMAKE_BUILD_TYPE Release)

# Major change in the Core/Physics (affecting the detector/analysis/simulation)
set(NPLIB_VERSION_MAJOR 2)
# Minor change in the Core/Physics (not affecting any other part)
set(NPLIB_VERSION_MINOR 1)
# Change any of the detector in NPA
set(NPLIB_VERSION_DETA 35)

configure_file(Core/NPLibVersion.h.in Core/NPLibVersion.h @ONLY)
set(DETLIST ${ETLIST})

string(COMPARE EQUAL "${DETLIST}" "" rdet)
if(rdet)
    message("Building all detectors")
  else()
    message("Building the following detectors ${DETLIST}") 
endif()

# Setting the policy to match Cmake version
cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})

# This suppress the Up-to-Date message of file installed for cmake 3.1 and above
set(CMAKE_INSTALL_MESSAGE LAZY) 

# look for Root
include("ressources/CMake/Root.cmake")

# Check for user disabling of c++11 support
string(COMPARE EQUAL "${CPP11}" "no" nocpp11)
if(nocpp11)
 message(" -> C++11 support disable")
endif()


# If the compiler is Clang, silence the unrecognised flags
if(${CMAKE_CXX_COMPILER_ID} MATCHES ".*Clang.*")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments -fcolor-diagnostics")
  if( UNIX AND NOT APPLE )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  endif()
  if( APPLE AND CLANG_VERSION_MAJOR VERSION_LESS 5 AND NOT nocpp11)
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  endif()
endif()

# Adjust the compiler language flag
set(NOCPPFLAGS true)
if(${CMAKE_CXX_FLAGS} MATCHES ".*std=.*11")
  set(NOCPPFLAGS false)
elseif(${CMAKE_CXX_FLAGS} MATCHES ".*std=.*0x")
  set(NOCPPFLAGS false) 
endif()

if(NOCPPFLAGS)
  CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
  if(COMPILER_SUPPORTS_CXX11 AND NOT nocpp11)
	    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else()
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
  endif()

  if(COMPILER_SUPPORTS_CXX0X AND NOT nocpp11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  endif()
endif()

set(CMAKE_BINARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
set(CMAKE_INCLUDE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/include )

# A macro that return the list of directory to compile
MACRO(subdirlist result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/Detectors/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      string(COMPARE EQUAL "${child}" "CMakeFiles" r0)
      string(COMPARE EQUAL "${child}" ".DS_Store" r1) 
      set(det "")
      string(REPLACE "Detectors/" "" det ${child})

      IF(NOT r0 AND NOT r1)
        string(COMPARE EQUAL "${DETLIST}" "" r2)
        IF(r2)
          LIST(APPEND dirlist ${child})
        ELSEIF(${DETLIST} MATCHES ".*${det}.*" )
          LIST(APPEND dirlist ${child}) 
        ENDIF()
      ENDIF()
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

# Add include directories to all target
include_directories("Core/")
include_directories("Physics/")
include_directories("Online/")

# Call the macro
subdirlist(SUB_DIRECTORY ${CMAKE_BINARY_DIR})
set(SUB_DIRECTORY ${SUB_DIRECTORY} Core Physics Calibration Online Utility)
# Add each sub folder to the project
set(TARGET_LIST "")
foreach(subdir ${SUB_DIRECTORY})
# add the subdirectory
  add_subdirectory(${subdir})
  set(target "")
  string(REPLACE "Detectors/" "" target ${subdir})
  string(COMPARE EQUAL "${target}" "Utility" r0)
  string(COMPARE EQUAL "${target}" "Maya" r1)
   
  if(NOT r0 AND NOT r1)
    LIST(APPEND TARGET_LIST NP${target})
  endif()
  if(r1 AND DEFINED ENV{ROOTSYS})
	LIST(APPEND TARGET_LIST NP${target})
  endif()

endforeach()

export(TARGETS ${TARGET_LIST} FILE NPLibTargets.cmake)
export(PACKAGE NPLib)
file(RELATIVE_PATH REL_INCLUDE_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" "${CMAKE_INCLUDE_OUTPUT_DIRECTORY}")
set(CONF_INCLUDE_DIRS "./" "./")
configure_file(ressources/CMake/NPLibConfig.cmake.in "${PROJECT_BINARY_DIR}/NPLibConfig.cmake" @ONLY)
configure_file(scripts/nptool-cleaner.sh "${CMAKE_BINARY_OUTPUT_DIRECTORY}/nptool-cleaner" @ONLY)
configure_file(scripts/nptool-wizard.sh "${CMAKE_BINARY_OUTPUT_DIRECTORY}/nptool-wizard" @ONLY)
