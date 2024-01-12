include(FetchContent)

macro(add_dependency)
    set(oneValueArgs NAME PACKAGE REPO HASH URL)
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
	if(NOT DEPSADD_PACKAGE)
		set(DEPSADD_PACKAGE ${DEPSADD_NAME})
	endif()
	message(STATUS "Defined ${DEPSADD_NAME} with PACKAGE_NAME ${DEPSADD_PACKAGE}")
	set(DEPS_${DEPSADD_PACKAGE}_DEFINED TRUE)
	set(DEPS_${DEPSADD_PACKAGE}_NAME ${DEPSADD_NAME})
endmacro()

macro(use_dependency DEP_NAME)
    if(NOT DEPS_${DEP_NAME}_USED)
		set(DEPS_${DEP_NAME}_USED)
		if(EXISTS "${VCPKG_INSTALLED_DIR}")
			message(STATUS "Looking for ${DEP_NAME}")
			find_package(${DEP_NAME} CONFIG QUIET)
		endif()
		if(NOT (EXISTS "${VCPKG_INSTALLED_DIR}" AND ${DEP_NAME}_FOUND))
			message(STATUS "Trying to fetch ${DEPS_${DEP_NAME}_NAME}")
			if(NOT DEPS_${DEP_NAME}_DEFINED)
				message(FATAL_ERROR "${DEPS_${DEP_NAME}_NAME} not defined for FetchContent")
			endif()
			FetchContent_MakeAvailable(${DEPS_${DEP_NAME}_NAME})
		endif()
	endif()
endmacro()