cmake_minimum_required(VERSION 4.0.0)

project(Szakdoga)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

#if(CMAKE_EXPORT_COMPILE_COMMANDS)
#	set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
#		${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
#endif()

add_library(GLFW INTERFACE)
set_target_properties(GLFW PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(GLFW
	INTERFACE
		"${GLFW_LIB}/include"
)
target_link_directories(GLFW
	INTERFACE
		"${GLFW_LIB}/lib"
)
target_link_libraries(GLFW
	INTERFACE
		glfw
)

add_library(GLEW_FRAMEWORK INTERFACE)
set_target_properties(GLEW_FRAMEWORK PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(GLEW_FRAMEWORK
	INTERFACE
		"${GLEW_LIB}/include"
)
target_link_directories(GLEW_FRAMEWORK
	INTERFACE
		"${GLEW_OUT}/lib"
)
target_link_libraries(GLEW_FRAMEWORK
	INTERFACE
		GLEW
)

add_library(GLM INTERFACE)
target_include_directories(GLM
	INTERFACE
		"${GLM_LIB}/include"
)
target_link_directories(GLM
	INTERFACE
		"${GLM_LIB}/lib"
)
target_link_libraries(GLM
	INTERFACE
		glm
)

add_executable(Exec)
set_target_properties(Exec PROPERTIES OUTPUT_NAME "demo.elf")
file(GLOB SOURCE_FILES
	"src/*.cpp"
)
target_sources(Exec
	PRIVATE
		${SOURCE_FILES}
)
target_link_libraries(Exec
	PRIVATE
		GLM
		GLEW_FRAMEWORK
		GLFW
		OpenGL
)
target_compile_options(Exec
	PUBLIC
		-Wall
		-Wextra
)
