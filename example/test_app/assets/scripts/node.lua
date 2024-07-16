local node = {}

function node:setX(x)
  Node.setX(self, x * 2)
end

function node:onMouseEvent(event)
  Growl.log("Mouse", "(" .. event.mouseX .. ", " .. event.mouseY .. ")")
  Node.onMouseEvent(self, event)
end

return node
