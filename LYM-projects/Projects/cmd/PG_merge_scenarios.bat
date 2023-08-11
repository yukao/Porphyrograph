rem if opt in ("-i", "--input_scenes_file_name"):
rem elif opt in ("-f", "--input_footer_scenario_file_name"):
rem elif opt in ("-F", "--input_full_scenario_file_name"):
rem elif opt in ("-o", "--output_scenario_file_name"):

python.exe PG_merge_scenarios-v1.py -i ../scenarios/LYM_file_scenes-v6.csv -f ../scenarios/YN_Argenteuil_danse-scenario-v1.csv -F ../scenarios/YN_Argenteuil_danse-scenario-v1.csv -o ../scenarios/YN_Argenteuil_danse-scenario-v1-new.csv
