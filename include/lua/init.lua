dofile("..\\include\\lua\\core.lua");
function f_onkey(data)
	local key = tonumber(data);
	if(key == KEY_ESC) then
        ex:exit();	
	end
end

evt_on(cam,EVENT_ENGINE_KEYBOARD,f_onkey);
ex:bgColor(0.7,0.7,0.7);

--fps();
infowin(0,20);


dofile("..\\include\\lua\\7.lua");

--print("lua script is already init finished !");

--local function f_onkey(data)
--	local key = tonumber(data);
--    if(key == KEY_ESC) then
--        ex:exit();	
--	end
--end

--evt_on(cam,EVENT_ENGINE_KEYBOARD,f_onkey);

--local btn = btn_create(0,0,80,18,"gundi.png");
--local ft = func_ftext_create(16,15);

--func_ftext_parse(ft,"a��B");