import sys
import logging
sys.path.insert(0, "/Users/christianurich/Documents/dynamind/build/output/")


logging.basicConfig(level=logging.INFO)
import time
from wbmodel import WaterCycleModel




for i in [10]:
    start = time.time()
    WaterCycleModel(i)
    end = time.time()
    logging.info(str(end - start))

