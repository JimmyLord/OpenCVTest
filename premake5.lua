------------------------------------------------ Pure Lua

-- Copy the OpenCV dll into the OpenCVTest folder if it's not already there.
function file_exists(name)
   local f = io.open( name,"r" )
   if f ~= nil then io.close( f ) return true else return false end
end

if( file_exists( "OpenCVTest/opencv_world411.dll" ) == false ) then
    print( "Copying OpenCV dll to OpenCVTest folder..." )
    os.copyfile( "Libraries/OpenCV/x64/vc15/bin/opencv_world411.dll", "OpenCVTest/opencv_world411.dll" )
    print( "done" )
end

------------------------------------------------ Solution
workspace "OpenCVTest"
    configurations  { "Debug", "Release" }
    location        ( "build" )
    startproject    "OpenCVTest"

    filter "system:windows"
        platforms       { "x64" }
        characterset    "MBCS"

------------------------------------------------ OpenCVTest Project 
project "OpenCVTest"
    location    "build/OpenCVTest"
    kind        "WindowedApp"
    language    "C++"
    targetdir   "$(SolutionDir)Output/%{cfg.platform}-%{prj.name}-%{cfg.buildcfg}"
    objdir      "$(SolutionDir)Output/Intermediate/%{cfg.platform}-%{prj.name}-%{cfg.buildcfg}"
    debugdir    "OpenCVTest"

    includedirs {
        "OpenCVTest/Source",
        "Libraries/OpenCV/include"
    }

    files {
        "OpenCVTest/Source/**.cpp",
        "OpenCVTest/Source/**.h",
        "premake5.lua",
        ".gitignore",
    }

    vpaths {
        [""] = {
            "premake5.lua",
            ".gitignore",
        },
        -- Place the SourceCommon and SourceEditor folders in the root of the project.
        ["*"] = {
            "OpenCVTest",
        },
    }

    links {
        "opengl32",
    }

    filter "configurations:Release"
        links { "Libraries/OpenCV/x64/vc15/lib/opencv_world411.lib" }

    filter "configurations:Debug"
        -- Debug lib giving linker errors, using release, TODO: Look into it.
        links { "Libraries/OpenCV/x64/vc15/lib/opencv_world411.lib" }
