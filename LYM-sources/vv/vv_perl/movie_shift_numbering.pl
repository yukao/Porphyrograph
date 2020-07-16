#!/usr/bin/perl 

use strict;
use warnings;

#my $fileInName = "/mnt/d/sync.com/Sync/YN-works/YN_LaTempete_2018/video-longues/movie_SVGs/5248b_SVG_";
#my $fileOutName = "/mnt/d/sync.com/Sync/YN-works/YN_LaTempete_2018/video-longues/movie_SVGs/5248b_SVG_";
my $fileInName = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/movie_SVGs/thread-holder-01-translated/contour-03_";
my $fileOutName = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/movie_SVGs/thread-holder-01-translated/thread-holder-01-translated_";
my $initialInNo = 1;
my $finalInNo = 266;
my $initialOutNo = 1;
#my $extension = "svg";
my $extension = "svg";

for(my $i = $finalInNo ; $i >= $initialInNo ; $i--) {
# for(my $i = $initialInNo ; $i <= $finalInNo ; $i++) {
    my $count = sprintf "%04d", $i;
    my $newcount = sprintf "%04d", $i - $initialInNo + $initialOutNo;
    print "mv ${fileInName}${count}.${extension} ${fileOutName}${newcount}.${extension}\n";
    system("mv ${fileInName}${count}.${extension} ${fileOutName}${newcount}.${extension}");
}
