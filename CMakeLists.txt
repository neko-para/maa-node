cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0042 NEW)

project(maa-node)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

if(WIN32)
  add_compile_options("/utf-8")
endif()

if(APPLE)
  set(CMAKE_INSTALL_RPATH "@loader_path;@executable_path")
elseif(UNIX)
  set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif()

set(CMAKE_JS_VERSION "7.3.0")
set(CMAKE_JS_INC
    "${CMAKE_CURRENT_LIST_DIR}/node_modules/node-api-headers/include;node_modules/node-addon-api"
)

if(WIN32)
  set(CMAKE_JS_SRC
      "${CMAKE_CURRENT_LIST_DIR}/node_modules/cmake-js/lib/cpp/win_delay_load_hook.cc"
  )
endif()

set(NODE_RUNTIME "node")
set(NODE_RUNTIMEVERSION "20.11.1")
set(NODE_ARCH "x64")

if(WIN32)
  set(CMAKE_JS_LIB "${CMAKE_BINARY_DIR}/node.lib")
  set(CMAKE_JS_NODELIB_DEF
      "${CMAKE_CURRENT_LIST_DIR}/node_modules/node-api-headers/def/node_api.def"
  )
  set(CMAKE_JS_NODELIB_TARGET ${CMAKE_JS_LIB})
endif()

if(WIN32)
  set(CMAKE_SHARED_LINKER_FLAGS "/delayload:node.exe")
  link_libraries(delayimp)
elseif(APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "-undefined dynamic_lookup")
endif()

include(
  ${CMAKE_CURRENT_LIST_DIR}/maa/share/cmake/MaaFramework/MaaFramework.cmake)

add_definitions(-DNAPI_VERSION=8)

include_directories(${CMAKE_JS_INC})

set(maa_src
    src/main.cpp src/utility/utility.cpp src/instance/context.cpp
    src/instance/controller.cpp src/instance/resource.cpp
    src/instance/tasker.cpp)

add_library(maa SHARED ${maa_src} ${CMAKE_JS_SRC})
set_target_properties(maa PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(maa ${CMAKE_JS_LIB} MaaFramework::MaaFramework
                      MaaFramework::MaaToolkit)
target_compile_definitions(maa PRIVATE NODE_ADDON_API_ENABLE_TYPE_CHECK_ON_AS)

if(WIN32
   AND CMAKE_JS_NODELIB_DEF
   AND CMAKE_JS_NODELIB_TARGET)
  # Generate node.lib
  if(MSVC)
    execute_process(
      COMMAND ${CMAKE_AR} /def:${CMAKE_JS_NODELIB_DEF}
              /out:${CMAKE_JS_NODELIB_TARGET} ${CMAKE_STATIC_LINKER_FLAGS})
  else()
    execute_process(
      COMMAND ${CMAKE_LINKER} /def:${CMAKE_JS_NODELIB_DEF}
              /out:${CMAKE_JS_NODELIB_TARGET} ${CMAKE_STATIC_LINKER_FLAGS})
  endif()
endif()

install(
  TARGETS maa
  RUNTIME DESTINATION .
  LIBRARY DESTINATION .
  ARCHIVE DESTINATION .)

file(GLOB MaaDlls "${CMAKE_CURRENT_LIST_DIR}/maa/bin/*.dll")
install(FILES ${MaaDlls} DESTINATION .)

if(WIN32)
  add_custom_command(
    TARGET maa
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${MaaDlls} $<TARGET_FILE_DIR:maa>
    COMMAND_EXPAND_LISTS)
endif()
