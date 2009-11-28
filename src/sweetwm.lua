wm.events = {}
wm.xevents = {}

function wm.events.X(name, ...)
	local handler = wm.xevents[name]
	if type(handler) == "function" then
		handler(...)
	else
		print("Unhandled X event: ", name, ...)
	end
end

function wm.xevents.map_request(w)
	wm.map_window(w)
	wm.process_window(w)
end

function wm.xevents.ConfigureRequest(w, x, y, width, height)
	wm.move_window(w, x, y)
	wm.resize_window(w, width, height)
end

function wm.event(name, ...)
	local handler = wm.events[name]
	if type(handler) == "function" then
		handler(...)
	else
		print("Unhandled event: ", name, ...)
	end
end


