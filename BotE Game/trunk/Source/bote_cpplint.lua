require "lfs"
local currentdir = lfs.currentdir()
local allowed_types = {"hpp","h","cpp"}

local function files(path, allowed_types)
	assert(type(path) == "string")
	local length = string.len(path)
	if string.sub(path, length) == dirsep then
		if unix then path = string.sub(path, 1, length - 1)
		else path = string.sub(path, 1, length - 2)
		end
	end
	local result = {}
	local function sub_files(path)
		local attr = assert(lfs.attributes(path))
		if attr.mode == "directory" then
			for name, test in lfs.dir(path) do
				if string.sub(name, 1, 1) ~= "." then
					sub_files(string.format("%s%s%s", path, "\\", name))
				end
			end
		elseif attr.mode == "file" then
			local insert = true
			if allowed_types then
				local dot_index = string.find(string.reverse(path), "%.")
				insert = false
				if dot_index then
					local ending = string.sub(path, string.len(path) - dot_index + 2)
					for i, v in ipairs(allowed_types) do
						if v == ending then insert = true; break end
					end
				end
			end
			if insert then
				table.insert(result, path)
			end
		else assert(false)
		end
	end
	sub_files(path)
	return result
end

for index, filenamestring in ipairs(files(currentdir, allowed_types)) do
	local file = io.open(filenamestring, "rb")
	local line_number = 1
	for line in file:lines() do
		local s, e = string.find(line, "%.Format%(")
		if s then
			local var_start = string.find(line,"[^%s]")
			local var = string.sub(line, var_start, s - 1)
			local args_s, args_e = string.find(line, "%b()", e - 1)
			if args_s and args_e then
				local args = string.sub(line, args_s, args_e)
				local evil = string.find(args, " " .. var .. ",")
				evil = evil or string.find(args, "," .. var .. ",")
				if evil then
					print(filenamestring .. ": " .. line_number .. ": CString passed to its own format call")
				end
			end
		end
		line_number = line_number + 1
	end
	file:close()
end
