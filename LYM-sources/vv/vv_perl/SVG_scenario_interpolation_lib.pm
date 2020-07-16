package SVG_scenario_interpolation_lib;

##################################################################
# COLOR MIXTURE
##################################################################
sub mix_color {
    my ($initial_color, $target_color, $local_percentage) = @_;
    my $target_red = '0x' . substr $target_color , 0 , 2;
    my $target_green = '0x' . substr $target_color , 2 , 2;
    my $target_blue = '0x' . substr $target_color , 4 , 2;
    my $initial_red = '0x' . substr $initial_color , 0 , 2;
    my $initial_green = '0x' . substr $initial_color , 2 , 2;
    my $initial_blue = '0x' . substr $initial_color , 4 , 2;
    my $mix_red = (1 - $local_percentage) * (eval $initial_red) + $local_percentage * (eval $target_red);
    my $mix_green = (1 - $local_percentage) * (eval $initial_green) + $local_percentage * (eval $target_green);
    my $mix_blue = (1 - $local_percentage) * (eval $initial_blue) + $local_percentage * (eval $target_blue);
    $mix_red = sprintf("%02X" , $mix_red);
    $mix_green = sprintf("%02X" , $mix_green);
    $mix_blue = sprintf("%02X" , $mix_blue);
    my $mixed_color = $mix_red . $mix_green . $mix_blue;
    # print "target_color initial_color local_percentage: $target_color, $initial_color, $local_percentage / mixed_color: $mixed_color \n";
    return $mixed_color;
}

##################################################################
# VALUES INTERPOLATION
##################################################################
sub win_chomp {
    my $loc_chain = shift;
    $loc_chain =~ s/\r?\n$//;
    return $loc_chain;
}

##################################################################
# VALUES INTERPOLATION
##################################################################
sub clamp {
    my ($value) = @_;
    if($value > 1) {
        $value = 1;
    }
    elsif($value < 0.0) {
        $value = 0;
    }
    return $value;
}

sub Bezier_01_01 {
    my ($value) = @_;
    my @P0 = (0,0);
    my @P1 = (1,0);
    my @P2 = (0,1);
    my @P3 = (1,1);
    my $return;
    if($value > 1) {
        $return = 1;
    }
    elsif($value < 0.0) {
        $return = 0;
    }
    else {
        $return = ((1-$value) ** 3) * $P0[1] + 3 * $value * ((1-$value) ** 2) * $P1[1] + 3 * ($value ** 2) * (1-$value) * $P2[1] + ($value ** 3) * $P3[1]; 
    }
    return $return;
}

sub Bezier_steep_01_01 {
    my ($value) = @_;
    my @P0 = (0,0);
    my @P1 = (0,1);
    my @P2 = (1,0);
    my @P3 = (1,1);
    my $return;
    if($value > 1) {
        $return = 1;
    }
    elsif($value < 0.0) {
        $return = 0;
    }
    else {
        $return = ((1-$value) ** 3) * $P0[1] + 3 * $value * ((1-$value) ** 2) * $P1[1] + 3 * ($value ** 2) * (1-$value) * $P2[1] + ($value ** 3) * $P3[1]; 
    }
    return $return;
}

sub Gaussian_01_01 { # double Bezier
    my ($value) = @_;
    my @P0 = (0,0);
    my @P1 = (1,0);
    my @P2 = (0,1);
    my @P3 = (1,1);
    my $return;
    if($value > 1) {
        $return = 1;
    }
    elsif($value < 0.0) {
        $return = 0;
    }
    else {
      if($value <= 0.5) {
        $value = 2 * $value;
        $return = ((1-$value) ** 3) * $P0[1] + 3 * $value * ((1-$value) ** 2) * $P1[1] + 3 * ($value ** 2) * (1-$value) * $P2[1] + ($value ** 3) * $P3[1]; 
      }
      else {
        $value = 2 * (1 - $value);
        $return = ((1-$value) ** 3) * $P0[1] + 3 * $value * ((1-$value) ** 2) * $P1[1] + 3 * ($value ** 2) * (1-$value) * $P2[1] + ($value ** 3) * $P3[1]; 
      }
    }
    return $return;
}

sub Gaussian_steep_01_01 { # double Bezier
    my ($value) = @_;
    my @P0 = (0,0);
    my @P1 = (0,1);
    my @P2 = (0,1);
    my @P3 = (1,1);
    my $return;
    if($value > 1) {
        $return = 1;
    }
    elsif($value < 0.0) {
        $return = 0;
    }
    else {
      if($value <= 0.5) {
        $value = 2 * $value;
        $return = ((1-$value) ** 3) * $P0[1] + 3 * $value * ((1-$value) ** 2) * $P1[1] + 3 * ($value ** 2) * (1-$value) * $P2[1] + ($value ** 3) * $P3[1]; 
      }
      else {
        $value = 2 * (1 - $value);
        $return = ((1-$value) ** 3) * $P0[1] + 3 * $value * ((1-$value) ** 2) * $P1[1] + 3 * ($value ** 2) * (1-$value) * $P2[1] + ($value ** 3) * $P3[1]; 
      }
    }
    return $return;
}

sub NonCentered_Gaussian_01_01 { # double Bezier
    my ($value,$curveCenter) = @_;
    my @P0 = (0,0);
    my @P1 = (1,0);
    my @P2 = (0,1);
    my @P3 = (1,1);
    my $return;
    if($value > 1) {
        $return = 1;
    }
    elsif($value < 0.0) {
        $return = 0;
    }
    elsif($curveCenter <= 0.0) {
        $return = Bezier_01_01(1 - $value);
    }
    elsif($curveCenter >= 1.0) {
        $return = Bezier_01_01($value);
    }
    else {
      if($value <= $curveCenter) {
        $value = $value/$curveCenter;
        $return = ((1-$value) ** 3) * $P0[1] + 3 * $value * ((1-$value) ** 2) * $P1[1] + 3 * ($value ** 2) * (1-$value) * $P2[1] + ($value ** 3) * $P3[1]; 
      }
      else {
        $value = (1 - $value)/(1 - $curveCenter);
        $return = ((1-$value) ** 3) * $P0[1] + 3 * $value * ((1-$value) ** 2) * $P1[1] + 3 * ($value ** 2) * (1-$value) * $P2[1] + ($value ** 3) * $P3[1]; 
      }
    }
    return $return;
}

# calculation of interpolation value
sub interpolated_value_calculation {
  my ($local_var_type, $local_var_init, $local_var_fin, $local_interpolator, $local_scene_percentage)= @_;
  my $interpolated_value;

  # extraction of initial and final color in case of color interpolation
  my $color_initial;
  my $color_final;
  if($local_var_type eq "hexcolor") {
      # color calculation as an interpolation between 2 colors with a blending coef
      $color_initial = $local_var_init;
      $color_initial =~ s/^\#//s; # removes 1st char
      $color_final = $local_var_fin;
      $color_final =~ s/^\#//s; # removes 1st char
  }

  #interpolation calculation
  if($local_interpolator eq "l") {
    if($local_var_type eq "float") {
      $interpolated_value = $local_var_init
          + $local_scene_percentage * ($local_var_fin - $local_var_init);
    }
    elsif($local_var_type eq "hexcolor") {
      # color calculation as an interpolation between 2 colors with a blending coef
      $interpolated_value = '#' . mix_color($color_initial, $color_final, $local_scene_percentage);
    }
    elsif($local_var_type eq "int") {
      # color calculation as an interpolation between 2 colors with a blending coef
      $interpolated_value = int($local_var_init
          + $local_scene_percentage * ($local_var_fin - $local_var_init));
    }
  }
  # Bezier smooth 00 01 10 11
  elsif($local_interpolator eq "b") {
    my $local_percentage = Bezier_01_01($local_scene_percentage);
    if($local_var_type eq "float") {
      $interpolated_value = $local_var_init
          + $local_percentage * ($local_var_fin - $local_var_init);
    }
    elsif($local_var_type eq "hexcolor") {
      # color calculation as an interpolation between 2 colors with a blending coef
      $interpolated_value = '#' . mix_color($color_initial, $color_final, $local_percentage);
    }
    elsif($local_var_type eq "int") {
      # color calculation as an interpolation between 2 colors with a blending coef
      $interpolated_value = int($local_var_init
          + $local_scene_percentage * ($local_var_fin - $local_var_init));
    }
  }
  # Bezier steep 00 10 01 11
  elsif($local_interpolator eq "t") {
    my $local_percentage = Bezier_steep_01_01($local_scene_percentage);
    if($local_var_type eq "float") {
      $interpolated_value = $local_var_init
          + $local_percentage * ($local_var_fin - $local_var_init);
    }
    elsif($local_var_type eq "hexcolor") {
      # color calculation as an interpolation between 2 colors with a blending coef
      $interpolated_value = '#' . mix_color($color_initial, $color_final, $local_percentage);
    }
    elsif($local_var_type eq "int") {
      # color calculation as an interpolation between 2 colors with a blending coef
      $interpolated_value = int($local_var_init
          + $local_scene_percentage * ($local_var_fin - $local_var_init));
    }
  }
  # Gaussian
  elsif($local_interpolator eq "g") {
    my $local_percentage = Gaussian_01_01($local_scene_percentage);
    if($local_var_type eq "float") {
      $interpolated_value = $local_var_init
          + $local_percentage * ($local_var_fin - $local_var_init);
    }
    elsif($local_var_type eq "hexcolor") {
      # color calculation as an interpolation between 2 colors with a blending coef
      $interpolated_value = '#' . mix_color($color_initial, $color_final, $local_percentage);
    }
    elsif($local_var_type eq "int") {
      # color calculation as an interpolation between 2 colors with a blending coef
      $interpolated_value = int($local_var_init
          + $local_scene_percentage * ($local_var_fin - $local_var_init));
    }
  }
  # Gaussian steep
  elsif($local_interpolator eq "h") {
    my $local_percentage = Gaussian_steep_01_01($local_scene_percentage);
    if($local_var_type eq "float") {
      $interpolated_value = $local_var_init
          + $local_percentage * ($local_var_fin - $local_var_init);
    }
    elsif($local_var_type eq "hexcolor") {
      # color calculation as an interpolation between 2 colors with a blending coef
      $interpolated_value = '#' . mix_color($color_initial, $color_final, $local_percentage);
    }
    elsif($local_var_type eq "int") {
      # color calculation as an interpolation between 2 colors with a blending coef
      $interpolated_value = int($local_var_init
          + $local_scene_percentage * ($local_var_fin - $local_var_init));
    }
  }
  # non-centered Gaussian
  elsif($local_interpolator =~ /^i/) {
    my @interpolation_values = split(' ',$local_interpolator);
    my $center =  clamp($interpolation_values[1]);
    my $local_percentage = NonCentered_Gaussian_01_01($local_scene_percentage, $center);
    if($local_var_type eq "float") {
      $interpolated_value = $local_var_init
          + $local_percentage * ($local_var_fin - $local_var_init);
    }
    elsif($local_var_type eq "hexcolor") {
      # color calculation as an interpolation between 2 colors with a blending coef
      $interpolated_value = '#' . mix_color($color_initial, $color_final, $local_percentage);
    }
    elsif($local_var_type eq "int") {
      # color calculation as an interpolation between 2 colors with a blending coef
      $interpolated_value = int($local_var_init
          + $local_scene_percentage * ($local_var_fin - $local_var_init));
    }
  }
  # linear on a sub part
  elsif($local_interpolator =~ /^L/) {
    my @interpolation_values = split(' ',$local_interpolator);
    my $perc_ini =  clamp($interpolation_values[1]);
    my $perc_fin =  clamp($interpolation_values[1] + $interpolation_values[2]);
    # printf "perc ini fin %.2f %.2f\n", $perc_ini, $perc_fin;
    if($local_scene_percentage <= $perc_ini) {
      $interpolated_value = $local_var_init;
    }
    elsif($local_scene_percentage >= $perc_fin) {
      $interpolated_value = $local_var_fin;
    }
    elsif($perc_ini >= $perc_fin) {
      $interpolated_value = $local_var_init;
    }
    else {
      my $local_percentage = ($local_scene_percentage - $perc_ini)/($perc_fin - $perc_ini);
      if($local_var_type eq "float") {
        $interpolated_value = $local_var_init
            + $local_percentage * ($local_var_fin - $local_var_init);
      }
      elsif($local_var_type eq "hexcolor") {
        # color calculation as an interpolation between 2 colors with a blending coef
        $interpolated_value = '#' . mix_color($color_initial, $color_final, $local_percentage);
      }
      elsif($local_var_type eq "int") {
        # color calculation as an interpolation between 2 colors with a blending coef
        $interpolated_value = int($local_var_init
            + $local_scene_percentage * ($local_var_fin - $local_var_init));
      }
    }
  }
  # Bezier smooth 00 01 10 11 on a subpart
  elsif($local_interpolator =~ /^B/) {
    my @interpolation_values = split(' ',$local_interpolator);
    my $perc_ini =  clamp($interpolation_values[1]);
    my $perc_fin =  clamp($interpolation_values[1] + $interpolation_values[2]);
    # printf "perc ini fin %.2f %.2f\n", $perc_ini, $perc_fin;
    if($local_scene_percentage <= $perc_ini) {
      $interpolated_value = $local_var_init;
    }
    elsif($local_scene_percentage >= $perc_fin) {
      $interpolated_value = $local_var_fin;
    }
    elsif($perc_ini >= $perc_fin) {
      $interpolated_value = $local_var_init;
    }
    else {
      my $local_percentage = ($local_scene_percentage - $perc_ini)/($perc_fin - $perc_ini);
      $local_percentage = Bezier_01_01($local_percentage);
      if($local_var_type eq "float") {
        $interpolated_value = $local_var_init
            + $local_percentage * ($local_var_fin - $local_var_init);
      }
      elsif($local_var_type eq "hexcolor") {
        # color calculation as an interpolation between 2 colors with a blending coef
        $interpolated_value = '#' . mix_color($color_initial, $color_final, $local_percentage);
      }
      elsif($local_var_type eq "int") {
        # color calculation as an interpolation between 2 colors with a blending coef
        $interpolated_value = int($local_var_init
            + $local_scene_percentage * ($local_var_fin - $local_var_init));
      }
    }
  }
  # Bezier steep 00 10 01 11 on a subpart
  elsif($local_interpolator =~ /^T/) {
    my @interpolation_values = split(' ',$local_interpolator);
    my $perc_ini =  clamp($interpolation_values[1]);
    my $perc_fin =  clamp($interpolation_values[1] + $interpolation_values[2]);
    # printf "perc ini fin %.2f %.2f\n", $perc_ini, $perc_fin;
    if($local_scene_percentage <= $perc_ini) {
      $interpolated_value = $local_var_init;
    }
    elsif($local_scene_percentage >= $perc_fin) {
      $interpolated_value = $local_var_fin;
    }
    elsif($perc_ini >= $perc_fin) {
      $interpolated_value = $local_var_init;
    }
    else {
      my $local_percentage = ($local_scene_percentage - $perc_ini)/($perc_fin - $perc_ini);
      $local_percentage = Bezier_steep_01_01($local_percentage);
      if($local_var_type eq "float") {
        $interpolated_value = $local_var_init
            + $local_percentage * ($local_var_fin - $local_var_init);
      }
      elsif($local_var_type eq "hexcolor") {
        # color calculation as an interpolation between 2 colors with a blending coef
        $interpolated_value = '#' . mix_color($color_initial, $color_final, $local_percentage);
      }
      elsif($local_var_type eq "int") {
        # color calculation as an interpolation between 2 colors with a blending coef
        $interpolated_value = int($local_var_init
            + $local_scene_percentage * ($local_var_fin - $local_var_init));
      }
    }
  }
  # Gaussian on a subpart
  elsif($local_interpolator =~ /^G/) {
    my @interpolation_values = split(' ',$local_interpolator);
    my $perc_ini =  clamp($interpolation_values[1]);
    my $perc_fin =  clamp($interpolation_values[1] + $interpolation_values[2]);
    # printf "perc ini fin %.2f %.2f\n", $perc_ini, $perc_fin;
    if($local_scene_percentage <= $perc_ini) {
      $interpolated_value = $local_var_init;
    }
    elsif($local_scene_percentage >= $perc_fin) {
      $interpolated_value = $local_var_init;
    }
    elsif($perc_ini >= $perc_fin) {
      $interpolated_value = $local_var_init;
    }
    else {
      my $local_percentage = ($local_scene_percentage - $perc_ini)/($perc_fin - $perc_ini);
      $local_percentage = Gaussian_01_01($local_percentage);
      if($local_var_type eq "float") {
        $interpolated_value = $local_var_init
            + $local_percentage * ($local_var_fin - $local_var_init);
      }
      elsif($local_var_type eq "hexcolor") {
        # color calculation as an interpolation between 2 colors with a blending coef
        $interpolated_value = '#' . mix_color($color_initial, $color_final, $local_percentage);
      }
      elsif($local_var_type eq "int") {
        # color calculation as an interpolation between 2 colors with a blending coef
        $interpolated_value = int($local_var_init
            + $local_scene_percentage * ($local_var_fin - $local_var_init));
      }
    }
  }
  # Gaussian steep on a subpart
  elsif($local_interpolator =~ /^H/) {
    my @interpolation_values = split(' ',$local_interpolator);
    my $perc_ini =  clamp($interpolation_values[1]);
    my $perc_fin =  clamp($interpolation_values[1] + $interpolation_values[2]);
    # printf "perc ini fin %.2f %.2f\n", $perc_ini, $perc_fin;
    if($local_scene_percentage <= $perc_ini) {
      $interpolated_value = $local_var_init;
    }
    elsif($local_scene_percentage >= $perc_fin) {
      $interpolated_value = $local_var_init;
    }
    elsif($perc_ini >= $perc_fin) {
      $interpolated_value = $local_var_init;
    }
    else {
      my $local_percentage = ($local_scene_percentage - $perc_ini)/($perc_fin - $perc_ini);
      $local_percentage = Gaussian_steep_01_01($local_percentage);
      if($local_var_type eq "float") {
        $interpolated_value = $local_var_init
            + $local_percentage * ($local_var_fin - $local_var_init);
      }
      elsif($local_var_type eq "hexcolor") {
        # color calculation as an interpolation between 2 colors with a blending coef
        $interpolated_value = '#' . mix_color($color_initial, $color_final, $local_percentage);
      }
      elsif($local_var_type eq "int") {
        # color calculation as an interpolation between 2 colors with a blending coef
        $interpolated_value = int($local_var_init
            + $local_scene_percentage * ($local_var_fin - $local_var_init));
      }
    }
  }
  # non-centered Gaussian on a subpart
  elsif($local_interpolator =~ /^I/) {
    my @interpolation_values = split(' ',$local_interpolator);
    my $center =  clamp($interpolation_values[1]);
    my $perc_ini =  clamp($interpolation_values[2]);
    my $perc_fin =  clamp($interpolation_values[2] + $interpolation_values[3]);
    # printf "perc ini fin %.2f %.2f\n", $perc_ini, $perc_fin;
    if($local_scene_percentage <= $perc_ini) {
      $interpolated_value = $local_var_init;
    }
    elsif($local_scene_percentage >= $perc_fin) {
      $interpolated_value = $local_var_init;
    }
    elsif($perc_ini >= $perc_fin) {
      $interpolated_value = $local_var_init;
    }
    else {
      my $local_percentage = ($local_scene_percentage - $perc_ini)/($perc_fin - $perc_ini);
      my $local_center = $perc_ini + $center * ($perc_fin - $perc_ini);
      $local_percentage = NonCentered_Gaussian_01_01($local_percentage, $local_center);
      if($local_var_type eq "float") {
        $interpolated_value = $local_var_init
            + $local_percentage * ($local_var_fin - $local_var_init);
      }
      elsif($local_var_type eq "hexcolor") {
        # color calculation as an interpolation between 2 colors with a blending coef
        $interpolated_value = '#' . mix_color($color_initial, $color_final, $local_percentage);
      }
      elsif($local_var_type eq "int") {
        # color calculation as an interpolation between 2 colors with a blending coef
        $interpolated_value = int($local_var_init
            + $local_scene_percentage * ($local_var_fin - $local_var_init));
      }
    }
  }
  elsif($local_interpolator eq "s") {
    $interpolated_value = $local_var_init;
  }
  else {
    $interpolated_value = $local_var_init;
  }
  return $interpolated_value;
}

1;