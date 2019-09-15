------------------------------------------------ Pure Lua ---------------------------------------------------

-- Copy some dlls into the OpenCVTest folder if they're not already there.
function FileExists(name)
   local f = io.open( name,"r" )
   if f ~= nil then io.close( f ) return true else return false end
end

function CopyFile(source, destination)
    if( FileExists( destination ) == false ) then
        print( "Copying " .. source )
        os.copyfile( source, destination )
        print( "done" )
    end
end

CopyFile( "Libraries/OpenCV/x64/vc15/bin/opencv_world411.dll", "OpenCVTest/opencv_world411.dll" )
CopyFile( "Libraries/OpenCV/x64/vc15/bin/opencv_world411d.dll", "OpenCVTest/opencv_world411d.dll" )
CopyFile( "Libraries/Framework/Libraries/pthreads-w32/dll/x64/pthreadVC2.dll", "OpenCVTest/pthreadVC2-x64.dll" )

-- Helper to include other premake.lua files.
local rootFolder = os.getcwd()

BuildSingleProjectPremake = function(folder, filename)
    os.chdir( folder )
    include( filename )
    os.chdir( rootFolder )
end

------------------------------------------------- Solution --------------------------------------------------
workspace "OpenCVTest"
    configurations  { "Debug", "Release" }
    location        ( "build" )
    startproject    "OpenCVTest"

    filter "system:windows"
        defines         "MYFW_WINDOWS"
        systemversion   "latest"
        platforms       { "x64" }
        characterset    "MBCS"

------------------------------------------------ MyFramework  -----------------------------------------------
BuildSingleProjectPremake( "Libraries/Framework/", "premake5inc.lua" )

--------------------------------------------- OpenCVTest Project  -------------------------------------------
project "OpenCVTest"
    location    "build"
    kind        "WindowedApp"
    language    "C++"
    targetdir   "$(SolutionDir)Output/%{cfg.platform}-%{prj.name}-%{cfg.buildcfg}"
    objdir      "$(SolutionDir)Output/Intermediate/%{cfg.platform}-%{prj.name}-%{cfg.buildcfg}"
    debugdir    "OpenCVTest"
    dependson   "MyFramework"
    pchheader   "OpenCVPCH.h"
    pchsource   "OpenCVTest/Source/OpenCVPCH.cpp"

    includedirs {
        "OpenCVTest/Source",
        "$(SolutionDir)../",
        "$(SolutionDir)../Libraries/Framework/Libraries/b2Settings",
        "$(SolutionDir)../Libraries/Framework/Libraries/Box2D",
        "Libraries/OpenCV/include",
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
        "MyFramework",
    }

    filter "configurations:Release"
        defines         "NDEBUG"
        optimize        "Full"
        links { "Libraries/OpenCV/x64/vc15/lib/opencv_world411.lib" }

    filter "configurations:Debug"
        defines         "_DEBUG"
        symbols         "on"
        links { "Libraries/OpenCV/x64/vc15/lib/opencv_world411d.lib" }

    filter "system:windows"
        libdirs {
            "Libraries/Framework/Libraries/pthreads-w32/lib/x64",
        }

        links {
            "pthreadVC2",
            "delayimp",
            "Ws2_32",
            "opengl32",
            "glu32",
            "xinput",
        }

        linkoptions { "/DELAYLOAD:pthreadVC2.dll" }
