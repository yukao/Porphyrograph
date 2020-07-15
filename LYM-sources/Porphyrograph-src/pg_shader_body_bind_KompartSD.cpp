  uniform_Update_scenario_var_data = glGetUniformLocation(shader_programme[pg_shader_Update], "uniform_Update_scenario_var_data");
  if ( uniform_Update_scenario_var_data == -1 ) {
    fprintf(stderr, "Could not bind uniform Update uniform_Update_scenario_var_data : %d\n" , uniform_Update_scenario_var_data );
  }
  uniform_Master_scenario_var_data = glGetUniformLocation(shader_programme[pg_shader_Master], "uniform_Master_scenario_var_data");
  if ( uniform_Master_scenario_var_data == -1 ) {
    fprintf(stderr, "Could not bind uniform Master uniform_Master_scenario_var_data : %d\n" , uniform_Master_scenario_var_data );
  }
  uniform_Mixing_scenario_var_data = glGetUniformLocation(shader_programme[pg_shader_Mixing], "uniform_Mixing_scenario_var_data");
  if ( uniform_Mixing_scenario_var_data == -1 ) {
    fprintf(stderr, "Could not bind uniform Mixing uniform_Mixing_scenario_var_data : %d\n" , uniform_Mixing_scenario_var_data );
  }
