# -*- coding: utf-8 -*-
"""
Created on Mon Nov  5 14:40:48 2012

@author: christianurich
"""



import unittest
from pydynamind import *
from pydmtoolbox import *
import pydmextensions

class TestSequenceFunctions(unittest.TestCase):

    def setUp(self):
       self.sys = System()
       n1 = self.sys.addNode(1,1,0)
       n2 = self.sys.addNode(2,1,0)
       n3 = self.sys.addNode(2,2,0)
       n4 = self.sys.addNode(1,2,0)
       nodelist = nodevector()
       nodelist.append(n1)
       nodelist.append(n2)
       nodelist.append(n3)
       nodelist.append(n4)
       nodelist.append(n1)
       self.f = self.sys.addFace(nodelist)
    def test_calculateArea(self):
        # make sure the shuffled sequence does not lose any elements
        area = TBVectorData_CalculateArea(self.sys, self.f)
        self.assertEqual(area, 1.0)
        
    def test_tringulation(self):
        triangles = pydmextensions.CGALGeometry_FaceTriangulation(self.sys, self.f)
        area = 0;
        for i in range(len(triangles)/3):
            nodelist = nodevector()
            for j in range(3):
                n1 = triangles[i*3+j]
                n = self.sys.addNode(n1.getX(), n1.getY(), n1.getZ())
                nodelist.append(n)
            nodelist.push_back(nodelist[0])
            self.sys.addFace()
            f = self.sys.addFace(nodelist)
            area+= TBVectorData_CalculateArea(self.sys, f)
        self.assertEqual(area, 1.0)

if __name__ == '__main__':
    unittest.main()