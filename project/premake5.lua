-- command-line
--   [Windows] premake5.exe --file=premake5.lua vs2015
--   [Mac] premake5 --file=premake5.lua xcode4

solution "cmdline"
	configurations { "Debug", "Release" }
	platforms { "x86", "x64" }

	-- disambiguate for VS version
	if string.startswith(_ACTION, "vs") then
		filename "cmdline.%{string.sub(_ACTION, 3)}"
	end

project "cmdline"
	kind "ConsoleApp"
	warnings "Extra"

	-- disambiguate for VS version
	if string.startswith(_ACTION, "vs") then
		filename "cmdline.%{string.sub(_ACTION, 3)}"
	end

	-- Set architecture from platform tag
	filter { 'platforms:x86' }
		architecture "x32"
	filter { 'platforms:x64' }
		architecture "x64"
    filter {}

	files { "../source/**" }

	-- Debug-specific settings
	filter "configurations:Debug"
		defines { "_DEBUG" }
		flags { "Symbols" }

	-- Release-specific settings
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
    filter {}

	-- Add missing settings on Mac (Premake defect)
	filter { "action:xcode*" }
		xcodebuildsettings
		{
			["COMBINE_HIDPI_IMAGES"] = "YES";
			["CLANG_CXX_LANGUAGE_STANDARD"] = "c++14";
			["CLANG_CXX_LIBRARY"] = "libc++";
		}
	filter {}

	-- Put intermediate objects in build/CONFIG-ARCH/obj
	objdir("%{wks.location}/../build/%{cfg.buildcfg}-%{cfg.architecture}/obj")

	-- Put built binaries in build/CONFIG_ARCH/bin
	targetdir("%{wks.location}/../build/%{cfg.buildcfg}-%{cfg.architecture}/bin")
