SELECT METRO.LSAD AREA_NAME, --All "metropolitan statistical areas"
       METRO.NETMIG2011 NET_MIGRATION --Net migration from those LSADs
  FROM POP_ESTIMATE_METRO_MICRO METRO;
