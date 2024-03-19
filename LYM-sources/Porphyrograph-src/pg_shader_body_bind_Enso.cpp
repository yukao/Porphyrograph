  pg_allocateBindAndCheckUniform(indConfig, &uniform_Update_scenario_var_data, "uniform_Update_scenario_var_data", _pg_shader_Update);
  pg_allocateBindAndCheckUniform(indConfig, &uniform_Master_scenario_var_data, "uniform_Master_scenario_var_data", _pg_shader_Master);
  pg_allocateBindAndCheckUniform(indConfig, &uniform_Mixing_scenario_var_data, "uniform_Mixing_scenario_var_data", _pg_shader_Mixing);
  pg_allocateBindAndCheckUniform(indConfig, &uniform_ParticleAnimation_scenario_var_data, "uniform_ParticleAnimation_scenario_var_data", _pg_shader_ParticleAnimation);
  ParticleAnimation_scenario_var_data[indConfig] = new float[46];
  Update_scenario_var_data[indConfig] = new float[67];
  Mixing_scenario_var_data[indConfig] = new float[8];
  Master_scenario_var_data[indConfig] = new float[24];
