
function(prefix_entries OUTPUT_LIST PREFIX INPUT_LIST)
	string(REGEX REPLACE "([^;]+)" "${PREFIX}\\1" TEMP_LIST "${INPUT_LIST}")
	set(${OUTPUT_LIST} "${TEMP_LIST}" PARENT_SCOPE)
endfunction(prefix_entries)