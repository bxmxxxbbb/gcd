CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
PROJECT(XMFunctionTemplateGenerate)

SET(CMAKE_BUILD_TYPE "Release")

INCLUDE_DIRECTORIES(include ${XMFilePath}/include ${XMStringPath}/include)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall")

AUX_SOURCE_DIRECTORY(src SRC_LIST)
AUX_SOURCE_DIRECTORY(${XMFilePath}/src SRC1_LIST)
AUX_SOURCE_DIRECTORY(${XMStringPath}/src SRC2_LIST)

ADD_EXECUTABLE(CodeGenerate ${SRC_LIST} ${SRC1_LIST} ${SRC2_LIST})

target_link_libraries(CodeGenerate cjson)