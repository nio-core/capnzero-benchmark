set(SBE_HEADER
        "${CMAKE_CURRENT_BINARY_DIR}/Message.h"
        "${CMAKE_CURRENT_BINARY_DIR}/MessageHeader.h"
        "${CMAKE_CURRENT_BINARY_DIR}/VarStringEncoding.h"
        )

add_custom_command(OUTPUT ${SBE_HEADER}
        COMMAND java -Dsbe.target.language=cpp -Dsbe.output.dir=${CMAKE_CURRENT_BINARY_DIR} -jar sbe-all-1.16.1.jar ${PROJECT_SOURCE_DIR}/msg/sbe_schema.xml
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        VERBATIM
        )

add_custom_target(generate_sbe_header DEPENDS ${SBE_HEADER})