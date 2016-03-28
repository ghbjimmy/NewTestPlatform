local lapp = require("pl.lapp")

local __parser={};

local args = lapp [[
lua xxx -p xxx.sqlite
    -p,--profile     (default development.sqlite3)   sqlite file to load.
    -u,--uut        (default 1)                     UUT slot number (used for IP/Port selection)
]]

local file = args.profile

local function replaceSpecialChaForDiags(txt)
	if(txt == nil or txt == "")then return nil end
	txt = string.gsub(txt,"\\","\\\\")
	txt = string.gsub(txt,"\"","\\\"")
	-- txt = string.gsub(txt,"\'","\\\'")
	return txt
end

function buildTestItem(tid, row)
-- {tid="A10", name="Measure_BATT_Voltage",      lower=-10,    upper=10,    unit="",   entry=measure,      parameter=VT.PP_BATT_VCC_FB,  visible=1},
	local itemTable = {}
	if(row.tid and row.tid ~= "") then 
		itemTable.tid = row.tid
	else
		itemTable.tid = tid;
	end
	itemTable.description =  row.description--name
	itemTable.low = row.low--low
	itemTable.high = row.high
	itemTable.unit =  row.unit
	itemTable["function"]= row["function"]
	itemTable.param1= row.param1
	itemTable.param2= row.param2
	itemTable.timeout= row.timeout
	itemTable.visible = 1
	itemTable.globalKey = row.globalKey
	itemTable.globalValue = row.globalValue
	return itemTable
end

function buildKeyName(str)--testKey
	return "[" .. str .. "]"
end

--block list table
function buildItem(name, subTable)
	-- local Boot_Item  = {name="[BOOT]", entry=nil,parameter=nil,sub=boot_sub};
	local itemTable = {}
	if(type(subTable)=="table" and #subTable>0) then
		itemTable.name = name
		itemTable.entry = nil
		itemTable.parameter = nil
		itemTable.sub = subTable
	else
		itemTable = nil
	end
	return itemTable
end

--this is just for debug
function printTable(it)
	if(type(it)=="table") then
		local tmp = ""
		for i=1, #it do
			tmp = tmp .. "\t\t{tid="..it[i].tid ..",name="..it[i].name..",lower="..tostring(it[i].lower)..",upper="..tostring(it[i].upper)..",unit="..tostring(it[i].unit)..
			",entry="..it[i].entry..",param1="..tostring(it[i].param1)..",param2="..tostring(it[i].param2)..",pdca_name="..tostring(tostring(it[i].pdca_name))..
			",globalKey="..tostring(tostring(it[i].globalKey))..",globalValue="..tostring(tostring(it[i].globalValue)).."}\r"
		end
		if(#tmp) then
			print(tmp)
		end
	end
end

function __parser.printItems(it)
	for i =1, #it do
		item = it[i]
		local tmp = ""
		tmp = tmp .. "\tname="..tostring(item.name)..",\tentry="..tostring(item.entry)..",\tparameter="..tostring(item.parametr)..",\tsub={"
		print(tmp)
		printTable(item.sub)
		print("\t\t};")
	end
end

-- local items = {
-- 	{name="sub",entry=nil,parameter=nil,sub=
-- 		{
-- 			{tid="A1",name="item1",lower="l1",upper="u1",unit="unit1",entry="entry1”,param1="param1",param2="param2",sub=nil,visible=1,globalKey="globalKey1",globalValue="globalValue1",pdcaName="pdcaName1"},
-- 			{tid="A2",name="item1",lower="l2",upper="u2",unit="unit2",entry="entry2",param1="param2",param2="param2",sub=nil,visible=1,globalKey="globalKey2",globalValue="globalValue2",pdcaName="pdcaName2"},
-- 		}
-- 	},
-- 	{name="sub2",entry=nil,parameter=nil,sub=
-- 		{
-- 			{tid="B1",name="Btem1",lower="BL1",upper="Bu1",unit="Bunit1",entry="Bentry1",parameter={param1="Bparam1",param2="Bparam2"},sub=nil,visible=1,globalKey="BglobalKey1",globalValue="BglobalValue1",pdcaName="BpdcaName1"},
-- 			{tid="B2",name="Btem1",lower="BL2",upper="Bu2",unit="Bunit2",entry="Bentry2",parameter={param1="Bparam2",param2="Bparam2"},sub=nil,visible=1,globalKey="BglobalKey2",globalValue="BglobalValue2",pdcaName="BpdcaName2"},
-- 		}
-- 	},
-- }

function __parser.ParseDB(f)
	print("sqilte file: ", tostring(f))
	local sqlite = require "lsqlite3"
	print(sqlite.version())
	_PDCA_KEY_MATCH_ = "{{.-}}"
	local items = {}
	local db = sqlite.open(f)
	for prow in db:nrows('select * from sequences') do 
		if(prow.name) then 
			print(prow.name.."--------------------")
			Module = prow.name
			local ver = ""
			if(prow.version) then
				ver = prow.version
			end
			if(prow.created_at) then
				ver = ver .. " "..prow.updated_at
			end
			Version = ver
	
			for  subSysRow in db:nrows(string.format('select * from test_groups where sequence_id = "%d"', prow.id)) do--select subsystems 
				if(subSysRow.name) then 
					-- print("\t"..subSysRow.name)
					-- local tidChar = 0x41
					local tidNum = 1
					local SubSysSetupTable = {}
		
					for SubSysSetupRow in db:nrows(string.format('select * from connects where test_group_id = "%d"', subSysRow.id)) do --build subsystem setup
						if(SubSysSetupRow.description) then 
							SubSysSetupRow["function"] = "relay"
							-- print("			" .. SubSysSetupRow.description .. buildLimit(SubSysSetupRow))
							local tid  = tostring(tidNum) -- string.char(tidChar) .. tostring(tidNum);
							table.insert(SubSysSetupTable, buildTestItem(tid, SubSysSetupRow))
							tidNum = tidNum + 1
						end
					end
					local SetUpTmp = buildItem(buildKeyName(subSysRow.name),SubSysSetupTable)
					if(SetUpTmp) then table.insert(items, SetUpTmp) end;
		
					for testBlockRow in db:nrows(string.format('select * from test_blocks where test_group_id = "%d"', subSysRow.id)) do-- test block begin
						if(testBlockRow.name) then 
							local testBlockTable = {}
							for testBlockItemRow in db:nrows(string.format('select * from tests where test_block_id = "%d"', testBlockRow.id)) do--build block test item
								if(testBlockItemRow.description) then 
									-- print("				"..testBlockItemRow.description .. buildLimit(testBlockItemRow))
									local tid  = tostring(tidNum)--string.char(tidChar) .. tostring(tidNum);
									table.insert(testBlockTable, buildTestItem(tid, testBlockItemRow))
									tidNum = tidNum + 1
								end
							end
							local blockTmp = buildItem(buildKeyName(testBlockRow.name), testBlockTable)
							if(blockTmp) then table.insert(items, blockTmp) end
						end
					end
	
					DisconnetTable = {}
					for DisSetupRow in db:nrows(string.format('select * from disconnects where test_group_id = "%d"', subSysRow.id)) do --build subsystem setup
						if(DisSetupRow.description) then 
							DisSetupRow["function"] = "disconnect"
							local tid  = tostring(tidNum);--string.char(tidChar) .. tostring(tidNum);
							table.insert(DisconnetTable, buildTestItem(tid, DisSetupRow))
							tidNum = tidNum + 1
						end
					end
					local DisTmp = buildItem(buildKeyName(subSysRow.name),DisconnetTable)
					if(DisTmp) then table.insert(items, DisTmp) end;
				end
			end
		end
	end
	db:close()
	return items;
end
do return __parser end

-- local it = ParseDB(file);

 -- printItems(it)

-- return it;