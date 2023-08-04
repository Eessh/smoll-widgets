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
			"src/*.c",
		})
	
	-- SDL2 + Cairo example
	project("sdl2-cairo-example")
		kind("WindowedApp")
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
			libdirs({ "backends/sdl2_cairo/SDL2-2.26.5/x86_64-w64-mingw32/lib", "backends/sdl2_cairo/cairo-windows-1.17.2/lib/x64", "bin/Debug" })
		filter("configurations:Release")
			libdirs({ "backends/sdl2_cairo/SDL2-2.26.5/x86_64-w64-mingw32/lib", "backends/sdl2_cairo/cairo-windows-1.17.2/lib/x64", "bin/Release" })
		filter({})
