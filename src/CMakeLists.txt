add_library(wecan
        SHARED
        libwecan.c)

install (TARGETS wecan
        DESTINATION lib)

file(
        GLOB
        headers
        *.h
        )

install(FILES ${headers}
        DESTINATION include/${CMAKE_PROJECT_NAME})
