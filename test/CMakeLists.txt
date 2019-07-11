set(TEST_SOURCE_FILES
        ci/teamcity_messages.cpp
        ci/teamcity_gtest.cpp

        main_gtest.cpp

        test_model.cpp
        test_protocol.cpp
    )

enable_testing()

add_executable(test_${PROJECT_NAME} EXCLUDE_FROM_ALL ${TEST_SOURCE_FILES})

if(TRIBE_GTEST_SUPPORT)
    message(STATUS, "Using local version of GTest")

    add_subdirectory(../${TRIBE_EXTERNAL_DEP_GTEST_DIR} ${CMAKE_BINARY_DIR}/gtest EXCLUDE_FROM_ALL)
else()
    message(STATUS, "Using provided version of GTest")
endif()

target_link_libraries(test_${PROJECT_NAME}
    ${PROJECT_NAME}
    gtest
    $<$<NOT:$<PLATFORM_ID:Darwin>>:rt>
    )

add_test(NAME test_${PROJECT_NAME}
    COMMAND test_${PROJECT_NAME}
    )
