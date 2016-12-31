SELECT div.division_desc, COUNTY.STNAME, --County's state name
       COUNTY.CTYNAME COUNTY, --County name
       MIN( COUNTY.NPOPCHG_2012 )  POPULATION_CHANGE --Minimum population change
  FROM POP_ESTIMATE_STATE_COUNTY COUNTY, 
       DIVISION DIV
 WHERE COUNTY.STNAME != COUNTY.CTYNAME --No state = county names columns
       AND
       ( COUNTY.NPOPCHG_2012 > 0 ) --Want MIN() to choose only positive min
       AND
       DIV.DIVISION_CD = COUNTY.DIVISION --Join tables needed
 GROUP BY COUNTY.STNAME
 ORDER BY DIV.DIVISION_DESC ASC, --Divisions in lexicographic order
           COUNTY.POPESTIMATE2012 DESC; --Popestimate2012 in decreasing order
