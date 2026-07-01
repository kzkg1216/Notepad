cmake_minimum_required(VERSION 3.10)
project(NotepadMVP CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(notepad WIN32 src/main.cpp)
target_link_libraries(notepad PRIVATE comdlg32 gdi32 user32 kernel32)

if(MINGW)
  target_link_options(notepad PRIVATE -static -static-libgcc -static-libstdc++)
endif()
