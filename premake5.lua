-- premake5.lua
workspace "me6"
	configurations { "Release", "Debug" }

project "server"
	kind "ConsoleApp"
	language "C++"
	targetdir "./"
	targetname "me6"
	objdir ".build-%{prj.name}-%{cfg.buildcfg}"

	flags { "MultiProcessorCompile", "Verbose", "ShowCommandLine", "Color" }

	includedirs{ "server" }

	files { "server/*.hpp", "server/*.cpp", "server/public/inc/*.h" }

	defines { "verbose" }

	filter {"system:linux"}
		cppdialect  "C++20"
		filter {"system:linux", "configurations:Debug"}
			symbols "On"
			buildoptions { "-O0 -g -fsanitize=address -fno-omit-frame-pointer -W -Wall -Wextra -pedantic" }
			linkoptions  { "-fsanitize=address" }
			links { "asan" }
			libdirs { os.findlib("asan") }
		filter {"system:linux", "configurations:Release"}
			symbols "Off"
			buildoptions { "-O3 -march=native -fomit-frame-pointer -W -Wall -Wextra -pedantic" }
	filter {"system:windows"}
		cppdialect  "C++20"
		filter {"system:linux", "configurations:Debug"}
			symbols "On"
		filter {"system:linux", "configurations:Release"}
			symbols "Off"
