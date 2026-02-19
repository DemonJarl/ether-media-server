cmake_minimum_required(VERSION 4.0)
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.8/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)
project(ether-media-server CXX)

include(CheckIncludeFileCXX)

check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)
if (NOT "${CMAKE_CXX_STANDARD}" STREQUAL "")
    # Do nothing
elseif (HAS_ANY AND HAS_STRING_VIEW )#AND HAS_COROUTINE) clang не поддерживает корутины 
    set(CMAKE_CXX_STANDARD 20)
elseif (HAS_ANY AND HAS_STRING_VIEW)
    set(CMAKE_CXX_STANDARD 17)
else ()
    set(CMAKE_CXX_STANDARD 14)
endif ()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_executable(${PROJECT_NAME} main.cc)

# ##############################################################################
# If you include the drogon source code locally in your project, use this method
# to add drogon 
# add_subdirectory(drogon) 
# target_link_libraries(${PROJECT_NAME} PRIVATE drogon)
#
# and comment out the following lines
set(CMAKE_POLICY_VERSION_MINIMUM 3.5) 


find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
    libavdevice
    libavfilter
    libavformat
    libavcodec
    libswresample
    libswscale
    libavutil
)
include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 99f140d) 
#FetchContent_Declare(thread-pool https://github.com/DeveloperPaul123/thread-pool
#                         GIT_TAG f1dc484145c3542d84a7757513cea356da273042) 
#FetchContent_MakeAvailable(thread-pool)
FetchContent_MakeAvailable(cpr)
FetchContent_Declare(libcron GIT_REPOSITORY https://github.com/PerMalmberg/libcron.git
                         GIT_TAG ee34810b11bd23c8be637345532f91059b68b2d7) 
FetchContent_MakeAvailable(libcron)
FetchContent_Declare(bcrypt GIT_REPOSITORY https://github.com/hilch/Bcrypt.cpp)
FetchContent_MakeAvailable(bcrypt)
# Fetch the project and make it available for use.
include(FetchContent)
FetchContent_Declare(
    libcoro
    GIT_REPOSITORY https://github.com/jbaldwin/libcoro.git
    GIT_TAG        v0.15.0
)
FetchContent_MakeAvailable(libcoro)

CPMAddPackage(
    NAME BS_thread_pool
    GITHUB_REPOSITORY bshoshany/thread-pool
    VERSION 5.0.0
    EXCLUDE_FROM_ALL
    SYSTEM
)

CPMAddPackage(
    #NAME algorithm
    GITHUB_REPOSITORY boostorg/boost
    GIT_TAG boost-1.89.0
)

CPMAddPackage(
    NAME cache
    GITHUB_REPOSITORY vpetrigo/caches
    GIT_TAG v0.1.1
)

add_library(BS_thread_pool INTERFACE)
target_include_directories(BS_thread_pool INTERFACE ${BS_thread_pool_SOURCE_DIR}/include)
find_package(Drogon CONFIG REQUIRED)
find_package(spdlog REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::LIBAV tbb Boost::algorithm BS_thread_pool bcrypt libcron spdlog::spdlog cpr::cpr Drogon::Drogon caches PkgConfig::LIBAV libcoro pthread rt) 

#set (CMAKE_CXX_FLAGS  "-${CMAKE_CXX_FLAGS} ${SEQAN_CXX_FLAGS} -ltbb")
# ##############################################################################

if (CMAKE_CXX_STANDARD LESS 17)
    message(FATAL_ERROR "c++17 or higher is required")
elseif (CMAKE_CXX_STANDARD LESS 20)
    message(STATUS "use c++17")
else ()
    message(STATUS "use c++20")
endif ()

aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)
aux_source_directory(utils UTILS_SRC)
# aux_source_directory(MetaDataAPI METADATAAPI_SRC)
aux_source_directory(MetaDataAPI/TMDB TMDBAPI_SRC)
aux_source_directory(MetaDataAPI/TMDB/Models TMDBAPI_MODELS_SRC)
aux_source_directory(MetaDataAPI/TMDB/Endpoints TMDBAPI_ENDPOINTS_SRC)
aux_source_directory(MetaDataAPI/TMDB/Enums TMDBAPI_ENUMS_SRC)
aux_source_directory(MetaDataAPI/TMDB/Utils TMDBAPI_UTILS_SRC)


drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
                    ${CMAKE_CURRENT_BINARY_DIR})
# use the following line to create views with namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
#                     ${CMAKE_CURRENT_BINARY_DIR} TRUE)
# use the following line to create views with namespace CHANGE_ME prefixed
# and path namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
#                     ${CMAKE_CURRENT_BINARY_DIR} TRUE CHANGE_ME)

target_include_directories(${PROJECT_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR}/models
                                   ${CMAKE_CURRENT_SOURCE_DIR}/utils
                                   ${CMAKE_CURRENT_SOURCE_DIR}/MetaDataAPI
                                   ${CMAKE_CURRENT_SOURCE_DIR}/MetaDataAPI/TMDB
                                   ${CMAKE_CURRENT_SOURCE_DIR}/MetaDataAPI/TMDB/Models
                                   ${CMAKE_CURRENT_SOURCE_DIR}/MetaDataAPI/TMDB/Endpoints
                                   ${CMAKE_CURRENT_SOURCE_DIR}/MetaDataAPI/TMDB/Enums
                                   ${CMAKE_CURRENT_SOURCE_DIR}/MetaDataAPI/TMDB/Utils
                                   )
target_sources(${PROJECT_NAME}
               PRIVATE
               ${SRC_DIR}
               ${CTL_SRC}
               ${FILTER_SRC}
               ${PLUGIN_SRC}
               ${MODEL_SRC}
               ${UTILS_SRC}
               ${METADATAAPI_SRC}
               ${TMDBAPI_SRC}
               ${TMDBAPI_MODELS_SRC}
               ${TMDBAPI_ENDPOINTS_SRC}
               ${TMDBAPI_ENUMS_SRC}
               ${TMDBAPI_UTILS_SRC}
               )
# ##############################################################################
# uncomment the following line for dynamically loading views 
# set_property(TARGET ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS ON)

# ##############################################################################

add_subdirectory(test)