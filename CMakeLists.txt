cmake_minimum_required(VERSION 3.14)

project(route_opt_project C)

set(CMAKE_C_STANDARD 11)

# Paths
set(SDL3_DIR "C:/coding/cpp_shit/SDL3")
set(SDL3_BIN "C:/coding/cpp_shit/SDL3/bin")
set(SDL3_IMAGE_DIR "C:/coding/cpp_shit/SDL3_libs")
set(SDL3_TTF_DIR "C:/coding/cpp_shit/SDL3_libs")
set(SDL3_LIBS_BIN "C:/coding/cpp_shit/SDL3_libs/bin")

# Set where to find config files
set(CMAKE_PREFIX_PATH
    "${SDL3_DIR}"
    "${SDL3_IMAGE_DIR}"
    "${SDL3_TTF_DIR}"
)

# Find packages
find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)

# Includes
include_directories(
    "${SDL3_DIR}/include"
    "${SDL3_IMAGE_DIR}/include"
    "${SDL3_TTF_DIR}/include"
    "${CMAKE_SOURCE_DIR}/include"
)

# Your source
set(SOURCES 
  src/main.c
  src/graph.c
  src/pathfinding.c
  src/render.c
  src/ui.c
  src/import.c
  src/ui_panel.c
)
add_executable(route_opt_project ${SOURCES})

# Link libraries
target_link_libraries(route_opt_project PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
)

# Add lib dirs if needed (for static libs)
target_link_directories(route_opt_project PRIVATE
    "${SDL3_DIR}/lib"
    "${SDL3_IMAGE_DIR}/lib"
    "${SDL3_TTF_DIR}/lib"
)

# Copy DLLs after build
if(WIN32)
    add_custom_command(TARGET route_opt_project POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SDL3_BIN}/SDL3.dll"
            "${SDL3_LIBS_BIN}/SDL3_image.dll"
            "${SDL3_LIBS_BIN}/SDL3_ttf.dll"
            $<TARGET_FILE_DIR:route_opt_project>
    )

    add_custom_command(TARGET route_opt_project POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:route_opt_project>/assets"
    )
endif()

