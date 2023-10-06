-- 设置平台
set_config("plat", os.host())

-- 工程名
set_project("TCP_IP")

-- 工程版本
set_version("1.0.0", {build = "%Y%m%d%H%M", soname = true})

-- 设置需要的最低xmake版本
set_xmakever("2.3.1")

-- 设置语言标准
set_languages("cxx11", "c11")
set_warnings("all")     -- -Wall
--set_symbols("debug")    -- dbg symbols
--add_rules("mode.debug", "mode.release")

-- 这是 DreamSky 的依赖
if is_os("windows") then 
    add_requires("rapidjson")
    add_requires("pthreads4w")
end

-- 这是 DreamSky 的库
add_repositories("local-repo DreamSky")
add_requires("dreamcommon")
add_requires("dreamsocket")
add_requires("dreamprocess")
add_requires("dreamthread")

-- 找到所有的 chapter
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

-- 包含子项目
for _, chapter in ipairs(chapter_directory()) do 
    includes(chapter)
end
