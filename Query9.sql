SELECT STATE.STNAME, --State name
       ( ( ( SUM( POP.POPESTIMATE2011 ) * 1.0 ) / ( NATION.POPESTIMATE2011 * 1.0 )  ) * 100.0 )  PERCENTAGE --Ratio
  FROM HOUSING_UNITS_STATE_LEVEL STATE, 
       POP_ESTIMATE_STATE_AGE_SEX_RACE_ORIGIN POP, 
       POP_ESTIMATE_NATION_STATE_PR NATION
 WHERE POP.STATE = STATE.STATE 
       AND
       POP.SEX = '0' --Want to count only one instance
       AND
       POP.ORIGIN = '0' --Want to count only one instance
       AND
       POP.AGE > '20' --For pop table that has age, was only 21 or older
       AND
       POP.STATE = NATION.STATE
       AND NATION.STATE > '0' --No United States or divisons
       AND NATION.NAME != 'Puerto Rico Commonwealth' --No Puerto Rico
 GROUP BY STATE.STATE; --Group by state
