local json = require("dkjson")


print("<   Loading dummy sequence..... >")


local seq={}

local sub_item1={
	{name="TestItem1",entry="dmm",parar1="",para2=""},
	{name="TestItem2",entry="dmm",parar1="",para2=""},
	{name="TestItem3",entry="dmm",parar1="",para2=""}
}

local items={
	{name="MainItem1",entry="",sub=sub_item1}
}


function seq.get_item(index)
	local t = sub_item1[index];
	if (not t) then
		return nil;
	else
		local str = json.encode(t);
		str = str:gsub("(\"(entry)\"","\"function\"");
		return str
	end
end

return seq;
