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

function sweetwm.get_window(id)
	local window = {
		attributes = {},
		properties = {},
		wmhints = {},
		sizehints = {}
	}
	local callbacks = {
		attribute = function(name, value)
			window.attributes[name] = value
		end,
		property = function(name, value)
			window.properties[name] = value
		end,
		wmhint = function(name, value)
			window.wmhints[name] = value
		end,
		sizehint = function(name, value)
			window.sizehints[name] = value
		end,
		}
	sweetwm.process_window(id, function(event, ...)
		callbacks[event](...)
	end)
	return window
end

function sweetwm.get_window_size(id)
	local window = sweetwm.get_window(id)
	local root = sweetwm.get_window(window.attributes.root)
	local x = window.attributes.x
	local y = window.attributes.y
	local w = window.attributes.width
	local h = window.attributes.height
	local rw = root.attributes.width
	local rh = root.attributes.height
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


