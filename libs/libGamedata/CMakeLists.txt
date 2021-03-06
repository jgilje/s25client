SET(SOURCES_SUBDIRS )
MACRO(AddDirectory dir)
	FILE(GLOB SUB_FILES ${dir}/*.cpp ${dir}/*.h ${dir}/*.hpp ${dir}/*.tpp)
	SET(SOURCES_SUBDIRS ${SOURCES_SUBDIRS} ${SUB_FILES})
	SOURCE_GROUP(${dir} FILES ${SUB_FILES})
ENDMACRO()

AddDirectory(gameData)
AddDirectory(lua)

FILE(GLOB SOURCES_OTHER *.cpp *.h)
SOURCE_GROUP(other FILES ${SOURCES_OTHER})

find_package(Lua 5.1 REQUIRED)

include(GatherDll)
gather_dll(Lua)

ADD_LIBRARY(gamedata STATIC ${SOURCES_OTHER} ${SOURCES_SUBDIRS})
target_include_directories(gamedata PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(gamedata SYSTEM PUBLIC ${LUA_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(gamedata PUBLIC
	s25util::common
	mygettext
	s25Common
	kaguya
	${LUA_LIBRARIES}
	PRIVATE rttrConfig Boost::nowide
)
include(EnableWarnings)
enable_warnings(gamedata)
