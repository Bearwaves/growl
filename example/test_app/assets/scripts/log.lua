for var = 1, 5 do
  Growl.log("Test", "var = " .. var);
end

local function printtable(table)
  for k, _ in pairs(table) do
    Growl.log("Table", k)
  end
end

local cat = Node.new("ElGato")
local cat2 = Node.new("Kato")

function cat:setX(x)
  Node.setX(self, x + 100)
end

cat:setX(100)
cat:setY(100)
cat2:setX(3)
cat2:setY(7)

Growl.log("Cat", "X: " .. cat:getX())
Growl.log("Cat", "Y: " .. cat:getY())
Growl.log("Kato", "X: " .. cat2:getX())
Growl.log("Kato", "Y: " .. cat2:getY())
