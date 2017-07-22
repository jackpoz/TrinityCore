file(GLOB_RECURSE cli_sources *.cpp *.h)

set(cli_includes
  ${CMAKE_SOURCE_DIR}/src/cli
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/dep/libmpq
  ${CMAKE_SOURCE_DIR}/dep/zlib
  ${CMAKE_SOURCE_DIR}/dep/bzip2
  ${CMAKE_SOURCE_DIR}/dep/g3dlite/include
  ${CMAKE_SOURCE_DIR}/dep/cppformat
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Recast
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Recast/Include
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Detour
  ${CMAKE_SOURCE_DIR}/dep/recastnavigation/Detour/Include
  ${CMAKE_SOURCE_DIR}/src/common
  ${CMAKE_SOURCE_DIR}/src/common/Collision
  ${CMAKE_SOURCE_DIR}/src/common/Collision/Maps
  ${CMAKE_SOURCE_DIR}/src/common/Collision/Management
  ${CMAKE_SOURCE_DIR}/src/common/Collision/Models
  ${CMAKE_SOURCE_DIR}/src/common/Debugging
  ${CMAKE_SOURCE_DIR}/src/common/Logging
  ${CMAKE_SOURCE_DIR}/src/common/Utilities
  ${CMAKE_SOURCE_DIR}/src/server/database/Database
  ${CMAKE_SOURCE_DIR}/src/server/shared
  ${CMAKE_SOURCE_DIR}/src/server/shared/DataStores
  ${CMAKE_SOURCE_DIR}/src/server/shared/Packets
  ${CMAKE_SOURCE_DIR}/src/server/game/Conditions
  ${CMAKE_SOURCE_DIR}/src/server/game/DataStores
  ${CMAKE_SOURCE_DIR}/src/server/game/Entities/Object
  ${CMAKE_SOURCE_DIR}/src/server/game/Maps
  ${CMAKE_SOURCE_DIR}/src/server/game/Miscellaneous
  ${CMAKE_SOURCE_DIR}/src/server/game/Spells
)

if( WIN32 )
  set(clincludes
    ${cli_includes}
    ${CMAKE_SOURCE_DIR}/dep/libmpq/win
    )
endif()

include_directories(${cli_includes})

add_library(cli SHARED ${cli_sources} 
  ${CMAKE_SOURCE_DIR}/src/server/game/DataStores/DBCStores.cpp
  ${CMAKE_SOURCE_DIR}/src/common/DataStores/DBCFileLoader.cpp)

target_link_libraries(cli
  common
  format
  g3dlib
  Recast
  Detour
  ${BZIP2_LIBRARIES}
  ${ZLIB_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_REGEX_LIBRARY}
  ${Boost_IOSTREAMS_LIBRARY}
)

add_definitions( -D_UNICODE -DUNICODE -DLOCKEDQUEUE_H -D_DATABASEWORKERPOOL_H -D_WORLDDATABASE_H -DDATABASEENV_H -D_MUTEX_ -DTRANSPORTMGR_H)
if(NOT CMAKE_CXX_FLAGS MATCHES "/clr")
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

set_target_properties(cli PROPERTIES VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.6.1")