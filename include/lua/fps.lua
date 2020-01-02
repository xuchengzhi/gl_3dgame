--Fps��ͼ��
FpsView = {
	label,
	timer,
	formatstr,
};

FpsView.__index = FpsView;
setmetatable(FpsView, Instance);--�̳��Ե���

function FpsView:new()
	local self = Instance:new();
	setmetatable(self, FpsView);
	self.label = NLabel:new(64,64);--label_create(64,64);
	return self;
end
local function f_fps_timer(data,param)
	local self = param;
	local _fps = JEngine:getIns():get_fps();--engine_get_fps();
	local str = _fps;
	if(self.formatstr) then
		str = string.format(self.formatstr,_fps);
	end
	
	--label_set_text(self.label,str);
    self.label:set_text(str);
end

function FpsView:dispose()
	if(self.timer) then
		timelater_remove(self.timer);
	end
	self:hide();
	--label_dispose(self.label);
    self.label:dispose();
end
function FpsView:show(x,y,formatstr)
	--label_set_pos(self.label,x or 0,y or 0);
--	label_set_visible(self.label,1);

    self.label:set_pos(x or 0,y or 0);
    self.label:visible(true);
	--print(self.x);
	self.formatstr = formatstr;
	if(self.timer==nil) then
		local timer = timelater_new(1000);
		self.timer = timer;
		evt_on(timer,EVENT_TIMER,f_fps_timer,self);
	end
end
function FpsView:hide()
	--ftext_vis(self.tf,0);
--	label_set_visible(self.label,0);
    self.label:visible(false);
	local timer = self.timer;
	evt_off(timer,EVENT_TIMER,f_fps_timer);
end