function(check_coverage out_var)
    set(oneValueArgs TARGET)
    find_program(LCOV lcov)
    find_program(GENHTML genhtml)
    set(TOOLS_FOUND FALSE)
    if(LCOV AND GENHTML)
        set(TOOLS_FOUND TRUE)
    endif()
    set(${out_var} ${TOOLS_FOUND} PARENT_SCOPE)
endfunction()

macro(add_coverage)
    set(oneValueArgs TARGET)
    set(multiValueArgs IGNORE_LIST)

    cmake_parse_arguments(COVERAGE "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
    
    set(IGNORE_LIST "")
    foreach(arg IN LISTS COVERAGE_IGNORE_LIST)
      # Escape the argument and append it to the string
      string(REPLACE "*" "\\*" escaped_arg "${arg}")
      list(APPEND IGNORE_LIST "${escaped_arg}")
    endforeach()

    find_program(LCOV lcov REQUIRED)
    find_program(GENHTML genhtml REQUIRED)

    add_custom_target(${COVERAGE_TARGET}_coverage
      # gather data
      COMMAND ${LCOV} --directory . --capture --output-file ${COVERAGE_TARGET}_capture_coverage.info
      COMMAND ${LCOV} --extract ${COVERAGE_TARGET}_capture_coverage.info ${CMAKE_CURRENT_LIST_DIR}/\\* --output-file ${COVERAGE_TARGET}_extract_coverage.info
      COMMAND ${LCOV} --remove ${COVERAGE_TARGET}_extract_coverage.info ${IGNORE_LIST} ${CMAKE_CURRENT_BINARY_DIR}/\\* --output-file ${COVERAGE_TARGET}_remove_coverage.info
      # generate report
      COMMAND ${GENHTML} --demangle-cpp -o ${COVERAGE_TARGET}_coverage ${COVERAGE_TARGET}_remove_coverage.info
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    add_dependencies(${COVERAGE_TARGET}_coverage ${COVERAGE_TARGET})
endmacro()