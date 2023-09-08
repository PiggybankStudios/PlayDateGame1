import "CoreLibs/graphics"
import "CoreLibs/object"

local gfx <const> = playdate.graphics

class("game1").extends()

function game1:init(xspeed, yspeed)
	self.label = {
		x = 155,
		y = 110,
		xspeed = xspeed,
		yspeed = yspeed,
		width = 171,
		height = 20
	}
end

function game1:swapColors()
	if (gfx.getBackgroundColor() == gfx.kColorWhite) then
		gfx.setBackgroundColor(gfx.kColorBlack)
		gfx.setImageDrawMode("inverted")
	else
		gfx.setBackgroundColor(gfx.kColorWhite)
		gfx.setImageDrawMode("copy")
	end
end

function game1:update()
	local label = self.label;
	local swap = false
	
	label.x += label.xspeed
	label.y += label.yspeed
	
	if (label.x >= 400 - label.width) then
		label.x = 2*(400 - label.width) - label.x;
		label.xspeed = -label.xspeed;
		swap = true
	end
	
	if (label.x <= 0) then
		label.x = 0 - label.x;
		label.xspeed = -label.xspeed;
		swap = true
	end
	
	if (label.y >= 240 - label.height) then
		label.y = 2*(240 - label.height) - label.y;
		label.yspeed = -label.yspeed;
		swap = true
	end
	
	if (label.y <= 0) then
		label.y = 0 - label.y;
		label.yspeed = -label.yspeed;
		swap = true
	end
	
	if (swap) then
		self:swapColors()
	end
end

function game1:draw()
	local label = self.label;
	gfx.drawTextInRect("Taylor Robbins", label.x, label.y, label.width, label.height)
end