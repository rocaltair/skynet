local liblist = {
	"cluster", "datacenter", "dns",
	"md5", "mongo", "mqueue", "multicast",
	"mysql", "redis", "sharedata", "sharemap",
	"skynet", "snax", "socket", "socketchannel",
	"sproto", "sprotoloader", "sprotoparser",
	"http.httpc", "http.httpd", "http.internal", "http.sockethelper", "http.url",
	"sharedata.corelib",
	"skynet.coroutine", "skynet.debug", "skynet.harbor",
	"skynet.injectcode",
	"skynet.manager", "skynet.queue", "skynet.remotedebug",
	"snax.gateserver", "snax.hotfix", "snax.interface", "snax.loginserver",
	"snax.msgserver",
}

function pairs_orderly(t, comp)
	local keys = {}
	for k, _ in pairs(t) do
		table.insert(keys, k)
	end
	table.sort(keys, comp)
	local i = 0
	return function()
		i = i + 1
		local key = keys[i]
		return key, t[key]
	end
end

function printf(fmt, ...)
	local msg = string.format(fmt, ...)
	print(msg)
end

function func_param(func, n, isvararg)
	local l = {}
	for i = 1, n do
		local name = debug.getlocal(func, i)
		table.insert(l, name)
	end
	if isvararg then
		table.insert(l, "...")
	end
	return table.concat(l, ", ")
end

function func_info(func)
	if type(func) == "function" then
		local info = debug.getinfo(func, "Slu")
		if info.what == "Lua" then
			local params = func_param(func, info.nparams, info.isvararg)
			return string.format("(%s) == %s +%d", params, info.short_src, info.linedefined)
		end
		return "<C>"
	end
	return tostring(func)
end

function show_lib(lib, name)
	for k, v in pairs_orderly(lib) do
		printf("%s.%s%s", name, k, func_info(v))
	end
end

function show_lib_byname(name)
	local lib = require(name)
	local tname = type(lib)
	if tname == "table" then
		show_lib(lib, name)
	elseif tname == "function" then
		local info = func_info(lib)
		printf("%s%s", name, info)
	end
end


function dump()
	for k, v in pairs(liblist) do
		show_lib_byname(v)
	end
end

dump()
