--�����Զ���ĸ�����
local function f_set_customParent(skin,parent,x,y)
	skin.customParent = parent;
	skin.ox = x or 0;
	skin.oy = y or 0;
end


--��ȡ���׽ڵ�
local function f_get_parent(list,node)
	local parentName = xml_get_str(node,"parent");
	if(parentName ~= "") then
		local fnode	 = stack_find_by_name(list,parentName);
		return func_get_container(fnode);
	end
	return nil;
end

--parent:����������
local function f_itemskin_load(xmlurl,parent,x,y)
	local skin = skin_load(xmlurl);
	f_set_customParent(skin,parent,x,y);
	skin_parse(skin);
	return skin;
end

local function f_create_by_node(skin,node,name,_type)
	local list = skin.list;
	local parent = nil;
	
	if(skin.customParent ~= nil) then
		--ʹ���Զ���ĸ���������
		parent = skin.customParent;
	else
		--ʹ�÷��Զ���ĸ�������ʱ��,�ҵ�������ĸ�����
		parent = f_get_parent(list,node);
	end
	
	local x = xml_get_float(node,"x");
	local y = xml_get_float(node,"y");
	--print("�������=============>",x,y,"name=",name,_type);
	
	x = x + skin.ox or 0;
	y = y + skin.oy or 0;

	
	if(_type == "Panel") then																													
		
		local a = alert_init();
		a.name = name;
		
		local center = xml_get_float(node,"center");
		local drag = xml_get_float(node,"drag");
		local width = xml_get_float(node,"width");
		local height = xml_get_float(node,"height");
		
		alert_start(a,nil,width,height);
		if(drag == 1)then
			alert_set_drag(a,true);
		end
		if(center == 1) then
			alert_enable_resize(a,true);
		end
		
		stack_push(list,a);
	elseif(_type == "Label") then
		local label =  label_create();
		label.name = name;
		local str = xml_get_str(node,"label");
		label_set_text(label, str);
		
		func_addnode(parent,label,x,y);
		
		stack_push(list,label);
	elseif(_type == "Button") then
		
		local btn = btn_create();
		btn.name = name;

		local str = xml_get_str(node,"label");
		if(str~="")then
			btn_label(btn,str);
		end
		func_addnode(parent,btn,x,y);
		stack_push(list,btn);
		
	elseif(_type == "ScrollBar") then
		local sc = scrollBar_new();
		sc.name = name;
		func_addnode(parent,sc,x,y);
		stack_push(list,sc);
	elseif(_type == "Skin") then
		--string.format("%s")
		local url = xml_get_str(node,"url");
		local itemSkin = f_itemskin_load(url,parent,x,y);
		stack_push(list,itemSkin);
	end
	--print("====>",parent);
end

function skin_parse(skin)
	
	local ins = skin;
	local xml = xml_load(ins.url);

	local cnt = xml_get_length(xml);

	--print(cnt);

	local n = 0;
	for n = 0,cnt-1 do
		local node = xml_get_node_by_index(xml,n);
		local name = xml_get_str(node,"name");
		local _type = xml_get_str(node,"type");
		--print("n = ",n,name,_type);
		f_create_by_node(ins,node,name,_type);
	end
	xml_del(xml);
	
	if(ins.completeCallBack) then
		ins.completeCallBack(ins);
	end
end

local function f_tex_complete(param)
	skin_parse(param);
end
--��ȡ��ǰƤ������Ĳ���
function skin_get_param(skin)
	return skin.param;
end

--����skin����е�name��ȡ���
function skin_find(skin,name)
	local list = skin.list;
	return stack_find_by_name(list,name);
end
--����xml���ؽ���skin,�����ɹ�֮��ص�completeCallBack
--���completeCallBack������ֵ,�ǲ������첽�ص���
function skin_load(url,
	completeCallBack,param,texs)
	
	local ins = {
		type = 5;
		container = nil;--��ǰskin������
		customParent = nil;--�Զ���һ��������
		--ƫ������
		ox = 0;
		oy = 0;
		
		list = nil;--ջ����
		url = nil;--xml url
		completeCallBack = nil;--������ɻص�
		
		param = nil;
	};
	ins.url = string.format("\\resource\\%s",url);
	
	ins.list = stack_new();
	
	if(completeCallBack) then 
		ins.completeCallBack = completeCallBack;
		ins.param = param;
		loadtexs(texs or "gundi.png;checkbox.png",f_tex_complete,ins);
	end
	return ins;
end

local function f_delAll(n)
	func_dispose(n);
end	
--����Ƥ�����
function skin_dispose(skin)
	--����Ƥ�����
	stack_foreach(skin.list,f_delAll,nil,true);
	
	
	stack_del(skin.list);
	func_clearTableItem(skin);
end