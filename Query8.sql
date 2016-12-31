SELECT DIV.DIVISION_DESC DIVISION_NAME, --Division name
       HOUSING.STNAME STATE_NAME, --State name
       MAX(HOUSING.HUEST_2011 - HOUSING.HUEST_2010)  INCREASE --Greatest increase in housing estimates
  FROM DIVISION DIV, 
       HOUSING_UNITS_STATE_LEVEL HOUSING
 WHERE DIV.DIVISION_CD = HOUSING.DIVISION --Join needed tables
 GROUP BY DIV.DIVISION_DESC; --Group by division names
 