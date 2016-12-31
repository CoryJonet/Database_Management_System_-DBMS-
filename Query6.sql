SELECT POP.AGE, --Woman's age
       POP.POPESTIMATE2011 POPULATION --Population for a given woman's age
  FROM SEX S, 
       POP_ESTIMATE_STATE_AGE_SEX_RACE_ORIGIN POP, 
       POP_ESTIMATE_NATION_STATE_PR NATION
 WHERE S.SEX_DESC = 'Female' --Want females only
       AND
       S.SEX_CD = POP.SEX --Join tables needed
       AND
       NATION.STATE > '0' --No United States only or divisions
       AND
       NATION.NAME != 'Puerto Rico Commonwealth'  --No Puerto Rico
       AND
       NATION.STATE = POP.STATE 
       AND
       POP.AGE < '46' --Woman's ages 46 or greater thrown out
       AND
       POP.AGE > '20'; --Woman's ages 20 or less thrown out
