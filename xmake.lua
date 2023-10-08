set_config("plat", os.host())
set_project("TCP_IP")
set_version("1.0.0")
set_xmakever("2.3.1")
set_languages("cxx11", "c11")
set_warnings("all")

if is_os("windows") then 
    add_requires("rapidjson")
    add_requires("pthreads4w")
end

add_repositories("local-repo DreamSky")
add_requires("dreamcommon")
add_requires("dreamsocket")
add_requires("dreamprocess")
add_requires("dreamthread")

function chapter_directory() 
    local res = {}
    for _, chapter in ipairs(os.dirs("./*")) do
        local tmp_dir = path.join(chapter, "src")
        if (os.exists(tmp_dir)) then
            table.insert(res, chapter)
        end
    end
    return res
end

for _, chapter in ipairs(chapter_directory()) do 
    includes(chapter)
end
