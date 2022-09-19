
PROJECT_DIR = path.getabsolute("..")
local BUILD_DIR = path.join(PROJECT_DIR, ".build")
local DART_DIR = path.join(PROJECT_DIR, "dart-sdk", "sdk")

solution "dart_dll"
    configurations {
        "Debug",
        "Release",
    }
    platforms { "x64" }

    configuration { "Debug" }
		defines {
			"_DEBUG",
		}

	configuration { "Release" }
		flags {
			"NoBufferSecurityCheck",
			"OptimizeSpeed",
		}
		defines {
			"NDEBUG",
		}

    language "C++"

    flags {
        "Cpp14",
		"ExtraWarnings",
		"NoPCH",
		"NativeWChar",
		"NoRTTI",
		"NoExceptions",
		"NoEditAndContinue",
		"NoFramePointer",
		"Symbols",
	}

	defines {
		"__STDC_LIMIT_MACROS",
		"__STDC_FORMAT_MACROS",
		"__STDC_CONSTANT_MACROS",
	}

    location (path.join(BUILD_DIR, "projects", _ACTION))

    configuration { "vs*" }
        defines { 
            "_HAS_EXCEPTIONS=0",
			"_SCL_SECURE=0",
			"_SECURE_SCL=0",
			"_SCL_SECURE_NO_WARNINGS",
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_SECURE_NO_DEPRECATE"
        }
        targetdir (path.join(BUILD_DIR, "win64_" .. _ACTION, "bin"))
        objdir (path.join(BUILD_DIR, "win64_" .. _ACTION, "obj"))

    configuration {}

    project ("dart_dll")
        uuid (os.uuid("dart_dll"))
        kind "SharedLib"

        configuration { "windows" }
            defines {
                "DART_DLL_EXPORTING"
            }

            -- Both Debug and Release need to use the Release Static CRT
            flags {
                "StaticRuntime",
                "OptimizeSpeed"
            }
        
        configuration {}
        
        files {
            path.join(PROJECT_DIR, "src", "**.cpp"),
            path.join(PROJECT_DIR, "src", "**.h")
        }

        includedirs {
            path.join(DART_DIR, "runtime")
        }

        links {
            "libdart",
            "dbghelp",
            "bcrypt",
            "rpcrt4",
            "ws2_32",
            "Iphlpapi",
            "Psapi",
            "shlwapi"
        }

        libdirs {
            path.join(DART_DIR, "out", "ReleaseX64", "obj", "runtime", "bin")
        }
        
    project ("simple_example")
        uuid (os.uuid("simple_example"))
        kind "ConsoleApp"

        debugdir (path.join(PROJECT_DIR, "examples", "simple_example"))
    
        files {
            path.join(PROJECT_DIR, "examples", "simple_example", "**.cpp"),
            path.join(PROJECT_DIR, "examples", "simple_example", "**.h")
        }

        links {
            "dart_dll"
        }

        includedirs {
            path.join(PROJECT_DIR, "src"),
            path.join(DART_DIR, "runtime", "include")
        }

        configuration "windows"
            postbuildcommands {  "xcopy /Y \"" .. (path.translate(path.join(PROJECT_DIR, "examples", "simple_example", "*.dart"))) .. "\" \"$(TARGETDIR)\"" }