sweetwm.events = {
	xevent = {
		map_request = function(w)
			sweetwm.map_window(w)
			sweetwm.process_window(w)
		end,
		ConfigureRequest = function(w, x, y, width, height)
			sweetwm.move_window(w, x, y)
			sweetwm.resize_window(w, width, height)
		end
	}
}

function sweetwm.error(...)
	print("Error:", ...)
end

function sweetwm.event(name, ...)
	local function f(t, name, ...)
		if name == nil then
			sweetwm.error("Event name undefined")
			return
		end
		local h = t[name]
		if type(h) == "function" then
			h(...)
		elseif type(h) == "table" then
			f(h, ...)
		else
			sweetwm.error("Unhandled event: ", name, ...)
		end
	end
	local ok, errmsg = pcall(f, sweetwm.events, name, ...)
	if not ok then
		sweetwm.error(errmsg)
	end
end


