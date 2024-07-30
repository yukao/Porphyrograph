
    if (pg_shader_programme[0][_pg_shader_Update]) {
    	pg_allocateBindAndCheckUniform(0,  uniform_Update_scenario_var_data, "uniform_Update_scenario_var_data", _pg_shader_Update);
    }
    if (pg_shader_programme[0][_pg_shader_Master]) {
    	pg_allocateBindAndCheckUniform(0,  uniform_Master_scenario_var_data, "uniform_Master_scenario_var_data", _pg_shader_Master);
    }
    if (pg_shader_programme[0][_pg_shader_ParticleAnimation]) {
    	pg_allocateBindAndCheckUniform(0,  uniform_ParticleAnimation_scenario_var_data, "uniform_ParticleAnimation_scenario_var_data", _pg_shader_ParticleAnimation);
    }
    if (pg_shader_programme[0][_pg_shader_Mixing]) {
    	pg_allocateBindAndCheckUniform(0,  uniform_Mixing_scenario_var_data, "uniform_Mixing_scenario_var_data", _pg_shader_Mixing);
    }
	ParticleAnimation_scenario_var_data[0]  = new float[47];
	Update_scenario_var_data[0]  = new float[81];
	Mixing_scenario_var_data[0]  = new float[9];
	Master_scenario_var_data[0]  = new float[32];
