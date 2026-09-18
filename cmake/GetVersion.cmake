# Compute the rolling version (YY.MM.NN) at configure time.
# Falls back to a hardcoded version when tools/version.sh is unavailable
# (e.g. when building from a release tarball without the tools/ directory).
set(SAS_CORE_FALLBACK_VERSION "26.09.00")

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../tools/version.sh")
    execute_process(
        COMMAND bash "${CMAKE_CURRENT_LIST_DIR}/../tools/version.sh"
        OUTPUT_VARIABLE _sas_core_version
        RESULT_VARIABLE _sas_core_version_result
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(NOT _sas_core_version_result EQUAL 0 OR NOT _sas_core_version MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+$")
        set(_sas_core_version "${SAS_CORE_FALLBACK_VERSION}")
    endif()
else()
    set(_sas_core_version "${SAS_CORE_FALLBACK_VERSION}")
endif()

set(SAS_CORE_VERSION "${_sas_core_version}")
message(STATUS "marinholab_sas_core version: ${SAS_CORE_VERSION}")
