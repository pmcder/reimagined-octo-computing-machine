cmake_minimum_required(VERSION 3.16)

project(props-parse C)

set(CMAKE_C_STANDARD 11)

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

#add_library(props-parse STATIC ${SOURCES})
#install(TARGETS props-parse EXPORT props-parse-targets
#        ARCHIVE DESTINATION lib
#        INCLUDES DESTINATION include)
#install(DIRECTORY include/ DESTINATION include)

