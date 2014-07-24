file(GLOB sources *.cpp)
file(GLOB headers *.hpp)

add_executable(${CMAKE_PROJECT_NAME} ${sources} ${headers})
if(WIN32)
	target_link_libraries(${CMAKE_PROJECT_NAME} ${Boost_SYSTEM_LIBRARY} wsock32 ws2_32 tank)
else()
	target_link_libraries(${CMAKE_PROJECT_NAME} ${Boost_SYSTEM_LIBRARY} tank)
endif()
