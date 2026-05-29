cmake_minimum_required(VERSION 3.16)
project(Fairy LANGUAGES CXX)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем SFML
add_subdirectory(libs/SFML)


# Создаём игру
add_executable(Fairy
    main.cpp
    player.cpp
    map.cpp
 "pushable_box.h" "log.h")

# Подключаем SFML
target_link_libraries(Fairy PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# Копируем картинки
add_custom_command(TARGET Fairy POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/images
        $<TARGET_FILE_DIR:Fairy>/images
)

# Для Windows копируем DLL
if(WIN32)
    add_custom_command(TARGET Fairy POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-graphics>
            $<TARGET_FILE:sfml-window>
            $<TARGET_FILE:sfml-system>
            $<TARGET_FILE:sfml-audio>
            $<TARGET_FILE_DIR:Fairy>
    )
endif()