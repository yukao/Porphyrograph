#/usr/bin/perl
use Math::Trig;

$rMax = 10;
print "const float weights[",$rMax - 2,"][",$rMax,"] = { \n";
for($r = 2; $r < $rMax; $r++) {
	$rows = $r*2+1;
	$sigma = $r/3;
	$sigma22 = 2*$sigma*$sigma;
	$sigmaPi2 = 2*pi*$sigma;
	$sqrtSigmaPi2 = sqrt($sigmaPi2);
	$radius2 = $r*$r;
	$total = 0;
	$index = 0;
	@matrix = ();
	for ($row = 0; $row <= $r; $row++) {
		for ($line = 0; $line <= $r; $line++) {
			$distance = $row*$row + $line*$line;
			if ($distance > $radius2) {
				$matrix[$row][$line] = 0;
			}
			else {
				$matrix[$row][$line] = exp(-($distance)/$sigma22) / $sqrtSigmaPi2;
				if($line > 0 && $ row >= 0 ) {
				    $total += 4 * $matrix[$row][$line];
				}
				elsif ($row == 0 && $line == 0) {
					$total += $matrix[$row][$line];
				}
			}
		}
	}
	$new_total = 0;
	for ($row = 0; $row <= $r; $row++) {
		for ($line = 0; $line <= $r; $line++) {
			$matrix[$row][$line] /= $total;
			if( $row > 0) {
				$new_total += $matrix[$row][$line];
			}
		}
	}
	# just to check the algo // print "Total= ", 4 * $new_total + $matrix[0][0], "\n";
	print "  { ";
	for ($row = 0; $row <= $r; $row++) {
		print $matrix[$row][0] , ", ";
	}
	for ($row = $r + 1 ; $row < $rMax; $row++) {
		print "0, ";
	}
	print "},\n";
}
print "};\n";
