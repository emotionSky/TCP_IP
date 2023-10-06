-- 使用函数来遍历cpp文件夹，后续根据遍历的结果来处理
function all_files()
    local res = {}
    for _, x in ipairs(os.files("**.cpp")) do
        local item = {}
        local s = path.filename(x)
        table.insert(item, s:sub(1, #s - 4))       -- 取文件名来作为target的名字
        table.insert(item, path.relative(x, "."))  -- 利用path.relative来转换相对路径，即将 x 转换为相对于 . 的相对路径
        table.insert(res, item)
    end
    return res
end

local chapter = "ch20"
for _, file in ipairs(all_files()) do
local target_name = chapter .. "_" .. file[1]
target(target_name)
    set_kind("binary")
    -- set_default(false)
    add_files(file[2])
    add_packages("dreamcommon")
    add_packages("dreamsocket")
    add_packages("dreamthread")
end