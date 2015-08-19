__author__ = 'jbreman'


import MainInterface as MI
import C_UnitType as Unit
import C_ApplianceType as Appliance

demand_data = MI.run(1,1)


def createInputArray():

# create empty array's for each appliance, sum of water use and number of actors


    input_array = MI.initialiseTimeseries(24)               # get the initialised array created in the MainInterface

    # change the key name:
    input_array["R"] = input_array.pop("RESIDENTIAL")       #change key name from 'residential' to 'R'
    input_array["C"] = input_array.pop("COMMERCIAL")        # change key name from 'commercial' to 'C'

    # add in an array for the total volume demand of a unit:
    input_array["C"]["unit_sum"] = []
    input_array["R"]["unit_sum"] = []
    unit_types = ["R","C"]

    # append it with an array of 24 zero's (24hrs in a day)
    for i in unit_types:
        input_array[i]["unit_sum"]=[]

        for j in range(24):
            input_array[i]["unit_sum"].append(0)

    return input_array


def sumVolumes():

    output = createInputArray()         # initialise the empty array
    volumes = demand_data               # grab the volumes

    "sum the volume separately for each appliance and for the whole unit:"
    for u in volumes.keys():            # for each unit
        unit_name =u[0]                       # as key in volumes is R1, R2, C1, C2 etc. Needs to just be R and C to match key in output array

        # add water use volumes to the relevant hour:

        if u[0] == "R":
            appliances = Appliance.residential_appliance_types

        elif u[0] == "C":
            appliances = Appliance.commercial_appliance_types

        for a in appliances:

            for i in range(24):
                time = i
                output[unit_name][a][time] += volumes[u][a][time]       # sum up volumes for each appliance
                output[unit_name]["unit_sum"][time] += volumes[u][a][time] # sum up volumes for the whole unit

        # sum up the number of actors modelled
        output[unit_name]["actors"] += volumes[u]["actors"]

    return output

results = sumVolumes()

print "Residential:"
print "tap", results["R"]["tap"]
print "toilet", results["R"]["toilet"]
print "shower", results["R"]["shower"]
print "washing machine", results["R"]["washing_machine"]
print "bath", results["R"]["bath"]
print "dish washer", results["R"]["dish_washer"]
print "sum", results["R"]["unit_sum"]
print "actors", results["R"]["actors"]
# #
print
#
print "Commercial:"
print "tap", results["C"]["tap"]
print "toilet", results["C"]["toilet"]
print "shower", results["C"]["shower"]
print "dish washer", results["C"]["dish_washer"]
print "sum", results["C"]["unit_sum"]
print "actors", results["C"]["actors"]


# plot(time,toilet_volume, label = "toilet")                                       # plot time vs volume
# plot(time,shower_volume, label = "shower")
# plot(time,washing_machine_vol, label = "washing machine")
# plot(time,dish_washer_vol, label = "dish washer")
# plot(time,tap_volume, label = "taps")
# plot(time,bath_volume, label = "bath")
# plot(time,household_vol, label = "household")
#
# legend(loc = 'upper right')
# xlabel("time")
# ylabel(" volume (L)")
# show()
