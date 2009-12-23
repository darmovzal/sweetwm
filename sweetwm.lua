sweetwm.events = {
	xevent = {
		map_request = function(id)
			sweetwm.process_window(id, print)
			local window = sweetwm.window
			sweetwm.windows[id] = window
			sweetwm.window = nil
			local x, y, w, h = sweetwm.get_window_size(window)
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
			print("Enter", id)
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
		end,
		key_release = function(key)
		end,
	},
--[[
	window = {
		begin = function(w)
			sweetwm.window = {
				id = w,
				attributes = {},
				properties = {},
				wmhints = {},
				sizehints = {}
			}
		end,
		attribute = function(name, value)
			sweetwm.window.attributes[name] = value
		end,
		property = function(name, value)
			sweetwm.window.properties[name] = value
		end,
		wmhint = function(name, value)
			sweetwm.window.wmhints[name] = value
		end,
		sizehint = function(name, value)
			sweetwm.window.sizehints[name] = value
		end,
		["end"] = function() end
	}
]]
}

sweetwm.windows = {}

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

function sweetwm.get_window_size(window)
	return window.attributes.x + 50, window.attributes.y + 50, window.attributes.width, window.attributes.height
end

for n, v in pairs(sweetwm) do
	print(n, v)
end
print("Screens:")
print(sweetwm.get_screen_size(0))


