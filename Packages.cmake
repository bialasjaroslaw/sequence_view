include(FetchContent)

macro(add_dependency)
    set(oneValueArgs NAME REPO HASH URL)
    cmake_parse_arguments(DEPSADD "${optionalValueArgs}" "${oneValueArgs}" "" ${ARGN})
    if(DEPSADD_REPO AND DEPSADD_HASH)
	    FetchContent_Declare(
			${DEPSADD_NAME}
			GIT_REPOSITORY ${DEPSADD_REPO}
			GIT_TAG        ${DEPSADD_HASH}
		)
	elseif(DEPSADD_URL)
		FetchContent_Declare(
			${DEPSADD_NAME}
			URL ${DEPSADD_URL}
		)
	else()
		message(FATAL_ERROR "At least one have to be specified REPO/HASH OR URL")
	endif()
	message(STATUS "Defined ${DEPSADD_NAME}")
	set(DEPS_${DEPSADD_NAME}_DEFINED TRUE)
endmacro()

macro(use_dependency DEP_NAME)
    if(NOT DEPS_${DEP_NAME}_USED)
    	set(DEPS_${DEP_NAME}_USED)
    	if(EXISTS "${VCPKG_INSTALLED_DIR}")
		    find_package(${DEP_NAME} CONFIG QUIET)
		endif()
		if(NOT (EXISTS "${VCPKG_INSTALLED_DIR}" AND ${DEP_NAME}_FOUND))
			if(NOT DEPS_${DEP_NAME}_DEFINED)
		    	message(FATAL_ERROR "${DEP_NAME} not defined for FetchContent")
		    endif()
		    FetchContent_MakeAvailable(${DEP_NAME})
		endif()
    endif()
endmacro()