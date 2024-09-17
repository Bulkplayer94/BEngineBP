local testValue = 5
testValue = 5 * 2

for k=0,100 do
	testValue = testValue + 5
end

print(testValue  )

hook.Add("DasDrawHookVonMenu", "Draw", function() 

	imgui.Menu("Hurensohn")

end)