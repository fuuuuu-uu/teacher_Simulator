cmake_minimum_required(VERSION 3.10)
project(TeacherSimulator)

# 設定 C++ 標準
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ?? 告訴 CMake 標頭檔 (.h) 在哪裡
include_directories(include)

# ?? 告訴 CMake 要編譯的原始碼檔案 (.cpp) 有哪些
add_executable(TeacherSimulator 
    src/main.cpp 
    src/GameObjects.cpp
)
# 當編譯完成後，自動將 event 資料夾複製到建置目錄（執行檔旁邊）
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/event" 
     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
