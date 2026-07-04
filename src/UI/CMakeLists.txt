SET(SOURCES ui.c img_logo.c img_bg.c splash_anim.c gif_anim.c bg_login.c ui_helpers.c components/ui_comp_hook.c
    screens/ui_Splash.c screens/ui_Login.c screens/ui_Register.c screens/ui_MainMenu.c
    screens/ui_Game.c screens/ui_Snake.c screens/ui_2048.c
    screens/ui_Album.c screens/ui_Music.c screens/ui_Video.c
    screens/ui_Recognition.c screens/ui_Controls.c
    ../lib/auth.c ../lib/preload.c ../lib/camera.c ../lib/album_mgr.c ../lib/face_detect.c)
add_library(ui ${SOURCES})
target_include_directories(ui PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ui PUBLIC lvgl pthread)
