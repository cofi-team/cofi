project(cofi)

cmake_minimum_required(VERSION 2.8)

# Add FindTBB directory to CMake's module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

find_package(Protobuf REQUIRED)
find_package(PQXX REQUIRED)

set(srcFolder src)
set(cofiAppSrc ${srcFolder}/CommandRequest.cpp ${srcFolder}/DBConnection.cpp ${srcFolder}/SqlStatement.cpp ${srcFolder}/Server.cpp ${srcFolder}/protobuf/command_java.pb.cc)

add_executable(cofiapp ${cofiAppSrc})

# compile protoFiles
set(protoFiles
  command_java
)
foreach(protofile ${protoFiles})
  add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${srcFolder}/protobuf/${protofile}.pb.h ${CMAKE_CURRENT_SOURCE_DIR}/${srcFolder}/protobuf/${protofile}.pb.cc
    COMMAND protoc -I${CMAKE_CURRENT_SOURCE_DIR}/header/protobuf --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/${srcFolder}/protobuf ${CMAKE_CURRENT_SOURCE_DIR}/header/protobuf/${protofile}.proto
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${srcFolder}/protobuf ${CMAKE_CURRENT_SOURCE_DIR}/header/protobuf/${protofile}.proto
  )
endforeach(protofile)

# ParallelServer
set(ext_libraries
  ${PROTOBUF_LIBRARIES}
  ${PQXX_LIBRARIES}
)

target_link_libraries(cofiapp ${ext_libraries})

install(TARGETS cofiapp DESTINATION bin)
