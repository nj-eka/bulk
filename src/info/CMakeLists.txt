cmake_minimum_required(VERSION 3.26)

project(
    info_static
    VERSION ${PROJECT_VERSION} # todo: check that version is derived from the base project 
    DESCRIPTION "Project info static lib"
    LANGUAGES CXX
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/info/config.h.in 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/info/config.h
    # ESCAPE_QUOTES
    # NEWLINE_STYLE UNIX
    @ONLY
)

add_library(info_static STATIC)
set_target_properties(
    info_static PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        VERSION ${PROJECT_VERSION} # set properties for the target. VERSION set the library version to the project version
        OUTPUT_NAME info  # ipfilter_info(.a|.dll)
)
target_include_directories(
    info_static
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_sources(
    info_static
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/version.cpp
)

##### TESTS #######
if(BUILD_TESTING)
    if(PROJECT_IS_TOP_LEVEL)
        include(CTest)
    endif() 
    
    find_package(GTest REQUIRED)
    add_executable(
        info_unit_tests 
        "tests/version.gtest.cpp"
    )
    set_target_properties(
        info_unit_tests 
        PROPERTIES
            CXX_STANDARD ${CMAKE_CXX_STANDARD}
            CXX_STANDARD_REQUIRED ${CMAKE_CXX_STANDARD_REQUIRED}
    )
    target_include_directories(
        info_unit_tests
        PRIVATE
            ${GTEST_INCLUDE_DIRS}
            "${CMAKE_CURRENT_SOURCE_DIR}/include" 
    )
    target_link_libraries(
        info_unit_tests
        PRIVATE 
            ${GTEST_BOTH_LIBRARIES}
            info_static
    )
    add_test(info_unit_tests info_unit_tests)
    if (MSVC)
        target_compile_options(
            info_unit_tests
            PRIVATE  
                /W4
        )
    else()
        target_compile_options(
            info_unit_tests 
            PRIVATE
                -Wall -Wextra -pedantic -Werror
        )
    endif(MSVC) 
endif(BUILD_TESTING)