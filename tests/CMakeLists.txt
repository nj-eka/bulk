cmake_minimum_required(VERSION 3.26)

########## PROJECT ###########
project(bulk_gtest
    VERSION ${GITVERSIONDETECT_VERSION_MAJOR}.${GITVERSIONDETECT_VERSION_MINOR}.${GITVERSIONDETECT_VERSION_PATCH}
    DESCRIPTION "bulk unit (google) tests" 
    LANGUAGES CXX
)

if(PROJECT_IS_TOP_LEVEL)
    find_package(GTest) # 1.13 REQUIRED)
    # find_package(Threads REQUIRED)    
    include(CTest)
    enable_testing()
endif() 

add_executable(
    ${PROJECT_NAME}
    "${CMAKE_CURRENT_SOURCE_DIR}/ostream_writer_gtest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/file_writer_gtest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bulk_translator_gtest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/console_parser_gtest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/hw_gtest.cpp"
)
set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD ${CMAKE_CXX_STANDARD}
        CXX_STANDARD_REQUIRED ${CMAKE_CXX_STANDARD_REQUIRED}
        # COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${GTEST_INCLUDE_DIRS}
        "${CMAKE_SOURCE_DIR}/src"            
)
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        ${GTEST_BOTH_LIBRARIES}
)
add_test(${PROJECT_NAME} ${PROJECT_NAME})
# set_tests_properties(${PROJECT_NAME} PROPERTIES ENVIRONMENT "EXAMPLE1=\"${CMAKE_CURRENT_SOURCE_DIR}/examples/cmds1\";EXAMPLE2=\"${CMAKE_CURRENT_SOURCE_DIR}/examples/cmds2\"")

if (MSVC)
    target_compile_options(
        ${PROJECT_NAME}
        PRIVATE  
            /W4
    )
else()
    target_compile_options(
        ${PROJECT_NAME}
        PRIVATE
            -Wall -Wextra -pedantic
    )
endif(MSVC)
