add_library (psx sim_connection.cpp)
target_compile_features(psx PRIVATE cxx_nonstatic_member_init cxx_delegating_constructors) 
target_link_libraries(psx PUBLIC ${SDL2_LIBRARIES})
target_include_directories(psx PUBLIC ${SDL2_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}/../include)
