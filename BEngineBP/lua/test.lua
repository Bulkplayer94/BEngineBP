print("This is test.lua!")

local obj = MyObject(42)
print(obj:get())    -- 42
obj:set(-1.5)
print(obj:get())    -- -1.5

local obj2 = Vector(5, 4, 3)
--print(obj2.x)
local obj3 = Vector(62, 12, 5)

local scaledVec = obj2:scale(10)
local addedVec = obj2:add(obj3)
local subVec = obj2:subtract(obj3)

print(scaledVec:toString(), addedVec:toString(), subVec:toString())

--print(scaledVec:toString())

--local x,y,z = obj2:getComponents()