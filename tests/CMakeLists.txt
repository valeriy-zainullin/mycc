set(TESTS "")
function(add_test test_name)
	add_executable("test_${test_name}.hostcc" "${CMAKE_CURRENT_LIST_DIR}/${test_name}.c")
	add_sanitizers("test_${test_name}.hostcc")

	add_custom_target("test_${test_name}.mycc"
		COMMAND
			echo "Test ${test_name}: compiling using mycc..."
		COMMAND
			"$<TARGET_FILE:mycc>"
			"-o"
			"${CMAKE_CURRENT_BINARY_DIR}/${test_name}.mycc"
			"${CMAKE_CURRENT_LIST_DIR}/${test_name}.c"
		SOURCES "${CMAKE_CURRENT_LIST_DIR}/${test_name}.c"
	)

	# All tests before this one must be executed to proceed.
	foreach(earlier_test ${TESTS})
		add_dependencies("test_${test_name}.hostcc" "${earlier_test}")
		add_dependencies("test_${test_name}.mycc"   "${earlier_test}")
	endforeach()

	add_custom_target("test_${test_name}_run"
		COMMAND
			echo "Test ${test_name}: running..."
		COMMAND "$<TARGET_FILE:test_${test_name}.hostcc>       > ${CMAKE_CURRENT_BINARY_DIR}/${test_name}.hostcc-output.txt"
		COMMAND "${CMAKE_CURRENT_BINARY_DIR}/${test_name}.mycc > ${CMAKE_CURRENT_BINARY_DIR}/${test_name}.mycc-output.txt'"
		DEPENDS "test_${test_name}.mycc"
		USES_TERMINAL
	)
	add_custom_target("test_${test_name}_check"
		COMMAND
			echo "Test ${test_name}: comparing output..."
		COMMAND cmINFOp "${CMAKE_CURRENT_BINARY_DIR}/${test_name}.hostcc-output.txt" "${CMAKE_CURRENT_BINARY_DIR}/${test_name}.mycc-output.txt"
		DEPENDS "test_${test_name}_run"
	)

	add_custom_target("test_${test_name}" DEPENDS "test_${test_name}_check")

	set(TESTS "${TESTS};test_${test_name}" PARENT_SCOPE)
endfunction()

add_test(01-typedef)
add_test(02-global_variable)
add_test(03-hello_world)

add_custom_target(tests ALL DEPENDS "${TESTS}")
