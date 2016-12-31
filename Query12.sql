SELECT STATE.AGE AGE, --Age by state
       SUM( ABS( STATE.POPESTIMATE2011 - STATE.POPESTIMATE2010 )  )  DIFFERENCE, --Absolute difference between population counts
       CASE( STATE.POPESTIMATE2011 - STATE.POPESTIMATE2010 ) --Case statement for indicator
            WHEN( ( STATE.POPESTIMATE2011 - STATE.POPESTIMATE2010 )  ) > '0' THEN 'INCREASED' --Positive
            WHEN( ( STATE.POPESTIMATE2011 - STATE.POPESTIMATE2010 )  ) = '0' THEN 'SAME' --Equal
            ELSE 'DECREASED' --Otherwise negative
       END INDICATOR
  FROM POP_ESTIMATE_STATE_AGE_SEX_RACE_ORIGIN STATE
  WHERE STATE.SEX = '0' AND STATE.ORIGIN = '0' --Count only once
 GROUP BY STATE.AGE; --Group by age
