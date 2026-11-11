cmake_minimum_required (VERSION 3.10)

if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT 
        "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>"
    )
endif()

project ( CodeCamp )

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

add_executable(CodeCamp main.cpp "shader.h" stb_image.h "camera.h"  "camera.cpp" mesh.h "model.h" "model_demo.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET CodeCamp PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(CodeCamp PRIVATE 
    glfw
    glad::glad
    OpenGL::GL
    glm::glm
    assimp::assimp
)

add_custom_command(TARGET CodeCamp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/shader.vs"
        $<TARGET_FILE_DIR:CodeCamp>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/shader.fs"
        $<TARGET_FILE_DIR:CodeCamp>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/model_loading.vs"
        $<TARGET_FILE_DIR:CodeCamp>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/model_loading.fs"
        $<TARGET_FILE_DIR:CodeCamp>
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:CodeCamp>/resources"
    COMMENT "Copying shaders and resources to build directory"
)
