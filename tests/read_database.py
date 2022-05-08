import subprocess

# change working dir to /workspaces/DynaMind-ToolBox/build/output/ 
import os
os.chdir("/workspaces/DynaMind-ToolBox/build/output/")

config = {}
config["root_folder"] = "/workspaces/DynaMind-ToolBox/tests/FINAL/workflows"


# open exisiting sqlite3 database
import sqlite3
conn = sqlite3.connect('/workspaces/DynaMind-ToolBox/tests/FINAL/databases/4_FinalResults.sqlite')
c = conn.cursor()

# sqlite3 enable load extension
conn.enable_load_extension(True)
conn.load_extension("/workspaces/DynaMind-ToolBox/build/output/SqliteExtension/libdm_sqlite_plugin.so")

# select all data from table wb_unit_flows
# dm_vector_to_string(pervious_storage)

c.execute("SELECT ogc_fid, wb_soil_id FROM wb_unit_flow")
rows = c.fetchall()
for r in rows:
    print(r)
