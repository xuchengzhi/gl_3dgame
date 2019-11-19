local g_gap = 15--(一行的高度间隔)
local g_width = 60 --(一行的宽度)

local r = 1.0;
local g = 0.0;
local b = 0.0;

local function f_create() 
	local listBox=
	{
	
		name,
		type = 8,
        --*******************************************
		b_drop = false,--是否是下拉着的
		
		tflist={},--textFiled列表
		
		bg=nil,--spirte,容器
		
		callBack = nil,--选择回调
		param;
		
		x=0,y=0,
		
		index = 0,--选择的索引0开始,默认初始化为0
		
		--tf,	--文本
		
		titletf,--列表标题文本
		
		data;--数据
	};
	return listBox
end

--显示隐藏文本
local function f_tf_vis(list)
	local v = list.b_drop
	for key, value in pairs(list.tflist) do      
		local tf =  value;
		label_set_visible(tf,v);
	end 
end
--获取索引(-1开始)
local function f_get_index(list)
	local x,y = func_get_sprite_mouse_xy(list.bg);
	local f = y / g_gap;	
	return math.floor(f) - 1;
end
function listbox_select(list,n)
	if(n >= 0) then
		list.index = n
		if(list.callBack) then
			--print(list.index);
			local label = nil;
			if(list.data) then
				label = list.data[list.index + 1];
			end
            list.callBack(list,list.index,label,list.param);
        end
	end
end

function listbox_get_container(list)
	return list.bg;
end

local function f_select_call(list)
	list.b_drop = not list.b_drop
	local height = g_gap
	f_tf_vis(list)
	
	if(list.b_drop) then
		--展开
		height = (func_get_table_count(list.tflist) + 1) * g_gap
	end
	
	func_set_sprite_size(list.bg,g_width,height)--重绘背景宽高
	--设置点击区域范围
	sprite_set_hit_rect(list.bg,0,0,g_width,height);

	if(list.b_drop == false) then
        listbox_select(list,f_get_index(list));
    end
end

--获取其索引
function listbox_get_index(list)
	return list.index
end

function listbox_get_param(list)
	return list.param;
end
local function f_click(name,param)

	local list = param;
	f_select_call(list);
end
function listbox_new(_x,_y)
	_x = _x or 0
	
	_y = _y or 0
	
	local list = f_create()
	
	list.x = _x
	list.y = _y
	
	list.bg = func_create_grid_sprite(_x,_y,g_width,g_gap,"checkbox.png");--gundi.png
	
	evt_on(list.bg,EVENT_ENGINE_SPRITE_CLICK,f_click,list);

    return list
end

--绑定一个回调函数
function 
listbox_bind(list,callBack,param)
	list.callBack = callBack;
	list.param = param;
end

--获取选择的文本字符
function listbox_get_select_label(list)
	local n = listbox_get_index(list);
    return label_get_text(list.tflist[n]);
end

--增加一个节点
function 
listbox_add(list,str)
	local count =	func_get_table_count(list.tflist) + 1;	
	local tf = label_create();
	list.tflist[count - 1] = tf;
	label_set_visible(tf,0);
	label_set_text(tf,str);
	func_addnode(list.bg,tf,0,g_gap*(count));

	if(list.data==nil)then
		list.data = {};
	end
	list.data[#list.data+1]=str;
	--print(#list.data);
end

--为列表选项卡设置一个标题
function listbox_set_title(self,str)
	if(self.titletf == nil) then
		local tf = label_create();
		--label_set_pos(tf, self.x + g_width, self.y);
		func_addnode(self.bg,tf,0, 0);
		self.titletf = tf;
	end
	label_set_text(self.titletf, str);
end

--销毁listbox组件
function 
listbox_del(list)
	evt_off(list.bg,EVENT_ENGINE_SPRITE_CLICK,f_click);

	ptr_remove(list.bg)

	for key, value in pairs(list.tflist) do
		label_dispose(value)
	end
	--ptr_remove(list.tf)
	
	--if(list.tf) then 
	--	label_dispose(list.tf); 
	--end
	
	if(list.titletf)then
		label_dispose(list.titletf); 
	end
	
	if(list.data) then
		func_clearTableItem(list.data);
	end
	
	func_clearTableItem(list.tflist)
	func_clearTableItem(list)
end
