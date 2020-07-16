#!/usr/bin/perl 

use strict;
use warnings;

#my $fileInName = "/mnt/d/sync.com/Sync/YN-works/YN_LaTempete_2018/video-longues/movie_SVGs/5248b_SVG_";
#my $fileOutName = "/mnt/d/sync.com/Sync/YN-works/YN_LaTempete_2018/video-longues/movie_SVGs/5248b_SVG_";
my $fileInName = "/mnt/d/sync.com/Sync/YN-works/YN_LaTempete_2018/SVG_movies/movie_SVGs/5248b_SVG/5248_SVG_";
my $fileOutName = "/mnt/d/sync.com/Sync/YN-works/YN_LaTempete_2018/SVG_movies/movie_SVGs/5248b_SVG/5248b_SVG_";
my $initialInNo = 480;
my $finalInNo = 2440;
my $initialOutNo = 1;
#my $extension = "svg";
my $extension = "svg";

for(my $i = $finalInNo ; $i >= $initialInNo ; $i--) {
    my $count = sprintf "%04d", $i;
    my $newcount = sprintf "%04d", $i - $initialInNo + $initialOutNo;
    print "mv ${fileInName}${count}.${extension} ${fileOutName}${newcount}.${extension}\n";
    system("mv ${fileInName}${count}.${extension} ${fileOutName}${newcount}.${extension}");
}
