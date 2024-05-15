
	pg_allocateBindAndCheckUniform(0,  uniform_Update_scenario_var_data, "uniform_Update_scenario_var_data", _pg_shader_Update);
	pg_allocateBindAndCheckUniform(0,  uniform_Master_scenario_var_data, "uniform_Master_scenario_var_data", _pg_shader_Master);
	pg_allocateBindAndCheckUniform(0,  uniform_Mixing_scenario_var_data, "uniform_Mixing_scenario_var_data", _pg_shader_Mixing);
	pg_allocateBindAndCheckUniform(0,  uniform_ParticleAnimation_scenario_var_data, "uniform_ParticleAnimation_scenario_var_data", _pg_shader_ParticleAnimation);
	ParticleAnimation_scenario_var_data[0]  = new float[47];
	Update_scenario_var_data[0]  = new float[82];
	Mixing_scenario_var_data[0]  = new float[8];
	Master_scenario_var_data[0]  = new float[33];
