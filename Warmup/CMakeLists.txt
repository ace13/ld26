SET(Warmup_Headers
	MenuState.hpp
	InputManager.hpp
	MainClass.hpp
	SettingsManager.hpp
	StateManager.hpp

	WarmupState.hpp
	Components.hpp
)

SET(Warmup_Source
	Main.cpp

	MenuState.cpp
	InputManager.cpp
	MainClass.cpp
	SettingsManager.cpp
	StateManager.cpp

	WarmupState.cpp
	Components.cpp
)

SET(Warmup_Code ${Warmup_Source} ${Warmup_Headers})

FIND_PACKAGE(SFML REQUIRED COMPONENTS system window graphics audio)
FIND_PACKAGE(Boost REQUIRED COMPONENTS system thread)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/..
  	${CMAKE_CURRENT_BINARY_DIR}/../Kunlaboro
	${SFML_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
)

LINK_DIRECTORIES(
	${Boost_LIBRARY_DIRS}
)

ADD_EXECUTABLE(LD26_Warmup ${Warmup_Code})

TARGET_LINK_LIBRARIES(LD26_Warmup
	Kunlaboro
	${SFML_LIBRARIES}
	${Boost_LIBRARIES}
)