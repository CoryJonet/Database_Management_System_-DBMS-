SELECT STATE.STNAME STATE_NAME --State name
FROM HOUSING_UNITS_STATE_LEVEL STATE, POP_ESTIMATE_NATION_STATE_PR NATION
WHERE (((SELECT NATION.POPESTIMATE2011 FROM POP_ESTIMATE_NATION_STATE_PR NATION WHERE NATION.STATE > '0' AND NATION.NAME != 'Puerto Rico Commonwealth') / 
      (SELECT STATE.HUEST_2011)) < 
    ((SELECT SUM(NATION.POPESTIMATE2011)
        FROM POP_ESTIMATE_NATION_STATE_PR NATION WHERE NATION.STATE > '0' AND NATION.NAME != 'Puerto Rico Commonwealth') / 
    (SELECT SUM(STATE.HUEST_2011)
        FROM HOUSING_UNITS_STATE_LEVEL STATE))) --So this is probably confusing but we want the ratio of Popestimate2011 to Huest_2011. No United States only or division and no Puerto Rico
GROUP BY STATE.STNAME; --Group by state name
