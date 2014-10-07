file(GLOB_RECURSE cli_sources *.cpp *.h)

set(cli_includes
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/dep/libmpq
  ${CMAKE_SOURCE_DIR}/dep/zlib
  ${CMAKE_SOURCE_DIR}/dep/bzip2
  ${CMAKE_SOURCE_DIR}/dep/g3dlite/include
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Recast
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Recast/Include
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Detour
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Detour/Include
  ${CMAKE_SOURCE_DIR}/src/server/shared
  ${CMAKE_SOURCE_DIR}/src/server/shared/Utilities
  ${CMAKE_SOURCE_DIR}/src/server/shared/Threading
  ${CMAKE_SOURCE_DIR}/src/server/game/Conditions
  ${CMAKE_SOURCE_DIR}/src/server/game/Miscellaneous
  ${CMAKE_SOURCE_DIR}/src/server/collision
  ${CMAKE_SOURCE_DIR}/src/server/collision/Management
  ${CMAKE_SOURCE_DIR}/src/server/collision/Maps
  ${CMAKE_SOURCE_DIR}/src/server/collision/Models
)

if( WIN32 )
  set(clincludes
    ${cli_ncludes}
    ${CMAKE_SOURCE_DIR}/dep/libmpq/win
    )
endif()

include_directories(${cli_includes})

add_library(cli SHARED ${cli_sources})

target_link_libraries(cli
  collision
  g3dlib
  Recast
  Detour
  ${BZIP2_LIBRARIES}
  ${ZLIB_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES}
)

if(NOT CMAKE_CXX_FLAGS_DEBUG MATCHES "/clr")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /clr /EHa")
endif()

if(CMAKE_CXX_FLAGS_DEBUG MATCHES "/RTC1")
   string(REPLACE "/RTC1" " " CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
endif()

if(CMAKE_CXX_FLAGS MATCHES "/EHsc")
   string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

if( UNIX )
  install(TARGETS cli DESTINATION bin)
elseif( WIN32 )
  install(TARGETS cli DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()