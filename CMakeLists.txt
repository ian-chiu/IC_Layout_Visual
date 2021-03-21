cmake_minimum_required(VERSION 3.15)
project(learn_opengl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(vendors/glfw)
add_subdirectory(vendors/glad)
add_subdirectory(vendors/spdlog)
add_subdirectory(vendors/glm)
add_subdirectory(vendors/imgui)
add_subdirectory(vendors/sqlite3)

file(GLOB_RECURSE learn_opengl_SRC src/*.cpp src/*.h)

add_executable(learn_opengl ${learn_opengl_SRC})

add_custom_command(TARGET learn_opengl POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:learn_opengl>/assets
)

target_link_libraries(learn_opengl
    PRIVATE 
        openGL32
        glad
        glfw
        spdlog
        glm
        imgui
        sqlite3
)

target_include_directories(learn_opengl
    PRIVATE
        src/Engine
        src/Application
        vendors/glfw/include 
        vendors/glad/include
        vendors/spdlog/include
        vendors/glm
        vendors/stb_image
        vendors/imgui
        vendors/sqlite3
        vendors/sqlite_modern_cpp/hdr
)

target_compile_definitions(learn_opengl
    PRIVATE 
        STB_IMAGE_IMPLEMENTATION
        GLFW_INCLUDE_NONE
)

# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()