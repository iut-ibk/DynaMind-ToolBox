# -*- coding: utf-8 -*-
"""
Created on Tue Jan 06 12:27:02 2015

@author: Acer
"""

from Global_meaning_list import Global_meaning_list

def Purpose_Finder_Collectors(word):
    for i in range(len(Global_meaning_list.collectors)):
        if word in Global_meaning_list.collectors[i]:
            print Global_meaning_list.collectors[i][1]
        else:
            pass
            

Purpose_Finder_Collectors('Collector_116')

