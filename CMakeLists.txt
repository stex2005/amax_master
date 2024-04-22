file(GLOB MY_PROJECT_HEADERS *.h)
file(GLOB MY_PROJECT_SOURCES *.cpp *.c)

add_executable(test ${MY_PROJECT_SOURCES} ${MY_PROJECT_HEADERS})
target_link_libraries(test ethercat_driver esmacat)
install(TARGETS test DESTINATION ./bin)
