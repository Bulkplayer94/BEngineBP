hook = hook or {}
hook.tbl = hook.tbl or {}

function hook.Add(hook, hookName, func)

	hook.tbl[hook] = hook.tbl[hook] or {}
	hook.tbl[hook][hookName] = func

end

function hook.Run(hook, ...)

	for name, hookFunc in pairs(hookTbl.tbl[hook]) do
		
		local result = hookFunc(args)
		if (result != nil) then
			return result
		end

	end

end

