sweetwm.events = {
	map_request = function(win)
		local x, y, w, h = sweetwm.get_window_size(win)
		win:move(x, y)
		win:resize(w, h)
		win:set_border_width(2)
		win:set_border_color(1, 1, 0)
		win:map()
	end,
	configure_request = function(win, x, y, w, h)
		win:move(x, y)
		win:resize(w, h)
	end,
	enter_notify = function(win) end,
	leave_notify = function(win) end,
	create_notify = function() end,
	destroy_notify = function() end,
	map_notify = function() end,
	unmap_notify = function() end,
	configure_notify = function() end,
	key_press = function(key)
		print("Key", key)
	end,
	key_release = function(key) end,
}

function sweetwm.error(...)
	print("Error:", ...)
end

function sweetwm.event(name, ...)
	local f = sweetwm.events[name]
	if f then f(...) end
end

function sweetwm.get_window_size(w)
	local watts = {}
	w:attributes(function(n, v) watts[n] = v end)
	local ratts = {}
	watts.root:attributes(function(n, v) ratts[n] = v end)
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


