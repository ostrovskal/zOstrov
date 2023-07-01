
cmake_minimum_required(VERSION 3.22.1)

include_directories(D:/Dev/Include/) 

project("zssh")

add_library(zssh SHARED
		android_native.cpp
		sshApp.cpp
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
add_library(curl STATIC IMPORTED)
set_target_properties(curl PROPERTIES IMPORTED_LOCATION d:/Dev/Lib/${ANDROID_ABI}/libcurl.a)
target_include_directories(zssh PUBLIC include)
target_link_libraries(zssh android EGL GLESv2 curl OpenSLES z log openMAXAL zstandard::zstandard)
