file( GLOB fbs "*.fbs" )
# Agglomeration of generated include files (*_generated.h).
set( generated_files "" )
foreach( fb ${fbs} )
    get_filename_component( stem ${fb} NAME_WE )
    get_filename_component( name ${fb} NAME )
#    set( generated_include ${CMAKE_CURRENT_BINARY_DIR}/${stem}_generated.h )
    set( generated_include ${PROJECT_SOURCE_DIR}/include/capnzero-base-msgs/${stem}_generated.h )
    add_custom_command(
            OUTPUT ${generated_include}
            COMMENT "Compiling flatbuffer for ${name}"
            COMMAND flatc # This will be a target recognized by CMake.
            --cpp
            --scoped-enums          # (optional)
            --reflect-types         # (optional)
            --reflect-names         # (optional)
#            -o ${CMAKE_CURRENT_BINARY_DIR}
            -o ${PROJECT_SOURCE_DIR}/include/capnzero-base-msgs
#            -I ${CMAKE_CURRENT_SOURCE_DIR}
            -I ${CMAKE_CURRENT_SOURCE_DIR}
            ${fb}
            DEPENDS flatc ${fb}
            WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    )
    list( APPEND generated_files ${generated_include} )
endforeach()
# Create a custom target that depends on all the generated files.
# Depending on this will trigger all schemas to be compiled.
add_custom_target( flatbuffers_message DEPENDS ${generated_files} )
set_property(
        TARGET flatbuffers_message
        PROPERTY INCLUDE_DIR
        ${CMAKE_CURRENT_BINARY_DIR}
)