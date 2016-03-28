local lapp = require("pl.lapp")

local args = lapp [[
lua xxx -p xxx.sqlite
    
    -p,--profile     (default development.sqlite3)   sqlite file to load.
]]

local file = args.profile
print(file)
local sqlite = require "lsqlite3"
print(sqlite.version())
local items={}
_PDCA_KEY_MATCH_ = "{{.-}}"
local db = sqlite.open(file)

local function replaceSpecialChaForDiags(txt)
	if(txt == nil or txt == "")then return txt end
	txt = string.gsub(txt,"\\","\\\\")
	txt = string.gsub(txt,"\"","\\\"")
	txt = string.gsub(txt,"\'","\\\'")
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
	itemTable.name = row.description

	local upper = row.high
	local lower = row.low
	local unit = row.unit
	if(unit == nil) then unit = "" end
	if(lower == nil) then
		upper = ""
		lower = ""
	else
		upper = tostring(upper)
		lower = tostring(lower)
	end

	itemTable.lower = lower
	itemTable.upper = upper
	itemTable.unit = unit
	itemTable.entry = row["function"]
	itemTable.parameter = "{param1="..tostring(replaceSpecialChaForDiags(row.param1))..",param2="..tostring(replaceSpecialChaForDiags(row.param2)).."}"
	itemTable.visible = 1
	if(row.globalKey and row.globalKey ~= "") then
		local keyNameTmp = string.match(row.globalKey, _PDCA_KEY_MATCH_)
		if(keyNameTmp) then
			itemTable.globalKey = keyNameTmp
		else
			itemTable.globalKey = row.globalKey
		end
		itemTable.globalValue = row.globalValue
	end
	if(row.param2 and row.param2~= "") then 
		local pdcaName = string.match(row.param2, _PDCA_KEY_MATCH_)
		if(pdcaName) then
			itemTable.pdca_name = pdcaName
		end
	end
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
			tmp = tmp .. "\t\t{tid="..it[i].tid ..",\tname="..it[i].name..",\tlower="..tostring(it[i].lower)..",\tupper="..tostring(it[i].upper)..",\tunit="..tostring(it[i].unit)..",\tentry="..it[i].entry.."},\r\n"
		end
		if(#tmp) then
			print(tmp)
		end
	end
end

function printItems(it)
	for i =1, #it do
		item = it[i]
		local tmp = ""
		tmp = tmp .. "\tname="..tostring(item.name)..",\tentry="..tostring(item.entry)..",\tparameter="..tostring(item.parametr)..",\tsub={"
		print(tmp)
		printTable(item.sub)
		print("\t\t};")
	end
end

items = {}
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
				print("\t"..subSysRow.name)
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

-- local function printItems(item)
-- 	for i=1, #item do
-- 		if(type(item[i].sub=="table")) then
-- 			print("------------------\r\n",item[i].name)
-- 			local tbl = item[i].sub
-- 			for j=1, #tbl do
-- 				print(tbl[j].tid,tbl[j].name,tbl[j].lower,tbl[j].upper,tbl[j].unit,tbl[j].entry,tbl[j].parameter.param1,tbl[j].parameter.param2,tbl[j].globalKey, tbl[j].globalValue)
-- 			end
-- 		end
-- 	end
-- end

-- printItems(items)

