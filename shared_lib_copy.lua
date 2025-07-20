function copy_shared_lib(name, platform, outputdir)
	local shared_lib_name

	if platform == "windows" then
		shared_lib_name = name .. ".dll"
	elseif platform == "linux" then
		shared_lib_name = "lib" .. name .. ".so"
	else
		print("Warning: Shared Library automatic copy not supported on " .. platform)
		return
	end

	local shared_lib_path = path.join("%{wks.location}/out/bin/", outputdir, name, shared_lib_name)

	postbuildcommands {
		"{COPY} " .. shared_lib_path .. " " .. "%{cfg.targetdir}"
	}
end