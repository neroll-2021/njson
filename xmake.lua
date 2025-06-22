add_rules("mode.debug", "mode.release")

set_languages("c++20")
set_warnings("all", "error")

target("njson")
    set_kind("binary")
    add_includedirs("include")
    add_files("src/*.cpp")
    set_rundir("$(projectdir)")