
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
		zViewPopup.cpp
        zCommonAdapters.cpp
)

find_package(zstandard REQUIRED CONFIG)
target_include_directories(zssh PUBLIC include)
target_link_libraries(zssh android EGL GLESv2 OpenSLES z log zstandard::zstandard)
