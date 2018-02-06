cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)

include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/Goganych/hunter/archive/v0.19.123.tar.gz"
    SHA1 "2193dcd2972d8f299818a237a516d04de2e52984"
)

project(builder)

# download boost.process
hunter_add_package(Boost COMPONENTS filesystem system program_options)
hunter_add_package(BoostProcess)

find_package(Boost COMPONENTS filesystem system program_options REQUIRED)
find_package(BoostProcess CONFIG REQUIRED)

add_executable(builder ${CMAKE_CURRENT_SOURCE_DIR}/builder.cpp)

target_link_libraries(builder Boost::filesystem Boost::system BoostProcess::boost_process Boost::program_options)

install(TARGETS builder RUNTIME DESTINATION bin)
