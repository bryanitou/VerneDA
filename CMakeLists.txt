############################################
# VerneDA COMPILATION CMAKE SCRIPT
############################################

# CMAKE MINIMUM REQUIRED VERSION
cmake_minimum_required(VERSION 3.21)

# PROJECT: VerneDA
# Create project, load parameters (flags & default values)
project(VerneDA)

# CXX Standard to be used by the compiler
set(CMAKE_CXX_STANDARD 17)

############################################
# GENERAL INFORMATION
############################################

# OS name & version
#cmake_host_system_information(RESULT OS_NAME QUERY OS_NAME)
#cmake_host_system_information(RESULT OS_RELEASE QUERY OS_RELEASE)
#cmake_host_system_information(RESULT OS_VERSION QUERY OS_VERSION)
#cmake_host_system_information(RESULT OS_PLATFORM QUERY OS_PLATFORM)
#message(STATUS "SYSTEM: ${OS_NAME} ${OS_RELEASE} ${OS_VERSION} ${OS_PLATFORM}")

# CPU info
#cmake_host_system_information(RESULT PROC_NAME QUERY PROCESSOR_DESCRIPTION)
#message(STATUS "PROCESSOR: ${PROC_NAME}")

# Memory
#cmake_host_system_information(RESULT VIRT_MEM QUERY TOTAL_PHYSICAL_MEMORY)
#message(STATUS "MEMORY: ${VIRT_MEM} MiB")

############################################
# GET NUMBER OF CPU CORES
############################################

# Load ProcessorCount module
include(ProcessorCount)

# Try to get number of cores
ProcessorCount(N_CORES)

# Safety-check: if unable to get number of cores, assume quad-core system
if(N_CORES EQUAL 0)
    set(N_CORES 4)
endif()

# Info
message(STATUS "N_CORES: ${N_CORES}")

# Set global compilation flag: number of jobs
set(CTEST_BUILD_FLAGS -j${N_CORES})

############################################
# CMAKE OPTIONS
############################################
option(BUILD_DACE_MASTER_LIBS "Build DACE library from MASTER" OFF)
option(BUILD_DACE_AFOSSA_LIBS "Build DACE library from AFOSSA fork" OFF)

# Message
MESSAGE( STATUS "BUILD_DACE_MASTER_LIBS: ${BUILD_DACE_MASTER_LIBS}")
MESSAGE( STATUS "BUILD_DACE_AFOSSA_LIBS: ${BUILD_DACE_AFOSSA_LIBS}")

# Options safety checks
if (BUILD_DACE_MASTER_LIBS EQUAL BUILD_DACE_AFOSSA_LIBS OR (NOT BUILD_DACE_MASTER_LIBS) EQUAL (NOT BUILD_DACE_AFOSSA_LIBS))
    MESSAGE( FATAL_ERROR "Please choose at least one: BUILD_DACE_MASTER_LIBS or BUILD_DACE_AFOSSA_LIBS. Not both.")
endif ()

if (BUILD_DACE_MASTER_LIBS)
    MESSAGE( STATUS "DACE will be built from git MASTER branch tared file: version on Oct 22, 2022. <- Frozen version.")
endif ()

if (BUILD_DACE_AFOSSA_LIBS)
    MESSAGE( STATUS "DACE will be built from git AFOSSA (Alberto Fossa) fork. <- Version of this fork and updates are totally user responsible.")
endif ()

############################################
# SET OUTPUT DIRECTORIES
############################################

# Check for install prefix flag, not set (loaded default /usr/local) -> set it to current dir
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX  "${CMAKE_CURRENT_BINARY_DIR}"
            CACHE PATH "${CMAKE_CURRENT_BINARY_DIR}" FORCE)
endif()

# 3rd parties repositories
set(CMAKE_3RDPARTY_REPS "${CMAKE_SOURCE_DIR}/reps"
        CACHE STRING "Default directories where all the thirdparty repositories will be")
message(STATUS "CMAKE_3RDPARTY_REPS: ${CMAKE_3RDPARTY_REPS}")
string(APPEND UCFLAGS " -D CMAKE_3RDPARTY_REPS=\"\\\"${CMAKE_3RDPARTY_REPS}\\\"\"")

# 3rd parties directory
set(CMAKE_3RDPARTY_DIR "${CMAKE_SOURCE_DIR}/build/VerneDA-3rdparty"
        CACHE STRING "Default directory to extracted 3rd parties")
message(STATUS "CMAKE_3RDPARTY_DIR: ${CMAKE_3RDPARTY_DIR}")
string(APPEND UCFLAGS " -D CMAKE_3RDPARTY_DIR=\"\\\"${CMAKE_3RDPARTY_DIR}\\\"\"")

# DACE directories
set(CMAKE_3RDPARTY_DIR_DACE_MASTER "${CMAKE_3RDPARTY_DIR}/dace-master"
        CACHE STRING "Default directory to extracted 3rd parties: dace-master")
message(STATUS "CMAKE_3RDPARTY_DIR_DACE_MASTER: ${CMAKE_3RDPARTY_DIR_DACE_MASTER}")
string(APPEND UCFLAGS " -D CMAKE_3RDPARTY_DIR_DACE_MASTER=\"\\\"${CMAKE_3RDPARTY_DIR_DACE_MASTER}\\\"\"")

set(CMAKE_3RDPARTY_DIR_DACE_AFOSSA "${CMAKE_3RDPARTY_DIR}/dace-afossa"
        CACHE STRING "Default directory to extracted 3rd parties: dace-afossa")
message(STATUS "CMAKE_3RDPARTY_DIR_DACE_AFOSSA: ${CMAKE_3RDPARTY_DIR_DACE_AFOSSA}")
string(APPEND UCFLAGS " -D CMAKE_3RDPARTY_DIR_DACE_AFOSSA=\"\\\"${CMAKE_3RDPARTY_DIR_DACE_AFOSSA}\\\"\"")

# RSJp directories
set(CMAKE_3RDPARTY_DIR_RJSP_MASTER "${CMAKE_3RDPARTY_DIR}/RSJp-cpp-master"
        CACHE STRING "Default directory to extracted 3rd parties: RSJp-cpp-master")
message(STATUS "CMAKE_3RDPARTY_DIR_RJSP_MASTER: ${CMAKE_3RDPARTY_DIR_RJSP_MASTER}")
string(APPEND UCFLAGS " -D CMAKE_3RDPARTY_DIR_RJSP_MASTER=\"\\\"${CMAKE_3RDPARTY_DIR_RJSP_MASTER}\\\"\"")

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/bin/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib/")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib/")

# Define var to store all user flags
set(UCFLAGS " ")

# Some user defined variables TODO: Set this outside here
set(PYPLOT_SIMPLE_PATH "${CMAKE_SOURCE_DIR}/common/tools-py/plotter.py")
set(PYPLOT_BANANA_PATH "${CMAKE_SOURCE_DIR}/common/tools-py/banana.py")
set(PYPLOT_TYPE_TRANSLATION "translation")
set(PYPLOT_TYPE_ATTITUDE "attitude")

# Application version information
# This is automatic; do not touch anything
string(APPEND UCFLAGS " -D CODE_VERSION=\"\\\"$(shell cat ${CMAKE_SOURCE_DIR}/VERSION)\\\"\"")
string(APPEND UCFLAGS " -D OS_VERSION=\"\\\"$(shell uname -iors)\\\"\"")
string(APPEND UCFLAGS " -D GIT_BRANCH=\"\\\"$(shell git symbolic-ref --short HEAD)\\\"\"")
string(APPEND UCFLAGS " -D GIT_HASH=\"\\\"$(shell git log -1 --pretty=format:\"%H\")\\\"\"")
string(APPEND UCFLAGS " -D USER_NAME=\"\\\"$(USER)\\\"\"")
string(APPEND UCFLAGS " -D PYPLOT_SIMPLE=\"\\\"${PYPLOT_SIMPLE_PATH}\\\"\"")
string(APPEND UCFLAGS " -D PYPLOT_BANANA=\"\\\"${PYPLOT_BANANA_PATH}\\\"\"")
string(APPEND UCFLAGS " -D PYPLOT_TRANSLATION=\"\\\"${PYPLOT_TYPE_TRANSLATION}\\\"\"")
string(APPEND UCFLAGS " -D PYPLOT_ATTITUDE=\"\\\"${PYPLOT_TYPE_ATTITUDE}\\\"\"")

# Force re-compilation of STATIC_FLAGS.h
# This phony target will always re-compile STATIC_FLAGS.h, thus always updating the compilation flag's values
# add_custom_target(RECOMPILE_STATIC_FLAGS touch ${CMAKE_SOURCE_DIR}/src/core/STATIC_FLAGS.h)

# Note for cmake developers: add this phony dependency to all main programs and subroutines that use static flags.
# Ex: add_dependencies(VerneDA_some_executable RECOMPILE_STATIC_FLAGS)

############################################
# BUILD EXTERNAL PROJECTS
############################################
# Note for cmake developers: this projects have their own CMakeLists, we have to build and install them
# TODO: Do something like this if the project requires some kind of system library to be installed, like boost.
# find_package(Eigen3 REQUIRED)
# IF( NOT EIGEN3_INCLUDE_DIR )
#     MESSAGE( FATAL_ERROR "Please point the environment variable EIGEN3_INCLUDE_DIR to the include directory of your Eigen3 installation.")
# ENDIF()
# message(STATUS "Eigen is in: ${EIGEN3_INCLUDE_DIR}")
#
# find_package(Boost COMPONENTS program_options)
# IF( NOT Boost_INCLUDE_DIRS )
#     MESSAGE( FATAL_ERROR "Please point the environment variable Boost_INCLUDE_DIRS to the include directory of your BOOST installation.")
# ENDIF()
# message(STATUS "Boost is in: ${Boost_INCLUDE_DIRS}")

# Load ExternalProject module
include(ExternalProject)

## TODO: Let the user choose between CMAKE_3RDPARTY_DIR_DACE_MASTER or CMAKE_3RDPARTY_DIR_DACE_AFOSSA
if (BUILD_DACE_MASTER_LIBS)
    set(CMAKE_3RDPARTY_DIR_DACE "${CMAKE_3RDPARTY_DIR_DACE_MASTER}")
elseif (BUILD_DACE_AFOSSA_LIBS) # TODO: Try to download this from the internet directly instead of having it as a submodule
    set(CMAKE_3RDPARTY_DIR_DACE "${CMAKE_3RDPARTY_DIR_DACE_AFOSSA}")
endif ()

## Build DACE Application, it can be found in this link: https://github.com/dacelib/dace/wiki/Getting-Started
ExternalProject_Add(dace
        SOURCE_DIR ${CMAKE_3RDPARTY_DIR_DACE}/
        PREFIX ${CMAKE_3RDPARTY_DIR_DACE}/
        CMAKE_COMMAND cmake ..
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_INSTALL_PREFIX=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}dace/
        BUILD_COMMAND make -j${N_CORES}
        INSTALL_DIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}dace/
        INSTALL_COMMAND make install
)

# Build RJSP: Ridiculously JSON Parser: https://github.com/subh83/RSJp-cpp
add_custom_command(OUTPUT ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}RSJp/include/RSJparser.tcc
        COMMAND make PREFIX=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}RSJp
        WORKING_DIRECTORY ${CMAKE_3RDPARTY_DIR_RJSP_MASTER}
)

#ExternalProject_Add_StepTargets(rsjplib install)
#file(GLOB MY_PUBLIC_HEADERS
#"${CMAKE_3RDPARTY_DIR_RJSP_MASTER}/*.tcc"
#)
#file(COPY ${MY_PUBLIC_HEADERS} DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}RSJp)

# TODO: OPTION TO EXPLORE! -> https://stackoverflow.com/questions/10487256/cmake-how-to-properly-copy-static-librarys-header-file-into-usr-include
# INSTALL(TARGETS mylib
#         LIBRARY DESTINATION some/libpath
#         PUBLIC_HEADER DESTINATION some/includepath
#         )

############################################
# EXTERNAL INFORMATION
############################################
# Get info from external dace library
ExternalProject_Get_Property(dace install_dir)
MESSAGE( STATUS "Getting DACE libraries from the installed directory...: '${install_dir}'")
set(daceinclude "${install_dir}/include")
set(dacelib_dir "${install_dir}/lib")
set(dacelib_file "${CMAKE_SHARED_LIBRARY_PREFIX}dace${CMAKE_SHARED_LIBRARY_SUFFIX}")
set(dacelib_path "${dacelib_dir}/${dacelib_file}")

############################################
# FILES LOCATION
############################################
# TODO: Modularize this hard path
set(CMD_WHO whoami)
execute_process(
        COMMAND ${CMD_WHO}
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE WHO
        RESULT_VARIABLE ENV_RES
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

# Include MATLAB stuff (optional) for MEX files
option(BUILD_WITH_MATLAB "Build with matlab libraries" OFF)
MESSAGE( STATUS "BUILD_WITH_MATLAB: ${BUILD_WITH_MATLAB}")
if (BUILD_WITH_MATLAB)

    # Look for MATLAB
    # Note: matlab should be in the global path (test whereis matlab in PATH for UNIX)
    find_package(Matlab 23.2 EXACT REQUIRED)

    IF( NOT Matlab_INCLUDE_DIRS )
        MESSAGE( FATAL_ERROR "Please point the environment variable Matlab_INCLUDE_DIRS to the include directory of your MATLAB installation.")
    ELSE ()
        MESSAGE( STATUS "Matlab root dir: ${Matlab_ROOT_DIR}")
        MESSAGE( STATUS "Matlab header files: ${Matlab_INCLUDE_DIRS}")
        MESSAGE( STATUS "Matlab mex library: ${Matlab_MEX_LIBRARY}")
        MESSAGE( STATUS "Matlab mex extension: ${Matlab_MEX_EXTENSION}")
        MESSAGE( STATUS "Matlab mx library: ${Matlab_MX_LIBRARY}")
    ENDIF()

    # TODO: Usually matlab is built in home: ATTENTION HERE!
    MESSAGE( STATUS "Including MATLAB MEX files includes from '${WHO}'s home...")
    include_directories(
            ${daceinclude}
            src/core
            ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}RSJp/include
            # Matlab stuff
            ${Matlab_INCLUDE_DIRS}
    )
else ()
    include_directories(
            ${daceinclude}
            src/core
            ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}RSJp/include)
endif ()

if (BUILD_WITH_MATLAB)
    #===========================================
    # LIBRARY: MATLAB LIBRARIES (IMPORTED LIBRARY)
    #===========================================
    #MESSAGE( STATUS "Including MATLAB extern libraries in '${WHO}'s home...")
    #set(MatlabEngine_path "/home/${WHO}/MATLAB/R2023b/extern/bin/glnxa64/libMatlabEngine.so" )
    #set(MatlabDataArray_path "/home/${WHO}/MATLAB/R2023b/extern/bin/glnxa64/libMatlabDataArray.so" )
    #
    #set(LIBRARY_MatlabEngine "MatlabEnginelib")
    #
    #add_library(${LIBRARY_MatlabEngine} SHARED IMPORTED)
    #
    ## Provide the full path to the library, so CMake knows where to find it
    #set_target_properties(${LIBRARY_MatlabEngine} PROPERTIES
    #        IMPORTED_LOCATION ${MatlabEngine_path})
    #
    #set(LIBRARY_MatlabDataArray "MatlabDataArraylib")
    #
    #add_library(${LIBRARY_MatlabDataArray} SHARED IMPORTED)
    #
    ## Provide the full path to the library, so CMake knows where to find it
    #set_target_properties(${LIBRARY_MatlabDataArray} PROPERTIES
    #        IMPORTED_LOCATION ${MatlabDataArray_path})
endif ()

#===========================================
# LIBRARY: DACE (IMPORTED LIBRARY)
#===========================================
set(LIBRARY_DACE "dacelib")

add_library(${LIBRARY_DACE} SHARED IMPORTED)

# Provide the full path to the library, so CMake knows where to find it
set_target_properties(${LIBRARY_DACE} PROPERTIES
        IMPORTED_LOCATION ${dacelib_path})

#===========================================
# LIBRARY: RSJp-cpp-master (IMPORTED LIBRARY)
#===========================================
# TODO: Fix this bullshit
set(LIBRARY_RSJP_LIB "rsjp")

add_library(${LIBRARY_RSJP_LIB} SHARED
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}RSJp/include/RSJparser.tcc)

# Provide the full path to the library, so CMake knows where to find it
set_target_properties(${LIBRARY_RSJP_LIB} PROPERTIES
        LINKER_LANGUAGE CXX
        COMPILE_FLAGS "-fPIC"
        PUBLIC_HEADER "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}RSJp/RSJparser.tcc"
        LINK_FLAGS "-Wl,-rpath,./")

#===========================================
# LIBRARY: VerneDA BASE
#===========================================
add_library(base SHARED
        src/core/base/enums.h
        src/core/base/constants.h
        src/core/base/Header_Info.cpp
        src/core/base/structs.h
)

set_target_properties(base
        PROPERTIES COMPILE_FLAGS "-fPIC"
        LINK_FLAGS "-Wl,-rpath,./") # To use relative paths in shared libs
#
#===========================================
# LIBRARY: VerneDA TOOLS
#===========================================
set(LIBRARY_TOOLS "tools")

add_library(${LIBRARY_TOOLS} SHARED
        src/core/tools/str.cpp
        src/core/tools/math.cpp
        src/core/tools/ep.cpp)

add_dependencies(tools
        base)

target_link_libraries(${LIBRARY_TOOLS}
        ${LIBRARY_BASE})

set_target_properties(${LIBRARY_TOOLS} PROPERTIES
        COMPILE_FLAGS "-fPIC"
        LINK_FLAGS "-Wl,-rpath,./") # To use relative paths in shared libs

#
#===========================================
# LIBRARY: VerneDA DA_TOOLS
#===========================================
set(LIBRARY_DATOOLS "datools")

add_library(${LIBRARY_DATOOLS} SHARED
        src/core/tools/vo.cpp
        src/core/tools/io_dace.cpp
)

add_dependencies(datools
        dacelib
        tools)

target_link_libraries(${LIBRARY_DATOOLS}
        ${LIBRARY_DACE}
        ${LIBRARY_TOOLS})

set_target_properties(${LIBRARY_DATOOLS} PROPERTIES
        COMPILE_FLAGS "-fPIC"
        LINK_FLAGS "-Wl,-rpath,./") # To use relative paths in shared libs

#===========================================
# LIBRARY: VerneDA SPECIFICATIONS
#===========================================
set(LIBRARY_SPECIFICATIONS "specs_input")

add_library(${LIBRARY_SPECIFICATIONS} SHARED
        src/core/specs/args_input.cpp
)

add_dependencies(specs_input
        base)

target_link_libraries(${LIBRARY_SPECIFICATIONS}
        ${LIBRARY_BASE})

set_target_properties(${LIBRARY_SPECIFICATIONS} PROPERTIES
        COMPILE_FLAGS "-fPIC"
        LINK_FLAGS "-Wl,-rpath,./") # To use relative paths in shared libs

#===========================================
# LIBRARY: VerneDA JSON parser
#===========================================
set(LIBRARY_JSON_PARSER "json")

add_library(${LIBRARY_JSON_PARSER} SHARED
        src/core/json/json_parser.cpp)

add_dependencies(json
        rsjp
        tools
        specs_input)

target_link_libraries(json
        rsjp
        tools
        specs_input)

set_target_properties(${LIBRARY_JSON_PARSER} PROPERTIES
        COMPILE_FLAGS "-fPIC"
        LINK_FLAGS "-Wl,-rpath,./") # To use relative paths in shared libs

#===========================================
# LIBRARY: VerneDA CORE
#===========================================
set(LIBRARY_CORE "core")

add_library(${LIBRARY_CORE} SHARED
        src/core/conversions/angles.cpp
        src/core/geometry/point.cpp
        src/core/solids/solid.cpp
        src/core/scv.cpp
        src/core/integrator.cpp
        src/core/problems.cpp
        src/core/delta.cpp
        src/core/quaternion.cpp
        src/core/tools/io.cpp)

add_dependencies(core
        datools
)

target_link_libraries(${LIBRARY_CORE}
        ${LIBRARY_DATOOLS}
)

set_target_properties(${LIBRARY_CORE} PROPERTIES
        COMPILE_FLAGS "-fPIC"
        LINK_FLAGS "-Wl,-rpath,./") # To use relative paths in shared libs

#===========================================
# LIBRARY: VerneDA ADOS (Automatic DOmain Splitting)
#===========================================
set(LIBRARY_ADS "ads")

add_library(${LIBRARY_ADS} SHARED
        src/core/ads/Manifold.cpp
        src/core/ads/SuperManifold.cpp
        src/core/ads/Patch.cpp
        src/core/ads/SplittingHistory.cpp
)

add_dependencies(ads
        core)

target_link_libraries(${LIBRARY_ADS}
        ${LIBRARY_CORE}
)

set_target_properties(${LIBRARY_ADS} PROPERTIES
        COMPILE_FLAGS "-fPIC"
        LINK_FLAGS "-Wl,-rpath,./") # To use relative paths in shared libs

#===========================================
# LIBRARY: VerneDA WRITER (Writer)
#===========================================
set(LIBRARY_WRITER "writer")

add_library(${LIBRARY_WRITER} SHARED
        src/core/writer.cpp)

add_dependencies(writer
        core)

target_link_libraries(${LIBRARY_WRITER}
        ${LIBRARY_CORE}
)

set_target_properties(${LIBRARY_WRITER} PROPERTIES
        COMPILE_FLAGS "-fPIC"
        LINK_FLAGS "-Wl,-rpath,./") # To use relative paths in shared libs

#===========================================
# LIBRARY: VerneDA FILE PROCESSOR (File processor)
#===========================================
set(LIBRARY_FILE_PROCESSOR "file_processor")

add_library(${LIBRARY_FILE_PROCESSOR} SHARED
        src/core/FileProcessor.cpp)

add_dependencies(file_processor
        tools)

target_link_libraries(${LIBRARY_FILE_PROCESSOR}
        ${LIBRARY_TOOLS}
)

set_target_properties(${LIBRARY_FILE_PROCESSOR} PROPERTIES
        COMPILE_FLAGS "-fPIC"
        LINK_FLAGS "-Wl,-rpath,./") # To use relative paths in shared libs

#===========================================
# LIBRARY: VerneDA MEX FILES and library for utils (mex_utils)
#===========================================
if (BUILD_WITH_MATLAB)
    # MEX functions files
    matlab_add_mex(NAME mex_vsaod SRC src/main/mex_vsaod.cpp)

    target_link_libraries(mex_vsaod
            base
            ads
    )

endif ()
########################################################################################################################
# EXECUTABLES: DACE mains
########################################################################################################################
############################################
# EXECUTABLES: EXAMPLE 1
############################################
set(EXECUTABLE_NAME "Example1-dace_test")

add_executable(${EXECUTABLE_NAME}
        src/main/tutorials/Example1-dace_test.cpp
)

# add_dependencies(${EXECUTABLE_NAME}
#                   dependency1
#                   dependency2
#                   ....
#                   )

target_link_libraries(${EXECUTABLE_NAME}
        core
        dacelib
)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        COMPILE_FLAGS "${UCFLAGS} -D PROGRAM_NAME=\"\\\"${PROGRAM_NAME}\\\"\"" # Compilation flags
        LINK_FLAGS "-Wl,-rpath,./") # Use cwd to search shared libs

############################################
# EXECUTABLES: EXAMPLE 2
############################################
set(EXECUTABLE_NAME "Example2-dace_test")

add_executable(${EXECUTABLE_NAME}
        src/main/tutorials/Example2-dace_test.cpp
)

# add_dependencies(${EXECUTABLE_NAME}
#                   dependency1
#                   dependency2
#                   ....
#                   )

target_link_libraries(${EXECUTABLE_NAME}
        core
        dacelib
)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        COMPILE_FLAGS "${UCFLAGS} -D PROGRAM_NAME=\"\\\"${PROGRAM_NAME}\\\"\"" # Compilation flags
        LINK_FLAGS "-Wl,-rpath,./") # Use cwd to search shared libs


############################################
# EXECUTABLES: EXAMPLE 3
############################################
set(EXECUTABLE_NAME "Example3-dace_test")

add_executable(${EXECUTABLE_NAME}
        src/main/tutorials/Example3-dace_test.cpp
)

# add_dependencies(${EXECUTABLE_NAME}
#                   dependency1
#                   dependency2
#                   ....
#                   )

target_link_libraries(${EXECUTABLE_NAME}
        core
        dacelib
)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        COMPILE_FLAGS "${UCFLAGS} -D PROGRAM_NAME=\"\\\"${PROGRAM_NAME}\\\"\"" # Compilation flags
        LINK_FLAGS "-Wl,-rpath,./") # Use cwd to search shared libs


############################################
# EXECUTABLES: EXAMPLE 4
############################################
set(EXECUTABLE_NAME "Example4-dace_test")

add_executable(${EXECUTABLE_NAME}
        src/main/tutorials/Example4-dace_test.cpp
)

# add_dependencies(${EXECUTABLE_NAME}
#                   dependency1
#                   dependency2
#                   ....
#                   )

target_link_libraries(${EXECUTABLE_NAME}
        core
        dacelib
)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        COMPILE_FLAGS "${UCFLAGS} -D PROGRAM_NAME=\"\\\"${PROGRAM_NAME}\\\"\"" # Compilation flags
        LINK_FLAGS "-Wl,-rpath,./") # Use cwd to search shared libs

############################################
# EXECUTABLES: DACE_VSAD (DACE VERY SIMPLE ATTITUDE DETERMINATION)
############################################
set(EXECUTABLE_NAME "dace_vsad")

add_executable(${EXECUTABLE_NAME}
        src/main/dace_vsad.cpp
)

# add_dependencies(${EXECUTABLE_NAME}
#                   dependency1
#                   dependency2
#                   ....
#                   )

target_link_libraries(${EXECUTABLE_NAME}
        base
        json
        ads
        file_processor
        writer
)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        COMPILE_FLAGS "${UCFLAGS} -D PROGRAM_NAME=\"\\\"${PROGRAM_NAME}\\\"\"" # Compilation flags
        LINK_FLAGS "-Wl,-rpath,./") # Use cwd to search shared libs

############################################
# EXECUTABLES: DACE_VSOD (DACE VERY SIMPLE ORBIT DETERMINATION)
############################################
set(EXECUTABLE_NAME "dace_vsod")

add_executable(${EXECUTABLE_NAME}
        src/main/dace_vsod.cpp
)

# add_dependencies(${EXECUTABLE_NAME}
#                   dependency1
#                   dependency2
#                   ....
#                   )

target_link_libraries(${EXECUTABLE_NAME}
        base
        json
        ads
        file_processor
        writer
)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        COMPILE_FLAGS "${UCFLAGS} -D PROGRAM_NAME=\"\\\"${PROGRAM_NAME}\\\"\"" # Compilation flags
        LINK_FLAGS "-Wl,-rpath,./") # Use cwd to search shared libs

############################################
# EXECUTABLES: DACE_STATIC_TRANSFORMATION
############################################
set(EXECUTABLE_NAME "dace_st")

add_executable(${EXECUTABLE_NAME}
        src/main/dace_st.cpp
)

# add_dependencies(${EXECUTABLE_NAME}
#                   dependency1
#                   dependency2
#                   ....
#                   )

target_link_libraries(${EXECUTABLE_NAME}
        base
        json
        ads
        file_processor
        writer
)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        COMPILE_FLAGS "${UCFLAGS} -D PROGRAM_NAME=\"\\\"${PROGRAM_NAME}\\\"\"" # Compilation flags
        LINK_FLAGS "-Wl,-rpath,./") # Use cwd to search shared libs

############################################
# EXECUTABLES: Kepler (Author: Alberto Fossà)
############################################
set(EXECUTABLE_NAME "Kepler")

add_executable(${EXECUTABLE_NAME}
        src/main/kepler.cpp
)

# add_dependencies(${EXECUTABLE_NAME}
#                   dependency1
#                   dependency2
#                   ....
#                   )

target_link_libraries(${EXECUTABLE_NAME}
        core
        dacelib
)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        COMPILE_FLAGS "${UCFLAGS} -D PROGRAM_NAME=\"\\\"${PROGRAM_NAME}\\\"\"" # Compilation flags
        LINK_FLAGS "-Wl,-rpath,./") # Use cwd to search shared libs

########################################################################################################################
##################################### EXECUTABLES: ADS Automatic Domain Splitting ######################################
########################################################################################################################

############################################
# EXECUTABLES: ADS Test (Author: Alberto Fossà)
############################################
set(EXECUTABLE_NAME "ads_test")

add_executable(${EXECUTABLE_NAME}
        src/main/ads/testProp.cpp
)

# add_dependencies(${EXECUTABLE_NAME}
#                   dependency1
#                   dependency2
#                   ....
#                   )

target_link_libraries(${EXECUTABLE_NAME}
        ads
        dacelib
        core
)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        COMPILE_FLAGS "${UCFLAGS} -D PROGRAM_NAME=\"\\\"${PROGRAM_NAME}\\\"\"" # Compilation flags
        LINK_FLAGS "-Wl,-rpath,./") # Use cwd to search shared libs
