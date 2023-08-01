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
