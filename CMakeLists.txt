cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

########## INCLUDE ###########
file(GLOB MODULE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/*/" LIST_DIRECTORIES true)
list(APPEND CMAKE_MODULE_PATH "${MODULE_DIRS}")
include(git_version_detect)
include(target_properties)

########## PROJECT ###########
project(bulk
    VERSION ${GITVERSIONDETECT_VERSION_MAJOR}.${GITVERSIONDETECT_VERSION_MINOR}.${GITVERSIONDETECT_VERSION_PATCH}
    DESCRIPTION "course c++ developer: hw7 - batch (bulk) commands processing program" 
    LANGUAGES CXX
)

include(CMakePrintHelpers)
get_cmake_property(varNames VARIABLES)
cmake_print_variables(${varNames})

###### Dependencies ######

## BOOST
# add_definitions(-BOOST_LOG_DYN_LINK)
set(BOOST_ENABLE_CMAKE ON)
set(Boost_FIND_QUIETLY OFF)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS program_options)

if (Boost_FOUND)
    message("Boost VERSION: ${Boost_VERSION}")
    message("Boost INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message("Boost Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
    message("Boost LIBRARIES: ${Boost_LIBRARIES}")
    # include_directories(${Boost_INCLUDE_DIRS}) # -> set(TARGETS ... INCLUDE_DIRECTORIES ...)
    # link_directories(${Boost_LIBRARY_DIRS})    
endif()

####### TESTING ########
if(BUILD_TESTING)
    find_package(GTest) # 1.13 REQUIRED)

    include(CTest)
    enable_testing()

    add_subdirectory(tests)
endif(BUILD_TESTING)

########## INFO STATIC LIBRARY ###########
add_subdirectory(src/info) # static

########## BULK PROGRAM ###########
add_executable(${PROJECT_NAME} src/main.cpp)
set_target_properties(${PROJECT_NAME} 
    PROPERTIES
        CXX_STANDARD ${CMAKE_CXX_STANDARD}
        CXX_STANDARD_REQUIRED ${CMAKE_CXX_STANDARD_REQUIRED}
)
target_compile_options(${PROJECT_NAME} 
    PRIVATE
	    -Wall -Wextra -pedantic -fpermissive # -Werror
)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${Boost_INCLUDE_DIRS}    
        "${CMAKE_CURRENT_SOURCE_DIR}/src/info/include"        
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_link_libraries(
    ${PROJECT_NAME}
    Boost::program_options
    info_static    
)
print_target_properties(${PROJECT_NAME})

########## DOXYGEN ###########
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in 
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
    @ONLY
)

########## INSTALL ############
install(TARGETS ${PROJECT_NAME})

########## PACKAGE ############
if (RELEASE_VERSION)
    set(CPACK_GENERATOR "${PACKAGE}")
    set(CPACK_PACKAGE_VERSION "${RELEASE_VERSION}")
    set(CPACK_PACKAGE_CONTACT "${GIT_DESCRIBE_PROJECT_REPOSITORY_URL}")
    include(CPack)
endif()
