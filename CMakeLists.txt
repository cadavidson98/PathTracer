cmake_minimum_required(VERSION 3.9)
project(Path_Tracer LANGUAGES C CXX)

message("Current dir: ${CMAKE_CURRENT_SOURCE_DIR}")

set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(MATH_DIR ${CMAKE_CURRENT_SOURCE_DIR}/math)
set(MAT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mat)
set(LIGHT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/light)
set(GEOM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/geom)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)

file(GLOB_RECURSE MATH_SOURCE math/*.cpp)
file(GLOB_RECURSE MATH_HEADER math/*.h)

set(LIGHT_SOURCE ${LIGHT_DIR}/area_light.cpp ${LIGHT_DIR}/direction_light.cpp)
set(LIGHT_HEADER ${LIGHT_DIR}/light.h ${LIGHT_DIR}/area_light.h ${LIGHT_DIR}/direction_light.h)

file(GLOB_RECURSE GEOM_SOURCE geom/*.cpp geom/bounding_volume.tpp)
file(GLOB_RECURSE GEOM_HEADER geom/*.h)

file(GLOB_RECURSE CORE_SOURCE src/*.cpp)
file(GLOB_RECURSE CORE_HEADER include/*.h)

file(GLOB_RECURSE MAT_SOURCE mat/*.cpp)
file(GLOB_RECURSE MAT_HEADER mat/*.h)

add_executable(${PROJECT_NAME} ${MATH_SOURCE} ${MATH_HEADER} ${LIGHT_SOURCE} ${LIGHT_HEADER} ${GEOM_SOURCE} ${GEOM_HEADER} ${MAT_SOURCE} ${MAT_HEADER} ${CORE_SOURCE} ${CORE_HEADER})

set(BUILD_SHARED_LIBS FALSE CACHE BOOL "x" FORCE)
set(ASSIMP_NO_EXPORT TRUE CACHE BOOL "x" FORCE)
set(ASSIMP_BUILD_TESTS FALSE CACHE BOOL "x" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT FALSE CACHE BOOL "x" FORCE)
set(ASSIMP_INSTALL_PDB FALSE CACHE BOOL "x" FORCE)
set(ASSIMP_BUILD_ZLIB TRUE CACHE BOOL "x" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS FALSE CACHE BOOL "x" FORCE)
set(ASSIMP_BUILD_COLLADA_IMPORTER TRUE CACHE BOOL "x" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER TRUE CACHE BOOL "x" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER TRUE CACHE BOOL "x" FORCE)
set(ASSIMP_BUILD_BLEND_IMPORTER TRUE CACHE BOOL "x" FORCE)

add_subdirectory(extern/stbimage)
add_subdirectory(extern/pugixml)

find_package(OpenMP)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR} include ${MATH_DIR} ${GEOM_DIR} ${MAT_DIR} ${LIGHT_DIR})
target_link_libraries(${PROJECT_NAME} stbimage OpenMP::OpenMP_CXX pugixml)

set(DATA_DIR_BUILD ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(DATA_DIR_INSTALL ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/data)

message(STATUS "Data dir (in build tree): ${DATA_DIR_BUILD}")
message(STATUS "Data dir (in install tree): ${DATA_DIR_INSTALL}")

# Configure a header file to pass some of the CMake settings to the source code
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

install(
    DIRECTORY data/
    DESTINATION ${DATA_DIR_INSTALL}
)