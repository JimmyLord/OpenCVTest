------------------------------------------------ Pure Lua ---------------------------------------------------

-- Copy some dlls into the OpenCVTest folder if they're not already there.
function FileExists(name)
   local f = io.open( name,"r" )
   if f ~= nil then io.close( f ) return true else return false end
end

function CopyFile(source, destination)
    if( FileExists( destination ) == false ) then
        print( "Copying " .. source )
        local ret, err = os.copyfile( source, destination )
        if ret == nil then print( err ) end
        print( "done" )
    end
end

--monoInstallationPath = "C:/Program Files/Mono" -- TODO: Don't hardcode the path to mono installation.

CopyFile( "Libraries/OpenCV/x64/vc15/bin/opencv_world453.dll", "OpenCVTest/opencv_world453.dll" )
CopyFile( "Libraries/OpenCV/x64/vc15/bin/opencv_world453d.dll", "OpenCVTest/opencv_world453d.dll" )
CopyFile( "Libraries/Framework/Libraries/pthreads-w32/dll/x64/pthreadVC2.dll", "OpenCVTest/pthreadVC2-x64.dll" )
--CopyFile( monoInstallationPath .. "/bin/mono-2.0-sgen.dll", "OpenCVTest/mono-2.0-sgen.dll" )
--os.mkdir( "OpenCVTest/mono/lib/mono/4.5" )
--CopyFile( monoInstallationPath .. "/lib/mono/4.5/mscorlib.dll", "OpenCVTest/mono/lib/mono/4.5/mscorlib.dll" )

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
    defines         { "MYFW_EDITOR", "MYFW_USING_IMGUI" }
    cppdialect      "C++17"

    filter "system:windows"
        defines         "MYFW_WINDOWS"
        systemversion   "latest"
        platforms       { "x64" }
        characterset    "MBCS"

----------------------------------------------- All Projects ------------------------------------------------
PremakeConfig_UseMemoryTracker = false
PremakeConfig_UseLua = false              -- Also: added 'defines "MYFW_USE_LUA=0"' in project below.
PremakeConfig_UseMono = false
PremakeConfig_UseBox2D = false            -- Also: added 'defines "MYFW_USE_BOX2D=0"' in project below.
PremakeConfig_UseBullet = false           -- Also: added 'defines "MYFW_USE_BULLET=0"' in project below.

------------------------------------------------ MyFramework ------------------------------------------------
MyFrameworkPremakeConfig_ForceIncludeEditorFiles = true
BuildSingleProjectPremake( "Libraries/Framework/", "premake5inc.lua" )

-------------------------------------------------- MyEngine -------------------------------------------------
MyEnginePremakeConfig_FrameworkFolder = "$(SolutionDir)../Libraries/Framework"
MyEnginePremakeConfig_ForceIncludeEditorFiles = true
BuildSingleProjectPremake( "Libraries/Engine/", "premake5inc.lua" )

----------------------------------------------- SharedGameCode ----------------------------------------------
SharedGameCodePremakeConfig_EngineFolder = "$(SolutionDir)../Libraries/Engine"
SharedGameCodePremakeConfig_FrameworkFolder = "$(SolutionDir)../Libraries/Framework"
SharedGameCodePremakeConfig_SharedGameCodeFolder = "$(SolutionDir)../Libraries/SharedGameCode"
BuildSingleProjectPremake( "Libraries/Engine/Libraries/SharedGameCode/", "premake5inc.lua" )

group "Physics"
    --BuildSingleProjectPremake( "Libraries/Engine/Libraries/", "premake5inc-bullet.lua" )
    --BuildSingleProjectPremake( "Libraries/Framework/Libraries/", "premake5inc-box2d.lua" )
group ""

--------------------------------------------- OpenCVTest Project --------------------------------------------
project "OpenCVTest"
    location    "build"
    kind        "WindowedApp"
    language    "C++"
    targetdir   "$(SolutionDir)Output/%{cfg.platform}-%{prj.name}-%{cfg.buildcfg}"
    objdir      "$(SolutionDir)Output/Intermediate/%{cfg.platform}-%{prj.name}-%{cfg.buildcfg}"
    debugdir    "OpenCVTest"
    dependson   { "MyFramework", "MyEngine" }
    pchheader   "OpenCVPCH.h"
    pchsource   "OpenCVTest/Source/OpenCVPCH.cpp"

    includedirs {
        "OpenCVTest/Source",
        "$(SolutionDir)../",
        --"$(SolutionDir)../Libraries/Framework/Libraries/b2Settings",
        --"$(SolutionDir)../Libraries/Framework/Libraries/Box2D",
        "Libraries/OpenCV/include",
        --monoInstallationPath .. "/include/mono-2.0",
    }

    files {
        "OpenCVTest/Data/**.opencvnodegraph",
        "OpenCVTest/Data/Shaders/**.glsl",
        "OpenCVTest/Source/**.cpp",
        "OpenCVTest/Source/**.h",
        "Libraries/Delaunator/**.cpp",
        "Libraries/Delaunator/**.hpp",
        "PremakeGenerateBuildFiles.bat",
        "premake5.lua",
        "PremakeGenerateBuildFiles.bat",
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
        "MyFramework",
        "MyEngine",
        "SharedGameCode",
        --"Box2D",
        --"BulletCollision",
        --"BulletDynamics",
        --"LinearMath",
    }

    if PremakeConfig_UseLua == false then
        defines "MYFW_USE_LUA=0"
    end
    if PremakeConfig_UseBox2D == false then
        defines "MYFW_USE_BOX2D=0"
    end
    if PremakeConfig_UseBullet == false then
        defines "MYFW_USE_BULLET=0"
    end

    filter "configurations:Release"
        defines         "NDEBUG"
        optimize        "Full"
        links { "Libraries/OpenCV/x64/vc15/lib/opencv_world453.lib" }

    filter "configurations:Debug"
        defines         "_DEBUG"
        symbols         "on"
        links { "Libraries/OpenCV/x64/vc15/lib/opencv_world453d.lib" }
if PremakeConfig_UseMemoryTracker == true then
        defines         "MYFW_USE_MEMORY_TRACKER"
end

    filter "system:windows"
        libdirs {
            "Libraries/Framework/Libraries/pthreads-w32/lib/x64",
            --monoInstallationPath .. "/lib",
        }

        links {
            "pthreadVC2",
            "delayimp",
            "Ws2_32",
            "opengl32",
            "glu32",
            "xinput",
            --"mono-2.0-sgen",
        }

        linkoptions { "/DELAYLOAD:pthreadVC2.dll" }
