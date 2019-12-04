include "../../tools/dragoncore.lua"

print("[PCGTowers] Building PCGTowers Project Files!")

workspace "PCGTowers"

    startproject "PCGTowers"
    dragon_workspace_defaults()
    
project "PCGTowers"

    dragon_project_defaults()

    location "%{prj.name}"
    kind "ConsoleApp"

    files 
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs 
    { 
        "%{prj.name}/src",
    }

    include_dragoncore("../../")
    links { "DragonCore" }
