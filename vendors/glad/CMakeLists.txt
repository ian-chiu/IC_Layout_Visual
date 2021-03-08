project(glad LANGUAGES C)

add_library(glad "")

target_sources(glad
    PRIVATE src/glad.c
    PRIVATE include/glad/glad.h
    PRIVATE include/KHR/khrplatform.h
)

target_include_directories(glad 
    PRIVATE include
)