
set(SOURCE_FILES
    networkAddress.cpp
    ostream.cpp
    model.cpp
    broadcastModel.cpp

    protocol/decoder.cpp
    protocol/encoder.cpp
    protocol/messageParser.cpp
    protocol/messageWriter.cpp
    )


add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${CONAN_LIBS})

install(TARGETS ${PROJECT_NAME}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
