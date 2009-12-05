sweetwm.events = {}
sweetwm.xevents = {}

function sweetwm.events.X(name, ...)
	local handler = sweetwm.xevents[name]
	if type(handler) == "function" then
		handler(...)
	else
		print("Unhandled X event: ", name, ...)
	end
end

function sweetwm.xevents.map_request(w)
	sweetwm.map_window(w)
	sweetwm.process_window(w)
end

function sweetwm.xevents.ConfigureRequest(w, x, y, width, height)
	sweetwm.move_window(w, x, y)
	sweetwm.resize_window(w, width, height)
end

function sweetwm.event(name, ...)
	local handler = sweetwm.events[name]
	if type(handler) == "function" then
		handler(...)
	else
		print("Unhandled event: ", name, ...)
	end
end


