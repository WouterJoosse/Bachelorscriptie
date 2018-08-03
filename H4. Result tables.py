#%%
import pandas as pd
import numpy as np
import os
import matplotlib.pyplot as plt

matching_type = 'overlijden'
lvl_3_overlijden = pd.read_csv('datafiles/dataframes/working/' + matching_type + '_lvl_3_extended_2.csv', sep="|", index_col=False)
lvl_4_overlijden = pd.read_csv('datafiles/dataframes/working/' + matching_type + '_lvl_4_extended_2.csv', sep="|", index_col=False)
lvl_5_overlijden = pd.read_csv('datafiles/dataframes/working/' + matching_type + '_lvl_5_extended_2.csv', sep="|", index_col=False)

lvl_4_overlijden_accepted_loose = pd.read_csv('datafiles/dataframes/working/' + matching_type + '_extra_lvl_4_loose.csv', sep='|', index_col=False)
lvl_4_overlijden_accepted_strict = pd.read_csv('datafiles/dataframes/working/' + matching_type + '_extra_lvl_4_strict.csv', sep='|', index_col=False)
lvl_5_overlijden_accepted_loose = pd.read_csv('datafiles/dataframes/working/' + matching_type + '_extra_lvl_5_loose.csv', sep='|', index_col=False)
lvl_5_overlijden_accepted_strict = pd.read_csv('datafiles/dataframes/working/' + matching_type + '_extra_lvl_5_strict.csv', sep='|', index_col=False)

matching_type = 'geboorte'
lvl_3_geboorte = pd.read_csv('datafiles/dataframes/working/' + matching_type + '_lvl_3_extended_2.csv', sep="|", index_col=False)
lvl_4_geboorte = pd.read_csv('datafiles/dataframes/working/' + matching_type + '_lvl_4_extended_2.csv', sep="|", index_col=False)
lvl_5_geboorte = pd.read_csv('datafiles/dataframes/working/' + matching_type + '_lvl_5_extended_2.csv', sep="|", index_col=False)

lvl_4_geboorte_accepted_loose = pd.read_csv('datafiles/dataframes/working/' + matching_type + '_extra_lvl_4_loose.csv', sep='|', index_col=False)
lvl_4_geboorte_accepted_strict = pd.read_csv('datafiles/dataframes/working/' + matching_type + '_extra_lvl_4_strict.csv', sep='|', index_col=False)
lvl_5_geboorte_accepted_loose = pd.read_csv('datafiles/dataframes/working/' + matching_type + '_extra_lvl_5_loose.csv', sep='|', index_col=False)
lvl_5_geboorte_accepted_strict = pd.read_csv('datafiles/dataframes/working/' + matching_type + '_extra_lvl_5_strict.csv', sep='|', index_col=False)


matching_type = 'huwelijk'
lvl_3_huwelijk = pd.read_csv('datafiles/dataframes/working/' + matching_type + '_lvl_3_extended_2.csv', sep="|", index_col=False)
lvl_4_huwelijk = pd.read_csv('datafiles/dataframes/working/' + matching_type + '_lvl_4_extended_2.csv', sep="|", index_col=False)
lvl_5_huwelijk = pd.read_csv('datafiles/dataframes/working/' + matching_type + '_lvl_5_extended_2.csv', sep="|", index_col=False)


lvl_4_huwelijk_accepted_loose = pd.read_csv('datafiles/dataframes/working/' + matching_type + '_extra_lvl_4_loose.csv', sep='|', index_col=False)
lvl_4_huwelijk_accepted_strict = pd.read_csv('datafiles/dataframes/working/' + matching_type + '_extra_lvl_4_strict.csv', sep='|', index_col=False)
lvl_5_huwelijk_accepted_loose = pd.read_csv('datafiles/dataframes/working/' + matching_type + '_extra_lvl_5_loose.csv', sep='|', index_col=False)
lvl_5_huwelijk_accepted_strict = pd.read_csv('datafiles/dataframes/working/' + matching_type + '_extra_lvl_5_strict.csv', sep='|', index_col=False)
