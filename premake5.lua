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

	includedirs{ ".", "llama.cpp" }

	files {
		"webserver/*.hpp", "webserver/*.cpp", "webserver/public/inc/*.h",
		"inference/*.hpp", "inference/*.cpp",
		"common/*.hpp", "common/*.cpp",
		"lib/*.hpp", "lib/*.cpp", "lib/*.h", "lib/*.c"
	}

	defines { "verbose" }

	prebuildcommands { "( /usr/bin/env bash -c \"./deps.sh\" )" }

	filter {"system:linux"}
		cppdialect  "C++20"
		architecture "x86_64"
		filter {"system:linux", "configurations:Debug"}
			toolset ("gcc")
			prebuildcommands {
				-- "cd llama.cpp && ( mkdir build_debug || true ) && cd build_debug && echo llamabuild && echo pwd $$(pwd) && cmake .. -DCMAKE_BUILD_TYPE=Debug -DLLAMA_STATIC=ON -DLLAMA_CUBLAS=ON -DLLAMA_NATIVE=ON -DLLAMA_BUILD_EXAMPLES=OFF -DLLAMA_BUILD_TESTS=OFF -DLLAMA_BUILD_SERVER=OFF && cmake --build . --config Debug -j"
				"cd llama.cpp && ( mkdir build_release || true ) && cd build_release && echo llamabuild && echo pwd $$(pwd) && cmake .. -DCMAKE_BUILD_TYPE=Release -DLLAMA_STATIC=ON -DLLAMA_CUBLAS=ON -DLLAMA_NATIVE=ON -DLLAMA_BUILD_EXAMPLES=OFF -DLLAMA_BUILD_TESTS=OFF -DLLAMA_BUILD_SERVER=OFF && cmake --build . --config Release -j"
			}
			defines { "GGML_USE_CUBLAS" }
			symbols "On"
			--buildoptions { "-O0 -g -fsanitize=address -fno-omit-frame-pointer -W -Wall -Wextra -pedantic" }
			buildoptions { "-O0 -g -fno-omit-frame-pointer -W -Wall -Wextra -pedantic" }
			--linkoptions  { "-fsanitize=address", "-Wl,-rpath=./llama.cpp/build_debug", "-Wl,-rpath-link=./llama.cpp/build_debug" }
			--linkoptions  { "-fsanitize=address" }
			linkoptions  {  }
			--links { "llama", "ggml_shared", "llama.cpp/build_debug/common/CMakeFiles/build_info.dir/build-info.cpp.o", "llama.cpp/build_debug/common/CMakeFiles/common.dir/common.cpp.o", "cublas", "cuda", "culibos", "cudart", "cublasLt", "pthread", "dl", "rt", "asan" }
			--links { "llama:static", "ggml_static:static", "common:static", "cublas", "cuda", "culibos", "cudart", "cublasLt", "pthread", "dl", "rt", "asan" }
			links { "llama:static", "ggml_static:static", "common:static", "cublas", "cuda", "culibos", "cudart", "cublasLt", "pthread", "dl", "rt" }
			--libdirs { "llama.cpp/build_debug", "llama.cpp/build_debug/common", os.findlib("cublas"), os.findlib("cuda"), os.findlib("asan") }
			libdirs { "llama.cpp/build_release", "llama.cpp/build_release/common", os.findlib("cublas"), os.findlib("cuda") }
		filter {"system:linux", "configurations:Release"}
			prebuildcommands {
				"( cd llama.cpp && echo pwd $$(pwd) && make LLAMA_CUBLAS=1 libllama.so -j )"
			}
			symbols "Off"
			buildoptions { "-O3 -march=native -fomit-frame-pointer -W -Wall -Wextra -pedantic" }
	filter {"system:windows"}
		cppdialect  "C++20"
		filter {"system:linux", "configurations:Debug"}
			symbols "On"
		filter {"system:linux", "configurations:Release"}
			symbols "Off"
