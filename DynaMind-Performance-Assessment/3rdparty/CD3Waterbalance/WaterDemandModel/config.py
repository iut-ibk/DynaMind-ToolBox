__author__ = 'JBreman'
__project__ = 'WaterDemandModel'

import os

RAW_FILE_PATH = "C:\Users\Acer\Documents\GitHub\CD3Waterbalance\WaterDemandModel"

RESIDENTIAL_DIURNAL_PATTERN_DATABASE = os.path.join(RAW_FILE_PATH, "csvFiles\ResidentialDiurnalPattern.csv")

RESIDENTIAL_UNIT_SIZE_DATABASE = os.path.join(RAW_FILE_PATH, "csvFiles\ResidentialUnitSize.csv")

RESIDENTIAL_TOILET_FLUSH_FREQ_DATABASE = os.path.join(RAW_FILE_PATH, "csvFiles\ResidentialToiletFlushFrequency.csv")

RESIDENTIAL_SHOWER_FREQUENCY_DATABASE = os.path.join(RAW_FILE_PATH, "csvFiles\ResidentialShowerFrequency.csv")

RESIDENTIAL_SHOWER_DURATION_DATABASE = os.path.join(RAW_FILE_PATH, "csvFiles\ResidentialShowerDuration.csv")

RESIDENTIAL_BATH_FREQUENCY_DATABASE = os.path.join(RAW_FILE_PATH, "csvFiles\ResidentialBathUseFrequency.csv")

RESIDENTIAL_WASH_MACHINE_FREQ_DATABASE = os.path.join(RAW_FILE_PATH, "csvFiles\ResidentialWashMachineFrequency.csv")


COMMERCIAL_DIURNAL_PATTERN_DATABASE = os.path.join(RAW_FILE_PATH, "csvFiles\CommercialDiurnalPattern.csv")

COMMERCIAL_BUILDING_NLA = os.path.join(RAW_FILE_PATH, "csvFiles\CommercialBuildingNLA.csv")

COMMERCIAL_BUILDING_OWP = os.path.join(RAW_FILE_PATH, "csvFiles\CommercialBuilding_m2_per_OWP.csv")

COMMERCIAL_TOILET_FLUSH_FREQ_DATABASE = os.path.join(RAW_FILE_PATH, "csvFiles\CommercialToiletFlushFrequency.csv")

COMMERCIAL_SHOWER_FREQUENCY_DATABASE = os.path.join(RAW_FILE_PATH, "csvFiles\CommercialShowerFrequency.csv")

COMMERCIAL_SHOWER_DURATION_DATABASE = os.path.join(RAW_FILE_PATH, "csvFiles\CommercialShowerDuration.csv")

COMMERCIAL_BATH_FREQUENCY_DATABASE = os.path.join(RAW_FILE_PATH, "csvFiles\CommercialBathUseFrequency.csv")







SHOWER_FLOWRATE_DATABASE = os.path.join(RAW_FILE_PATH, "csvFiles\ShowerFlowRate.csv")

TOILET_FLUSH_VOLUME_DATABASE = os.path.join(RAW_FILE_PATH, "csvFiles\ToiletFlushVolume.csv")

TOP_LOADER_WM_VOLUME_DATABASE = os.path.join(RAW_FILE_PATH, "csvFiles\TopLoadWM_Volume.csv")

FRONT_LOADER_WM_VOLUME_DATABASE = os.path.join(RAW_FILE_PATH, "csvFiles\FrontLoadWM_Volume.csv")

DISH_WASHER_VOLUME_DATABASE = os.path.join(RAW_FILE_PATH, "csvFiles\DishWasherVolume.csv")

TAP_USE_DURATION_DATABASE = os.path.join(RAW_FILE_PATH, "csvFiles\TapUseDuration.csv")

TAP_FLOWRATE_DATABASE = os.path.join(RAW_FILE_PATH, "csvFiles\TapFlowrate.csv")



# Probabilities:

FRACTION_TOP_LOADERS_IN_AUST = 0.64         # Percentage of Australian households with a top loader washing machine. source: http://www.abs.gov.au/ausstats/abs@.nsf/Lookup/2D44AFB5BA8D43ABCA257A670013AFE9


DISTRIBUTIONS = {}
DISTRIBUTIONS["shower_duration"] = [1.84767700916, 0.540286764206]          #[mu,sigma]





