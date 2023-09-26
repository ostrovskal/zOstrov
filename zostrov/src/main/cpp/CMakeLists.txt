
cmake_minimum_required(VERSION 3.22.1)

project(zostrov VERSION 1.0.0 LANGUAGES CXX)

include_directories(D:/Dev/Include/)

add_library(zostrov STATIC
		zViewManager.cpp
		zView.cpp
		zViewGroup.cpp
		zDrawable.cpp
		zHandler.cpp
		zTexture.cpp
		zTheme.cpp
		zTouch.cpp
		zSpanText.cpp
		zCommonLayout.cpp
		zViewText.cpp
		zViewWidgets.cpp
		zViewForms.cpp
		zViewKeyboard.cpp
		zViewEdit.cpp
		zViewRibbons.cpp
        zViewGrid.cpp
		zViewSelect.cpp
        zCommonAdapters.cpp
		zActionBar.cpp
		zSoundManager.cpp
		zSensorManager.cpp
		zViewChart.cpp
		zViewVideo.cpp
)

find_package(zstandard REQUIRED CONFIG)
#add_library(curl STATIC IMPORTED)
#set_target_properties(curl PROPERTIES IMPORTED_LOCATION d:/Dev/Lib/${ANDROID_ABI}/libcurl.a)
target_include_directories(zostrov PUBLIC include)
#target_link_libraries(zostrov android EGL GLESv2 OpenSLES z log openMAXAL zstandard::zstandard)
target_link_libraries(zostrov android log zstandard::zstandard)
