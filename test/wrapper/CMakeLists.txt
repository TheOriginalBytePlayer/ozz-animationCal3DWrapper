add_executable(test_skeleton_converter
  skeleton_converter_tests.cc
  ../../wrapper/src/skeleton_converter.cpp)

target_link_libraries(test_skeleton_converter
  ozz_animation_offline
  ozz_animation
  ozz_base
  gtest)

target_include_directories(test_skeleton_converter PRIVATE
  ${CMAKE_SOURCE_DIR}/wrapper/include
  ${CMAKE_SOURCE_DIR}/wrapper)

target_copy_shared_libraries(test_skeleton_converter)
set_target_properties(test_skeleton_converter PROPERTIES FOLDER "ozz/tests/wrapper")
add_test(NAME test_skeleton_converter COMMAND test_skeleton_converter)
