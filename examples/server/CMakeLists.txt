set(TARGET server)
add_executable(${TARGET} server.cpp httplib.h json.hpp)

install(TARGETS ${TARGET} RUNTIME)
target_link_libraries(${TARGET} PRIVATE bark common ${CMAKE_THREAD_LIBS_INIT})
