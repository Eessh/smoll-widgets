workspace("smoll-widgets")
	configurations({ "Debug", "Release" })
	filter("configurations:Debug")
		defines({ "DEBUG" })
		symbols("On")
	filter("configurations:Release")
		defines({ "NDEBUG", "O3" })
		optimize("On")
	filter({})
	targetdir("bin/%{cfg.buildcfg}/")

	-- Smoll Widgets
	project("smoll-widgets")
		kind("StaticLib")
		language("C")
		includedirs({
			"include",
		})
		files({
			"src/**.c",
		})
	
	-- SDL2 + Cairo example
	project("sdl2-cairo-example")
		language("C")
		includedirs({
			"include",
			"backends/sdl2_cairo",
		})
		files({
			"backends/sdl2_cairo/*.c",
		})
		links({
			"SDL2main",
			"SDL2",
			"cairo",
			"smoll-widgets"
		})
		filter("configurations:Debug")
    		kind("ConsoleApp")
			libdirs({
			    "backends/sdl2_cairo/SDL2-2.26.5/x86_64-w64-mingw32/lib",
			    "backends/sdl2_cairo/cairo-windows-1.17.2/lib/x64",
			    "bin/Debug" -- smoll-widgets built library path
			})
		filter("configurations:Release")
    		kind("WindowedApp")
			libdirs({
			    "backends/sdl2_cairo/SDL2-2.26.5/x86_64-w64-mingw32/lib",
			    "backends/sdl2_cairo/cairo-windows-1.17.2/lib/x64",
			    "bin/Release" -- smoll-widgets built library path
			})
		filter({})

	-- Win32 + Cairo example
    project("win32-cairo-example")
        language("C")
        includedirs({
            "include",
            "backends/win32_cairo",
        })
        files({
            "backends/win32_cairo/*.c",
        })
        links({
            "gdi32",
            "cairo",
            "smoll-widgets"
        })
        filter("configurations:Debug")
            kind("ConsoleApp")
            libdirs({
                "backends/win32_cairo/cairo-windows-1.17.2/lib/x64",
                "bin/Debug" -- smoll-widgets built library path
            })
        filter("configurations:Release")
            kind("WindowedApp")
            libdirs({
                "backends/win32_cairo/cairo-windows-1.17.2/lib/x64",
                "bin/Release" -- smoll-widgets built library path
            })
        filter({})
