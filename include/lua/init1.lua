dofile("..\\include\\lua\\core.lua");

--example_stack();

--########################################
--node_fbo();

--btn_create(200,50,60,20,"smallbtn.png");
--btn_create(200,70,120,40,"smallbtn.png");

--[[
local function f_callback2(data,obj)
	print("******************************���ؽ���2",data,obj);	
	local spr = sprite_create_9grid(data,100,50,200,50,0,3,3,3,3);
	engine_addNode(spr);
end--]]

--print(string.format("res = %d",res));

--[[local btn = btn_create(0,50);
btn_bindClick(btn,f_onClick);--]]
--**********************************************************

function f_onkey(data)
	local key = tonumber(data);
	print("key = "..key);
	if(key == KEY_ESC) then
         engine_exit();
	elseif(key == 13) then
	--�س�
	--	func_ex_info();
	elseif(key == 49) then
		func_ex_info();
	
		--setv(btnspr,FLAGS_DRAW_PLOYGON_LINE );
		--setv(btnspr,FLAGS_DISABLE_CULL_FACE );
		if(btnspr) then
			if(getv(btnspr,FLAGS_REVERSE_FACE) == 1) then
				resetv(btnspr,FLAGS_REVERSE_FACE );
			else
				setv(btnspr,FLAGS_REVERSE_FACE );
			end
		end
	elseif(key == 50) then
		--[[if(btnspr) then
			setv(btnspr,FLAGS_DRAW_PLOYGON_LINE );
		end--]]
		if(sv) then
			scrollView_dispose(sv);
		end
	end
end
evt_on(cam,EVENT_ENGINE_KEYBOARD,f_onkey);

engine_setBg(0.3,0.3,0.3);
--********************************************
local function uiinit()
	fps();
	btn_create(150,0,100,45);
	example_input(0,80);
	infowin(150,50);
	example_srollView();
	alert("aa");
	dofile("..\\include\\lua\\1.lua");--md2��������
	dofile("..\\include\\lua\\5.lua");--���ʲ���
end
--********************************************


--input_dispose(_in);
--func_fixed_load("\\resource\\obj\\torus.obj");
--dofile("..\\include\\lua\\quaternion.lua");--

--arrow();

--infowin(150,50);

--crl_init();

--uiinit();

--dofile("..\\include\\lua\\3.lua");

--alert("aa");

--example_unit();

--//���ֵ�һ���ֽڵķ�Χ
--static int
--f_is_first_code(unsigned char c){
--	return (0x80 <= c) && (0xF7 >= c);
--}
--//���ֵڶ����ֽڵķ�Χ
--static int
--f_is_second_code(unsigned char c){
--	return (0xA1 <= c) && (0xFE > c);
--}

--print(0x80,0xF7,0xA1,0xFE);


--local t = func_get_longTime();
--for i=1,1000000,1 do

--    math.random();
--end
--print(func_get_longTime()-t);

--***************************************************



--[[



<ui name="1" type="Panel" drag="1" center="1"/>
<ui name="2" type="Label" x="10" y="20" parent="1"/>
<ui name="3" type="Button" x="0" y="50" parent="1"/>


--]]

local function btnClick(btnName,p)
		print(btnName,p);--p = abc
end

local function f_animscHandle(sc)
   -- func_rotate(crl.o, key, sc.value);
	--print(sc.value);
	
	local label = scrollBar_get_param(sc);
	label_set_text(label,sc.value);
end

local function f_callBack(skin)
	--
	--print(skin);
	
	local p = skin_get_param(skin);
	--func_print(string.format("****************[%s]",tostring(p)));
	
	
	local btn = skin_find(skin,"3");
	btn_bindClick(btn,btnClick,"abc");

	local sc = skin_find(skin,"4");
	
	local label = skin_find(skin,"2");
	
	--print(sc,label);
	scrollBar_bind(sc, f_animscHandle,label);
	
	skin_dispose(skin);

end
skin_load("crl.xml",f_callBack,"myParam");

engine_refreshCam3d();