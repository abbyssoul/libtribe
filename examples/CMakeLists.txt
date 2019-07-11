# Build examples
# TODO(abbyssoul): Disabled as not supported by clang (without gold linker that is)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto -fwhole-program")

# Example of message parser
set(EXAMPLE_MESSAGE_DECODER_SOURCE_FILES message_decoder.cpp)
add_executable(message_decoder ${EXAMPLE_MESSAGE_DECODER_SOURCE_FILES})
target_link_libraries(message_decoder PUBLIC ${PROJECT_NAME} ${CONAN_LIBS})


add_custom_target(examples
    DEPENDS message_decoder)
