--插件接口定义
IPlugin = {
	
};

IPlugin.__index = IPlugin;

--插件加载
function IPlugin:new()
	func_error("IPlugin:new()接口未实现");
end

--插件卸载,此接口会卸载并销毁该插件所有的资源
function IPlugin:dispose()
	func_error("IPlugin:dispose()接口未实现");
end

function IPlugin:getName()
	func_error("IPlugin:getName()接口未实现");	
end