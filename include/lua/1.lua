--md2动作控制器管理
dofile("..\\include\\lua\\core.lua")

print('md2动作控制')

local function getPath(name)
	return "\\resource\\md2\\"..name..".md2"
end

local function 
f_load(line)
	line = line or false
	local md2
	local name = "horse";
	--local md2=load_md2vbo_byName(name,func_get_default_tex())
	
	--local tex = func_get_default_tex()
	
	--local md2 = func_loadmd2(name,tex)
	
	--vbo
	md2=load_VBO_model(name,getPath(name));
	--非vbo要去检查mat是否是支持非vbo的材质
	--md2 = load_model(name,getPath(name));	
	
	setMaterial(md2,func_load("//resource//material//horse.mat"));
	
	setv(md2,FLAGS_VISIBLE);

	setv(md2,FLAGS_RAY);
	if(line)then
		setv(md2,FLAGS_DRAW_PLOYGON_LINE);--设置线框渲染
	end
	setv(md2,FLAGS_DRAW_RAY_COLLISION);
	change_attr(md2,"collide_load","\\resource\\md2\\"..name..".md2");
	
	--设置旋转轴
	func_setRotateX(md2,PI/2);
	
	--	"stand,0,39@run,40,45@attack,46,53@pain,54,65@jump,66,71@filp,72,83@salute,84,94@taunt,95,111@dead,178,197@"
	--切割一个动作
	
	func_anim_push(md2,"stand",0,39);
	func_anim_push(md2,"run",40,45);
	func_anim_push(md2,"attack",45,53);
	func_anim_push(md2,"jump",66,71);
	func_anim_push(md2,"crwalk",153,159);

	
	--设置初始动作
	change_attr(md2,"animtor_setcur","jump");
	
	--change_attr(md2,"animtor_pause");--暂停
	change_attr(md2,"animtor_play");--播放
	
	return md2;
end

--动作控制器
local function 
f_anim_control_ui(m)
	local animsc = scrollBar_new(0,0)
	scrollBar_setRange(animsc,0,197)
	local animTF = scrollBar_add_text(animsc,'anim')
	scrollBar_bind(animsc,
		function(sc)
			local v = string.format('%.0f',sc.value)
			func_play_anim(m,v,v)
			--func_setRotateX(func_find_obj(ModelName),sc.value)
			tf_setText(animTF,'anim '..v)
		end
	)
	
	local list =  listbox_new(0,15)
	listbox_add(list,"stand")
	listbox_add(list,"run")
	listbox_add(list,"attack")
	listbox_add(list,"jump")
	listbox_add(list,"crwalk")

	listbox_bind(list,
		function(_l)
			
			local label = listbox_get_label(_l)
			func_set_anim(m,label)
			--string.format('%s vbo=%s',label,tostring(func_is_vbo(obj)) )
			listbox_set_label(_l,label)

		end
	)
	listbox_select(list,0)--默认选0号索引
end
---------------------------------------------------------------------------
func_set_camera_pos(0,0,-90)
f_anim_control_ui(f_load())