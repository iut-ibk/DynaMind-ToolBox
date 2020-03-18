

Lot
===

Parameters
----------


-------
Streams
-------

Internal
--------
- _grey_water
- _black_water
- _roof_runoff
- _pervious_runoff
- _impervious_runoff
- _potable_demand
- _non_potable_demand

Configuration internal streams

.. code-block:: python

   "storages":
            [{"inflow": "_roof_runoff", "demand": "_non_potable_demand", "volume": 5},
             {"inflow": "_grey_water", "demand": "_non_potable_demand", "volume": 0}]


External
--------
- potable_demand
- non_potable_demand
- storm_water_runoff
- sewerage
- grey_water

Group elements to districts
----------------------------

Based on SA1 data

