sweetwm.events = {
	map_request = function(id)
		local x, y, w, h = sweetwm.get_window_size(id)
		sweetwm.move_window(id, x, y)
		sweetwm.resize_window(id, w, h)
		sweetwm.set_border_width(id, 5)
		sweetwm.set_border_color(id, "rgb:ff/00/00")
		sweetwm.map_window(id)
	end,
	configure_request = function(id, x, y, w, h)
		sweetwm.move_window(id, x, y)
		sweetwm.resize_window(id, w, h)
	end,
	enter_notify = function(id)
	end,
	leave_notify = function(id)
	end,
	create_notify = function()
	end,
	destroy_notify = function()
	end,
	map_notify = function()
	end,
	unmap_notify = function()
	end,
	configure_notify = function()
	end,
	key_press = function(key)
		print("Key", key)
	end,
	key_release = function(key)
	end,
}

function sweetwm.error(...)
	print("Error:", ...)
end

function sweetwm.event(name, ...)
	local f = sweetwm.events[name]
	if f then f(...) end
end

function sweetwm.get_window_size(id)
	local watts = {}
	sweetwm.attributes(id, function(n, v) watts[n] = v end)
	local ratts = {}
	sweetwm.attributes(watts.root, function(n, v) ratts[n] = v end)
	local x, y, w, h = watts.x, watts.y, watts.width, watts.height
	local rw, rh = ratts.width, ratts.height
	if x == 0 and y == 0 then
		x = (rw - w) / 2
		y = (rh - h) / 2
	end
	return x, y, w, h
end

print("Screens:")
for screen = 0, sweetwm.get_screen_count() - 1 do
	print(screen .. ":", sweetwm.get_screen_size(screen))
end


