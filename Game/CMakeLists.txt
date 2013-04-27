SET(Game_Headers
	Components.hpp
	MenuState.hpp
	InputManager.hpp
	Math.hpp
	MainClass.hpp
	EnemyController.hpp
	PlayerController.hpp
	SettingsManager.hpp
	StateManager.hpp
	Telemetry.hpp

	MenuState.hpp
	GameState.hpp
)

SET(Game_Source
	Main.cpp

	Components.cpp
	MenuState.cpp
	InputManager.cpp
	MainClass.cpp
	EnemyController.cpp
	PlayerController.cpp
	SettingsManager.cpp
	StateManager.cpp
	Telemetry.cpp

	MenuState.cpp
	GameState.cpp
)

SET(Game_Code ${Game_Source} ${Game_Headers})

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

ADD_EXECUTABLE(LD26_Game ${Game_Code})

TARGET_LINK_LIBRARIES(LD26_Game
	Kunlaboro
	${SFML_LIBRARIES}
)

if (NOT WIN32)
	TARGET_LINK_LIBRARIES(LD26_Game
		${Boost_LIBRARIES}
	)
endif()