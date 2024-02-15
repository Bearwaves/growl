local node = {}

function node:setX(x)
  Node.setX(self, x * 2)
end

return node
