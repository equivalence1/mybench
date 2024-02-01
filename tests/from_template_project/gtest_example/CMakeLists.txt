add_executable(gtest_example test.cpp)

target_link_libraries(gtest_example GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(gtest_example)
