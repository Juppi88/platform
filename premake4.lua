-- A library for all platform specific functionality

project "Lib-Platform"
	kind "StaticLib"
	language "C"
	files { "**.h", "**.c", "premake4.lua" }
	vpaths { [""] = { "../Libraries/Platform/Src", "../Libraries/Platform" } }
	includedirs { ".", ".." }
	location ( "../../Projects/" .. os.get() .. "/" .. _ACTION )
	
	-- Linux specific stuff
	configuration "linux"
		targetextension ".a"
		configuration "Debug" targetname "libplatformd"
		configuration "Release" targetname "libplatform"
	
	-- Windows specific stuff
	configuration "windows"
		targetextension ".lib"
		buildoptions { "/wd4054" } -- C4054: cast from 'FARPROC' to 'void*'
		configuration "Debug" targetname "platformd"
		configuration "Release" targetname "platform"
