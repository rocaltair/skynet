local skynet = require "skynet"
local socket = require "socket"

skynet.start(function()
	local host = "0.0.0.0:12345"
	local id = socket.listen(host)
	print("listen on", host, id)
	socket.start(id, function(peer, addr)
		print("accept", peer, addr)
		socket.start(peer)
		while true do 
			local str = socket.read(peer)
			if not str then
				print("close", peer)
				break
			end
			print(str)
		end
		socket.close(peer)
	end)
end)
