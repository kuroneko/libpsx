add_library (psx src/sim_connection.cpp src/wire_pair.cpp src/sim_connection_sdl)
target_compile_features(psx PRIVATE cxx_nonstatic_member_init cxx_delegating_constructors) 
target_link_libraries(psx PUBLIC ${SDL2_LIBRARIES})
target_include_directories(psx PUBLIC ${SDL2_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)
