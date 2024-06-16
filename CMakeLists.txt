cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0042 NEW)

project(maa-node)

include(
  ${CMAKE_CURRENT_LIST_DIR}/maa/share/cmake/MaaFramework/MaaFramework.cmake)

add_definitions(-DNAPI_VERSION=8)

include_directories(${CMAKE_JS_INC})

file(GLOB_RECURSE maa_src src/*.cpp)

add_library(maa SHARED ${maa_src} ${CMAKE_JS_SRC})
set_target_properties(maa PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(maa ${CMAKE_JS_LIB} MaaFramework::MaaFramework
                      MaaFramework::MaaToolkit)

if(WIN32
   AND CMAKE_JS_NODELIB_DEF
   AND CMAKE_JS_NODELIB_TARGET)
  # Generate node.lib
  execute_process(
    COMMAND ${CMAKE_AR} /def:${CMAKE_JS_NODELIB_DEF}
            /out:${CMAKE_JS_NODELIB_TARGET} ${CMAKE_STATIC_LINKER_FLAGS})
endif()

install(TARGETS maa RUNTIME DESTINATION .)

file(GLOB MaaDlls "${CMAKE_CURRENT_LIST_DIR}/maa/bin/*.dll")
install(FILES ${MaaDlls} DESTINATION .)

if(WIN32)
  add_custom_command(
    TARGET maa
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${MaaDlls} $<TARGET_FILE_DIR:maa>
    COMMAND_EXPAND_LISTS)
endif()
