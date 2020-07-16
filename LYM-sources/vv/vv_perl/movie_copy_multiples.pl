#!/usr/bin/perl 

use strict;
use warnings;

#my $fileInName = "/mnt/d/sync.com/Sync/YN-works/YN_LaTempete_2018/video-longues/movie_SVGs/5248b_SVG_";
#my $fileOutName = "/mnt/d/sync.com/Sync/YN-works/YN_LaTempete_2018/video-longues/movie_SVGs/5248b_SVG_";
my $fileInName = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/movie_SVGs/thread-holder-02/contour-03_";
my $fileOutName = "/mnt/d/sync.com/Sync/LYM-videos-sources/LYM_Annika_Arachnid_2020/SVG_movies/movie_SVGs/thread-holder-02/contour-03_";
my $initialInNo = 163;
my $initialOutNo = 164;
my $finalOutNo = 266;
#my $extension = "svg";
my $extension = "svg";

# for(my $i = $finalInNo ; $i >= $initialInNo ; $i--) {
for(my $i = $initialOutNo ; $i <= $finalOutNo ; $i++) {
    my $count = sprintf "%04d", $initialInNo;
    my $newcount = sprintf "%04d", $i;
    print "cp ${fileInName}${count}.${extension} ${fileOutName}${newcount}.${extension}\n";
    system("cp ${fileInName}${count}.${extension} ${fileOutName}${newcount}.${extension}");
}
