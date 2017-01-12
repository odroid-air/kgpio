remove_definitions(-DQT_NO_CAST_FROM_ASCII)

include(ECMMarkAsTest)

add_executable(synctest synctest.cpp)
target_link_libraries(synctest Qt5::Core)
ecm_mark_as_test(synctest)


