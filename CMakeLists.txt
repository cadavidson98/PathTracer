cmake_minimum_required(VERSION 3.9)
project(Path_Tracer LANGUAGES C CXX)

message("Current dir: ${CMAKE_CURRENT_SOURCE_DIR}")

set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(SOURCEFILES src/main.cpp 
                src/assimp_file_loader.cpp                 
                src/bvh.cpp 
                src/camera.cpp 
                src/cook_torrence.cpp
                src/disney_principled.cpp
                src/legacy_file_loader.cpp
                src/matrix.cpp 
                src/ray_tracer.cpp 
                src/scene.cpp
                src/sobol2D.cpp 
                src/random_sampler.cpp
                src/vec.cpp)

set(HEADERFILES include/config.h
                include/assimp_file_loader.h
                include/bvh.h 
                include/camera.h
                include/collision.h
                include/cook_torrence.h
                include/disney_principled.h
                include/file_loader.h
                include/legacy_file_loader.h
                include/math_helpers.h
                include/material.h
                include/matrix.h
                include/ray.h 
                include/ray_tracer.h
                include/sampler.h
                include/sampling_helpers.h
                include/scene.h
                include/shading_helpers.h
                include/sobol2D.h
                include/structs.h
                include/random_sampler.h
                include/vec.h)

add_executable(${PROJECT_NAME} ${SOURCEFILES} ${HEADERFILES})

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
add_subdirectory(extern/assimp)
add_subdirectory(extern/stbimage)

find_package(OpenMP)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} stbimage assimp OpenMP::OpenMP_CXX)

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