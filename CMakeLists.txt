cmake_minimum_required(VERSION 3.16)

project(props-parse C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(props-parse STATIC
        src/properties_parse.c
        src/properties_parse.h
        include/properties_parse_api.h
)

target_include_directories(props-parse
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

add_executable(props-parse-dev
        src/pptest.c
)

target_link_libraries(props-parse-dev PRIVATE props-parse)

install(TARGETS props-parse EXPORT props-parseTargets)
install(DIRECTORY include/ DESTINATION include)

install(EXPORT props-parseTargets
        FILE props-parseTargets.cmake
        NAMESPACE props-parse::
        DESTINATION lib/cmake/props-parse
)

install(FILES props-parseConfig.cmake
        DESTINATION lib/cmake/props-parse
)

enable_testing()

include(FetchContent)
FetchContent_Declare(
        unity
        GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
        GIT_TAG v2.7.0
)
FetchContent_GetProperties(unity)
if(NOT unity_POPULATED)
    FetchContent_Populate(unity)
endif()

# Build Unity ourselves from its single source file rather than
# add_subdirectory-ing Unity's own CMakeLists.txt - keeps the dependency
# surface to "one file we compile ourselves".
add_library(unity STATIC ${unity_SOURCE_DIR}/src/unity.c)
target_include_directories(unity SYSTEM PUBLIC ${unity_SOURCE_DIR}/src)

# Compile properties_parse.c directly into the test binary (rather than
# linking the props-parse static library) so it can be built with
# sanitizers below without instrumenting the shipped library artifact.
add_executable(props-parse-tests
        test/test_properties_parse.c
        src/properties_parse.c
)

target_link_libraries(props-parse-tests PRIVATE unity)

# The test suite exercises parsePropFile()/getValue() directly (white-box),
# which are only declared in the internal src/properties_parse.h, not the
# public include/properties_parse_api.h.
target_include_directories(props-parse-tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_definitions(props-parse-tests PRIVATE
        FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures/"
)

# AddressSanitizer + UndefinedBehaviorSanitizer on the test binary only -
# catches memory-safety and UB bugs in properties_parse.c for free, at the
# cost of a couple of compiler flags. Not applied to the shipped library.
if(NOT MSVC)
    target_compile_options(props-parse-tests PRIVATE -g -fsanitize=address,undefined)
    target_link_options(props-parse-tests PRIVATE -fsanitize=address,undefined)
endif()

add_test(NAME props-parse-unit-tests COMMAND props-parse-tests)

#add_library(props-parse STATIC ${SOURCES})
#install(TARGETS props-parse EXPORT props-parse-targets
#        ARCHIVE DESTINATION lib
#        INCLUDES DESTINATION include)
#install(DIRECTORY include/ DESTINATION include)

