import subprocess

# change working dir to /workspaces/DynaMind-ToolBox/build/output/ 
import os
os.chdir("/workspaces/DynaMind-ToolBox/build/output/")

config = {}
config["root_folder"] = "/workspaces/DynaMind-ToolBox/tests/FINAL/workflows"

# compile json file to dynamind

from scenario_tool_interface import Json2DynaMindXML

# Json2DynaMindXML().dump("FINAL/workflows/1_import_soil_paramters.json", "FINAL/workflows/1_import_soil_paramters.xml")
# Json2DynaMindXML().dump("FINAL/workflows/2_import_et.json", "FINAL/workflows/2_import_et.xml")
# Json2DynaMindXML().dump("FINAL/workflows/3_import_irrigation.json", "FINAL/workflows/3_import_irrigation.xml")
# Json2DynaMindXML().dump("FINAL/workflows/4_basic.json", "FINAL/workflows/4_basic.xml")

def compile_file(file_name):
    print("compiling file: " + file_name)
    Json2DynaMindXML().dump(f"{config['root_folder']}/{file_name}.json", f"{config['root_folder']}/compiled/{file_name}.xml",)
    return f"{config['root_folder']}/compiled/{file_name}.xml"


def run_file(name, simulation):

    # remove file /workspaces/DynaMind-ToolBox/tests/FINAL/workflows/outputs/wb_ress.dat if exists
    try:
        os.remove("/workspaces/DynaMind-ToolBox/tests/FINAL/workflows/outputs/wb_ress.dat")
    except OSError:
        pass

    print(f'{config["root_folder"]}/logs/')
    command = ['/workspaces/DynaMind-ToolBox/build/output/dynamind',simulation, '--logpath', f'{config["root_folder"]}/logs/{name}.log']
    
    # join command to string
    print(' '.join(command))

    p = subprocess.run(command)



workflow = ["1_import_soil_paramters", "2_import_et", "3_import_irrigation", "4_basic"]
workflow = [ "4_basic"]

for w in workflow:
    print("running workflow: ", w)
    run_file(w, compile_file(w))
