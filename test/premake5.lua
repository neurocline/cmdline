project "test-cmdline"
    kind "ConsoleApp"

    includedirs { "../include" }
    files { "*.h", "*.cpp" }
    links { "cmdline" }
