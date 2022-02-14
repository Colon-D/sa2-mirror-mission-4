cmake_minimum_required(VERSION 3.8)

project("sa2-mirror-mission-4")

# compile ModLoaderCommon (for IniFile)
add_library("mod-loader-common" STATIC
    "extern/mod-loader-common/ModLoaderCommon/IniFile.cpp"
    "extern/mod-loader-common/ModLoaderCommon/TextConv.cpp"
    "extern/mod-loader-common/ModLoaderCommon/FileSystem.cpp"
    "extern/mod-loader-common/ModLoaderCommon/CodeParser.cpp"
)
target_include_directories("mod-loader-common" PRIVATE
    "extern/mod-loader-common/ModLoaderCommon"
)

# compile part of libmodutils (for Trampoline)
add_library("libmodutils" STATIC
    "extern/sa2-mod-loader/libmodutils/Trampoline.cpp"
)
target_include_directories("libmodutils" PRIVATE
    "extern/sa2-mod-loader/SA2ModLoader/include"
    "extern/sa2-mod-loader/libmodutils"
)

# compile my code
add_library("${PROJECT_NAME}" SHARED
    "src/main.cpp"
)

target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_20)

target_include_directories("${PROJECT_NAME}" PRIVATE
    "extern/mod-loader-common/ModLoaderCommon"
    "extern/sa2-mod-loader/SA2ModLoader/include"
    "extern/sa2-mod-loader/libmodutils"
    "extern/SA2FlipScreen"
)

target_link_libraries("${PROJECT_NAME}" PRIVATE
    "mod-loader-common"
    "libmodutils"
)
